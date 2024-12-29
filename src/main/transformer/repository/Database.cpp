#include <transformer/build/Sources.h>
#include <transformer/Execute.h>
#include <transformer/repository/Database.h>
#include <transformer/repository/DescriptorReader.h>

#include <esl/system/Stacktrace.h>

#include <iostream>

namespace transformer {
namespace repository {

Database::Database()
: repositorySystem("linux-gcc")
{
	addRepository("local", repositoryLocal);
	addRepository("system", repositorySystem);
}

Database::~Database() {
}

void Database::setLocalRepositoryPath(const std::filesystem::path path) {
	repositoryLocal.setPath(path);
}

void Database::addRepository(const std::string& name, Repository& repository) {
	if(repositoriesByName.find(name) != std::end(repositoriesByName)) {
		throw esl::system::Stacktrace::add(std::runtime_error("repository \"" + name + "\" exists already"));
	}
	repositoriesByName.emplace(name, std::ref(repository));
	repositories.emplace_back(name, repository);
}

std::unique_ptr<model::Descriptor> Database::loadDescriptor(const std::filesystem::path path) {
	std::unique_ptr<model::Descriptor> descriptor(new model::Descriptor);
	DescriptorReader descriptorReader(*descriptor.get());
	descriptorReader.read(path);
	return descriptor;
}

const model::Descriptor* Database::getDescriptor(const std::string& artefactId, const std::string& artefactVersion) const {
	// check if descriptor is already loaded
	{
		auto iter = descriptorsByArtefactVersion.find(std::make_pair(artefactId, artefactVersion));
		if(iter != std::end(descriptorsByArtefactVersion)) {
			return iter->second.get();
		}
	}

	Repository* repository = getRepositoryByArtefactVersion(artefactId, artefactVersion);
	if(repository == nullptr) {
		return nullptr;
	}

	std::unique_ptr<model::Descriptor> descriptor = repository->loadDescriptor(artefactId, artefactVersion);
	model::Descriptor* descriptorPtr = descriptor.get();
	if(!descriptor) {
		std::cerr << "This should never happen. Was not able to load descriptor from repository\n";
		return nullptr;
	}

	descriptorsByArtefactVersion[std::make_pair(artefactId, artefactVersion)] = std::move(descriptor);
	artefactVersionsByApiVersion[std::make_pair(artefactId, descriptorPtr->getApiVersion())].second.insert(artefactVersion);

	return descriptorPtr;
}

const model::Descriptor* Database::getLatestDescriptor(const std::string& artefactId, const model::VersionRange& versionRange, const std::string& variantName) const {
	std::set<std::string> versions = getArtefactVersions(artefactId, versionRange);
	std::set<model::VersionNumber> versionNumbers(std::begin(versions), std::end(versions));

	for(auto riter = versionNumbers.rbegin(); riter != versionNumbers.rend(); ++riter) {
		const model::Descriptor* descriptor = getDescriptor(artefactId, riter->getString());

		if(descriptor == nullptr) {
//	        esl::logger.error << "ERROR: Should never happen. Cannot load Descriptor." << std::endl;
//	        esl::logger.error(__func__, __FILE__, __LINE__) << "ERROR: Should never happen. Cannot load Descriptor.\n";
			throw esl::system::Stacktrace::add(std::runtime_error("ERROR: Should never happen. Cannot load Descriptor."));
		}

		// check if variantName is not available
		if(!variantName.empty() && !descriptor->hasSpecialVariant(variantName)) {
			continue;
		}

		return descriptor;
	}

	return nullptr;
}

std::set<std::string> Database::getArtefactVersions(const std::string& artefactId, const model::VersionRange& versionRange) const {
	std::set<std::string> result;
	std::set<std::string> artefactVersions;

	if(versionRange.hasApiVersion()) {
		std::set<std::string> apiVersions = getApiVersions(artefactId, versionRange.getMinVersionApi(), versionRange.getMaxVersionApi());

		for(const auto& apiVersion : apiVersions) {
			std::set<std::string> tmpArtefactVersions = getArtefactVersions(artefactId, apiVersion);
			for(const auto& artefactVersion : tmpArtefactVersions) {
				artefactVersions.insert(artefactVersion);
			}
		}
	}
	else {
		artefactVersions = getArtefactVersions(artefactId);
	}

	for(const auto& artefactVersion : artefactVersions) {
		if(!versionRange.hasArtefactVersion() || versionRange.hasArtefactVersion(artefactVersion)) {
			result.insert(artefactVersion);
		}
	}

	return result;
}

std::set<std::string> Database::getArtefactVersions(const std::string& artefactId, const model::VersionRanges& versionRanges) const {
	std::set<std::string> artefactVersions;

	if(versionRanges.getRanges().empty()) {
		artefactVersions = getArtefactVersions(artefactId);
	}
	else {
		for(const auto& versionRange : versionRanges.getRanges()) {
			std::set<std::string> tmpArtefactVersions = getArtefactVersions(artefactId, versionRange);
			for(const auto& artefactVersion : tmpArtefactVersions) {
				artefactVersions.insert(artefactVersion);
			}
		}
	}

	return artefactVersions;
}

std::set<std::string> Database::getArtefactVersions(const std::string& artefactId) const {
	std::set<std::string> result;

	std::map<std::string, std::pair<bool, std::set<std::string>>>& artefactVersionsByRepositoryName = repositoryNameArtefactVersionsByArtefactId[artefactId];

	for(const auto& repository : repositories) {
		std::pair<bool, std::set<std::string>>& artefactVersions = artefactVersionsByRepositoryName[repository.first];

		if(artefactVersions.first == false) {
			artefactVersions.first = true;
			artefactVersions.second = repository.second.loadArtefactVersions(artefactId);
		}

		for(const auto& artefactVersion : artefactVersions.second) {
			result.insert(artefactVersion);

			auto iter = repositoryByArtefactVersion.find(std::make_pair(artefactId, artefactVersion));
			if(iter == std::end(repositoryByArtefactVersion)) {
				repositoryByArtefactVersion.emplace(std::make_pair(artefactId, artefactVersion), repository.second);
			}
		}
	}

	return result;
}

std::set<std::string> Database::getArtefactVersions(const std::string& artefactId, const std::string& apiVersion) const {
	std::map<std::string, std::pair<bool, std::set<std::string>>>& artefactVersionsByRepositoryName = repositoryNameArtefactVersionsByArtefactId[artefactId];
	std::pair<bool, std::set<std::string>>& artefactVersions = artefactVersionsByApiVersion[std::make_pair(artefactId, apiVersion)];

	if(artefactVersions.first == false) {
		artefactVersions.first = true;
		for(const auto& repository : repositories) {
			std::set<std::string> loadedArtefactVersions = repository.second.loadArtefactVersions(artefactId, apiVersion);
			std::pair<bool, std::set<std::string>>& otherArtefactVersions = artefactVersionsByRepositoryName[repository.first];

			for(const auto& artefactVersion : loadedArtefactVersions) {
				artefactVersions.second.insert(artefactVersion);

				if(otherArtefactVersions.first == false) {
					otherArtefactVersions.second.insert(artefactVersion);
				}

				auto iter = repositoryByArtefactVersion.find(std::make_pair(artefactId, artefactVersion));
				if(iter == std::end(repositoryByArtefactVersion)) {
					repositoryByArtefactVersion.emplace(std::make_pair(artefactId, artefactVersion), repository.second);
				}
			}
		}
	}

	return artefactVersions.second;
}

std::set<std::string> Database::getApiVersions(const std::string& artefactId, const std::string& apiVersionMin, const std::string& apiVersionMax) const {
	std::set<std::string> result;
	model::VersionRange apiVersionRange;
	apiVersionRange.setApiVersion(apiVersionMin, apiVersionMax);

	std::map<std::string, std::pair<bool, std::set<std::string>>>& apiVersionsByRepository = repositoryNameApiVersionsByArtefactId[artefactId];

	for(const auto& repository : repositories) {
		std::pair<bool, std::set<std::string>>& apiVersions = apiVersionsByRepository[repository.first];

		if(apiVersions.first == false) {
			if(apiVersionMin != apiVersionMax) {
				apiVersions.first = true;
				apiVersions.second = repository.second.loadApiVersions(artefactId);
			}
			else if(apiVersions.second.find(apiVersionMax) == std::end(apiVersions.second) && repository.second.hasApiVersion(artefactId, apiVersionMax)) {
				apiVersions.second.insert(apiVersionMax);
			}
		}

		for(const auto& apiVersion : apiVersions.second) {
			if(apiVersionRange.hasApiVersion(apiVersion)) {
				result.insert(apiVersion);
			}
		}
	}

	return result;
}

void Database::copyArtefactSource(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::filesystem::path& toPath) {
	makeLocalAvailable(artefactId, artefactVersion);
	repositoryLocal.copyArtefactSource(artefactId, artefactVersion, variant, toPath);
}

void Database::copyArtefactHeaders(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::filesystem::path& toPath) {
	makeLocalAvailable(artefactId, artefactVersion);
	repositoryLocal.copyArtefactHeaders(artefactId, artefactVersion, variant, toPath);
}

void Database::copyArtefactStaticLib(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const std::filesystem::path& toPath) {
	makeLocalAvailable(artefactId, artefactVersion);
	repositoryLocal.copyArtefactStaticLib(artefactId, artefactVersion, variant, architecture, toPath);
}

void Database::copyArtefactDynamicLib(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const std::filesystem::path& toPath) {
	makeLocalAvailable(artefactId, artefactVersion);
	repositoryLocal.copyArtefactDynamicLib(artefactId, artefactVersion, variant, architecture, toPath);
}

void Database::copyArtefactExecutable(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const std::filesystem::path& toPath) {
	makeLocalAvailable(artefactId, artefactVersion);
	repositoryLocal.copyArtefactExecutable(artefactId, artefactVersion, variant, architecture, toPath);
}

void Database::copyArtefactGenerated(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const std::string& generatorId, const std::filesystem::path& toPath) {
	makeLocalAvailable(artefactId, artefactVersion);
	repositoryLocal.copyArtefactGenerated(artefactId, artefactVersion, variant, architecture, generatorId, toPath);
}

//void Database::saveArtefact(const model::Descriptor& descriptor) const {
//}

Repository* Database::getRepositoryByArtefactVersion(const std::string& artefactId, const std::string& artefactVersion) const {
	// check if repository is known already for this version
	auto iter1 = repositoryByArtefactVersion.find(std::make_pair(artefactId, artefactVersion));
	if(iter1 != std::end(repositoryByArtefactVersion)) {
		return &iter1->second.get();
	}

	// check all repositories
	for(const auto& repository : repositories) {
		if(!repository.second.hasArtefactVersion(artefactId, artefactVersion)) {
			continue;
		}
		repositoryByArtefactVersion.emplace(std::make_pair(artefactId, artefactVersion), repository.second);
		repositoryNameArtefactVersionsByArtefactId[artefactId][repository.first].second.insert(artefactVersion);

		return &repository.second;
	}

	return nullptr;
}

void Database::uploadArtefact(const model::Descriptor& descriptor) const {
	build::Sources sources(descriptor);

	repositoryLocal.uploadDescriptor(descriptor);

	if(descriptor.getDefaultVariantName().empty()) {
		uploadArtefact(descriptor, "");
	}

	for(const auto& variantName : descriptor.getSpecialVariantNames()) {
		uploadArtefact(descriptor, variantName);
	}
}

void Database::makeLocalAvailable(const std::string& artefactId, const std::string& artefactVersion) {
	Repository* repository = getRepositoryByArtefactVersion(artefactId, artefactVersion);
	if(repository == nullptr) {
		throw esl::system::Stacktrace::add(std::runtime_error("cannot download artefact elements to local repository because cannot find source repository for artefact \"" + artefactId + "[" + artefactVersion + "]"));
	}

	if(repository == &repositoryLocal) {
		return;
	}

	const model::Descriptor* descriptor = getDescriptor(artefactId, artefactVersion);
	if(descriptor == nullptr) {
		throw esl::system::Stacktrace::add(std::runtime_error("cannot download artefact elements to local repository because cannot find descriptor for artefact \"" + artefactId + "[" + artefactVersion + "]"));
	}

	repositoryLocal.uploadDescriptor(*descriptor);

	std::vector<std::string> variantNames = descriptor->getSpecialVariantNames();
	if(descriptor->getDefaultVariantName().empty()) {
		variantNames.push_back("");
	}

	for(const auto& variantName : variantNames) {
		const model::Variant& variant = descriptor->getVariant(variantName);

		if(variant.isProvidedSourceEffective()) {
			repositoryLocal.saveArtefactSource(*repository, artefactId, artefactVersion, variantName);
		}

		if(variant.isProvidedDynamicEffective() || variant.isProvidedStaticEffective()) {
			repositoryLocal.saveArtefactHeaders(*repository, artefactId, artefactVersion, variantName);
		}

		for(const auto& architecture : variant.getArchitecturesEffective()) {
			if(variant.isProvidedDynamicEffective()) {
				repositoryLocal.saveArtefactDynamicLib(*repository, artefactId, artefactVersion, variantName, architecture);
			}
			if(variant.isProvidedStaticEffective()) {
				repositoryLocal.saveArtefactStaticLib(*repository, artefactId, artefactVersion, variantName, architecture);
			}
			if(variant.isProvidedExecutableEffective()) {
				repositoryLocal.saveArtefactExecutable(*repository, artefactId, artefactVersion, variantName, architecture);
			}
		}
	}

	auto iter = repositoryByArtefactVersion.find(std::make_pair(artefactId, artefactVersion));
	if(iter == std::end(repositoryByArtefactVersion)) {
		repositoryByArtefactVersion.emplace(std::make_pair(artefactId, artefactVersion), repositoryLocal);
	}
	else {
		iter->second = repositoryLocal;
	}
	repositoryNameArtefactVersionsByArtefactId[artefactId][""].second.insert(artefactVersion);
	repositoryNameApiVersionsByArtefactId[artefactId][""].second.insert(descriptor->getApiVersion());
}

void Database::uploadArtefact(const model::Descriptor& descriptor, const std::string& variantName) const {
	build::Sources sources(descriptor);
	const model::Variant& variant = descriptor.getVariant(variantName);

	uploadArtefactHeaders(descriptor, variantName);

	if(variant.isProvidedSourceEffective()) {
		uploadArtefactSource(descriptor, variantName);
	}

	for(const auto& architecture : variant.getArchitecturesEffective()) {
		std::map<std::string, model::Generator> generators = variant.getGeneratorsEffective();
		for(const auto& generatorsEntry : generators) {
			repositoryLocal.uploadArtefactGenerated(sources.getPathBuildGenerated(descriptor.getArtefactId(), descriptor.getArtefactVersion(), variantName, architecture, generatorsEntry.second.id), descriptor, variantName, architecture, generatorsEntry.second.id);
		}

		if(variant.isProvidedStaticEffective()) {
			repositoryLocal.uploadArtefactStaticLib(sources.getPathBuildLinkStatic(descriptor.getArtefactId(), descriptor.getArtefactVersion(), variantName, architecture), descriptor, variantName, architecture);
		}

		if(variant.isProvidedDynamicEffective()) {
			repositoryLocal.uploadArtefactDynamicLib(sources.getPathBuildLinkDynamic(descriptor.getArtefactId(), descriptor.getArtefactVersion(), variantName, architecture), descriptor, variantName, architecture);
		}

		if(variant.isProvidedExecutableEffective()) {
			repositoryLocal.uploadArtefactExecutable(sources.getPathBuildLinkExecutable(descriptor.getArtefactId(), descriptor.getArtefactVersion(), variantName, architecture), descriptor, variantName, architecture);
		}
	}
}

void Database::uploadArtefactSource(const model::Descriptor& descriptor, const std::string& variantName) const {
	const std::filesystem::path tmpPath = createTmpDirectory(descriptor);
	files::Source sourceTo(tmpPath);
	build::Sources sourcesFrom(descriptor);
	std::vector<std::string> filePatterns = descriptor.getVariant(variantName).getSourceFilePatternsEffective();

	for(const auto& sourceFrom : sourcesFrom.getSourcesSourceMain(variantName)) {
		const files::FileEntries& entriesFrom = sourceFrom.get().getFileEntries();
		const files::FileEntries& entriesTo = sourceTo.getFileEntries();
		files::FileEntries entriesFromFiltered = entriesFrom.makeFlat().filter([](const files::FileEntry& entry){
	        return !entry.isDirectory;
		}).filter(filePatterns, {});

		files::FileEntries::syncDirectoryStructure(entriesFrom, entriesTo, sourceTo.getPath(), false);
		for(const auto& entry : entriesFromFiltered.getList()) {
			std::filesystem::path fromPath = sourceFrom.get().getPath();
			fromPath /= entry.name;

			std::filesystem::path toPath = sourceTo.getPath();
			toPath /= entry.name;
			Execute::copyFile(fromPath, toPath);
		}
	}

	repositoryLocal.uploadArtefactSource(tmpPath, descriptor, variantName);

	Execute::removeAll(tmpPath);
}

void Database::uploadArtefactHeaders(const model::Descriptor& descriptor, const std::string& variantName) const {
	const std::filesystem::path tmpPath = createTmpDirectory(descriptor);
	files::Source sourceTo(tmpPath);
	build::Sources sourcesFrom(descriptor);
	std::vector<std::string> filePatterns = descriptor.getVariant(variantName).getHeaderFilePatternsEffective();

	for(const auto& sourceFrom : sourcesFrom.getSourcesHeader(variantName)) {
		const files::FileEntries& entriesFrom = sourceFrom.get().getFileEntries();
		const files::FileEntries& entriesTo = sourceTo.getFileEntries();
		files::FileEntries entriesFromFiltered = entriesFrom.makeFlat().filter([](const files::FileEntry& entry){
	        return !entry.isDirectory;
		}).filter(filePatterns, {});

		files::FileEntries::syncDirectoryStructure(entriesFrom, entriesTo, sourceTo.getPath(), false);
		for(const auto& entry : entriesFromFiltered.getList()) {
			std::filesystem::path fromPath = sourceFrom.get().getPath();
			fromPath /= entry.name;

			std::filesystem::path toPath = sourceTo.getPath();
			toPath /= entry.name;
			Execute::copyFile(fromPath, toPath);
		}

	}

	repositoryLocal.uploadArtefactHeaders(tmpPath, descriptor, variantName);

	Execute::removeAll(tmpPath);
}

std::filesystem::path Database::createTmpDirectory(const model::Descriptor& descriptor) const {
	std::filesystem::path path = descriptor.getBuildDirEffective();
	path /= "tmp";

	if(std::filesystem::exists(path)) {
		Execute::removeAll(path);
	}
	Execute::mkdir(path);

	return path;
}

} /* namespace repository */
} /* namespace transformer */
