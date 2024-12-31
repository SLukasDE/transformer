#include <transformer/build/gcc/BuildCompile.h>
#include <transformer/files/FileEntries.h>
#include <transformer/Execute.h>

namespace transformer {
namespace build {
namespace gcc {


BuildCompile::BuildCompile(const Builder& builder, const files::Source& targetSource, bool isTest)
: gcc::BuildBase(builder, targetSource),
  execCxxOpts(isTest ? builder.getExecCxxOptsCompileTest() : builder.getExecCxxOptsCompileMain())
{
}

BuildCompile::~BuildCompile() {
}

int BuildCompile::run() const {
	/* take all include paths into include search path */
	std::string includeArgs;
	std::string systemIncludeArgs;

	for(const auto& includePath: isSystemIncludeByPath) {
		if(includePath.second) {
			systemIncludeArgs += " -isystem";
			systemIncludeArgs += includePath.first.generic_string();
		}
		else {
			includeArgs += " -I";
			includeArgs += includePath.first.generic_string();
		}
	}

	/* prepare Define-Arguments */
	std::string defineArgs;
	for(const auto& define: defines) {
		defineArgs += " -D";
		defineArgs += define.second;
	}

	/* create commands for each source */
	std::vector<std::string> commands;
	for(const auto& source : getSources()) {
		/* synchronize directories for source */
		files::FileEntries::syncDirectoryStructure(source.get().getFileEntries(), getTargetSource().getFileEntries(), getTargetSource().getPath(), false);

		/* create commands for source */
		files::FileEntries entriesFromFiltered = source.get().getFileEntries().makeFlat().filter([](const files::FileEntry& entry){
	        return !entry.isDirectory;
		}).filter(getIncludeFilePatterns(source.get()), getExcludeFilePatterns(source.get()));

		for(const auto& entry :  entriesFromFiltered.getList()) {
			std::filesystem::path pathSourceFile = source.get().getPath();
			pathSourceFile /= entry.name;

			std::filesystem::path pathObjectFile = getTargetSource().getPath();
			pathObjectFile /= entry.name;
			pathObjectFile.replace_extension("o");

			std::string command = getGccBuilder().getExecCxx() + " " + execCxxOpts + " " + pathSourceFile.string() + " -c -o " + pathObjectFile.string();
			command += includeArgs;
			command += systemIncludeArgs;
			command += defineArgs;

			commands.push_back(command);
		}
	}

	int exitCode = 0;
	exitCode = Execute::run(commands);
	getTargetSource().invalidateFileEntries();
	return exitCode;
}

void BuildCompile::addIncludePath(const std::filesystem::path& path) {
	if(path.empty()) {
		return;
	}
	isSystemIncludeByPath[path] = false;
}

void BuildCompile::addSystemIncludePath(const std::filesystem::path& path) {
	if(path.empty()) {
		return;
	}
	isSystemIncludeByPath[path] = true;
}

void BuildCompile::addDefine(const std::string& key, const std::string& value) {
	if(value.empty()) {
		return;
	}
	defines[key] = value;
}

void BuildCompile::addSource(const files::Source& source) {
	addIncludePath(source.getPath());
	BuildBase::addSource(source);
}

} /* namespace gcc */
} /* namespace build */
} /* namespace transformer */
