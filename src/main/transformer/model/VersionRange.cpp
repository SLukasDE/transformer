#include <transformer/model/VersionRange.h>
#include <transformer/model/VersionNumber.h>

namespace transformer {
namespace model {

VersionRange::VersionRange() {
}

VersionRange::~VersionRange() {
}

bool VersionRange::isEmpty() const {
	return versionBoth.empty() && versionArtefact.empty() && versionApi.empty();
}

const std::string& VersionRange::getMinVersionArtefact() const {
	if(!versionBoth.empty()) {
		return versionBoth;
	}
	return versionArtefact;
}

const std::string& VersionRange::getMaxVersionArtefact() const {
	if(!versionBoth.empty()) {
		return versionBoth;
	}
	if(!versionArtefactMax.empty()) {
		return versionArtefactMax;
	}
	return versionArtefact;
}

const std::string& VersionRange::getMinVersionApi() const {
	if(!versionBoth.empty()) {
		return versionBoth;
	}
	return versionApi;
}

const std::string& VersionRange::getMaxVersionApi() const {
	if(!versionBoth.empty()) {
		return versionBoth;
	}
	if(!versionApiMax.empty()) {
		return versionApiMax;
	}
	return versionApi;
}

bool VersionRange::hasVersion(const Version& aVersion) const {
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

bool VersionRange::hasCommonVersion(const std::string& aCommonVersion) const {
	if(hasArtefactVersion(aCommonVersion) == false) {
		return false;
	}

	if(hasApiVersion(aCommonVersion) == false) {
		return false;
	}

	return true;
}

bool VersionRange::hasCommonArtefactVersion() const {
	return hasCommonVersion() || versionArtefactMax.empty();
}

bool VersionRange::hasCommonApiVersion() const {
	return hasCommonVersion() || versionApiMax.empty();
}
/*
bool VersionRange::hasVersion(const std::string& aArtefactVersion, const std::string& aVersionApi) const {
	return hasArtefactVersion(aArtefactVersion) && hasApiVersion(aVersionApi);
}
*/
bool VersionRange::hasArtefactVersion(const std::string& aArtefactVersion) const {
	if(!hasArtefactVersion()) {
		return false;
	}

	VersionNumber versionNumberArtefact(aArtefactVersion);
	VersionNumber versionNumberArtefactMin(getMinVersionArtefact());
	VersionNumber versionNumberArtefactMax(getMaxVersionArtefact());

	return versionNumberArtefact >= versionNumberArtefactMin && versionNumberArtefact <= versionNumberArtefactMax;
}

bool VersionRange::hasApiVersion(const std::string& aApiVersion) const {
	if(!hasApiVersion()) {
		return false;
	}

	VersionNumber versionNumberApi(aApiVersion);
	VersionNumber versionNumberApiMin(getMinVersionApi());
	VersionNumber versionNumberApiMax(getMaxVersionApi());

	return versionNumberApi >= versionNumberApiMin && versionNumberApi <= versionNumberApiMax;
}

bool VersionRange::hasCommonVersion() const {
	return !versionBoth.empty();
}

bool VersionRange::hasArtefactVersion() const {
	return hasCommonVersion() || !versionArtefact.empty();
}

bool VersionRange::hasApiVersion() const {
	return hasCommonVersion() || !versionApi.empty();
}

void VersionRange::setCommonVersion(const std::string& aCommonVersion) {
	versionBoth = aCommonVersion;
	versionArtefact.clear();
	versionArtefactMax.clear();
	versionApi.clear();
	versionApiMax.clear();
}

void VersionRange::setArtefactVersion(const std::string& aArtefactVersion) {
	versionBoth.clear();
	versionArtefact = aArtefactVersion;
	versionArtefactMax.clear();
}

void VersionRange::setArtefactVersion(const std::string& aMinArtefactVersion, const std::string& aMaxArtefactVersion) {
	versionBoth.clear();
	versionArtefact = aMinArtefactVersion;
	versionArtefactMax = aMaxArtefactVersion;

	if(VersionNumber(versionArtefact) > VersionNumber(versionArtefactMax)) {
		std::swap(versionArtefact, versionArtefactMax);
	}
}

void VersionRange::setApiVersion(const std::string& aVersionApi) {
	versionBoth.clear();
	versionApi = aVersionApi;
	versionApiMax.clear();
}

void VersionRange::setApiVersion(const std::string& aMinVersionApi, const std::string& aMaxVersionApi) {
	versionBoth.clear();
	versionApi = aMinVersionApi;
	versionApiMax = aMaxVersionApi;

	if(VersionNumber(versionApi) > VersionNumber(versionApiMax)) {
		std::swap(versionApi, versionApiMax);
	}
}

} /* namespace model */
} /* namespace transformer */
