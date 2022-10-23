#include <transformer/architectures/solver/Architecture.h>
#include <transformer/architectures/solver/Languages.h>
#include <transformer/architectures/solver/Tree.h>
#include <transformer/architectures/Language.h>
#include <transformer/Logger.h>

#include <list>
#include <set>
#include <algorithm>

namespace transformer {
namespace architectures {
namespace solver {
namespace {
Logger logger("transformer::architectures::solver::Architecture");
}

Architecture::Architecture(const std::vector<std::reference_wrapper<const Specifier>>& specifiers)
{
	logger.info << "Lookup languages for following build specifiers:\n";
	for(const auto& specifier : specifiers) {
		logger.info << "- \"" << specifier.get().getId() << "\"\n";
	}

	std::set<std::string> languages;
	languages = Languages(specifiers).getLanguages();

	logger.info << "Available languages:\n";
	for(const auto& language : languages) {
		logger.info << "- \"" << language << "\"\n";
	}

	logger.info << "Create Architectures for each language:\n";
	for(const auto& language : languages) {
		Architecture architectureSolver(specifiers, language);
		const Language& otherLanguage = architectureSolver.getArchitecture().getLanguage(language);
		if(otherLanguage.getDriver().empty()) {
			logger.debug << "-> skip language \"" << language << "\": driver is empty\n";
			continue;
		}


		Language& myLanguage = architecture.getLanguage(language);
		myLanguage = otherLanguage;
		logger.info << "- \"" << language << "\"\n";
		logger.info << "-> driver: \"" << myLanguage.getDriver() << "\"\n";
	}
}

Architecture::Architecture(const std::vector<std::reference_wrapper<const Specifier>>& specifiers, const std::string& language)
{
	std::set<std::string> specifierIds;

	/* create an entry for each specifier */
	for(auto specifier : specifiers) {
		specifierIds.insert(specifier.get().getId());
	}

	std::set<const architectures::Architecture*> architectureSet;

	/* iterate again over all specifiers and inspect their architectures */
	for(auto specifier : specifiers) {
		const auto& architecturesMap = specifier.get().getArchitectures();

		for(auto architectureEntry : architecturesMap) {
			const architectures::Architecture& architecture = architectureEntry.second.get();

			/* skip architecture if language is not supported */
			if(architecture.getLanguages().find(language) == architecture.getLanguages().end()) {
				continue;
			}

			/* check if current architecture only contains ids available in buildSpecifiers */
			bool containsAllIds = true;
			for(const auto& id : architecture.getArchitectureIds()) {
				if(specifierIds.find(id) == specifierIds.end()) {
					containsAllIds = false;
					break;
				}
			}

			/* skip architecture if it contains ids that are not available in buildSpecifiers */
			if(containsAllIds == false) {
				continue;
			}

			architectureSet.insert(&architecture);
		}
	}

	logger.debug << "Allowed architectures:\n";
	for(const architectures::Architecture* architecture : architectureSet) {
		logger.debug << "-";
		for(const auto& id : architecture->getArchitectureIds()) {
			logger.debug << " \"" << id << "\"";
		}
		logger.debug << "\n";
	}

	/* create tree */
	Tree tree;
	for(auto specifier : specifiers) {
		tree.add(specifier.get());
	}


	/* now we order our specifiers in tree */
	std::list<std::reference_wrapper<const Tree::Node>> nodeList;

	for(auto node : tree.getChildren()) {
		nodeList.push_back(node);
	}

	/* Collection of all architectures, ordered from lowest to most important architecture.
	 */
	std::list<const architectures::Architecture*> architectureList;
	while(nodeList.empty() == false) {
		const Specifier& specifier = *nodeList.front().get();

		std::vector<std::reference_wrapper<const architectures::Architecture>> specArchs = specifier.getOrderedArchitectures(language);
		logger.debug << "Architectures for specifier ID \"" << specifier.getId() << "\":\n";
		/* iterate architectures of current specifier from less important to most important architecture */
		for(auto architecture : specArchs) {
			logger.debug << "- check architecture...\n";
			logger.debug << "  driver=\"" << architecture.get().getLanguage(language).getDriver() << "\"\n";
#if 1
			logger.debug << "  profiles:";
			std::set<std::string> profileIds = architecture.get().getLanguage(language).getProfiles();
			for(const auto& profileId : profileIds) {
				logger.debug << " \"" << profileId << "\"";
			}
			logger.debug << "\n";
#endif
			/* skip architecture if it is not part of our architectures */
			if(architectureSet.find(&architecture.get()) == architectureSet.end()) {
				logger.debug << "  -> skip architecture\n";
				continue;
			}
			logger.debug << "  -> add architecture\n";

			/* delete current architecture for ordered architectureList if it exists already */
			auto architectureListIter = std::find(architectureList.begin(), architectureList.end(), &architecture.get());
			if(architectureListIter != architectureList.end()) {
				architectureList.erase(architectureListIter);
			}

			architectureList.push_back(&architecture.get());
		}

		/* put children at the end of the list and remove current node from top of the list.
		 */
		for(auto node : nodeList.front().get().getChildren()) {
			nodeList.push_back(node);
		}
		nodeList.pop_front();
	}

	if(architectureList.empty()) {
		std::string specifiersIdsStr;
		for(const auto& specifierId : specifierIds) {
			specifiersIdsStr += " \"" + specifierId + "\"";
		}
		throw std::runtime_error("architectureList.empty() for specifiers" + specifiersIdsStr + " and language \"" + language + "\"");
	}

	/* now create the final architecture form architectureList */
	Language& buildLanguage = architecture.getLanguage(language);

	/* initialize buildLanguage with language def. of first (most important) driver */
	const Language* driverLanguage = nullptr;
	for(auto iter = architectureList.rbegin(); iter != architectureList.rend(); ++iter) {
		const Language& currentLanguage = (*iter)->getLanguage(language);
		if(currentLanguage.getDriver().empty()) {
			continue;
		}

		logger.debug << "FOUND DRIVER: \"" << currentLanguage.getDriver() << "\"\n";

		driverLanguage = &currentLanguage;
		buildLanguage.setDriver(currentLanguage.getDriver());
		for(const auto& setting : currentLanguage.getDriverSettings()) {
			buildLanguage.addDriverSetting(setting.first, setting.second);
		}

		break;
	}

	if(buildLanguage.getDriver().empty()) {
		std::string specifiersIdsStr;
		for(const auto& specifierId : specifierIds) {
			specifiersIdsStr += " \"" + specifierId + "\"";
		}
		logger.debug << "No driver found for specifiers" << specifiersIdsStr << " and language \"" << language << "\"";
		return;
		//throw std::runtime_error("No driver found for specifiers" + specifiersIdsStr + " and language \"" + language + "\"");
	}

	for(auto iter = architectureList.rbegin(); iter != architectureList.rend(); ++iter) {
		const Language& currentLanguage = (*iter)->getLanguage(language);
		if(&currentLanguage != driverLanguage && currentLanguage.getDriver().empty() == false) {
			continue;
		}

		std::set<std::string> profiles = currentLanguage.getProfiles();
		for(const auto& profileId : profiles) {
			if(specifierIds.find(profileId) == specifierIds.end()) {
				logger.debug << "Skip profile id \"" << profileId << "\" because it's not part of current build architecture.\n";
				continue;
			}

			const auto& settings = currentLanguage.getProfileSettings(profileId);
			for(const auto& setting : settings) {
				buildLanguage.addProfileSetting(profileId, setting.first, setting.second);
			}
		}
	}
}

const architectures::Architecture& Architecture::getArchitecture() const noexcept {
	return architecture;
}

} /* namespace solver */
} /* namespace architectures */
} /* namespace transformer */
