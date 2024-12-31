#include <transformer/build/BuildManager.h>
#include <transformer/build/BuildManagerVariant.h>
#include <transformer/build/Builder.h>
#include <transformer/build/SolvedDescriptor.h>
#include <transformer/build/SolvedDescriptors.h>
#include <transformer/Execute.h>
#include <transformer/generate/CdtProject.h>

//#include <esl/logging/Logger.h>
#include <esl/system/Stacktrace.h>

#include <iostream>

namespace transformer {
namespace build {

Logger BuildManager::logger("transformer::build::BuildManager");

BuildManager::BuildManager(repository::Database& aDatabase, const model::Descriptor& aDescriptor, const std::set<std::string>& aOptionVariantNames, const std::set<std::string>& aOptionArchitectures)
: database(aDatabase),
  descriptor(aDescriptor),
  optionVariantNames(aOptionVariantNames),
  optionArchitectures(aOptionArchitectures),
  sources(aDescriptor)
{
	if(optionVariantNames.empty()) {
		if(descriptor.getDefaultVariantName().empty()) {
			optionVariantNames.emplace("");
		}
		for(const auto& variantName : descriptor.getSpecialVariantNames()) {
			optionVariantNames.emplace(variantName);
		}
	}

	if(optionArchitectures.empty()) {
		for(const auto& architecture : descriptor.getAllArchitectures()) {
			optionArchitectures.emplace(architecture);
		}
	}
}

BuildManager::~BuildManager() {
}

void BuildManager::addBuilderFactory(BuilderFactory& builderFactory) {
	builderFactories.emplace_back(builderFactory);
}

void BuildManager::initializeDependencies() {
	if(isInitializedDependencies) {
		return;
	}
	isInitializedDependencies = true;

	std::vector<std::string> variants;
#if 1
	for(const auto& variantName : optionVariantNames) {
		variants.push_back(variantName);
		buildManagerVariantByVariantName.emplace(variantName, std::unique_ptr<BuildManagerVariant>(new BuildManagerVariant(*this, variantName)));
	}
#else
	if(descriptor.getDefaultVariantName().empty()) {
		variants.push_back("");
		buildManagerVariantByVariantName.emplace("", std::unique_ptr<BuildManagerVariant>(new BuildManagerVariant(*this, "")));
	}
	for(const auto& variantName : descriptor.getSpecialVariantNames()) {
		variants.push_back(variantName);
		buildManagerVariantByVariantName.emplace(variantName, std::unique_ptr<BuildManagerVariant>(new BuildManagerVariant(*this, variantName)));
	}
#endif

#if 1
	for(const auto& architecture : optionArchitectures) {
#else
	for(const auto& architecture : descriptor.getAllArchitectures()) {
#endif
		bool builderNotFound = true;
		for(const auto& builderFactoryRef : builderFactories) {
			if(builderFactoryRef.get().hasArchitecture(architecture) == false) {
				continue;
			}

			builderNotFound = false;
			buildersByArchitecture[architecture] = builderFactoryRef.get().createBuilder(descriptor, architecture, sources, *this, variants);
			break;
		}
		if(builderNotFound) {
			std::cout << "WARNING: No builder found for architecture \"" << architecture << "\". Ignoring this architecture.\n";
		}
	}
}

void BuildManager::makeClean() {
	Execute::removeAll(descriptor.getBuildDirEffective());
}

int BuildManager::makeDependencies() {
	return onlyDependencies();
}

int BuildManager::makeGenerateSources() {
	int exitCode = makeDependencies();

	if(exitCode != 0) {
		return exitCode;
	}

	return onlyGenerateSources();
}

int BuildManager::makeCompile() {
	int exitCode = makeGenerateSources();

	if(exitCode != 0) {
		return exitCode;
	}

	return onlyCompile();
}

int BuildManager::makeTest() {
	int exitCode = makeCompile();

	if(exitCode != 0) {
		return exitCode;
	}

	return onlyTest();
}

int BuildManager::makeLink() {
	int exitCode = makeTest();

	if(exitCode != 0) {
		return exitCode;
	}

	return onlyLink();
}

int BuildManager::makeSite() {
	int exitCode = makeLink();

	if(exitCode != 0) {
		return exitCode;
	}

	return onlySite();
}

int BuildManager::makePackage() {
	int exitCode = makeSite();

	if(exitCode != 0) {
		return exitCode;
	}

	return onlyPackage();
}

int BuildManager::makeInstall() {
	int exitCode = makePackage();

	if(exitCode != 0) {
		return exitCode;
	}

	return onlyInstall();
}

int BuildManager::makeProvide() {
	int exitCode = makeInstall();

	if(exitCode != 0) {
		return exitCode;
	}

	return onlyProvide();
}

void BuildManager::generateCdtProject(const std::set<std::string>& variantNames, const std::string& architecture) {
	initializeDependencies();

	generate::CdtProject cdtProject(*this, architecture);
	cdtProject.generate(variantNames);
}

int BuildManager::onlyDependencies() {
	int exitCode = 0;

	initializeDependencies();
	for(const auto& architectureBuilder : buildersByArchitecture) {
		architectureBuilder.second->onlyDependencies();
	}

	return exitCode;
}

int BuildManager::onlyGenerateSources() {
	int exitCode = 0;

	initializeDependencies();
	for(const auto& architectureBuilder : buildersByArchitecture) {
		architectureBuilder.second->onlyGenerateSources();
	}

	return exitCode;
}

int BuildManager::onlyCompile() {
	int exitCode = 0;

	initializeDependencies();
	for(const auto& architectureBuilder : buildersByArchitecture) {
		architectureBuilder.second->onlyCompile();
	}

	return exitCode;
}

int BuildManager::onlyTest() {
	int exitCode = 0;

	initializeDependencies();
	for(const auto& architectureBuilder : buildersByArchitecture) {
		architectureBuilder.second->onlyTest();
	}

	return exitCode;
}

int BuildManager::onlyLink() {
	int exitCode = 0;

	initializeDependencies();
	for(const auto& architectureBuilder : buildersByArchitecture) {
		architectureBuilder.second->onlyLink();
	}

	return exitCode;
}

int BuildManager::onlySite() {
	int exitCode = 0;

	initializeDependencies();
	for(const auto& architectureBuilder : buildersByArchitecture) {
		architectureBuilder.second->onlySite();
	}

	return exitCode;
}

int BuildManager::onlyPackage() {
	int exitCode = 0;

	initializeDependencies();
	for(const auto& architectureBuilder : buildersByArchitecture) {
		architectureBuilder.second->onlyPackage();
	}

	return exitCode;
}

int BuildManager::onlyInstall() {
	int exitCode = 0;

	initializeDependencies();
	database.uploadArtefact(descriptor);

	return exitCode;
}

int BuildManager::onlyProvide() {
	int exitCode = 0;

	initializeDependencies();
	for(const auto& architectureBuilder : buildersByArchitecture) {
		architectureBuilder.second->onlyProvide();
	}

	return exitCode;
}

void BuildManager::addGeneratorDescriptor(const model::Generator& generator, const model::Descriptor& generatorDescriptor) {
	generatorDescriptorByGenerator.emplace(std::make_tuple(generator.id, generator.versionRange.getMaxVersionArtefact(), generator.versionRange.getMaxVersionApi(),  generator.variantName), generatorDescriptor);
}

const model::Descriptor& BuildManager::getGeneratorDescriptor(const model::Generator& generator) const {
	std::tuple<std::string, std::string, std::string, std::string> key = std::make_tuple(generator.id, generator.versionRange.getMaxVersionArtefact(), generator.versionRange.getMaxVersionApi(),  generator.variantName);

	auto iter = generatorDescriptorByGenerator.find(key);
	if(iter == std::end(generatorDescriptorByGenerator)) {
        throw esl::system::Stacktrace::add(std::runtime_error("ERROR: cannot find generatorDescriptor by artefactId=\"" + generator.id + "\", maxVersionArtefact=\"" + generator.versionRange.getMaxVersionArtefact() + "\", maxVersionApi=\"" + generator.versionRange.getMaxVersionApi() + "\", variantName=\"" + generator.variantName + "\"."));
	}

	return iter->second;
}

const SolvedDescriptors& BuildManager::getSolvedDescriptors(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName) const {
	std::tuple<std::string, std::string, std::string> key = std::make_tuple(artefactId, artefactVersion, artefactVersion);

	auto iter = solvedDescriptorsByArtefactIdVersionVariant.find(key);
	if(iter == std::end(solvedDescriptorsByArtefactIdVersionVariant)) {

		std::cout << "available solved descriptors for this coordinates:\n";
		for(const auto& entry : solvedDescriptorsByArtefactIdVersionVariant) {
			std::cout << " id=\"" + std::get<0>(entry.first) + "\", version=\"" + std::get<1>(entry.first) + "\", variantName=\"" + std::get<2>(entry.first) + "\".\n";
		}
        throw esl::system::Stacktrace::add(std::runtime_error("ERROR: cannot find solved descriptors caused by artefact with id=\"" + artefactId + "\", version=\"" + artefactVersion + "\", variantName=\"" + variantName + "\"."));
	}

	return iter->second;
}

bool BuildManager::addSolvedDescriptors(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const SolvedDescriptors& solvedDescriptors) {
	std::tuple<std::string, std::string, std::string> key = std::make_tuple(artefactId, artefactVersion, artefactVersion);

	return solvedDescriptorsByArtefactIdVersionVariant.emplace(key, solvedDescriptors).second;
}

SolvedStatus& BuildManager::getSolvedStatus(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName) {
	std::tuple<std::string, std::string, std::string> key = std::make_tuple(artefactId, artefactVersion, artefactVersion);
	return solvedStatusByArtefactIdVersionVariant[key];
}

repository::Database& BuildManager::getDatabase() const {
	return database;
}

const model::Descriptor& BuildManager::getDescriptor() const {
	return descriptor;
}

const Sources& BuildManager::getSources() const {
	return sources;
}

} /* namespace build */
} /* namespace transformer */
