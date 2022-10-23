#include <transformer/model/VersionNumber.h>
#include <algorithm>

namespace transformer {
namespace model {

VersionNumber::VersionNumber(std::string aVersionString)
: versionString(std::move(aVersionString))
{
	std::vector<std::string> splittedVersion = splitVersion(versionString);
	for(const auto& componentStr : splittedVersion) {
		bool componentBool = true;
		int componentNumber = 0;

		try {
			componentNumber = std::stoi(componentStr);
		}
		catch(...) {
			componentBool = false;
		}
		components.emplace_back(componentBool, componentStr, componentNumber);
	}
}

VersionNumber::~VersionNumber() {
}

const std::string& VersionNumber::getString() const {
	return versionString;
}

bool VersionNumber::operator<(const VersionNumber& aVersionString) const {
	return cmp(*this, aVersionString) < 0;
}

bool VersionNumber::operator>(const VersionNumber& aVersionString) const {
	return cmp(*this, aVersionString) > 0;
}

bool VersionNumber::operator<=(const VersionNumber& aVersionString) const {
	return cmp(*this, aVersionString) <= 0;
}

bool VersionNumber::operator>=(const VersionNumber& aVersionString) const {
	return cmp(*this, aVersionString) >= 0;
}

bool VersionNumber::operator==(const VersionNumber& aVersionString) const {
	return cmp(*this, aVersionString) == 0;
}

int VersionNumber::cmp(const VersionNumber& version1, const VersionNumber& version2) {
	std::size_t maxIndex = std::max(version1.components.size(), version2.components.size());

	for(std::size_t currentIndex=0; currentIndex<maxIndex; ++currentIndex) {
		if(std::get<0>(version1.components[currentIndex]) && std::get<0>(version2.components[currentIndex])) {
			if(std::get<1>(version1.components[currentIndex]) < std::get<1>(version2.components[currentIndex])) {
				return -1;
			}
			if(std::get<1>(version1.components[currentIndex]) > std::get<1>(version2.components[currentIndex])) {
				return -1;
			}
		}
		else {
			if(std::get<2>(version1.components[currentIndex]) < std::get<2>(version2.components[currentIndex])) {
				return -1;
			}
			if(std::get<2>(version1.components[currentIndex]) > std::get<2>(version2.components[currentIndex])) {
				return -1;
			}
		}
	}

	// "1.0.9" > "1.0"
	if(maxIndex < version1.components.size()) {
		return 1;
	}
	// "1.0" < "1.0-SNAPSHOT"
	if(maxIndex < version2.components.size()) {
		return -1;
	}

	return 0;
}

std::vector<std::string> VersionNumber::splitVersion(const std::string& version) {
	std::vector<std::string> result;

	std::string::const_iterator iterCurrent = std::begin(version);
	std::string::const_iterator iterLast = iterCurrent;
	for(;iterCurrent != std::end(version); ++iterCurrent) {
		if(*iterCurrent == '.') {
			result.emplace_back(iterLast, iterCurrent);
			iterLast = iterCurrent;
			++iterLast;
		}
	}
	result.emplace_back(iterLast, iterCurrent);

	return result;
}

} /* namespace model */
} /* namespace transformer */
