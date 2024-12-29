#ifndef TRANSFORMER_FILES_SOURCE_H_
#define TRANSFORMER_FILES_SOURCE_H_

#include <filesystem>
#include <string>

#include <transformer/files/FileEntries.h>

namespace transformer {
namespace files {

class Sources;

class Source {
friend class Sources;

public:
	Source() = default;
	Source(const std::filesystem::path& path);
	~Source() = default;

	void setPath(const std::filesystem::path& aPath);
	const std::filesystem::path& getPath() const;
	const FileEntries& getFileEntries() const;
	void invalidateFileEntries() const;

	static const Source& getEmptySource();

private:
	std::filesystem::path path;
	mutable bool isEmptyFileEntries = true;
	mutable FileEntries fileEntries;
};

} /* namespace files */
} /* namespace transformer */


#endif /* TRANSFORMER_FILES_SOURCE_H_ */
