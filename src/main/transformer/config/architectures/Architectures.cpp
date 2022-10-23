#include <transformer/config/architectures/Architectures.h>
#include <transformer/config/architectures/Language.h>
#include <transformer/config/architectures/Driver.h>
#include <transformer/config/architectures/Profile.h>
#include <transformer/config/Exception.h>
#include <transformer/Logger.h>

#include <esl/utility/String.h>

#include <vector>
#include <set>
#include <utility>

namespace transformer {
namespace config {
namespace architectures {

namespace {
Logger logger("transformer::config::architectures::Architectures");
/*
std::map<std::set<std::string>, std::unique_ptr<Architecture>> createArchitectures() {
	std::map<std::set<std::string>, std::unique_ptr<Architecture>> architectures;

	architectures.insert(std::make_pair(std::set<std::string>(), std::unique_ptr<Architecture>(new Architecture)));

	return architectures;
}
*/

std::string makeArchitectureNameString(const std::set<std::string>& architectureName) {
	std::string name;

	for(const auto& token : architectureName) {
		if(!name.empty()) {
			name += " ";
		}
		name += token;
	}

	return name;
}

std::vector<std::string> readListLowerCase(Parser& parser) {
	/* read ids after "branch: " */
	std::vector<std::string> ids = parser.readList();

	/* convert ids to lower case */
	for(auto& id : ids) {
		id = esl::utility::String::toLower(id);
	}

	return ids;
}
}

Architectures::Architectures()
: //architectures(createArchitectures()),
  activeArchitecture(new Architecture)
{
	//activeArchitecture = new Architecture;
	architectures[activeArchitecture->ids].reset(activeArchitecture);
//	activeProfile = nullptr;
}

const std::vector<std::vector<std::string>>& Architectures::getBranches() const noexcept {
	return branches;
}

const std::set<std::string>& Architectures::getDefaults() const noexcept {
	return defaults;
}

const std::set<std::string>& Architectures::getOptionals() const noexcept {
	return optionals;
}

const std::map<std::set<std::string>, std::unique_ptr<Architecture>>& Architectures::getArchitectures() const noexcept {
	return architectures;
}

void Architectures::load(const boost::filesystem::path& path) {
	Parser parser(path);
	load(parser);
	/*
	if(!boost::filesystem::exists(path)) {
		throw Exception(parser, "File \"" + path.generic_string() + "\" does not exists.");
	}
	if(!boost::filesystem::is_regular(path)) {
		throw Exception(parser, "Path \"" + path.generic_string() + "\" is not a file.");
	}
	std::ifstream istream(path.string());

	load(istream);
	*/
}

void Architectures::load(std::istream& iStream) {
	Parser parser(iStream);
	load(parser);
}

void Architectures::load(Parser& parser) {
	Language* activeLanguage = nullptr;
	Driver* activeDriver = nullptr;
	Profile* activeProfile = nullptr;

	while(parser.nextLine())  {
		std::string token;

		parser.readToken(token);
		token = esl::utility::String::trim(token);

		if(token.empty()) {
			continue;
		}

		if(token == "branch:") {
			activeArchitecture = nullptr;
			activeLanguage = nullptr;
			activeDriver = nullptr;
			activeProfile = nullptr;

			/* read tokens after "branch: " */
			std::vector<std::string> branchTokens = readListLowerCase(parser);

			/* add tokens to list of branches */
			branches.push_back(branchTokens);

			continue;
		}

		if(token == "default:") {
			activeArchitecture = nullptr;
			activeLanguage = nullptr;
			activeDriver = nullptr;
			activeProfile = nullptr;

			/* read tokens after "default: " */
			std::vector<std::string> defaultTokens = readListLowerCase(parser);

			/* add tokens to default list */
			for(const auto& defaultToken : defaultTokens) {
				defaults.insert(defaultToken);
			}

			continue;
		}

		if(token == "optional:") {
			activeArchitecture = nullptr;
			activeLanguage = nullptr;
			activeDriver = nullptr;
			activeProfile = nullptr;

			/* read tokens after "optional: " */
			std::vector<std::string> optionalTokens = readListLowerCase(parser);

			/* add tokens to optional list */
			for(const auto& optionalToken : optionalTokens) {
				optionals.insert(optionalToken);
			}

			continue;
		}

		if(token == "architecture:") {
			/* read tokens after "architecture: " */
			std::vector<std::string> architectureToken = readListLowerCase(parser);

			/* Generally it is not allowed to switch to an architecture that exists already.
			 * If you open a architectures configuration file the parser has switched already to the blank architecture,
			 * but it is a good style to start your configuration file by explicit switching to the blank architecture if you
			 * want do make definitions there. So, we allow to switch to this architecture even if is selected already.
			 */
			if(architectureToken.empty() && activeArchitecture && activeArchitecture->ids.empty()) {
				continue;
			}

			std::set<std::string> architectureName(architectureToken.begin(), architectureToken.end());

			/* check if architecture name exists already */
			auto iter = architectures.find(architectureName);
			if(iter != architectures.end()) {
				std::string name = makeArchitectureNameString(architectureName);
				throw Exception(parser, "multiple definition of architecture \"" + name + "\" is not allowed");
			}

			activeArchitecture = new Architecture;
			activeArchitecture->ids = architectureName;
			architectures.insert(std::make_pair(activeArchitecture->ids, std::unique_ptr<Architecture>(activeArchitecture)));
			activeLanguage = nullptr;
			activeDriver = nullptr;
			activeProfile = nullptr;

			continue;
		}

		if(token == "language:") {
			if(!activeArchitecture) {
				throw Exception(parser, "definition of driver is not allowed without architecture");
			}

			/* read language-name */
			std::string languageName;
			parser.readLastToken(languageName);
			languageName = esl::utility::String::toLower(languageName);

			std::unique_ptr<Language>& languagePtr = activeArchitecture->languages[languageName];
			if(languagePtr) {
				std::string architectureName = makeArchitectureNameString(activeArchitecture->ids);
				throw Exception(parser, "multiple definition of language \"" + languageName + "\" within architecture \"" + architectureName + "\"is not allowed");
			}

			activeLanguage = new Language;
			activeLanguage->name = languageName;
			languagePtr.reset(activeLanguage);
			activeDriver = nullptr;
			activeProfile = nullptr;

			continue;
		}

		if(token == "driver:") {
			if(!activeArchitecture) {
				throw Exception(parser, "definition of settings are not allowed without architecture");
			}

			if(!activeLanguage) {
				throw Exception(parser, "definition of driver is not allowed without language");
			}

			if(activeLanguage->driver) {
				std::string architectureName = makeArchitectureNameString(activeArchitecture->ids);
				throw Exception(parser, "multiple definition of driver for architecture \"" + architectureName + "\" is not allowed");
			}

			activeDriver = new Driver;
			activeLanguage->driver.reset(activeDriver);
			activeProfile = nullptr;

			/* read driver-name */
			parser.readLastToken(activeDriver->name);

			continue;
		}

		if(token == "profile:") {
			if(!activeArchitecture) {
				throw Exception(parser, "definition of profile are not allowed without architecture");
			}

			if(!activeLanguage) {
				std::string architectureName = makeArchitectureNameString(activeArchitecture->ids);
				throw Exception(parser, "definition of profile within architecture \"" + architectureName + "\" is not allowed without language");
			}

			/* read profile-name */
			std::string profileName;
			parser.readLastToken(profileName);
			profileName = esl::utility::String::toLower(profileName);


			/* check if profile name exists already for active architecture */
			auto iter = activeLanguage->profiles.find(profileName);
			if(iter != activeLanguage->profiles.end()) {
				std::string architectureName = makeArchitectureNameString(activeArchitecture->ids);
				throw Exception(parser, "multiple definition of profile \"" + profileName + "\" for architecture \"" + architectureName + "\" and language \"" + activeLanguage->name + "\" is not allowed");
			}

			activeDriver = nullptr;
			activeProfile = new Profile;
			activeProfile->name = profileName;
			activeLanguage->profiles[profileName].reset(activeProfile);
			//activeArchitecture->profiles.insert(std::make_pair(profileName, std::unique_ptr<Profile>(activeProfile)));

			continue;
		}

		if(token == "setting:") {
			if(!activeArchitecture) {
				throw Exception(parser, "definition of settings are not allowed without architecture");
			}

			if(!activeLanguage) {
				throw Exception(parser, "definition of profile is not allowed without language");
			}

			/* read setting-key */
			std::string settingKey;
			parser.readToken(settingKey);

			/* read setting-value */
			std::string settingValue;
			parser.readLastToken(settingValue);

			if(activeProfile) {
				activeProfile->settings.push_back(std::make_pair(settingKey, settingValue));
			}
			else if(activeDriver) {
				activeDriver->settings.push_back(std::make_pair(settingKey, settingValue));
			}
			else {
				throw Exception(parser, "definition of settings without driver or profile context");
			}

			continue;
		}

		throw Exception(parser, "unknown token \"" + token + "\"");
	}
}

} /* namespace architectures */
} /* namespace config */
} /* namespace transformer */
