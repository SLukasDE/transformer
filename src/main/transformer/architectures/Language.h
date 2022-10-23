#ifndef TRANSFORMER_ARCHITECTURES_LANGUAGE_H_
#define TRANSFORMER_ARCHITECTURES_LANGUAGE_H_

#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>

namespace transformer {
namespace architectures {

class Language {
public:
	using Settings = std::vector<std::pair<std::string, std::string>>;

	Language(std::string language);

	const std::string& getLanguage() const noexcept;

	const std::string& getDriver() const noexcept;
	void setDriver(const std::string& driverImplementation);

	const Settings& getDriverSettings() const noexcept;
	void addDriverSetting(const std::string& key, const std::string& value);

	std::set<std::string> getProfiles() const;
	const Settings& getProfileSettings(const std::string& profile) const;
	void addProfileSetting(const std::string& profile, const std::string& key, const std::string& value);

private:
	std::string language;
	std::string driverImplementation;
	Settings driverSettings;

	std::map<std::string, Settings> settingsByProfile;
};

} /* namespace architectures */
} /* namespace transformer */

#endif /* TRANSFORMER_ARCHITECTURES_LANGUAGE_H_ */
