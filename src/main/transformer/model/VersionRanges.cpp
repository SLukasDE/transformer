#include <transformer/model/VersionRanges.h>

namespace transformer {
namespace model {

VersionRanges::VersionRanges() {
}

VersionRanges::~VersionRanges() {
}

bool VersionRanges::isEmpty() const {
	for(const auto& versionRange : ranges) {
		if(versionRange.isEmpty()) {
			continue;
		}
		return false;
	}
	return true;
}

void VersionRanges::addVersionRange(VersionRange versionRange) {
	if(versionRange.isEmpty()) {
		return;
	}
	ranges.emplace_back(std::move(versionRange));
}

bool VersionRanges::hasVersion(const Version& version) const {
	for(const auto& versionRange : ranges) {
		if(versionRange.hasVersion(version)) {
			return true;
		}
	}
	return false;
}

bool VersionRanges::hasCommonVersion(const std::string& commonVersion) const {
	for(const auto& versionRange : ranges) {
		if(versionRange.hasCommonVersion(commonVersion)) {
			return true;
		}
	}
	return false;
}

bool VersionRanges::hasArtefactVersion(const std::string& artefactVersion) const {
	for(const auto& versionRange : ranges) {
		if(versionRange.hasArtefactVersion(artefactVersion)) {
			return true;
		}
	}
	return false;
}

bool VersionRanges::hasApiVersion(const std::string& apiVersion) const {
	for(const auto& versionRange : ranges) {
		if(versionRange.hasApiVersion(apiVersion)) {
			return true;
		}
	}
	return false;
}

const std::vector<VersionRange>& VersionRanges::getRanges() const {
	return ranges;
}

} /* namespace model */
} /* namespace transformer */
