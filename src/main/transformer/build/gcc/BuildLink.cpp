#include <transformer/build/gcc/BuildLink.h>

namespace transformer {
namespace build {
namespace gcc {

BuildLink::BuildLink(const Builder& builder, const files::Source& targetSource, std::string aTargetName)
: BuildBase(builder, targetSource),
  targetName(std::move(aTargetName))
{
}

BuildLink::~BuildLink() {
}

void BuildLink::addLibrary(const std::string& library) {
	if(library.empty()) {
		return;
	}
	libraries.insert(library);
}

const std::set<std::string>& BuildLink::getLibraries() const {
	return libraries;
}

void BuildLink::addLibrarySearchPath(const std::filesystem::path& path) {
	if(path.empty()) {
		return;
	}
	librarySearchPaths.insert(path);
}

const std::set<std::filesystem::path>& BuildLink::getLibrarySearchPaths() const {
	return librarySearchPaths;
}

const std::string& BuildLink::getTargetName() const {
	return targetName;
}

} /* namespace gcc */
} /* namespace build */
} /* namespace transformer */
