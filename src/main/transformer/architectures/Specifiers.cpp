#include <transformer/architectures/Specifiers.h>
#include <transformer/Logger.h>

#include <stdexcept>

namespace transformer {
namespace architectures {

namespace {
Logger logger("transformer::architectures::Specifiers");
}

void Specifiers::dumpTree() const {
	logger.debug << "Tree:\n";
	logger.debug << "-----\n";

	auto iter = primarySpecifiers.find("");
	if(iter == primarySpecifiers.end()) {
		return;
	}
	if(!iter->second) {
		return;
	}
	dumpTree(*iter->second, 0);
}

void Specifiers::dumpTree(const Specifier& specifier, std::size_t depth) const {
	for(std::size_t i=0; i<depth; ++i) {
		logger.debug << "|   ";
	}
	logger.debug << "+-> ";
	switch(specifier.getType()) {
	case Specifier::Type::isArchitecture:
		logger.debug << "A:";
		break;

	case Specifier::Type::isProfile:
		logger.debug << "P:";
		break;

	case Specifier::Type::isUndefined:
		logger.debug << "U:";
		break;
	}
	logger.debug << " \"" << specifier.getId() << "\"";
	if(specifier.getAvailability() == Specifier::Availability::isDefault) {
		logger.debug << " (default)";
	}
	if(specifier.getAvailability() == Specifier::Availability::isOptional) {
		logger.debug << " (optional)";
	}
	logger.debug << "\n";

	std::vector<std::reference_wrapper<const Specifier>> children = specifier.getChildren();
	for(auto child : children) {
		dumpTree(child.get(), depth+1);
	}
}

void Specifiers::print() const {
#if 0
	logger.debug << "Shortcuts:\n";
	logger.debug << "----------\n";
	for(const auto& specifierEntry : secondarySpecifiers) {
		std::string str = specifierEntry.first;
		while(str.size() < 10) {
			str += " ";
		}
		logger.debug << "- \"" << str << "\" -> (" << &specifierEntry.second.get() << ") \"" << specifierEntry.second.get().getId() << "\"\n";
	}
	logger.debug << "\n";

	logger.debug << "===============\n";
	logger.debug << "\n";
#endif
	logger.debug << "Specifiers (Architecture):\n";
	logger.debug << "--------------------------\n";
	for(const auto& specifierEntry : primarySpecifiers) {
		if(specifierEntry.second->getType() == Specifier::Type::isArchitecture) {
			logger.debug << "- \"" << specifierEntry.first << "\"\n";
		}
	}
	logger.debug << "\n";

	logger.debug << "Specifiers (Profile):\n";
	logger.debug << "---------------------\n";
	for(const auto& specifierEntry : primarySpecifiers) {
		if(specifierEntry.second->getType() == Specifier::Type::isProfile) {
			logger.debug << "- \"" << specifierEntry.first << "\"\n";
		}
	}
	logger.debug << "\n";

	logger.debug << "Specifiers (Undefined):\n";
	logger.debug << "-----------------------\n";
	for(const auto& specifierEntry : primarySpecifiers) {
		if(specifierEntry.second->getType() == Specifier::Type::isUndefined) {
			logger.debug << "- \"" << specifierEntry.first << "\"\n";
		}
	}
	logger.debug << "\n";
}

std::set<const Specifier*> Specifiers::getSpecifiers() const {
	std::set<const Specifier*> rv;

	for(const auto& specifier : primarySpecifiers) {
		rv.insert(specifier.second.get());
	}

	return rv;
}

const Specifier* Specifiers::getSpecifier(const std::string& id) const {
	auto iter = secondarySpecifiers.find(id);

	if(iter == secondarySpecifiers.end()) {
		return nullptr;
	}

	return &iter->second.get();
}

const Specifier& Specifiers::operator[](const std::string& id) const {
	const Specifier* specifier = getSpecifier(id);

	if(specifier == nullptr) {
		throw std::runtime_error("using operator[id] for unknown specifier with id=\"" + id + "\"");
	}

	return *specifier;
}


void Specifiers::addBranch(const std::set<std::string>& ids) {
	/* a single id makes no sense for a branch */
	if(ids.size() <= 1) {
		return;
	}

	std::set<Specifier*> specifiers;
	for(const auto& id : ids) {
		specifiers.insert(&addSpecifier(id, Specifier::isUndefined));
	}

	for(auto specifier1 : specifiers) {
		std::set<const Specifier*>& entry = branchesBySpecifier[specifier1];

		for(auto specifier2 : specifiers) {
			entry.insert(specifier2);
		}
	}
}

const std::set<const Specifier*>& Specifiers::getBranches(const std::string& id) const {
	const Specifier& specifier = (*this)[id];

	return getBranches(specifier);
}

const std::set<const Specifier*>& Specifiers::getBranches(const Specifier& specifier) const {
	static std::set<const Specifier*> emtpyBranches;

	auto iter = branchesBySpecifier.find(&specifier);
	if(iter == branchesBySpecifier.end()) {
		return emtpyBranches;
	}

	return iter->second;
}

std::map<const Specifier*, std::vector<std::reference_wrapper<const Specifier>>> Specifiers::getBranches() const {
	std::map<const Specifier*, std::vector<std::reference_wrapper<const Specifier>>> rv;

	for(const auto& branches : branchesBySpecifier) {
		std::vector<std::reference_wrapper<const Specifier>>& specifiers = rv[branches.first];
		for(auto specifier : branches.second) {
			specifiers.push_back(std::cref(*specifier));
		}
	}

	return rv;
}

void Specifiers::addArchitecture(Architecture architecture) {
	/* check if there exists already an architecture with exactly this ids */
	auto iter = architectureByIds.find(architecture.getArchitectureIds());
	if(iter != architectureByIds.end()) {
		/* if yes, then remove the existing architecture to *
		 * enable it to override it by this architecture    */
		for(const auto& id : architecture.getArchitectureIds()) {
			addSpecifier(id, Specifier::isArchitecture).removeArchitecture(architecture.getArchitectureIds());
		}
	}

	/* now set or override the registry by this architecture */
	Architecture* architecturePtr = new Architecture(architecture);
	architectureByIds[architecture.getArchitectureIds()] = std::unique_ptr<Architecture>(architecturePtr);

	/* enrich specifiers defined as ids in this architecture */
	for(const auto& id : architecture.getArchitectureIds()) {
		addSpecifier(id, Specifier::isArchitecture).addArchitecture(*architecturePtr);
	}

	for(const auto& languagesEntry : architecture.getLanguages()) {
		/* enrich specifiers defined as profiles in this architecture */
		std::set<std::string> profiles = languagesEntry.second.getProfiles();
		for(const auto& id : profiles) {
			addSpecifier(id, Specifier::isProfile).addArchitecture(*architecturePtr);
		}
	}
}

std::vector<std::reference_wrapper<const Architecture>> Specifiers::getArchitectures() const {
	std::vector<std::reference_wrapper<const Architecture>> rv;

	for(const auto& architecture : architectureByIds) {
		rv.push_back(std::cref(*architecture.second));
	}

	return rv;
}

void Specifiers::setOptional(const std::string& id) {
	Specifier& specifier = addSpecifier(id, Specifier::isUndefined);

	/* check if this specifier is set already to default availability */
	if(specifier.getAvailability() == Specifier::isDefault) {
		throw std::runtime_error("cannot set specifier to optional availability because it has been set already to default availability");
	}

	/* now we can set the availability to optional */
	specifier.setAvailability(Specifier::isOptional);
}

void Specifiers::setDefault(const std::string& id) {
	Specifier& specifier = addSpecifier(id, Specifier::isUndefined);

	/* first check if this specifier is set already to optional availability */
	if(specifier.getAvailability() == Specifier::isOptional) {
		throw std::runtime_error("cannot set specifier \"" + id + "\" to default availability because it has been set already to optional availability");
	}

	/* second check if another neighbor specifier is set already to default availability */
	if(specifier.getParent()) {
		std::vector<std::reference_wrapper<const Specifier>> neighbors = specifier.getParent()->getChildren();
		for(auto& neighbor : neighbors) {
			if(&neighbor.get() == &specifier) {
				continue;
			}
			if(neighbor.get().getAvailability() == Specifier::isDefault) {
				throw std::runtime_error("cannot set specifier \"" + id + "\" to default availability because it's neighbor  \"" + neighbor.get().getId() + "\"  has been set already to default availability");
			}
		}
	}

	/* now we can set the availability to default */
	specifier.setAvailability(Specifier::isDefault);
}

Specifier& Specifiers::addSpecifier(const std::string& id, Specifier::Type type) {
	std::unique_ptr<Specifier>& primarySpecifier = primarySpecifiers[id];

	/* check if there is already a primary entry for this id */
	if(primarySpecifier) {
		/* if yes, we have to merge the existing primary specifier with our type */

		switch(type) {
		case Specifier::isUndefined:
			type = primarySpecifier->getType();
			break;

		case Specifier::isProfile:
			if(primarySpecifier->getType() == Specifier::isArchitecture) {
				throw std::runtime_error("cannot add profile \"" + id + "\" because there exists already an architecture with same id.");
			}
			break;

		case Specifier::isArchitecture:
			if(primarySpecifier->getType() == Specifier::isProfile) {
				throw std::runtime_error("cannot add architecture \"" + id + "\" because there exists already a profile with same id.");
			}
			break;
		}

		primarySpecifier->setType(type);

		/* *********** *
		 * we are done *
		 * *********** */
		//return *primarySpecifier;
	}
	else {
		/* we add our id to primarySpecifiers */
		primarySpecifier = std::unique_ptr<Specifier>(new Specifier(*this, id, type));

		/* Check if there exists already an entry in secondarySpecifier of this ID.
		 * If yes, we have to replace it with our primarySpecifier.
		 */

		Specifier* specifierToReplace = nullptr;
		auto iter = secondarySpecifiers.find(id);
		if(iter != secondarySpecifiers.end()) {
			specifierToReplace = &iter->second.get();
		}

		for(std::size_t i = id.size(); true; --i) {
			std::string subId(id.substr(0, i));
			auto iter = secondarySpecifiers.find(subId);

			/* Check if there exists a SECONDARY specifier. */
			if(iter != secondarySpecifiers.end()) {
				Specifier& secondarySpecifier = iter->second.get();

				/* Stop if subId is available as PRIMARY specifier. */
				if(subId == secondarySpecifier.getId()) {
					break;
				}

				if(specifierToReplace != &secondarySpecifier) {
					/* it exists a secondary specifier, but it's not our own primary specifier
					 * and there exists no primary specifier with this subId.
					 * -> so, it's a secondary specifier form another id but now we have same subId and we must establish a new primary specifier.
					 *
					 * Example:
					 * there exists already a primary specifier with id "gcc-4.8" and now we add a "primary" specifier with "gcc-5.0".
					 * Then, there exists already a secondary specifier with key "gcc-" pointing to primary specifier "gcc-4.8"
					 * Now we have to replace all secondary keys beginning with "gcc-" ending to another primary key by a new primary specifier.
					 */

					if(i < id.size()) {
						addSpecifier(subId, Specifier::isUndefined);
						break;
					}
				}

				secondarySpecifiers.erase(iter);
			}

			secondarySpecifiers.insert(std::make_pair(subId, std::ref(*primarySpecifier.get())));

			if(i==0) {
				break;
			}
		}
	}

	return *primarySpecifier;
}

} /* namespace architectures */
} /* namespace transformer */
