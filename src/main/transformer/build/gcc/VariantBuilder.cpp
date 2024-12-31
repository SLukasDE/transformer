#include <transformer/build/gcc/VariantBuilder.h>
#include <transformer/build/gcc/Builder.h>
#include <transformer/build/gcc/BuildCompile.h>
#include <transformer/build/gcc/BuildLinkStatic.h>
#include <transformer/build/gcc/BuildLinkDynamic.h>
#include <transformer/build/gcc/BuildLinkExecutable.h>
#include <transformer/build/Sources.h>
#include <transformer/model/Generator.h>
#include <transformer/Execute.h>

#include <esl/system/Stacktrace.h>

#include <filesystem>
#include <map>

namespace transformer {
namespace build {
namespace gcc {

VariantBuilder::VariantBuilder(const Builder& aBuilder, std::string aVariantName)
: builder(aBuilder),
  variantName(std::move(aVariantName))
{
}

VariantBuilder::~VariantBuilder() {
}

int VariantBuilder::onlyDependencies() {
	const SolvedDescriptors& solvedDescriptors = getSolvedDescriptors(getDescriptor().getArtefactId(), getDescriptor().getArtefactVersion(), getVariantName());
	for(const auto& solvedDescriptor : solvedDescriptors.getElements()) {
		int exitCode = onlyDependency(solvedDescriptor.getVariant(), solvedDescriptor.getVariantName(), solvedDescriptors);
		if(exitCode != 0) {
			return exitCode;
		}
	}

	return 0;
}

int VariantBuilder::onlyGenerateSources() {
	return onlyGenerateSources(getDescriptor().getVariant(getVariantName()), getVariantName());
}

int VariantBuilder::onlyCompile() {
	int exitCode = 0;
	const model::Variant& variant = getDescriptor().getVariant(getVariantName());
	const SolvedDescriptors& solvedDescriptors = getSolvedDescriptors(getDescriptor().getArtefactId(), getDescriptor().getArtefactVersion(), getVariantName());

	const files::Source& sourceCompileMain = builder.getSources().getSourceBuildCompile(getDescriptor().getArtefactId(), getDescriptor().getArtefactVersion(), getVariantName(), builder.getArchitecture());
	const files::Source& sourceCompileTest = builder.getSources().getSourceBuildCompileTest(getDescriptor().getArtefactId(), getDescriptor().getArtefactVersion(), getVariantName(), builder.getArchitecture());
	BuildCompile buildCompileMain(builder, sourceCompileMain, false);
	BuildCompile buildCompileTest(builder, sourceCompileTest, true);

	/* compile main sources */
	addCompileSourcesMain(buildCompileMain, variant, getVariantName(), solvedDescriptors);

	exitCode = buildCompileMain.run();
	sourceCompileMain.invalidateFileEntries();
	if(exitCode != 0) {
		return exitCode;
	}

	/* check if test sources are empty, so we are done */
	{
		bool sourcesTestEmpty = true;
		std::vector<std::reference_wrapper<const files::Source>> sourcesTest = getSourcesSourceTest();
		for(const auto& sourceTest : sourcesTest) {
			const std::set<std::string>& includeFilePatterns = buildCompileTest.getIncludeFilePatterns(sourceTest);
			const std::set<std::string>& excludeFilePatterns = buildCompileTest.getExcludeFilePatterns(sourceTest);

			if(sourceTest.get().getFileEntries().makeFlat().filter(includeFilePatterns, excludeFilePatterns).getList().empty()) {
				sourcesTestEmpty = false;
				break;
			}
		}
		if(sourcesTestEmpty) {
			return exitCode;
		}
	}

	/* compile test sources */
	addCompileSourcesTest(buildCompileTest, variant, getVariantName(), solvedDescriptors);
	exitCode = buildCompileTest.run();
	sourceCompileTest.invalidateFileEntries();

	return exitCode;
}

int VariantBuilder::onlyTest() {
	const SolvedDescriptors& solvedDescriptors = getSolvedDescriptors(getDescriptor().getArtefactId(), getDescriptor().getArtefactVersion(), getVariantName());
	return onlyLinkExecutable(getDescriptor().getVariant(getVariantName()), getVariantName(), solvedDescriptors, true);
}

int VariantBuilder::onlyLink() {
	int exitCode = 0;
	const SolvedDescriptors& solvedDescriptors = getSolvedDescriptors(getDescriptor().getArtefactId(), getDescriptor().getArtefactVersion(), getVariantName());
	const model::Variant& variant = getDescriptor().getVariant(getVariantName());

	/* link main sources to static library */
	if(getDescriptor().getVariant(getVariantName()).isProvidedStaticEffective()) {
		exitCode = onlyLinkStatic(variant, getVariantName(), solvedDescriptors);
		if(exitCode != 0) {
			return exitCode;
		}
	}

	/* link main sources to dynamic library */
	if(getDescriptor().getVariant(getVariantName()).isProvidedDynamicEffective()) {
		exitCode = onlyLinkDynamic(variant, getVariantName(), solvedDescriptors);
		if(exitCode != 0) {
			return exitCode;
		}
	}

	/* link main sources to executable */
	const files::Source& sourceCompile = builder.getSources().getSourceBuildCompile(getDescriptor().getArtefactId(), getDescriptor().getArtefactVersion(), getVariantName(), builder.getArchitecture());
	if(!sourceCompile.getFileEntries().makeFlat().filter([](const files::FileEntry& entry) {
		return (entry.name.generic_string() == "main.o");
	}).getList().empty()) {
		exitCode = onlyLinkExecutable(variant, getVariantName(), solvedDescriptors);
		if(exitCode != 0) {
			return exitCode;
		}
	}

	return exitCode;
}

int VariantBuilder::onlySite() {
	return 0;
}

int VariantBuilder::onlyPackage() {
	return 0;
}

int VariantBuilder::onlyProvide() {
	return 0;
}







const BuildManager& VariantBuilder::getBuildManager() const {
	return builder.getBuildManager();
}

const model::Descriptor& VariantBuilder::getDescriptor() const {
	return builder.getDescriptor();
}

const std::string& VariantBuilder::getVariantName() const {
	return variantName;
}

const std::string& VariantBuilder::getArchitecture() const {
	return builder.getArchitecture();
}

int VariantBuilder::onlyDependency(const model::Variant& variant, const std::string& variantName, const SolvedDescriptors& solvedDescriptors, bool buildExecutable) {
	const files::Source& sourceLinkStatic = builder.getSources().getSourceBuildLinkStatic(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, builder.getArchitecture());
	const files::Source& sourceLinkDynamic = builder.getSources().getSourceBuildLinkDynamic(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, builder.getArchitecture());
	const files::Source& sourceLinkExecutable = builder.getSources().getSourceBuildLinkExecutable(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, builder.getArchitecture());

	/* check if everything is already provided that is provided by this dependency. Then we are done ! */
	if((!variant.isProvidedStaticEffective() || !sourceLinkStatic.getFileEntries().getList().empty())
	&& (!variant.isProvidedDynamicEffective() || !sourceLinkDynamic.getFileEntries().getList().empty())
	&& (!variant.isProvidedExecutableEffective() || !sourceLinkExecutable.getFileEntries().getList().empty() || !buildExecutable)) {
		return 0;
	}

	/* check if binaries and generated sources are provided by this dependency for this architecture. If not, then we have to build it! */
/*
	bool architectureMissing = true;
	for(const auto& architectureFound : variant.getArchitecturesEffective()) {
		if(getArchitecture() == architectureFound) {
			architectureMissing = false;
			break;
		}
	}
*/
	/* check if sources have been compiled already. If not then we have to generate sources and compile all together */
	const files::Source& sourceCompile = builder.getSources().getSourceBuildCompile(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, builder.getArchitecture());
	if(sourceCompile.getFileEntries().getList().empty()) {
		// install generators and generate sources
		int exitCode = onlyGenerateSources(variant, variantName);
		if(exitCode != 0) {
			return exitCode;
		}

		BuildCompile buildCompile(builder, sourceCompile, false);
		addCompileSources(buildCompile, variant, variantName, solvedDescriptors);

		exitCode = buildCompile.run();
		if(exitCode != 0) {
			// break;
			return exitCode;
		}
	}

	if(variant.isProvidedStaticEffective() && sourceLinkStatic.getFileEntries().getList().empty()) {
		int exitCode = onlyLinkStatic(variant, variantName, solvedDescriptors);
		if(exitCode != 0) {
			// break;
			return exitCode;
		}
	}

	if(variant.isProvidedDynamicEffective() && sourceLinkDynamic.getFileEntries().getList().empty()) {
		int exitCode = onlyLinkDynamic(variant, variantName, solvedDescriptors);
		if(exitCode != 0) {
			return exitCode;
		}
	}

	if(buildExecutable && variant.isProvidedExecutableEffective() && sourceLinkExecutable.getFileEntries().getList().empty()) {
		int exitCode = onlyLinkExecutable(variant, variantName, solvedDescriptors);
		if(exitCode != 0) {
			return exitCode;
		}
	}

	return 0;
}


int VariantBuilder::onlyGenerateSources(const model::Variant& variant, const std::string& variantName) {
	std::map<std::string, model::Generator> generators = variant.getGeneratorsEffective();
	for(const auto& generatorsEntry : generators) {
		const model::Generator& generator = generatorsEntry.second;
		const model::Descriptor& generatorDescriptor = builder.getGeneratorDescriptor(generator);
		const std::string generatorVariantName = generator.getEffectiveVariantName(generatorDescriptor);
		const model::Variant& generatorVariant = generatorDescriptor.getVariant(generatorVariantName);

		bool architectureMissing = true;
		for(const auto& architectureFound : generatorVariant.getArchitecturesEffective()) {
			if(getArchitecture() == architectureFound) {
				architectureMissing = false;
				break;
			}
		}
		if(architectureMissing) {
			const model::Variant& generatorVariant = generatorDescriptor.getVariant(generatorVariantName);
			const SolvedDescriptors& generatorSolvedDescriptors = getBuildManager().getSolvedDescriptors(generatorDescriptor.getArtefactId(), generatorDescriptor.getArtefactVersion(), generatorVariantName);

			// install generator
			int exitCode = onlyDependency(generatorVariant, generatorVariantName, generatorSolvedDescriptors, true);
			if(exitCode != 0) {
				return exitCode;
			}
		}


		std::filesystem::path pathGenerator = builder.getSources().getPathBuildLinkExecutable(generatorDescriptor.getArtefactId(), generatorDescriptor.getArtefactVersion(), generatorVariantName, getArchitecture());
		pathGenerator /= generatorDescriptor.getArtefactName();

		const files::Source& sourceGenerated = builder.getSources().getSourceBuildGenerated(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, getArchitecture(), generator.id);

		std::string command = pathGenerator.string() + " -o " + sourceGenerated.getPath().string();

		/* check if we have been called to do stuff with main-descriptor or with a plugin */
		if(&getDescriptor() == &variant.getDescriptor()) {
			std::vector<std::filesystem::path> sourcePaths = builder.getSources().getPathesSourceMain(getVariantName());
			for(const auto& sourcePath : sourcePaths) {
				command += " " + sourcePath.string();
			}
		}
		else {
			std::filesystem::path sourcePath = builder.getSources().getPathBuildSource(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName);
			command += " " + sourcePath.string();
		}

		int exitCode;
		Execute::run(command, exitCode);
		if(exitCode != 0) {
			return exitCode;
		}
		sourceGenerated.invalidateFileEntries();
	}

	return 0;
}


int VariantBuilder::onlyLinkStatic(const model::Variant& variant, const std::string& variantName, const SolvedDescriptors& solvedDescriptors) {
	const files::Source& sourceCompile = builder.getSources().getSourceBuildCompile(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, builder.getArchitecture());

	/* check if there are object files to link */
	if(sourceCompile.getFileEntries().getList().empty()) {
		return 0;
	}
	if(sourceCompile.getFileEntries().makeFlat().filter([](const files::FileEntry& entry){
        return !entry.isDirectory;
	}).filter(std::vector<std::string>{"*.o"}, {"lib.o", "main.o"}).getList().empty()) {
		return 0;
	}

	const files::Source& sourceLinkStatic = builder.getSources().getSourceBuildLinkStatic(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, builder.getArchitecture());

	/* create goalLinkStatic */
	BuildLinkStatic buildLinkStatic(builder, sourceLinkStatic, "lib" + variant.getDescriptor().getArtefactName() + ".a");

	/* generate rules for goal link-static */
	buildLinkStatic.addSource(sourceCompile);
	buildLinkStatic.addIncludeFilePattern(sourceCompile, "*.o");
	buildLinkStatic.addExcludeFilePattern(sourceCompile, "lib.o");
	buildLinkStatic.addExcludeFilePattern(sourceCompile, "main.o");

	/* link main sources to static library */
	int exitCode = buildLinkStatic.run();
	sourceLinkStatic.invalidateFileEntries();

	return exitCode;
}

int VariantBuilder::onlyLinkDynamic(const model::Variant& variant, const std::string& variantName, const SolvedDescriptors& solvedDescriptors) {
	const files::Source& sourceCompile = builder.getSources().getSourceBuildCompile(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, builder.getArchitecture());

	/* check if there are object files to link */
	if(sourceCompile.getFileEntries().makeFlat().filter([](const files::FileEntry& entry){
        return !entry.isDirectory;
	}).filter(std::vector<std::string>{"*.o"}, {"main.o"}).getList().empty()) {
		return 0;
	}

	const files::Source& sourceLinkDynamic = builder.getSources().getSourceBuildLinkDynamic(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, builder.getArchitecture());

	/* create goalLinkDynamic */
	std::string libName = "lib" + variant.getDescriptor().getArtefactName() + ".so";
	std::string soName;
	if(!getDescriptor().getApiVersion().empty()) {
		soName = libName + "." + getDescriptor().getApiVersion();
	}
	BuildLinkDynamic buildLinkDynamic(builder, sourceLinkDynamic, libName, soName);

	/* generate rules for goal link-dynamic */
	buildLinkDynamic.addSource(sourceCompile);
	buildLinkDynamic.addIncludeFilePattern(sourceCompile, "*.o");
	buildLinkDynamic.addExcludeFilePattern(sourceCompile, "main.o");

	/* add additional libraries to buildExecutable */
	for(const auto& solvedDescriptor : solvedDescriptors.getElements()) {
		if(solvedDescriptor.getDescriptor().getSystemPath()) {
			buildLinkDynamic.addLibrary(solvedDescriptor.getDescriptor().getSystemPath()->libraryName);
			buildLinkDynamic.addLibrarySearchPath(solvedDescriptor.getDescriptor().getSystemPath()->libraryPath);
		}
		else {
			if(solvedDescriptor.hasStatic) {
				const files::Source& sourceDependencyLinkStatic = builder.getSources().getSourceBuildLinkStatic(solvedDescriptor.getArtefactId(), solvedDescriptor.getArtefactVersion(), solvedDescriptor.getVariantName(), builder.getArchitecture());

				buildLinkDynamic.addSource(sourceDependencyLinkStatic);
				buildLinkDynamic.addIncludeFilePattern(sourceDependencyLinkStatic, "lib" + solvedDescriptor.getArtefactName() + ".a");
			}
			else if(solvedDescriptor.hasDynamic) {
				const files::Source& sourceDependencyLinkDynamic = builder.getSources().getSourceBuildLinkDynamic(solvedDescriptor.getArtefactId(), solvedDescriptor.getArtefactVersion(), solvedDescriptor.getVariantName(), builder.getArchitecture());

				std::string libName = "lib" + solvedDescriptor.getArtefactName() + ".so";
				//if(!solvedDescriptor.getDescriptor().getApiVersion().empty()) {
				//	libName = libName + "." + solvedDescriptor.getDescriptor().getApiVersion();
				//}

				buildLinkDynamic.addSource(sourceDependencyLinkDynamic);
				buildLinkDynamic.addIncludeFilePattern(sourceDependencyLinkDynamic, libName);
			}
		}
	}

	/* link main sources to dynamic library */
	int exitCode = buildLinkDynamic.run();
	if(exitCode == 0) {
		if(!soName.empty()) {
			std::filesystem::path pathNewSysLink = sourceLinkDynamic.getPath();
			pathNewSysLink /= soName;

			std::filesystem::create_symlink(libName, pathNewSysLink);
		}

		sourceLinkDynamic.invalidateFileEntries();
	}

	return exitCode;
}

int VariantBuilder::onlyLinkExecutable(const model::Variant& variant, const std::string& variantName, const SolvedDescriptors& solvedDescriptors, bool isTest) {
	const files::Source& sourceCompile = isTest ? builder.getSources().getSourceBuildCompileTest(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, builder.getArchitecture()) : builder.getSources().getSourceBuildCompile(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, builder.getArchitecture());
	const files::Source& sourceExecutable = isTest ? builder.getSources().getSourceBuildLinkExecutableTest(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, builder.getArchitecture()) : builder.getSources().getSourceBuildLinkExecutable(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName, builder.getArchitecture());

	/* check if there are objects available */
//	if(sourceCompile.getFileEntries().makeFlat().filter(std::vector<std::string>({"*.o"}), {}).getList().empty()) {
//		return 1;
//	}
	if(sourceCompile.getFileEntries().makeFlat().filter([](const files::FileEntry& entry){
        return !entry.isDirectory;
	}).filter(std::vector<std::string>{"*.o"}, {"lib.o"}).getList().empty()) {
		return 0;
	}

	/* create buildExecutable */
	BuildLinkExecutable buildExecutable(builder, sourceExecutable, variant.getDescriptor().getArtefactName(), isTest);

	/* generate rules for goal buildExecutable */
	buildExecutable.addSource(sourceCompile);
	buildExecutable.addIncludeFilePattern(sourceCompile, "*.o");
	buildExecutable.addExcludeFilePattern(sourceCompile, "lib.o");

	/* add additional libraries to buildExecutable */
	for(const auto& solvedDescriptor : solvedDescriptors.getElements()) {
		if(solvedDescriptor.getDescriptor().getSystemPath()) {
			buildExecutable.addLibrary(solvedDescriptor.getDescriptor().getSystemPath()->libraryName);
			buildExecutable.addLibrarySearchPath(solvedDescriptor.getDescriptor().getSystemPath()->libraryPath);
		}
		else {
			if(solvedDescriptor.hasStatic) {
				const files::Source& sourceDependencyLinkStatic = builder.getSources().getSourceBuildLinkStatic(solvedDescriptor.getArtefactId(), solvedDescriptor.getArtefactVersion(), solvedDescriptor.getVariantName(), builder.getArchitecture());

				buildExecutable.addSource(sourceDependencyLinkStatic);
				buildExecutable.addIncludeFilePattern(sourceDependencyLinkStatic, "lib" + solvedDescriptor.getArtefactName() + ".a");
			}
			else if(solvedDescriptor.hasDynamic) {
				const files::Source& sourceDependencyLinkDynamic = builder.getSources().getSourceBuildLinkDynamic(solvedDescriptor.getArtefactId(), solvedDescriptor.getArtefactVersion(), solvedDescriptor.getVariantName(), builder.getArchitecture());

				std::string libName = "lib" + solvedDescriptor.getArtefactName() + ".so";
				//if(!solvedDescriptor.getDescriptor().getApiVersion().empty()) {
				//	libName = libName + "." + solvedDescriptor.getDescriptor().getApiVersion();
				//}

				buildExecutable.addSource(sourceDependencyLinkDynamic);
				buildExecutable.addIncludeFilePattern(sourceDependencyLinkDynamic, libName);
			}
		}
	}

	/* link opjects and libraries  to executable */
	int exitCode = buildExecutable.run();

	/* copy dynamic libraries to executable folder */
	if(exitCode == 0) {
		for(const auto& solvedDescriptor : solvedDescriptors.getElements()) {
			if(solvedDescriptor.getDescriptor().getSystemPath()) {
				continue;
			}
			if(!solvedDescriptor.hasDynamic) {
				continue;
			}
			const files::Source& sourceDependencyLinkDynamic = builder.getSources().getSourceBuildLinkDynamic(solvedDescriptor.getArtefactId(), solvedDescriptor.getArtefactVersion(), solvedDescriptor.getVariantName(), builder.getArchitecture());

			std::string libName = "lib" + solvedDescriptor.getArtefactName() + ".so";

			std::filesystem::path pathFrom = sourceDependencyLinkDynamic.getPath();
			pathFrom /= libName;

			std::filesystem::path pathTo = sourceExecutable.getPath();
			pathTo /= libName;

			Execute::copy(pathFrom, pathTo);

			if(!solvedDescriptor.getDescriptor().getApiVersion().empty()) {
				libName = libName + "." + solvedDescriptor.getDescriptor().getApiVersion();

				std::filesystem::path pathFrom = sourceDependencyLinkDynamic.getPath();
				pathFrom /= libName;

				std::filesystem::path pathTo = sourceExecutable.getPath();
				pathTo /= libName;

				Execute::copy(pathFrom, pathTo);
			}
		}
	}

	sourceExecutable.invalidateFileEntries();

	return exitCode;
}

void VariantBuilder::addCompileSourcesMain(BuildCompile& buildCompile, const model::Variant& variant, const std::string& variantName, const SolvedDescriptors& solvedDescriptors) {
	std::vector<std::reference_wrapper<const files::Source>> sources = getSourcesSourceMain();

	/* fill includePatterns */
	for(const auto& source : sources) {
		for(const auto& sourceFilePattern : variant.getSourceFilePatternsEffective()) {
			buildCompile.addIncludeFilePattern(source.get(), sourceFilePattern);
		}
	}

	/* add file patters to compile */
	for(const auto& source : sources) {
		buildCompile.addSource(source.get());
	}

	addCompileSources(buildCompile, variant, variantName, solvedDescriptors);
}

void VariantBuilder::addCompileSourcesTest(BuildCompile& buildCompile, const model::Variant& variant, const std::string& variantName, const SolvedDescriptors& solvedDescriptors) {
	std::vector<std::reference_wrapper<const files::Source>> sourcesMain = getSourcesSourceMain();
	std::vector<std::reference_wrapper<const files::Source>> sourcesTest = getSourcesSourceTest();

	/* fill includePatterns */
	for(const auto& source : sourcesMain) {
		for(const auto& sourceFilePattern : variant.getSourceFilePatternsEffective()) {
			buildCompile.addIncludeFilePattern(source.get(), sourceFilePattern);

			/* check if "sourceMain" exists as "sourcesTest". If not, then exclude main.cpp and lib.cpp */
			bool foundInTestSources = false;
			for(const auto& sourceTest : sourcesTest) {
				if(source.get().getPath() == sourceTest.get().getPath()) {
					foundInTestSources = true;
					break;
				}
			}
			if(foundInTestSources == false) {
				buildCompile.addExcludeFilePattern(source.get(), "main.cpp");
				buildCompile.addExcludeFilePattern(source.get(), "lib.cpp");
			}
		}
	}

	for(const auto& source : sourcesTest) {
		for(const auto& sourceFilePattern : variant.getSourceFilePatternsEffective()) {
			buildCompile.addIncludeFilePattern(source.get(), sourceFilePattern);
		}
	}

	/* add file patters to compile */
	for(const auto& sourceMain : sourcesMain) {
		buildCompile.addSource(sourceMain.get());
	}
	for(const auto& sourceTest : sourcesTest) {
		buildCompile.addSource(sourceTest.get());
	}

	addCompileSources(buildCompile, variant, variantName, solvedDescriptors);
}

void VariantBuilder::addCompileSources(BuildCompile& buildCompile, const model::Variant& variant, const std::string& variantName, const SolvedDescriptors& solvedDescriptors) {
	/* check if we have been called to compile the main-descriptor or if it is called to compile a plugin */
	if(&getDescriptor() == &variant.getDescriptor()) {
		/* add own include path */
		for(const auto& headerIncludePattern : variant.getHeadersDirEffective()) {
			buildCompile.addIncludePath(headerIncludePattern);
		}
	}
	else {
		const files::Source& sourceMain = builder.getSources().getSourceBuildSource(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName);

		/* add file patters to compile */
		for(const auto& includeFile : variant.getSourceFilePatternsEffective()) {
			buildCompile.addIncludeFilePattern(sourceMain, includeFile);
		}
		buildCompile.addSource(sourceMain);

		/* add own include path */
		buildCompile.addIncludePath(builder.getSources().getPathBuildHeaders(variant.getDescriptor().getArtefactId(), variant.getDescriptor().getArtefactVersion(), variantName));
	}

	{
		std::map<std::string, model::Generator> generators = variant.getGeneratorsEffective();
		for(const auto& generator : generators) {
			const files::Source& source = builder.getSources().getSourceBuildGenerated(variant.getDescriptor().getArtefactId(), getDescriptor().getArtefactVersion(), variantName, getArchitecture(), generator.second.id);

			/* add file patters to compile */
			for(const auto& includeFile : variant.getSourceFilePatternsEffective()) {
				buildCompile.addIncludeFilePattern(source, includeFile);
			}

			buildCompile.addSource(source);  // adding include path is part of addSource(...)

			/* add own include path */
			//buildCompile.addIncludePath(source.getPath());
		}
	}

	/* add additional system-include-path and defines from dependencies */
	std::vector<std::reference_wrapper<const SolvedDescriptor>> dependencySolvedDescriptors = solvedDescriptors.getSolvedDescirptorsOfDependency(variant.getDescriptor().getArtefactId());
	for(const auto& solvedDescriptor : dependencySolvedDescriptors) {
//	for(const auto& solvedDescriptor : solvedDescriptors.getElements()) {
		const model::Descriptor& dependencyDescriptor = solvedDescriptor.get().getDescriptor();
		const model::Variant& dependencyVariant = solvedDescriptor.get().getVariant();

		if(dependencyDescriptor.getSystemPath()) {
			buildCompile.addSystemIncludePath(dependencyDescriptor.getSystemPath()->includePath);
		}
		else {
			buildCompile.addSystemIncludePath(builder.getSources().getPathBuildHeaders(solvedDescriptor.get().getArtefactId(), solvedDescriptor.get().getArtefactVersion(), solvedDescriptor.get().getVariantName()));

			std::map<std::string, model::Generator> generators = dependencyVariant.getGeneratorsEffective();
			for(const auto& generator : generators) {
				buildCompile.addSystemIncludePath(builder.getSources().getPathBuildGenerated(solvedDescriptor.get().getArtefactId(), solvedDescriptor.get().getArtefactVersion(), solvedDescriptor.get().getVariantName(), getArchitecture(), generator.second.id));
			}
		}

		std::map<std::string, std::string> defines = dependencyVariant.getDefinesPublicEffective();
		for(const auto& define : defines) {
			buildCompile.addDefine(define.first, define.second);
		}
	}

	/* add own defines */
	{
		std::map<std::string, std::string> defines = variant.getDefinesPublicEffective();
		for(const auto& define : defines) {
			buildCompile.addDefine(define.first, define.second);
		}
		defines = variant.getDefinesPrivateEffective();
		for(const auto& define : defines) {
			buildCompile.addDefine(define.first, define.second);
		}

		buildCompile.addDefine("TRANSFORMER_ARTEFACT_ID", "TRANSFORMER_ARTEFACT_ID=" + variant.getDescriptor().getArtefactId());
		buildCompile.addDefine("TRANSFORMER_ARTEFACT_NAME", "TRANSFORMER_ARTEFACT_NAME=" + variant.getDescriptor().getArtefactName());
		buildCompile.addDefine("TRANSFORMER_ARTEFACT_VERSION", "TRANSFORMER_ARTEFACT_VERSION=" + variant.getDescriptor().getArtefactVersion());
		buildCompile.addDefine("TRANSFORMER_ARTEFACT_API_VERSION", "TRANSFORMER_ARTEFACT_API_VERSION=" + variant.getDescriptor().getApiVersion());
		buildCompile.addDefine("TRANSFORMER_ARTEFACT_VARIANT", "TRANSFORMER_ARTEFACT_VARIANT=" + variantName);
		buildCompile.addDefine("TRANSFORMER_ARTEFACT_ARCHITECTURE", "TRANSFORMER_ARTEFACT_ARCHITECTURE=" + getArchitecture());
	}

}

const SolvedDescriptors& VariantBuilder::getSolvedDescriptors(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName) const {
	return getBuildManager().getSolvedDescriptors(artefactId, artefactVersion, variantName);
}

std::vector<std::reference_wrapper<const files::Source>> VariantBuilder::getSourcesSourceMain() const {
	return builder.getSources().getSourcesSourceMain(variantName);
}

std::vector<std::reference_wrapper<const files::Source>> VariantBuilder::getSourcesSourceTest() const {
	return builder.getSources().getSourcesSourceTest(variantName);
}

} /* namespace gcc */
} /* namespace build */
} /* namespace transformer */
