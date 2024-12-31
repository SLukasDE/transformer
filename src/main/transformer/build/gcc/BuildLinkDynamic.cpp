#include <transformer/build/gcc/BuildLinkDynamic.h>
#include <transformer/Execute.h>

namespace transformer {
namespace build {
namespace gcc {

BuildLinkDynamic::BuildLinkDynamic(const Builder& builder, const files::Source& targetSource, std::string aTargetName, std::string aSoName)
: BuildLink(builder, targetSource, std::move(aTargetName)),
  soName(std::move(aSoName))
{
}

BuildLinkDynamic::~BuildLinkDynamic() {
}

int BuildLinkDynamic::run() const {
	/* create target directoy if not exists */
	Execute::mkdir(getTargetSource().getPath());

	std::filesystem::path toPath = getTargetSource().getPath();
	toPath /= getTargetName();

	std::string command = getGccBuilder().getExecCxx() + " " + getGccBuilder().getExecCxxOptsLinkDynamic();
	if(!getSoName().empty()) {
		command += " -Wl,-soname," + getSoName();
	}
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

	for(const auto& library : getLibraries()) {
		command += " -l" + library;
	}

	int exitCode;
	Execute::run(command, exitCode);
	getTargetSource().invalidateFileEntries();

	return exitCode;
}

const std::string& BuildLinkDynamic::getSoName() const {
	return soName;
}

} /* namespace gcc */
} /* namespace build */
} /* namespace transformer */
