#ifndef TRANSFORMER_FILES_FILEENTRY_ENTRIES_H_
#define TRANSFORMER_FILES_FILEENTRY_ENTRIES_H_
#include <transformer/files/FileEntries.h>
#endif /* TRANSFORMER_FILES_FILEENTRY_ENTRIES_H_ */

#ifndef TRANSFORMER_FILES_FILEENTRY_H_
#define TRANSFORMER_FILES_FILEENTRY_H_

#include <string>
#include <vector>
#include <ctime>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS


namespace transformer {
namespace files {

struct FileEntry {
	bool isDirectory = false;
	boost::filesystem::path name;
	std::time_t lastWriteTime;

	FileEntries entries;
};

} /* namespace files */
} /* namespace transformer */

#endif /* TRANSFORMER_FILES_FILEENTRY_H_ */
