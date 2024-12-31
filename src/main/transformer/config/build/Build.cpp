#include <transformer/config/build/Build.h>
#include <transformer/config/build/Architecture.h>
#include <transformer/config/build/Define.h>
#include <transformer/config/build/Dependency.h>
#include <transformer/config/Exception.h>

#include <esl/utility/String.h>

#include <fstream>
#include <sstream>
#include <utility>

namespace transformer {
namespace config {
namespace build {

namespace {
std::vector<std::unique_ptr<Variant>> createVariants() {
	std::vector<std::unique_ptr<Variant>> variants;
	variants.push_back(std::unique_ptr<Variant>(new Variant("", false)));
	return variants;
}
}

Build::Build()
: variants(createVariants()),
  activeVariant(std::ref(*variants[0]))
{
	variantByName.insert(std::make_pair("", activeVariant));
}

void Build::load(const std::filesystem::path path) {
	Parser parser(path);
	load(parser);
	/*
	if(!std::filesystem::exists(path)) {
		throw Exception(parser, "File \"" + path.generic_string() + "\" does not exists.");
	}
	if(!std::filesystem::is_regular(path)) {
		throw Exception(parser, "Path \"" + path.generic_string() + "\" is not a file.");
	}
	std::ifstream istream(path.string());

	load(istream);
	*/
}

void Build::load(std::istream& iStream) {
	Parser parser(iStream);
	load(parser);
}

void Build::load(Parser& parser) {
//	Context context = Context::cxMain;
	bool hasId = false;
	bool hasName = false;
	bool hasBuildDir = false;
	//bool hasBaseName = false;

	while(parser.nextLine())  {
		std::string token;

		parser.readToken(token);
		token = esl::utility::String::trim(token);

		if(token.empty()) {
			continue;
		}

		/* check if we are still in a main section, not in a variant section.
		 * if yes then we have some extra configuration token.
		 */
		if(activeVariant.get().getName() == "") {
#if 0
			if(token == "base:") {
				if(hasBaseName) {
					throw Exception(parser, "multiple definition of base");
				}
				hasBaseName = true;

				/* read base name and version */
				readBaseName(iss);
			}
			continue;
#endif
			if(token == "id:") {
				if(hasId) {
					throw Exception(parser, "multiple definition of id");
				}
				hasId = true;

				/* read id */
				parser.readLastToken(id);
				if(id.empty()) {
					throw Exception(parser, "no id defined while reading id");
				}

				continue;
			}

			if(token == "name:") {
				if(hasName) {
					throw Exception(parser, "multiple definition of name");
				}
				hasName = true;

				/* read name */
				parser.readLastToken(name);
				if(name.empty()) {
					throw Exception(parser, "no name defined while reading name");
				}

				continue;
			}

			if(token == "build-dir:") {
				if(hasBuildDir) {
					throw Exception(parser, "multiple definition of 'build-dir'");
				}
				hasBuildDir = true;

				/* read buildDirectory */
				parser.readLastToken(buildDirectory);
				if(buildDirectory.empty()) {
					throw Exception(parser, "no buildDirectory defined while reading build-dir");
				}

				continue;
			}
		}

		if(token == "variant-public:" || token == "variant-private:") {
			bool isPublic = (token == "variant-public:");

			/* read variant-name */
			std::string variantName;
			parser.readLastToken(variantName);

			if(variantName == "default" && isPublic == false) {
				throw Exception(parser, "variant name \"default\" is reserved for public variant.");
			}

			auto iter = variantByName.find(variantName);
			if(iter != variantByName.end()) {
				throw Exception(parser, "multiple definition of variant \"" + variantName + "\"");
			}

			Variant* variantPtr = new Variant(variantName, isPublic);
			variants.push_back(std::unique_ptr<Variant>(variantPtr));

			activeVariant = std::ref(*variantPtr);
			variantByName.insert(std::make_pair(variantName, activeVariant));

			continue;
		}

		if(token == "version:") {
			if(activeVariant.get().getName() != "" && activeVariant.get().isPublic() == false) {
				throw Exception(parser, "setting version for private variant is not allowed");
			}

			/* read version */
			std::string version;
			parser.readLastToken(version);
			activeVariant.get().addVersion(version);

			continue;
		}

		if(token == "architecture:") {
			/* read architecture */
			auto list = parser.readList();

			Architecture architecture;
			architecture.features = std::set<std::string>(list.begin(), list.end());
			activeVariant.get().addArchitecture(architecture);

			continue;
		}

		if(token == "sources-dir:") {
			/* read source-dir */
			std::string sourceDirectory;
			parser.readLastToken(sourceDirectory);
			activeVariant.get().addSourceDirectory(sourceDirectory);

			continue;
		}

		if(token == "file-pattern:") {
			/* read file type */
			std::string fileType;
			bool hasComment = parser.readToken(fileType);
			fileType = esl::utility::String::trim(fileType);

			// is empty file type
			if(fileType.empty()) {
				throw Exception(parser, "file-pattern has empty file type");
			}

			// has comment
			if(hasComment) {
				throw Exception(parser, "file-pattern definition missing for file type \"" + fileType + "\"");
			}

			/* read file-pattern */
			std::string filePattern;
			parser.readLastToken(filePattern);
			activeVariant.get().addFilePattern(parser, fileType, filePattern);

			continue;
		}

		if(token == "provide:") {
			std::string provideToken;
			bool hasComment = false;

			while(!hasComment) {
				hasComment = parser.readToken(provideToken);
				if(provideToken.empty()) {
					break;
				}
				if(provideToken == "source") {
					activeVariant.get().setProvideSource();
				}
				else if(provideToken == "static") {
					activeVariant.get().setProvideStatic();
				}
				else if(provideToken == "dynamic") {
					activeVariant.get().setProvideDynamic();
				}
				else if(provideToken == "executable") {
					activeVariant.get().setProvideExecutable();
				}
				else {
					throw Exception(parser, "unknown token after 'provide: ... " + token + "'");
				}
			}

			continue;
		}

		if(token == "dynamic:" || token == "static:") {
			std::string dependencyToken;
			bool hasComment = false;
			std::pair<std::string, std::string> rv;

			Dependency dependency;
			dependency.type = (token == "dynamic:" ? Dependency::dtDynamic : Dependency::dtStatic);

			hasComment = parser.readToken(dependencyToken);
			rv = Parser::splitTokenWithOption(dependencyToken);
			dependency.name = rv.first;
			dependency.variant = rv.second;

			if(dependency.name.empty()) {
				throw Exception(parser, "missing dependency name");
			}

			if(!hasComment) {
				parser.readLastToken(dependencyToken);
				rv = Parser::splitTokenWithOption(dependencyToken);
				dependency.version = rv.first;
				dependency.versionSolved = rv.second;
			}

			activeVariant.get().addDependency(parser, dependency);

			continue;
		}

		if(token == "define-public:" || token == "define-private:") {
			Define define;
			define.isPublic = (token == "define-public:");

			parser.readLastToken(define.define);
			define.define = esl::utility::String::trim(define.define);
			if(define.define.empty()) {
				if(define.isPublic) {
					throw Exception(parser, "define-public has empty value");
				}
				else {
					throw Exception(parser, "define-private has empty value");
				}
			}

			define.key = esl::utility::String::split(define.define, '=')[0];
			activeVariant.get().addDefine(parser, define);

			continue;
		}
#if 0
		else if(token == "generator:" && (context == Context::cxMain || context == Context::cxVariant)) {

			readGenerator(iss);
		}
#endif

		throw Exception(parser, "unknown token: \"" + token + "\"");
	}

	if(id.empty()) {
		//id = std::filesystem::current_path().leaf().generic_string();
		auto path = std::filesystem::current_path();
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(path)) {
		     id = dirEntry.path().generic_string();
		}
	}

	if(name.empty()) {
		name = id;
	}

	/* ************************************** *
	 * add empty public variant "default"     *
	 * if there is no public variant present. *
	 * ************************************** */
	bool hasPublicVariant = false;
	for(auto& variant : variants) {
		if(variant->isPublic()) {
			hasPublicVariant = true;
			break;
		}
	}
	if(hasPublicVariant == false) {
		Variant* variantPtr = new Variant("default", true);
		variants.push_back(std::unique_ptr<Variant>(variantPtr));
		variantByName.insert(std::make_pair("default", std::ref(*variantPtr)));
	}

	/* **************************************** *
	 * add default version number "0.1.0"       *
	 * if there is no version number available. *
	 * **************************************** */
	bool hasVersion = false;
	for(auto& variant : variants) {
		if(variant->getVersions().empty() == false) {
			hasVersion = true;
			break;
		}
	}
	if(hasVersion == false) {
		getVariant("").addVersion("0.1.0");
	}
}

Variant& Build::getVariant(const std::string& variantName) {
	auto iter = variantByName.find(variantName);
	if(iter == variantByName.end()) {
		throw std::runtime_error("cannot find variant \"" + variantName + "\"");
	}
	return iter->second.get();
}

} /* namespace build */
} /* namespace config */
} /* namespace transformer */
