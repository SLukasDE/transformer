#include <transformer/Config.h>
#include <transformer/build/gcc/BuildLinkStatic.h>
#include <transformer/Execute.h>

#include <filesystem>


namespace transformer {
namespace build {
namespace gcc {

BuildLinkStatic::BuildLinkStatic(const Builder& builder, const files::Source& targetSource, std::string aTargetName)
: BuildLink(builder, targetSource, std::move(aTargetName))
{
}

BuildLinkStatic::~BuildLinkStatic() {
}

int BuildLinkStatic::run() const {
	/* create target directoy if not exists */
	Execute::mkdir(getTargetSource().getPath());

#if 1
	std::filesystem::path toPath = getTargetSource().getPath();
	toPath /= getTargetName();

	std::string command = getGccBuilder().getExecAr() + " " + getGccBuilder().getExecArOpts() + " " + toPath.string();
	/*
	std::string command = getGccBuilder().getExecCxx() + " " + getGccBuilder().getExecCxxOptsLinkDynamic();
	if(!getSoName().empty()) {
		command += " -Wl,-soname," + getSoName();
	}
	command += " -o " + toPath.string();
	*/

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

	/*
	for(const auto library : getLibraries()) {
		command += " -l" + library;
	}
	*/

	int exitCode;
	Execute::run(command, exitCode);
	getTargetSource().invalidateFileEntries();

	return exitCode;
#else
	/* ******************************************************* *
	 * create temporary folder to copy and rename object files *
	 * ******************************************************* */
	std::filesystem::path tmpPath = getTargetSource().getPath();
	tmpPath /= "tmp";

	if(Config::parallelCount == 0) {
		Execute::mkdir(tmpPath);
	}
	else {
		if(std::filesystem::exists(tmpPath)) {
			std::filesystem::remove_all(tmpPath);
		}
		std::filesystem::create_directory(tmpPath);
	}


	/* **************************** *
	 * copy and rename object files *
	 * **************************** */
	for(const auto& source : getSources()) {
		files::FileEntries entriesFromFiltered = source.get().getFileEntries().makeFlat().filter([](const files::FileEntry& entry){
	        return !entry.isDirectory;
		}).filter(getIncludeFilePatterns(source.get()), getExcludeFilePatterns(source.get()));

		for(const auto& entry :  entriesFromFiltered.getList()) {
			std::string dstString;
			for(std::filesystem::path p : entry.name) {
				if(!dstString.empty()) {
					dstString += "_.";
				}
				for(const auto& c : p.generic_string()) {
					if(c == '_') {
						dstString += "_";
					}
					dstString += c;
				}
			}

			std::filesystem::path fromPath = source.get().getPath();
			fromPath /= entry.name;

			std::filesystem::path toPath = tmpPath;
			toPath /= dstString;

			Execute::copyFile(fromPath, toPath);
			//commands.push_back(std::make_pair(pathLib.generic_string() + " <- " + toPath.generic_string(), getGccBuilder().getExecAr() + " " + getGccBuilder().getExecArOpts() + " " + pathLib.string() + " " + toPath.string()));
		}
	}

	/* ************************** *
	 * create static library file *
	 * ************************** */

	std::filesystem::path pathLib = getTargetSource().getPath();
	pathLib /= getTargetName();

	for(const auto& source : getSources()) {
		files::FileEntries entriesFromFiltered = source.get().getFileEntries().makeFlat().filter([](const files::FileEntry& entry){
	        return !entry.isDirectory;
		}).filter(getIncludeFilePatterns(source.get()), getExcludeFilePatterns(source.get()));

		for(const auto& entry :  entriesFromFiltered.getList()) {
			std::string dstString;
			for(std::filesystem::path p : entry.name) {
				if(!dstString.empty()) {
					dstString += "_.";
				}
				for(const auto& c : p.generic_string()) {
					if(c == '_') {
						dstString += "_";
					}
					dstString += c;
				}
			}

			std::filesystem::path toPath = tmpPath;
			toPath /= dstString;

			int exitCode = Execute::run(getGccBuilder().getExecAr() + " " + getGccBuilder().getExecArOpts() + " " + pathLib.string() + " " + toPath.string(), pathLib.generic_string() + " <- " + toPath.generic_string());
			if(exitCode != 0) {
				return exitCode;
			}
		}
	}

	getTargetSource().invalidateFileEntries();

	if(Config::parallelCount == 0) {
		Execute::rmdir("rmdir-temp", "rm -rf " + tmpPath.generic_string());
	}
	if(Config::parallelCount > 0) {
		std::filesystem::remove_all(tmpPath);
	}

	return 0;
#endif
}

} /* namespace gcc */
} /* namespace build */
} /* namespace transformer */
