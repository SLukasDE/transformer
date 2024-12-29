#include <transformer/Config.h>
#include <transformer/files/Source.h>
#include <transformer/Execute.h>

#include <esl/system/Stacktrace.h>

namespace transformer {
namespace files {

Source::Source(const std::filesystem::path& aPath)
: path(aPath)
{
}

void Source::setPath(const std::filesystem::path& aPath) {
	path = aPath;
	invalidateFileEntries();
}

const std::filesystem::path& Source::getPath() const {
	return path;
}

const FileEntries& Source::getFileEntries() const {
	if(isEmptyFileEntries) {
		fileEntries.loadDirectory(path);
		isEmptyFileEntries = false;
	}
	return fileEntries;
}

void Source::invalidateFileEntries() const {
	isEmptyFileEntries = true;
}

const Source& Source::getEmptySource() {
	static Source emptySource;
	return emptySource;
}

} /* namespace files */
} /* namespace transformer */
