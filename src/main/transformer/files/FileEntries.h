#ifndef TRANSFORMER_FILES_FILEENTRIES_H_
#define TRANSFORMER_FILES_FILEENTRIES_H_

#include <vector>
#include <set>
#include <string>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS


namespace transformer {
namespace files {

class FileEntry;

class FileEntries {
public:
	FileEntries();
	virtual ~FileEntries();

	void loadDirectory(const boost::filesystem::path& path);
	const std::vector<FileEntry>& getList() const;

	FileEntries& operator+=(const FileEntries& rhs);
	FileEntries operator+(const FileEntries& rhs) const;
	FileEntries operator-(const FileEntries& rhs) const;

	FileEntries makeFlat() const;
	FileEntries makePrefix(const boost::filesystem::path& path) const;

	template<typename Filter>
	FileEntries filter(Filter filterCondition) const;

	FileEntries filter(const std::set<std::string>& includePatterns, const std::set<std::string>& excludePatterns) const;
	FileEntries filter(const std::vector<std::string>& includePatterns, const std::vector<std::string>& excludePatterns) const;
//	FileEntries filterByExtension(std::string extension) const;
//	FileEntries filterByExtensions(const std::vector<boost::filesystem::path>& extensions) const;

	void print(const std::string& prefix) const;

	static void syncDirectoryStructure(const FileEntries& entriesActual, const FileEntries& entriesTarget, const boost::filesystem::path& path, bool withDelete);
	static void createDirectories(const boost::filesystem::path& path, const FileEntries& entries);
	static void deleteDirectories(const boost::filesystem::path& path, const FileEntries& entries);

private:
	std::vector<FileEntry> entries;
};

} /* namespace files */
} /* namespace transformer */

#include <transformer/files/FileEntry.h>

namespace transformer {
namespace files {

template<typename Filter>
FileEntries FileEntries::filter(Filter filterCondition) const {
	FileEntries result;

	for(const auto& entry : entries) {
		if(!filterCondition(entry)) {
			continue;
		}

		if (entry.isDirectory) {
			FileEntry newEntry;
			newEntry.isDirectory = true;
			newEntry.name = entry.name;
			newEntry.entries = entry.entries.filter(filterCondition);
			result.entries.push_back(std::move(newEntry));
			continue;
		}
		/* if it is a file */
		else {
			result.entries.push_back(entry);
			continue;
		}
	}

	return result;
}

} /* namespace files */
} /* namespace transformer */

#endif /* TRANSFORMER_FILES_FILEENTRIES_H_ */
