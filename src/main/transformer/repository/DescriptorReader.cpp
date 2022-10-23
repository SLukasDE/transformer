#include <transformer/repository/DescriptorReader.h>

#include <esl/system/Stacktrace.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace transformer {
namespace repository {

namespace {
std::string extractDefineName(const std::string& define) {
	return define.substr(0, define.find('='));
}

bool isComment(const std::string& str) {
	if(str.empty()) {
		return false;
	}
	// is comment
	return (str.at(0) == '#');
}

bool readCheckIfEmpty(std::istringstream& iss) {
	std::string token;
	iss >> token;
	return token.empty() || isComment(token);
}

bool readToken(std::string& token, std::istringstream& iss) {
	std::string tmpToken;

	iss >> tmpToken;
	token.clear();

	for(std::size_t index = 0; index < tmpToken.size(); ++index) {
		if(tmpToken.at(index) == '#') {
			return true;
			break;
		}

		token += tmpToken.at(index);
	}

	return false;
}

void readLastToken(std::string& token, std::istringstream& iss) {
	bool hasComment = readToken(token, iss);
	if(!hasComment && !readCheckIfEmpty(iss)) {
		throw esl::system::Stacktrace::add(std::runtime_error("unknown text after '" + token + "'"));
	}
}

std::vector<std::string> readList(std::istringstream& iss) {
	std::vector<std::string> result;
	std::string token;
	bool hasComment = false;

	while(!hasComment) {
		hasComment = readToken(token, iss);
		if(token.empty()) {
			break;
		}
		result.push_back(token);
	}

	return result;
}

std::vector<std::string> splitDependencyType(const std::string& types) {
	std::vector<std::string> result;

	std::string::const_iterator iterCurrent = std::begin(types);
	std::string::const_iterator iterLast = iterCurrent;
	for(;iterCurrent != std::end(types); ++iterCurrent) {
		if(*iterCurrent == ',') {
			result.emplace_back(iterLast, iterCurrent);
			iterLast = iterCurrent;
			++iterLast;
		}
	}
	result.emplace_back(iterLast, iterCurrent);

	return result;
}

}

DescriptorReader::DescriptorReader(model::Descriptor& aDescriptor)
: descriptor(aDescriptor),
  context(Context::cxMain),
  activeVariantNames{""}
{
}

DescriptorReader::~DescriptorReader() {
}


void DescriptorReader::read(const boost::filesystem::path path) {
	if(!boost::filesystem::exists(path)) {
		throw esl::system::Stacktrace::add(std::runtime_error("File \"" + path.generic_string() + "\" does not exists."));
	}
	if(!boost::filesystem::is_regular(path)) {
		throw esl::system::Stacktrace::add(std::runtime_error("Path \"" + path.generic_string() + "\" is not a file."));
	}
	std::ifstream istream(path.string());

	read(istream);
}

void DescriptorReader::read(std::istream& istream) {
	bool hasId = false;
	bool hasName = false;
	bool hasBaseName = false;
	std::map<std::string, bool> hasSourcesMainDir;
	std::map<std::string, bool> hasSourcesTestDir;
	std::map<std::string, bool> hasHeadersDir;
	std::map<std::string, bool> hasBuildDir;
	std::map<std::string, bool> hasSources;
	std::map<std::string, bool> hasHeaders;
	std::map<std::string, bool> hasArchitecture;
//	std::map<std::string, bool> hasGenerators;
	std::map<std::string, bool> hasProvide;

	std::string line;
	while(std::getline(istream, line))  {
		std::istringstream iss(line);
		std::string token;

		iss >> token;

		// is empty line
		if(token.empty()) {
			continue;
		}
		// is comment
		if(isComment(token)) {
			continue;
		}

		if(token == "id:" && context == Context::cxMain) {
			if(hasId) {
				throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of id"));
			}
			hasId = true;

			/* read name */
			bool hasComment = readIdAndVariantName(descriptor.artefactId, descriptor.variantName, iss);

			if(hasComment) {
				descriptor.artefactVersion = "1.0.0";
				descriptor.apiVersion = "1.0.0";
				descriptor.commonVersion = true;
			}
			else {
				model::Version version;
				hasComment = readVersion(version, iss);

				descriptor.commonVersion = version.hasCommonVersion();

				if(version.getVersionArtefact().empty()) {
					throw esl::system::Stacktrace::add(std::runtime_error("No artefact version defined at definition of artefact id"));
				}
				descriptor.artefactVersion = version.getVersionArtefact();

				if(version.getVersionApi().empty()) {
					throw esl::system::Stacktrace::add(std::runtime_error("No api version defined at definition of artefact id"));
				}
				descriptor.apiVersion = version.getVersionApi();
			}

			if(!hasComment && !readCheckIfEmpty(iss)) {
				throw esl::system::Stacktrace::add(std::runtime_error("unknown definition of artefact id"));
			}
		}
		else if(token == "name:" && context == Context::cxMain) {
			if(hasName) {
				throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of name"));
			}
			hasName = true;

			/* read name */
			readLastToken(descriptor.artefactName, iss);
		}
		else if(token == "base:" && context == Context::cxMain) {
			if(hasBaseName) {
				throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of base"));
			}
			hasBaseName = true;

			/* read base name and version */
			readBaseName(iss);
		}
		else if(token == "build-dir:" && (context == Context::cxMain)) {
			for(const auto& variantName : activeVariantNames) {
				if(hasBuildDir[variantName]) {
					throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of 'build-dir' for variant \"" + variantName + "\""));
				}
				hasBuildDir[variantName] = true;
			}

			readBuildDir(iss);
		}
		else if(token == "sources-main-dir:" && (context == Context::cxMain || context == Context::cxVariant)) {
			for(const auto& variantName : activeVariantNames) {
				if(hasSourcesMainDir[variantName]) {
					throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of 'sources-main-dir' for variant \"" + variantName + "\""));
				}
				hasSourcesMainDir[variantName] = true;
			}

			readSourcesMainDir(iss);
		}
		else if(token == "sources-test-dir:" && (context == Context::cxMain || context == Context::cxVariant)) {
			for(const auto& variantName : activeVariantNames) {
				if(hasSourcesTestDir[variantName]) {
					throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of 'sources-test-dir' for variant \"" + variantName + "\""));
				}
				hasSourcesTestDir[variantName] = true;
			}

			readSourcesTestDir(iss);
		}
		else if(token == "headers-dir:" && (context == Context::cxMain || context == Context::cxVariant)) {
			for(const auto& variantName : activeVariantNames) {
				if(hasHeadersDir[variantName]) {
					throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of 'headers-dir' for variant \"" + variantName + "\""));
				}
				hasHeadersDir[variantName] = true;
			}

			readHeadersDir(iss);
		}
		else if(token == "source-file-pattern:" && (context == Context::cxMain || context == Context::cxVariant)) {
			for(const auto& variantName : activeVariantNames) {
				if(hasSources[variantName]) {
					throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of 'sources' for variant \"" + variantName + "\""));
				}
				hasSources[variantName] = true;
			}

			readSourceFilePatterns(iss);
		}
		else if(token == "header-file-pattern:" && (context == Context::cxMain || context == Context::cxVariant)) {
			for(const auto& variantName : activeVariantNames) {
				if(hasHeaders[variantName]) {
					throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of 'headers' for variant \"" + variantName + "\""));
				}
				hasHeaders[variantName] = true;
			}

			readHeaderFilePatterns(iss);
		}
		else if(token == "architecture:" && (context == Context::cxMain || context == Context::cxVariant)) {
			for(const auto& variantName : activeVariantNames) {
				if(hasArchitecture[variantName]) {
					throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of architecture for variant \"" + variantName + "\""));
				}
				hasArchitecture[variantName] = true;
			}

			readArchitectures(iss);
		}
		else if(token == "generator:" && (context == Context::cxMain || context == Context::cxVariant)) {

			readGenerator(iss);
		}
		else if(token == "define-private:" && (context == Context::cxMain || context == Context::cxVariant)) {
			readDefinePrivate(iss);
		}
		else if(token == "define-public:" && (context == Context::cxMain || context == Context::cxVariant)) {
			readDefinePublic(iss);
		}
		else if(token == "provide:" && (context == Context::cxMain || context == Context::cxVariant)) {
			for(const auto& variantName : activeVariantNames) {
				if(hasProvide[variantName]) {
					throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of \"provide\" for variant \"" + variantName + "\""));
				}
				hasProvide[variantName] = true;
			}

			readProvide(iss);
		}
		else if(token == "variant:") {
			auto list = readList(iss);
			if(list.empty()) {
				throw esl::system::Stacktrace::add(std::runtime_error("missing variant name after 'variant:'"));
			}

			for(const auto& variantName : list) {
				addVariant(variantName);
			}
			activeVariantNames.clear();
			std::copy(std::begin(list), std::end(list), std::inserter(activeVariantNames, std::end(activeVariantNames)));

			context = Context::cxVariant;
		}
		else if(token == "solved:") {
			bool hasComment = readToken(solvedName, iss);
			if(solvedName.empty()) {
				throw esl::system::Stacktrace::add(std::runtime_error("missing variant name after 'solved:'"));
			}
			if(!hasComment && !readCheckIfEmpty(iss)) {
				throw esl::system::Stacktrace::add(std::runtime_error("unknown text after 'solved: " + solvedName + "'"));
			}
			addSolved(solvedName);
			context = Context::cxSolved;
		}
		else if(token.size() > 0 && token.at(token.size()-1) == ':') {
			bool allowedStatic = false;
			bool allowedDynamic = false;
			bool allowedSystem = false;

			token = token.substr(0, token.size()-1);
			std::vector<std::string> dependencyTypes = splitDependencyType(token);
			for(const auto& dependencyType : dependencyTypes) {
				if(dependencyType == "static") {
					allowedStatic = true;
				}
				else if(dependencyType == "dynamic") {
					allowedDynamic = true;
				}
				else if(dependencyType == "system") {
//					allowedStatic = true;
//					allowedDynamic = true;
					allowedSystem = true;
				}
				else {
					throw esl::system::Stacktrace::add(std::runtime_error("unknown token: \"" + token + "\" in line \"" + line + "\""));
				}
			}
			if(context == Context::cxMain || context == Context::cxVariant) {
				readDependency(allowedStatic, allowedDynamic, allowedSystem, iss);
			}
			else if(context == Context::cxSolved) {
				readDependencySolved(allowedStatic, allowedDynamic, allowedSystem, iss);
			}
			else {
				throw esl::system::Stacktrace::add(std::runtime_error("cannot read dependency for unknown context"));
			}
		}
		else {
			throw esl::system::Stacktrace::add(std::runtime_error("unknown token: \"" + token + "\" in line \"" + line + "\""));
		}
	}

	if(descriptor.artefactId.empty()) {
		descriptor.artefactId = boost::filesystem::current_path().leaf().generic_string();
		descriptor.artefactVersion = "1.0.0";
		descriptor.apiVersion = "1.0.0";
		descriptor.commonVersion = true;
	}

	if(descriptor.artefactName.empty()) {
		descriptor.artefactName = descriptor.artefactId;
	}
}

bool DescriptorReader::readIdAndVariantName(std::string& id, std::string& variantName, std::istringstream& iss) {
	std::string token;
	bool hasVariantName = false;
	bool hasComment = false;

	iss >> token;
	id.clear();
	variantName.clear();
	for(std::size_t index = 0; index < token.size(); ++index) {
		if(token.at(index) == '#') {
			hasComment = true;
			break;
		}

		if(token.at(index) == '[') {
			if(hasVariantName) {
				throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of variant"));
			}
			hasVariantName = true;
			for(++index; index < token.size(); ++index) {
				if(token.at(index) == '#') {
					throw esl::system::Stacktrace::add(std::runtime_error("incomplete definition of variant"));
				}
				if(token.at(index) == ']') {
					break;
				}
				variantName += token.at(index);
			}
			continue;
		}

		if(hasVariantName) {
			throw esl::system::Stacktrace::add(std::runtime_error("invalid character after definition of variant"));
		}

		id += token.at(index);
	}

	if(id.empty()) {
		throw esl::system::Stacktrace::add(std::runtime_error("no id defined by reading id and variantName"));
	}

	return hasComment;
}

void DescriptorReader::readBaseName(std::istringstream& iss) {
	bool hasComment = readToken(descriptor.baseName, iss);

	if(hasComment) {
		descriptor.baseArtefactVersion = "1.0.0";
	}
	else {
		model::Version version;
		readVersion(version, iss);

		if(version.hasApiVersion()) {
			std::cout << "warning: ignoring API version. Base version has to be specifies just by an artefact version.\n";
		}
		if(!version.hasArtefactVersion()) {
			std::cout << "Warning: assume base artefact version 1.0.0 because base version has been specified without artefact version.\n";
			descriptor.baseArtefactVersion	= "1.0.0";
		}
		else {
			descriptor.baseArtefactVersion	= version.getVersionArtefact();
		}
	}
}

bool DescriptorReader::readVersion(model::Version& version, std::istringstream& iss) {
	bool hasComment = false;
	bool hasVersionCommon = false;
	bool hasVersionArtefact = false;
	bool hasVersionApi = false;
	std::string versionCommon;
	std::string versionArtefact;
	std::string versionApi;
	std::string token;

	iss >> token;
	if(token.empty()) {
		throw esl::system::Stacktrace::add(std::runtime_error("cannot read version from empty token"));
	}

	for(std::size_t index = 0; index < token.size(); ++index) {
		if(token.at(index) == '#') {
			hasComment = true;
			break;
		}

		if(token.at(index) == '[') {
			if(hasVersionArtefact) {
				throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of artefact version"));
			}

			for(++index; index < token.size(); ++index) {
				if(token.at(index) == '#') {
					throw esl::system::Stacktrace::add(std::runtime_error("incomplete definition of artefact version. Missing ']' character because of starting a comment"));
				}
				if(token.at(index) == ']') {
					hasVersionArtefact = true;
					break;
				}
				versionArtefact += token.at(index);
			}
			if(!hasVersionArtefact) {
				throw esl::system::Stacktrace::add(std::runtime_error("incomplete definition of artefact version. Missing ']' character."));
			}
			continue;
		}

		if(token.at(index) == '{') {
			if(hasVersionApi) {
				throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of api version"));
			}

			for(++index; index < token.size(); ++index) {
				if(token.at(index) == '#') {
					throw esl::system::Stacktrace::add(std::runtime_error("incomplete definition of api version. Missing '}' character because of starting a comment"));
				}
				if(token.at(index) == '}') {
					hasVersionApi = true;
					break;
				}
				versionApi += token.at(index);
			}
			if(!hasVersionApi) {
				throw esl::system::Stacktrace::add(std::runtime_error("incomplete definition of api version. Missing '}' character."));
			}
			continue;
		}

		if(hasVersionArtefact) {
			throw esl::system::Stacktrace::add(std::runtime_error("cannot define a common version if artefact version is defined already"));
		}
		if(hasVersionApi) {
			throw esl::system::Stacktrace::add(std::runtime_error("cannot define a common version if api version is defined already"));
		}
		hasVersionCommon = true;
		for(; index < token.size(); ++index) {
			if(token.at(index) == '#') {
				hasComment = true;
				break;
			}
			versionCommon += token.at(index);
		}
		break;
	}

	if(hasVersionCommon) {
		version.setCommonVersion(versionCommon);
	}
	else if(hasVersionArtefact || hasVersionApi) {
		if(hasVersionArtefact) {
			version.setArtefactVersion(versionArtefact);
		}
		if(hasVersionApi) {
			version.setApiVersion(versionApi);
		}
	}
	else {
		throw esl::system::Stacktrace::add(std::runtime_error("reading version failed"));
	}

	return hasComment;
}

bool DescriptorReader::readVersionRange(model::VersionRange& versionRange, bool& isEmpty, std::istringstream& iss) {
	bool hasComment = false;
	bool hasVersionCommon = false;
	bool hasVersionArtefactMin = false;
	bool hasVersionArtefactMax = false;
	bool hasVersionApiMin = false;
	bool hasVersionApiMax = false;
	std::string versionCommon;
	std::string versionArtefactMin;
	std::string versionArtefactMax;
	std::string versionApiMin;
	std::string versionApiMax;
	std::string token;

	iss >> token;
	isEmpty = token.empty();
	if(isEmpty) {
		return false;
	}
	if(token.at(0) == '#') {
		return true;
	}

	for(std::size_t index = 0; index < token.size(); ++index) {
		if(token.at(index) == '#') {
			hasComment = true;
			break;
		}

		if(token.at(index) == '[') {
			if(hasVersionArtefactMin) {
				throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of artefact min-version"));
			}

			for(++index; index < token.size(); ++index) {
				if(token.at(index) == '#') {
					throw esl::system::Stacktrace::add(std::runtime_error("incomplete definition of artefact min-version. Missing ']' character because of starting a comment"));
				}
				if(token.at(index) == ']') {
					hasVersionArtefactMin = true;
					break;
				}
				versionArtefactMin += token.at(index);
			}
			if(hasVersionArtefactMin) {
				continue;
			}
			hasVersionArtefactMin = true;

			iss >> token;
			for(index = 0; index < token.size(); ++index) {
				if(token.at(index) == '#') {
					throw esl::system::Stacktrace::add(std::runtime_error("incomplete definition of artefact max-version. Missing ']' character because of starting a comment"));
				}
				if(token.at(index) == ']') {
					hasVersionArtefactMax = true;
					break;
				}
				versionArtefactMax += token.at(index);
			}

			if(!hasVersionArtefactMax) {
				throw esl::system::Stacktrace::add(std::runtime_error("incomplete definition of artefact max-version. Missing ']' character."));
			}
			continue;
		}

		if(token.at(index) == '{') {
			if(hasVersionApiMin) {
				throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of api min-version"));
			}

			for(++index; index < token.size(); ++index) {
				if(token.at(index) == '#') {
					throw esl::system::Stacktrace::add(std::runtime_error("incomplete definition of api min-version. Missing '}' character because of starting a comment"));
				}
				if(token.at(index) == '}') {
					hasVersionApiMin = true;
					break;
				}
				versionApiMin += token.at(index);
			}
			if(hasVersionApiMin) {
				continue;
			}
			hasVersionApiMin = true;

			iss >> token;
			for(index = 0; index < token.size(); ++index) {
				if(token.at(index) == '#') {
					throw esl::system::Stacktrace::add(std::runtime_error("incomplete definition of api max-version. Missing '}' character because of starting a comment"));
				}
				if(token.at(index) == '}') {
					hasVersionApiMax = true;
					break;
				}
				versionApiMax += token.at(index);
			}

			if(!hasVersionApiMax) {
				throw esl::system::Stacktrace::add(std::runtime_error("incomplete definition of api max-version. Missing '}' character."));
			}
			continue;
		}

		if(hasVersionArtefactMin) {
			throw esl::system::Stacktrace::add(std::runtime_error("cannot define a common version if artefact min-version is defined already"));
		}
		if(hasVersionApiMin) {
			throw esl::system::Stacktrace::add(std::runtime_error("cannot define a common version if api min-version is defined already"));
		}
		hasVersionCommon = true;
		for(; index < token.size(); ++index) {
			if(token.at(index) == '#') {
				hasComment = true;
				break;
			}
			versionCommon += token.at(index);
		}
		break;
	}

	if(hasVersionCommon) {
		versionRange.setCommonVersion(versionCommon);
	}
	else if(hasVersionArtefactMin || hasVersionApiMin) {
		if(hasVersionArtefactMin) {
			if(hasVersionArtefactMax) {
				versionRange.setArtefactVersion(versionArtefactMin, versionArtefactMax);
			}
			else {
				versionRange.setArtefactVersion(versionArtefactMin);
			}
		}
		if(hasVersionApiMin) {
			if(hasVersionApiMax) {
				versionRange.setApiVersion(versionApiMin, versionApiMax);
			}
			else {
				versionRange.setApiVersion(versionApiMin);
			}
		}
	}
	else {
		throw esl::system::Stacktrace::add(std::runtime_error("reading version-range failed"));
	}

	return hasComment;
}

void DescriptorReader::readSourcesMainDir(std::istringstream& iss) {
	std::vector<std::string> list = readList(iss);

	for(const auto& variantName : activeVariantNames) {
		getVariant(variantName).sourcesMainDir = list;
	}
}

void DescriptorReader::readSourcesTestDir(std::istringstream& iss) {
	std::vector<std::string> list = readList(iss);

	for(const auto& variantName : activeVariantNames) {
		getVariant(variantName).sourcesTestDir = list;
	}
}

void DescriptorReader::readHeadersDir(std::istringstream& iss) {
	std::vector<std::string> list = readList(iss);

	for(const auto& variantName : activeVariantNames) {
		getVariant(variantName).headersDir = list;
	}
}

void DescriptorReader::readBuildDir(std::istringstream& iss) {
	std::string token;

	readLastToken(token, iss);
	descriptor.buildDir = token;
}

void DescriptorReader::readSourceFilePatterns(std::istringstream& iss) {
	std::vector<std::string> list = readList(iss);

	for(const auto& variantName : activeVariantNames) {
		getVariant(variantName).sourceFilePatterns = list;
	}
}

void DescriptorReader::readHeaderFilePatterns(std::istringstream& iss) {
	std::vector<std::string> list = readList(iss);

	for(const auto& variantName : activeVariantNames) {
		getVariant(variantName).headerFilePatterns = list;
	}
}

void DescriptorReader::readArchitectures(std::istringstream& iss) {
	std::vector<std::string> list = readList(iss);

	for(const auto& variantName : activeVariantNames) {
		getVariant(variantName).architectures = list;
	}
}

void DescriptorReader::readGenerator(std::istringstream& iss) {
	model::Generator generator;

	bool hasComment = readIdAndVariantName(generator.id, generator.variantName, iss);
	if(generator.id.empty()) {
		throw esl::system::Stacktrace::add(std::runtime_error("missing ID at defintion of generator."));
	}

	if(!hasComment) {
		bool isEmpty = false;

		readVersionRange(generator.versionRange, isEmpty, iss);
	}

	for(const auto& variantName : activeVariantNames) {
		model::Variant& variant = getVariant(variantName);
		variant.generators[generator.id] = generator;
	}
}

void DescriptorReader::readDefinePrivate(std::istringstream& iss) {
	std::string token;
	std::string key;

	readLastToken(token, iss);
	if(token.empty()) {
		return;
	}
	key = extractDefineName(token);
	for(const auto& variantName : activeVariantNames) {
		getVariant(variantName).definesPrivate[key] = token;
	}
}

void DescriptorReader::readDefinePublic(std::istringstream& iss) {
	std::string token;
	std::string key;

	readLastToken(token, iss);
	if(token.empty()) {
		return;
	}
	key = extractDefineName(token);
	for(const auto& variantName : activeVariantNames) {
		getVariant(variantName).definesPublic[key] = token;
	}
}

void DescriptorReader::readProvide(std::istringstream& iss) {
	bool provideSource = false;
	bool provideStatic = false;
	bool provideDynamic = false;
	bool provideExecutable = false;

	std::string token;
	bool hasComment = false;
	while(!hasComment) {
		hasComment = readToken(token, iss);
		if(token.empty()) {
			break;
		}
		if(token == "source") {
			provideSource = true;
		}
		else if(token == "static") {
			provideStatic = true;
		}
		else if(token == "dynamic") {
			provideDynamic = true;
		}
		else if(token == "executable") {
			provideExecutable = true;
		}
		else {
			throw esl::system::Stacktrace::add(std::runtime_error("unknown token after 'provide: ... " + token + "'"));
		}
	}

	for(const auto& variantName : activeVariantNames) {
		model::Variant& variant = getVariant(variantName);
		variant.provideSource = provideSource;
		variant.provideStatic = provideStatic;
		variant.provideDynamic = provideDynamic;
		variant.provideExecutable = provideExecutable;
	}
}

void DescriptorReader::readDependency(bool allowedStatic, bool allowedDynamic, bool allowedSystem, std::istringstream& iss) {
	model::Dependency dependency;

	dependency.allowedTypeRepositoryStatic = allowedStatic;
	dependency.allowedTypeRepositoryDynamic = allowedDynamic;
	dependency.allowedTypeSystemDynamic = allowedSystem;

	bool hasComment = readIdAndVariantName(dependency.id, dependency.variantName, iss);
	if(dependency.id.empty() || hasComment) {
		throw esl::system::Stacktrace::add(std::runtime_error("missing version-range after definition of dependency \"" + dependency.id + "\" because it ends with comment"));
	}

	while(!hasComment) {
		model::VersionRange versionRange;
		bool isEmpty = false;

		hasComment = readVersionRange(versionRange, isEmpty, iss);
		if(isEmpty) {
			break;
		}

		dependency.versionRanges.ranges.push_back(versionRange);
	}

	for(const auto& variantName : activeVariantNames) {
		model::Variant& variant = getVariant(variantName);
		variant.dependencies.emplace(dependency.id, dependency);
	}
}

void DescriptorReader::readDependencySolved(bool allowedStatic, bool allowedDynamic, bool allowedSystem, std::istringstream& iss) {
	model::DependencySolved dependencySolved;

	dependencySolved.allowedTypeRepositoryStatic = allowedStatic;
	dependencySolved.allowedTypeRepositoryDynamic = allowedDynamic;
	dependencySolved.allowedTypeSystemDynamic = allowedSystem;

	bool hasComment = readIdAndVariantName(dependencySolved.name, dependencySolved.variantName, iss);
	if(dependencySolved.name.empty() || hasComment) {
		throw esl::system::Stacktrace::add(std::runtime_error("missing version after definition of solved dependency \"" + dependencySolved.name + "\""));
	}

	model::Version tmpVersion;
	hasComment = readVersion(tmpVersion, iss);
	if(!hasComment && !readCheckIfEmpty(iss)) {
		throw esl::system::Stacktrace::add(std::runtime_error("unknown characters after definition of solved dependency \"" + dependencySolved.name + "\""));
	}

	if(tmpVersion.hasCommonVersion()) {
		throw esl::system::Stacktrace::add(std::runtime_error("invalid version definition of solved dependency \"" + dependencySolved.name + "\". Only API version is allowed, but common version has been specified"));
	}
	if(!tmpVersion.getVersionArtefact().empty()) {
		throw esl::system::Stacktrace::add(std::runtime_error("invalid version definition of solved dependency \"" + dependencySolved.name + "\". Only API version is allowed, but artefact version has been specified"));
	}

	dependencySolved.versionApi = tmpVersion.getVersionApi();
	if(dependencySolved.versionApi.empty()) {
		throw esl::system::Stacktrace::add(std::runtime_error("invalid definition of solved dependency \"" + dependencySolved.name + "\". API version is empty"));
	}

	getSolved(solvedName).push_back(dependencySolved);
}

void DescriptorReader::addVariant(const std::string& name) {
	if(!name.empty() && !descriptor.hasSpecialVariant(name)) {
		descriptor.addSpecialVariant(name);
	}
}

void DescriptorReader::addSolved(const std::string& name) {
	if(!name.empty() && !descriptor.hasSolved(name)) {
		descriptor.addSolved(name);
	}
}

model::Variant& DescriptorReader::getVariant(const std::string& aVariantName) {
	if(aVariantName.empty()) {
		return descriptor.variant;
	}

	auto iter = descriptor.variants.find(aVariantName);
	if(iter == std::end(descriptor.variants)) {
		if(aVariantName == descriptor.variantName) {
			return descriptor.variant;
		}
		throw esl::system::Stacktrace::add(std::runtime_error("request for unknown variant \"" + aVariantName + "\""));
	}

	return *iter->second.get();
}

std::vector<model::DependencySolved>& DescriptorReader::getSolved(const std::string& aVariantName) {
	auto iter = descriptor.solved.find(aVariantName);
	if(iter == std::end(descriptor.solved)) {
		throw esl::system::Stacktrace::add(std::runtime_error("request for unknown solved variant \"" + aVariantName + "\""));
	}

	return iter->second;
}

} /* namespace repository */
} /* namespace transformer */
