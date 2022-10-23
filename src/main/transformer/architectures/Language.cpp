#include <transformer/architectures/Language.h>

#include <stdexcept>

namespace transformer {
namespace architectures {

Language::Language(std::string aLanguage)
: language(std::move(aLanguage))
{ }

const std::string& Language::getLanguage() const noexcept {
	return language;
}

const std::string& Language::getDriver() const noexcept {
	return driverImplementation;
}

void Language::setDriver(const std::string& aDriverImplementation) {
	driverImplementation = aDriverImplementation;
}

const Language::Settings& Language::getDriverSettings() const noexcept {
	return driverSettings;
}

void Language::addDriverSetting(const std::string& key, const std::string& value) {
	driverSettings.push_back(std::make_pair(key, value));
}

std::set<std::string> Language::getProfiles() const {
	std::set<std::string> profiles;

	for(const auto& entry : settingsByProfile) {
		profiles.insert(entry.first);
	}

	return profiles;
}

const Language::Settings& Language::getProfileSettings(const std::string& profile) const {
	auto iter = settingsByProfile.find(profile);

	if(iter == settingsByProfile.end()) {
		throw std::runtime_error("cannot get settings for unknown profile \"" + profile + "\"");
	}

	return iter->second;
}

void Language::addProfileSetting(const std::string& profile, const std::string& key, const std::string& value) {
	Settings& settings = settingsByProfile[profile];

	if(!key.empty() || !value.empty()) {
		settings.push_back(std::make_pair(key, value));
	}
}

} /* namespace architectures */
} /* namespace transformer */
