#ifndef TRANSFORMER_FILES_SOURCE_H_
#define TRANSFORMER_FILES_SOURCE_H_

#include <string>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

#include <transformer/files/FileEntries.h>

namespace transformer {
namespace files {

class Sources;

class Source {
friend class Sources;

public:
	Source() = default;
	Source(const boost::filesystem::path& path);
	~Source() = default;

	void setPath(const boost::filesystem::path& aPath);
	const boost::filesystem::path& getPath() const;
	const FileEntries& getFileEntries() const;
	void invalidateFileEntries() const;

	static const Source& getEmptySource();

private:
	boost::filesystem::path path;
	mutable bool isEmptyFileEntries = true;
	mutable FileEntries fileEntries;
};

} /* namespace files */
} /* namespace transformer */


#endif /* TRANSFORMER_FILES_SOURCE_H_ */
