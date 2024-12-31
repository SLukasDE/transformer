#include <transformer/build/gcc/BuildLinkExecutable.h>
#include <transformer/Execute.h>

namespace transformer {
namespace build {
namespace gcc {

BuildLinkExecutable::BuildLinkExecutable(const Builder& builder, const files::Source& targetSource, std::string targetName, bool isTest)
: BuildLink(builder, targetSource, std::move(targetName)),
  execCxxOpts(isTest ? builder.getExecCxxOptsLinkExecutableTest() : builder.getExecCxxOptsLinkExecutableMain())
{
}

BuildLinkExecutable::~BuildLinkExecutable() {
}

int BuildLinkExecutable::run() const {
	/* create target directoy if not exists */
	Execute::mkdir(getTargetSource().getPath());

	std::filesystem::path toPath = getTargetSource().getPath();
	toPath /= getTargetName();

	std::string command = getGccBuilder().getExecCxx() + " " + execCxxOpts;
	command += " -o " + toPath.string();

	/* create command-part for each source */
	for(const auto& source : getSources()) {
		files::FileEntries entriesFromFiltered = source.get().getFileEntries().makeFlat().filter([](const files::FileEntry& entry){
	        return !entry.isDirectory;
		}).filter(getIncludeFilePatterns(source.get()), getExcludeFilePatterns(source.get()));

		for(const auto& entry :  entriesFromFiltered.getList()) {
			std::filesystem::path pathSourceFile = source.get().getPath();
			pathSourceFile /= entry.name;

			command += " " + pathSourceFile.string();
		}
	}

	/* take additional paths form ruleId "library-search-path" into add command */
	for(const auto& librarySearchPath: getLibrarySearchPaths()) {
		command += " -L";
		command += librarySearchPath.string();
	}

	/* take additional libraries to link with form ruleId "libraries" into command */
	for(const auto& library: getLibraries()) {
		command += " -l";
		command += library;
	}

	int exitCode;
	Execute::run(command, exitCode);
	getTargetSource().invalidateFileEntries();

	return exitCode;
}

} /* namespace gcc */
} /* namespace build */
} /* namespace transformer */
