#ifndef TRANSFORMER_FILES_FILEENTRY_ENTRIES_H_
#define TRANSFORMER_FILES_FILEENTRY_ENTRIES_H_
#include <transformer/files/FileEntries.h>
#endif /* TRANSFORMER_FILES_FILEENTRY_ENTRIES_H_ */

#ifndef TRANSFORMER_FILES_FILEENTRY_H_
#define TRANSFORMER_FILES_FILEENTRY_H_

#include <ctime>
#include <filesystem>
#include <string>
#include <vector>


namespace transformer {
namespace files {

struct FileEntry {
	bool isDirectory = false;
	std::filesystem::path name;
	std::time_t lastWriteTime;

	FileEntries entries;
};

} /* namespace files */
} /* namespace transformer */

#endif /* TRANSFORMER_FILES_FILEENTRY_H_ */
