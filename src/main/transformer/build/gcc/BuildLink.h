#ifndef TRANSFORMER_BUILD_GCC_BUILDLINK_H_
#define TRANSFORMER_BUILD_GCC_BUILDLINK_H_

#include <filesystem>
#include <string>
#include <set>

#include <transformer/build/gcc/BuildBase.h>
#include <transformer/build/gcc/Builder.h>
#include <transformer/files/Source.h>

namespace transformer {
namespace build {
namespace gcc {

class BuildLink : public BuildBase {
public:
	BuildLink(const Builder& builder, const files::Source& targetSource, std::string targetName);
	~BuildLink();

	void addLibrary(const std::string& library);
	const std::set<std::string>& getLibraries() const;

	void addLibrarySearchPath(const std::filesystem::path& path);
	const std::set<std::filesystem::path>& getLibrarySearchPaths() const;

	const std::string& getTargetName() const;

private:
	const std::string targetName;

	std::set<std::string> libraries;
	std::set<std::filesystem::path> librarySearchPaths;
};

} /* namespace gcc */
} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_GCC_BUILDLINK_H_ */
