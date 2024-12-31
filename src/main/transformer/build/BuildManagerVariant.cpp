#include <transformer/build/BuildManagerVariant.h>
#include <transformer/build/BuildManager.h>
#include <transformer/build/Solver.h>
#include <transformer/repository/DescriptorWriter.h>
#include <transformer/model/VersionNumber.h>
#include <transformer/Execute.h>

#include <esl/system/Stacktrace.h>

#include <filesystem>
#include <fstream>
#include <tuple>

namespace transformer {
namespace build {

namespace {

/* returns true if already valid */
bool checkPathAndMakeCleanIfInvalid(const std::filesystem::path& path, const std::string& optionalFile = "") {
	if(!std::filesystem::exists(path)) {
		return false;
	}

	if(!std::filesystem::is_directory(path)) {
		Execute::removeAll(path);
		return false;
	}

	if(!optionalFile.empty()) {
		std::filesystem::path pathFile = path;
		pathFile /= optionalFile;
		if(!std::filesystem::exists(pathFile)) {
			Execute::removeAll(path);
			return false;
		}
	}

	return true;
}

bool createPathAndMakeCleanIfInvalid(const std::filesystem::path& path, const std::string& optionalFile = "") {
	if(checkPathAndMakeCleanIfInvalid(path, optionalFile) == false) {
		Execute::mkdir(path);
		return false;
	}
	return true;
}

} /* anonymous namespace */

BuildManagerVariant::BuildManagerVariant(BuildManager& aBuildManager, std::string aVariantName)
: buildManager(aBuildManager),
  variantName(std::move(aVariantName)),
  database(buildManager.getDatabase()),
  sources(buildManager.getSources()),
  variant(buildManager.getDescriptor().getVariant(variantName)),
  architectures(variant.getArchitecturesEffective())
{
	/* ******************************************************** *
	 * check if have to build an executable (or just libraries) *
	 * ******************************************************** */
	bool binaryArtefactsNeeded = false;

	if(variant.isProvidedExecutableEffective()) {
		binaryArtefactsNeeded = true;
	}
	else {
		for(const auto& source : sources.getSourcesSourceTest(variantName)) {
			if(!source.get().getFileEntries().makeFlat().filter(variant.getSourceFilePatternsEffective(), {}).getList().empty()) {
				binaryArtefactsNeeded = true;
				break;
			}
		}
	}


	/* ****************** *
	 * install generators *
	 * ****************** */
	std::map<std::string, model::Generator> generators = variant.getGeneratorsEffective();
	for(const auto& generatorsEntry : generators) {
		// download generators with dependencies - maybe we need to build generator or dependency
		loadGenerator(generatorsEntry.second);
	}


	/* ****************** *
	 * solve dependencies *
	 * ****************** */
	SolvedDescriptors solvedDescriptors = Solver(database, variant).solve();
	buildManager.addSolvedDescriptors(buildManager.getDescriptor().getArtefactId(), buildManager.getDescriptor().getArtefactVersion(), variantName, solvedDescriptors);

	/* *********************** *
	 * load solve dependencies *
	 * *********************** */
	for(const auto& solvedDescriptor : solvedDescriptors.getElements()) {
#if 1
		SolvedStatus& solvedStatus = buildManager.getSolvedStatus(solvedDescriptor.getArtefactId(), solvedDescriptor.getArtefactVersion(), solvedDescriptor.getVariantName());
		if(binaryArtefactsNeeded) {
			loadDependency(solvedDescriptor.getVariant(), solvedDescriptor.getVariantName(), solvedStatus, solvedDescriptor.hasStatic, solvedDescriptor.hasDynamic);
		}
		else {
			loadDependency(solvedDescriptor.getVariant(), solvedDescriptor.getVariantName(), solvedStatus, false, false);
		}
#else
		loadDependency(solvedDescriptor, binaryArtefactsNeeded);
#endif
	}
}

const SolvedDescriptors& BuildManagerVariant::getSolvedDescriptors(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName) const {
	return buildManager.getSolvedDescriptors(artefactId, artefactVersion, variantName);
}

void BuildManagerVariant::loadGenerator(const model::Generator& generator) {
	const model::Descriptor* generatorDescriptor = buildManager.getDatabase().getLatestDescriptor(generator.id, generator.versionRange, generator.variantName);

	if(generatorDescriptor == nullptr) {
		throw esl::system::Stacktrace::add(std::runtime_error("ERROR: No generator \"" + generator.id + "[" + generator.variantName + "]\" found for specified version range."));
	}

	const std::string generatorVariantName = generator.getEffectiveVariantName(*generatorDescriptor);
	const model::Variant& generatorVariant = generatorDescriptor->getVariant(generatorVariantName);

	if(!generatorVariant.isProvidedExecutableEffective()) {
		throw esl::system::Stacktrace::add(std::runtime_error("ERROR: executable is not provided by generator id=\"" + generatorDescriptor->getArtefactId() + "\", verison=\"" + generatorDescriptor->getArtefactVersion() + "\", variant=\"" + generatorVariantName + "\""));
	}

	SolvedStatus& solvedStatus = buildManager.getSolvedStatus(generatorDescriptor->getArtefactId(), generatorDescriptor->getArtefactVersion(), generatorVariantName);
	buildManager.addGeneratorDescriptor(generator, *generatorDescriptor);
	loadDependencyExecutable(generatorVariant, generatorVariantName, solvedStatus);

	bool architectureMissing = false;
	for(const auto& architecture : architectures) {
		// search if architecture is available for this generatorVariant (equal string, not compatible !)
		if(!generatorVariant.hasArchitecture(architecture)) {
			// architecture not found.
			architectureMissing = true;
			break;
		}
	}

	if(architectureMissing) {
		SolvedDescriptors generatorSolvedDescriptors = Solver(database, generatorVariant).solve();

		buildManager.addSolvedDescriptors(generatorDescriptor->getArtefactId(), generatorDescriptor->getArtefactVersion(), generatorVariantName, generatorSolvedDescriptors);
		for(const auto& solvedDescriptor : generatorSolvedDescriptors.getElements()) {
#if 1
			SolvedStatus& solvedStatus = buildManager.getSolvedStatus(solvedDescriptor.getArtefactId(), solvedDescriptor.getArtefactVersion(), solvedDescriptor.getVariantName());
			loadDependency(solvedDescriptor.getVariant(), solvedDescriptor.getVariantName(), solvedStatus, solvedDescriptor.hasStatic, solvedDescriptor.hasDynamic);
#else
			loadDependency(solvedDescriptor, true);
#endif
		}
	}
}

#if 1
void BuildManagerVariant::loadDependency(const model::Variant& variant, const std::string& variantName, SolvedStatus& solvedStatus, bool loadStaticLib, bool loadDynamicLib) {
	/* write descriptor file to build directory, just for human */
	loadDependencyDescriptor(variant.getDescriptor(), solvedStatus);

	if(variant.getDescriptor().getSystemPath()) {
		return;
	}

	/* install common headers */
	loadDependencyHeaders(variant, variantName, solvedStatus);

	/* install static lib if needed */
	if(loadStaticLib) {
		loadDependencyLibStatic(variant, variantName, solvedStatus);
	}

	/* install dynamic lib if needed */
	if(loadDynamicLib) {
		loadDependencyLibDynamic(variant, variantName, solvedStatus);
	}
}

#else

void BuildManagerVariant::loadDependency(const SolvedDescriptor& solvedDescriptor, bool isBinaryArtefactNeeded) {
	SolvedStatus& solvedStatus = buildManager.getSolvedStatus(solvedDescriptor.getArtefactId(), solvedDescriptor.getArtefactVersion(), solvedDescriptor.getVariantName());

	/* write descriptor file to build directory, just for human */
	loadDependencyDescriptor(solvedDescriptor.getDescriptor(), solvedStatus);

	/* install common headers */
	loadDependencyHeaders(solvedDescriptor.getVariant(), solvedDescriptor.getVariantName(), solvedStatus);

	bool architectureMissing = false;
	for(const auto& architecture : architectures) {
		// search for our architecture (equal string, not compatible) if there is an static- or dynamic-lib available
		if(!solvedDescriptor.getVariant().hasArchitecture(architecture)) {
			architectureMissing = true;
			break;
		}
	}
	if(architectureMissing) {
		loadDependencyGenerators(solvedDescriptor.getVariant(), solvedStatus);
	}

	if(isBinaryArtefactNeeded) {
		if(solvedDescriptor.getDescriptor().getSystemPath()) {
			return;
		}

		if(solvedDescriptor.hasStatic) {
			loadDependencyLibStatic(solvedDescriptor.getVariant(), solvedDescriptor.getVariantName(), solvedStatus);
		}
		if(solvedDescriptor.hasDynamic) {
			loadDependencyLibDynamic(solvedDescriptor.getVariant(), solvedDescriptor.getVariantName(), solvedStatus);
		}
//		loadDependencyExecutable(solvedDescriptor, solvedStatus);
	}
}
#endif

void BuildManagerVariant::loadDependencyDescriptor(const model::Descriptor& descriptor, SolvedStatus& solvedStatus) {
	if(solvedStatus.descriptorLoaded) {
		return;
	}
	solvedStatus.descriptorLoaded = true;

	/* write descriptor file to build directory, just for human */
	std::filesystem::path pathDescriptor = sources.getPathBuild(descriptor.getArtefactId(), descriptor.getArtefactVersion());
	Execute::mkdir(pathDescriptor);
	pathDescriptor /= "tbuild.cfg";
	Execute::remove(pathDescriptor);
	std::ofstream ostream(pathDescriptor.string());
	repository::DescriptorWriter descriptorWriter(descriptor);
	descriptorWriter.write(ostream);
	ostream.close();
}

void BuildManagerVariant::loadDependencyHeaders(const model::Variant& variant, const std::string& variantName, SolvedStatus& solvedStatus) {
	if(solvedStatus.headersLoaded) {
		return;
	}
	solvedStatus.headersLoaded = true;

	if(variant.getDescriptor().getSystemPath()) {
		return;
	}

	/* if headers are needed, then we need generated-sources (headers) as well) */
	loadDependencyGeneratedSources(variant, variantName, solvedStatus);

	/* install common headers */
	std::filesystem::path pathHeaders = sources.getPathBuildHeaders(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName);
	if(!createPathAndMakeCleanIfInvalid(pathHeaders)) {
		database.copyArtefactHeaders(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, pathHeaders);
	}
}

void BuildManagerVariant::loadDependencySources(const model::Variant& variant, const std::string& variantName, SolvedStatus& solvedStatus) {
	if(solvedStatus.sourcesLoaded) {
		return;
	}
	solvedStatus.sourcesLoaded = true;

	if(!variant.isProvidedSourceEffective()) {
		throw esl::system::Stacktrace::add(std::runtime_error("ERROR: sources are not provided by artefact id=\"" + variant.getDescriptor().getArtefactId() + "\", verison=\"" + variant.getDescriptor().getArtefactVersion() + "\", variant=\"" + variantName + "\""));
	}

	/* if sources are needed, then we need headers and generators as well */
	loadDependencyHeaders(variant, variantName, solvedStatus);
	loadDependencyGenerators(variant, solvedStatus);

	// architecture not found, so copy the sources.
	const files::Source& source = sources.getSourceBuildSource(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName);
	std::filesystem::path path = source.getPath();
	if(!createPathAndMakeCleanIfInvalid(path)) {
		database.copyArtefactSource(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, path);
		source.invalidateFileEntries();
	}
}

void BuildManagerVariant::loadDependencyGeneratedSources(const model::Variant& variant, const std::string& variantName, SolvedStatus& solvedStatus) {
	if(solvedStatus.generatedLoaded) {
		return;
	}
	solvedStatus.generatedLoaded = true;

	if(variant.getDescriptor().getSystemPath()) {
		return;
	}

	/* install architecture specific generated sources (if available) */
	for(const auto& architecture : architectures) {
		// search if generated-sources are available for this architecture (equal string, not compatible !)
		if(variant.hasArchitecture(architecture)) {
			// architecture found, so copy generated-sources library.
			std::map<std::string, model::Generator> generators = variant.getGeneratorsEffective();
			for(const auto& generatorsEntry : generators) {
				const model::Generator& generator = generatorsEntry.second;
				const files::Source& sourceGeneratedSource = sources.getSourceBuildGenerated(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, architecture, generator.id);
				std::filesystem::path pathGeneratedSource = sourceGeneratedSource.getPath();
				if(!createPathAndMakeCleanIfInvalid(pathGeneratedSource)) {
					database.copyArtefactGenerated(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, architecture, generator.id, pathGeneratedSource);
				}
			}
		}
		else {
			// architecture not found, so install generators.
			loadDependencyGenerators(variant, solvedStatus);
		}
	}
}

void BuildManagerVariant::loadDependencyGenerators(const model::Variant& variant, SolvedStatus& solvedStatus) {
	if(solvedStatus.generatorsLoaded) {
		return;
	}
	solvedStatus.generatorsLoaded = true;

	/* ****************** *
	 * install generators *
	 * ****************** */
	std::map<std::string, model::Generator> generators = variant.getGeneratorsEffective();
	for(const auto& generatorsEntry : generators) {
		// download generators with dependencies - maybe we need to build generator or dependency
		loadGenerator(generatorsEntry.second);
	}
}

void BuildManagerVariant::loadDependencyLibStatic(const model::Variant& variant, const std::string& variantName, SolvedStatus& solvedStatus) {
	if(solvedStatus.staticLibLoaded) {
		return;
	}
	solvedStatus.staticLibLoaded = true;

	if(!variant.isProvidedStaticEffective()) {
		throw esl::system::Stacktrace::add(std::runtime_error("ERROR: static-lib is not provided by artefact id=\"" + variant.getDescriptor().getArtefactId() + "\", verison=\"" + variant.getDescriptor().getArtefactVersion() + "\", variant=\"" + variantName + "\""));
	}

	for(const auto& architecture : architectures) {
		// search if static-lib is available for this architecture (equal string, not compatible !)
		if(variant.hasArchitecture(architecture)) {
			// architecture found, so copy the pre-compiled library.
			const files::Source& source = sources.getSourceBuildLinkStatic(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, architecture);
			std::filesystem::path path = source.getPath();
			if(!createPathAndMakeCleanIfInvalid(path)) {
				database.copyArtefactStaticLib(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, architecture, path);
				source.invalidateFileEntries();
			}
		}
		else {
			// architecture not found, so copy the sources.
			loadDependencySources(variant, variantName, solvedStatus);
		}
	}
}

void BuildManagerVariant::loadDependencyLibDynamic(const model::Variant& variant, const std::string& variantName, SolvedStatus& solvedStatus) {
	if(solvedStatus.dynamicLibLoaded) {
		return;
	}
	solvedStatus.dynamicLibLoaded = true;

	if(!variant.isProvidedDynamicEffective()) {
		throw esl::system::Stacktrace::add(std::runtime_error("ERROR: dynamic-lib is not provided by artefact id=\"" + variant.getDescriptor().getArtefactId() + "\", verison=\"" + variant.getDescriptor().getArtefactVersion() + "\", variant=\"" + variantName + "\""));
	}

	for(const auto& architecture : architectures) {
		// search if dynamic-lib is available for this architecture (equal string, not compatible !)
		if(variant.hasArchitecture(architecture)) {
			// architecture found, so copy the pre-compiled library.
			const files::Source& source = sources.getSourceBuildLinkDynamic(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, architecture);
			std::filesystem::path path = source.getPath();
			if(!createPathAndMakeCleanIfInvalid(path)) {
				database.copyArtefactDynamicLib(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, architecture, path);
				source.invalidateFileEntries();
			}
		}
		else {
			// architecture not found, so copy the sources.
			loadDependencySources(variant, variantName, solvedStatus);
		}
	}
}

void BuildManagerVariant::loadDependencyExecutable(const model::Variant& variant, const std::string& variantName, SolvedStatus& solvedStatus) {
	if(solvedStatus.executableLoaded) {
		return;
	}
	solvedStatus.executableLoaded = true;

	if(!variant.isProvidedExecutableEffective()) {
		throw esl::system::Stacktrace::add(std::runtime_error("ERROR: executable is not provided by artefact id=\"" + variant.getDescriptor().getArtefactId() + "\", verison=\"" + variant.getDescriptor().getArtefactVersion() + "\", variant=\"" + variantName + "\""));
	}

	for(const auto& architecture : architectures) {
		// search if executable is available for this architecture (equal string, not compatible !)
		if(variant.hasArchitecture(architecture)) {
			// architecture found, so copy the executable.
			const files::Source& source = sources.getSourceBuildLinkExecutable(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, architecture);
			std::filesystem::path path = source.getPath();
			if(!createPathAndMakeCleanIfInvalid(path)) {
				database.copyArtefactExecutable(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, architecture, path);
				source.invalidateFileEntries();
			}
		}
		else {
			// architecture not found, so copy the sources.
			loadDependencySources(variant, variantName, solvedStatus);
		}
	}
}

} /* namespace build */
} /* namespace transformer */
