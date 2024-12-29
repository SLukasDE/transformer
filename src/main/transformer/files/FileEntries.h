#ifndef TRANSFORMER_FILES_FILEENTRIES_H_
#define TRANSFORMER_FILES_FILEENTRIES_H_

#include <filesystem>
#include <set>
#include <string>
#include <vector>


namespace transformer {
namespace files {

class FileEntry;

class FileEntries {
public:
	FileEntries();
	virtual ~FileEntries();

	void loadDirectory(const std::filesystem::path& path);
	const std::vector<FileEntry>& getList() const;

	FileEntries& operator+=(const FileEntries& rhs);
	FileEntries operator+(const FileEntries& rhs) const;
	FileEntries operator-(const FileEntries& rhs) const;

	FileEntries makeFlat() const;
	FileEntries makePrefix(const std::filesystem::path& path) const;

	template<typename Filter>
	FileEntries filter(Filter filterCondition) const;

	FileEntries filter(const std::set<std::string>& includePatterns, const std::set<std::string>& excludePatterns) const;
	FileEntries filter(const std::vector<std::string>& includePatterns, const std::vector<std::string>& excludePatterns) const;
//	FileEntries filterByExtension(std::string extension) const;
//	FileEntries filterByExtensions(const std::vector<std::filesystem::path>& extensions) const;

	void print(const std::string& prefix) const;

	static void syncDirectoryStructure(const FileEntries& entriesActual, const FileEntries& entriesTarget, const std::filesystem::path& path, bool withDelete);
	static void createDirectories(const std::filesystem::path& path, const FileEntries& entries);
	static void deleteDirectories(const std::filesystem::path& path, const FileEntries& entries);

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
