#include <transformer/architectures/Config.h>
#include <transformer/architectures/Architecture.h>
#include <transformer/architectures/Language.h>
#include <transformer/config/architectures/Architecture.h>
#include <transformer/config/architectures/Profile.h>
#include <transformer/Logger.h>

#include <string>
#include <map>
#include <set>
#include <memory>

namespace transformer {
namespace architectures {
namespace {
Logger logger("transformer::architectures::Config");
}

Config::Config(Specifiers& aSpecifiers)
: specifiers(aSpecifiers)
{ }

void Config::load(const boost::filesystem::path& path) {
	configArchitectures.load(path);
	load();
	//transformer::architectures::Architecture architecture;

}

void Config::load(std::istream& iStream) {
	configArchitectures.load(iStream);
	load();
}

void Config::save(std::ostream& oStream) {
	std::map<const Specifier*, std::vector<std::reference_wrapper<const Specifier>>> allBranches = specifiers.getBranches();
	for(const auto& branches : allBranches) {
		oStream << "branch[" << branches.first->getId() << "]:";
		for(const auto& specifier : branches.second) {
			oStream << " " << specifier.get().getId();
		}
		oStream << "\n\n";
	}



	std::set<const Specifier*> allSpecifiers = specifiers.getSpecifiers();

	oStream << "optional:";
	for(const Specifier* specifier : allSpecifiers) {
		if(specifier->getAvailability() == Specifier::isOptional) {
			oStream << " " << specifier->getId();
		}
	}
	oStream << "\n\n";

	oStream << "default:";
	for(const Specifier* specifier : allSpecifiers) {
		if(specifier->getAvailability() == Specifier::isDefault) {
			oStream << " " << specifier->getId();
		}
	}
	oStream << "\n\n";



	std::vector<std::reference_wrapper<const Architecture>> architectures = specifiers.getArchitectures();
	for(const auto& architecture : architectures) {
		oStream << "architecture:";
		for(const auto& id : architecture.get().getArchitectureIds()) {
			oStream << " " << id;
		}
		oStream << "\n\n";

		for(const auto& language : architecture.get().getLanguages()) {
			oStream << "language: " << language.first << "\n";

			oStream << "driver: " << language.second.getDriver() << "\n";
			for(const auto& setting : language.second.getDriverSettings()) {
				oStream << "setting: " << setting.first << " " << setting.second << "\n";
			}
			oStream << "\n";

			std::set<std::string> profiles = language.second.getProfiles();
			for(const auto& profile : profiles) {
				oStream << "profile: " << profile << "\n";
				for(const auto& setting : language.second.getProfileSettings(profile)) {
					oStream << "setting: " << setting.first << " " << setting.second << "\n";
				}
				oStream << "\n";
			}
		}
	}
}

void Config::load() {
	const std::map<std::set<std::string>, std::unique_ptr<config::architectures::Architecture>>& configArchitecturesMap = configArchitectures.getArchitectures();

	for(const auto& configArchitecturesEntry : configArchitecturesMap) {
		if(!configArchitecturesEntry.second) {
			logger.warn << "configArchitecturesEntry.second (std::unique_ptr<config::architectures::Architecture>) is empty.\n";
			continue;
		}
		config::architectures::Architecture& configArchitecture = *configArchitecturesEntry.second;

		Architecture architecture;
		for(const auto& id : configArchitecture.ids) {
			architecture.addArchitectureId(id);
		}

		for(const auto& configLanguageEntry : configArchitecture.languages) {
			if(!configLanguageEntry.second) {
				logger.warn << "configLanguageEntry.second (std::unique_ptr<config::architectures::Language>) is empty.\n";
				continue;
			}
			config::architectures::Language& configLanguage = *configLanguageEntry.second;
			Language& language = architecture.getLanguage(configLanguage.name);

			if(configLanguage.driver) {
				language.setDriver(configLanguage.driver->name);
				for(const auto& driverSetting : configLanguage.driver->settings) {
					language.addDriverSetting(driverSetting.first, driverSetting.second);
				}
			}

			for(const auto& configProfileEntry : configLanguage.profiles) {
				if(!configProfileEntry.second) {
					logger.warn << "configProfileEntry.second (std::unique_ptr<config::architectures::Profile>) is empty.\n";
					continue;
				}

				config::architectures::Profile& configProfile = *configProfileEntry.second;
				for(const auto& profileSetting : configProfile.settings) {
					language.addProfileSetting(configProfile.name, profileSetting.first, profileSetting.second);
				}
			}

		}
		specifiers.addArchitecture(architecture);
	}

	for(const auto& configBranch : configArchitectures.getBranches()) {
		std::set<std::string> ids;
		for(const auto& id : configBranch) {
			ids.insert(id);
		}
		specifiers.addBranch(ids);
	}

	for(const auto& configDefault : configArchitectures.getDefaults()) {
		specifiers.setDefault(configDefault);
	}

	for(const auto& configOptional : configArchitectures.getOptionals()) {
		specifiers.setOptional(configOptional);
	}
}

} /* namespace architectures */
} /* namespace transformer */
