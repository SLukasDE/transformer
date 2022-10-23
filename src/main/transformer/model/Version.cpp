#include <transformer/model/Version.h>
#include <stdexcept>

namespace transformer {
namespace model {

Version::Version()
{
}

Version::~Version() {
}

bool Version::isEmpty() const {
	return versionBoth.empty() && versionArtefact.empty() && versionApi.empty();
}

const std::string& Version::getVersionArtefact() const {
	if(!versionBoth.empty()) {
		return versionBoth;
	}
	return versionArtefact;
}

const std::string& Version::getVersionApi() const {
	if(!versionBoth.empty()) {
		return versionBoth;
	}
	return versionApi;
}

bool Version::hasVersion(const Version& aVersion) const {
	if(aVersion.hasArtefactVersion()) {
		if(hasArtefactVersion(aVersion.getVersionArtefact()) == false) {
			return false;
		}
	}

	if(aVersion.hasApiVersion()) {
		if(hasApiVersion(aVersion.getVersionApi()) == false) {
			return false;
		}
	}

	return true;
}

bool Version::hasCommonVersion(const std::string& aCommonVersion) const {
	if(hasArtefactVersion(aCommonVersion) == false) {
		return false;
	}

	if(hasApiVersion(aCommonVersion) == false) {
		return false;
	}

	return true;
}

bool Version::hasArtefactVersion(const std::string& aArtefactVersion) const {
	if(!hasArtefactVersion()) {
		return false;
	}

	return aArtefactVersion == getVersionArtefact();
}

bool Version::hasApiVersion(const std::string& aApiVersion) const {
	if(!hasApiVersion()) {
		return false;
	}

	return aApiVersion == getVersionApi();
}
/*
bool Version::hasVersion(const std::string& aVersionArtefact, const std::string& aVersionApi) const {
	if(!aVersionArtefact.empty()) {
		if(aVersionArtefact != getVersionArtefact()) {
			return false;
		}
	}

	if(!aVersionApi.empty()) {
		if(aVersionApi != getVersionApi()) {
			return false;
		}
	}

	return true;
}
*/
bool Version::hasCommonVersion() const {
	return !versionBoth.empty();
}

bool Version::hasArtefactVersion() const {
	return hasCommonVersion() || versionArtefact.empty();
}

bool Version::hasApiVersion() const {
	return hasCommonVersion() || versionApi.empty();
}

void Version::setCommonVersion(const std::string& aCommonVersion) {
	versionBoth = aCommonVersion;
	versionArtefact.clear();
	versionApi.clear();
}

void Version::setArtefactVersion(const std::string& aVersionArtefact) {
	versionBoth.clear();
	versionArtefact = aVersionArtefact;
}

void Version::setApiVersion(const std::string& aVersionApi) {
	versionBoth.clear();
	versionApi = aVersionApi;
}

} /* namespace model */
} /* namespace transformer */
