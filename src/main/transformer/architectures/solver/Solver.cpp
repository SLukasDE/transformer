#if 0
#include <transformer/architectures/solver/Solver.h>
#include <transformer/architectures/solver/Architecture.h>
#include <transformer/Logger.h>

#include <vector>
#include <functional>

namespace transformer {
namespace architectures {
namespace solver {
namespace {
Logger logger("transformer::architectures::solver::Solver");

std::string spaces(std::size_t depth) {
	std::string rv;
	for(std::size_t i=0; i<depth; ++i) {
		rv += "  ";
	}
	return rv;
}
}

Solver::Solver(const Specifiers& aSpecifiers, const std::set<std::string>& requiredSpecifierIds)
: specifiers(aSpecifiers)
{
	//aSpecifiers.dumpTree();
	std::set<const Specifier*> requiredSpecifiers;

	for(const auto& requiredSpecifierId : requiredSpecifierIds) {
		requiredSpecifiers.insert(&specifiers[requiredSpecifierId]);
	}

	if(logger.trace) {
		logger.trace << "\n";
		logger.trace << "Required specifiers:\n";
		logger.trace << "->";
		dump(requiredSpecifiers);
	}

	std::set<const Specifier*> buildSpecifiers;

	/* fill up buildSpecifiers with requiredSpecifiers and all of it parents */
	for(auto requiredSpecifier : requiredSpecifiers) {
		for(const Specifier* specifier = requiredSpecifier; specifier != nullptr; specifier = specifier->getParent()) {
			if(hasBranchConflict(buildSpecifiers, *specifier)) {
				throw std::runtime_error("Cannot add specifier \"" + specifier->getId() + "\" because it conflicts with branches.");
			}
			buildSpecifiers.insert(specifier);
		}
	}

	/* fill up buildSpecifiers with default child if it's neighbor is not already present */
	for(auto buildSpecifier : buildSpecifiers) {
		for(const Specifier* defaultChild = findDefaultChild(*buildSpecifier); defaultChild; defaultChild = findDefaultChild(*defaultChild)) {
			if(hasBranchConflict(buildSpecifiers, *defaultChild)) {
				// TODO: is this a problem? Why not just "break;" the loop?
				throw std::runtime_error("Cannot add default specifier \"" + defaultChild->getId() + "\" because it conflicts with branches.");
			}
			buildSpecifiers.insert(defaultChild);
		}
	}

	if(logger.trace) {
		logger.trace << "\n";
		logger.trace << "Base specifiers:\n";
		logger.trace << "----------------\n";
		logger.trace << "->";
		dump(buildSpecifiers);
	}

	std::list<std::reference_wrapper<const Specifier>> newSpecifiers;
	newSpecifiers.push_back(std::cref(specifiers[""]));
	std::set<std::set<const Specifier*>> buildsWithUndefined = createBuildSpecifiers(buildSpecifiers, newSpecifiers);

	if(logger.trace) {
		logger.trace << "\n";
		logger.trace << "Build specifiers with undefined specifiers:\n";
		logger.trace << "-------------------------------------------\n";
		dump(buildsWithUndefined);
	}

	std::set<std::set<const Specifier*>> buildsWithoutUndefined;
	for(const std::set<const Specifier*>& buildSpecifiers : buildsWithUndefined) {
		std::set<const Specifier*> specifiers;

		for(const auto& buildSpecifier : buildSpecifiers) {
			if(!buildSpecifier) {
				logger.warn << "buildSpecifier = nullptr\n";
				continue;
			}

			if(buildSpecifier->getType() == Specifier::Type::isUndefined) {
				continue;
			}

			specifiers.insert(buildSpecifier);
		}

		buildsWithoutUndefined.insert(specifiers);
	}

	if(logger.trace) {
		logger.trace << "\n";
		logger.trace << "Build specifiers without undefined specifiers:\n";
		logger.trace << "----------------------------------------------\n";
		dump(buildsWithoutUndefined);
	}

	for(const std::set<const Specifier*>& buildSpecifiers : buildsWithoutUndefined) {
		std::vector<std::reference_wrapper<const Specifier>> buildSpecifiersRef;
		std::set<std::string> buildSpecifierIds;

		for(const auto& buildSpecifier : buildSpecifiers) {
			if(!buildSpecifier) {
				logger.warn << "buildSpecifier = nullptr\n";
				continue;
			}
			if(buildSpecifier->getType() == Specifier::Type::isUndefined) {
				continue;
			}

			buildSpecifiersRef.push_back(std::cref(*buildSpecifier));
			buildSpecifierIds.insert(buildSpecifier->getId());
		}

		architectures::Architecture architecture = Architecture(buildSpecifiersRef).getArchitecture();
		if(architecture.getLanguages().empty()) {
			continue;
		}

		std::set<std::string> reducedBuildSpecifierIds;
		for(const auto& buildSpecifierId : buildSpecifierIds) {
			std::size_t minSize = buildSpecifierId.size() + 1;
			bool isSubId = false;

			/* verify if buildSpecifierId is a SubId of another ID existing in buildSpecifierIds */
			for(const auto& verifyBuildSpecifierId : buildSpecifierIds) {
				if(verifyBuildSpecifierId.size() < minSize) {
					continue;
				}
				if(verifyBuildSpecifierId.substr(0, buildSpecifierId.size()) == buildSpecifierId) {
					isSubId = true;
					break;
				}
			}

			if(isSubId == false) {
				reducedBuildSpecifierIds.insert(buildSpecifierId);
			}
		}
		buildArchitectures[reducedBuildSpecifierIds] = architecture;
	}
}

std::map<std::set<std::string>, architectures::Architecture> Solver::getBuildArchitectures() const noexcept {
	return buildArchitectures;
}

std::set<std::set<const Specifier*>> Solver::createBuildSpecifiers(std::set<const Specifier*> buildSpecifiers, std::list<std::reference_wrapper<const Specifier>> newSpecifiers, std::size_t depth) const {
	std::set<std::set<const Specifier*>> rv;

	if(logger.trace) {
		logger.trace << spaces(depth) << "Depth: \"" << depth << "\"\n";
		logger.trace << spaces(depth) << "-------------------------------------\n";
	}

	while(newSpecifiers.empty() == false) {
		const Specifier& newSpecifier = newSpecifiers.front().get();
		newSpecifiers.pop_front();

		if(logger.trace) {
			logger.trace << spaces(depth) << "Current specifier: \"" << newSpecifier.getId() << "\"\n";
			logger.trace << spaces(depth) << "-------------------------------------\n";
		}

		if(newSpecifier.getType() == Specifier::isUndefined && specifiers.getBranches(newSpecifier).empty()) {
			buildSpecifiers.erase(&newSpecifier);
		}
		else {
			/* skip child with branch conflict */
			if(hasBranchConflict(buildSpecifiers, newSpecifier)) {
				if(logger.trace) {
					logger.trace << spaces(depth) << "- \"" << newSpecifier.getId() << "\" (branch conflict detected)\n";
				}
				continue;
			}
			buildSpecifiers.insert(&newSpecifier);
		}

		if(logger.trace) {
			logger.trace << spaces(depth) << "Current build specifiers :";
			for(auto buildSpecifier : buildSpecifiers) {
				logger.trace << " \"" << buildSpecifier->getId() << "\"";
			}
			logger.trace << "\n";
			logger.trace << spaces(depth) << "Remaining build specifiers :";
			for(auto newSpecifier : newSpecifiers) {
				logger.trace << " \"" << newSpecifier.get().getId() << "\"";
			}
			logger.trace << "\n";
		}

		std::vector<std::reference_wrapper<const Specifier>> allChildrenRef = newSpecifier.getChildren();

		if(logger.trace) {
			logger.trace << spaces(depth) << "Children (original):";
			for(auto child : allChildrenRef) {
				logger.trace << " \"" << child.get().getId() << "\"";
			}
			logger.trace << "\n";
		}

		/* first check if there is already a default child present and it exists as child in buildSpecifiers.
		 * If yes, handle child from buildSpecifiers as defaultChild
		 */
		const Specifier* defaultChild = nullptr;
		const Specifier* buildSpecifierChild = nullptr;
		for(auto childRef : allChildrenRef) {
			if(buildSpecifiers.find(&childRef.get()) != buildSpecifiers.end()) {
				buildSpecifierChild = &childRef.get();
			}

			if(childRef.get().getAvailability() == Specifier::isDefault) {
				defaultChild = &childRef.get();
			}
		}
		if(defaultChild && buildSpecifierChild) {
			defaultChild = buildSpecifierChild;
			if(logger.trace) {
				logger.trace << spaces(depth) << "- \"" << buildSpecifierChild->getId() << "\" (build specifier detected)\n";
			}
		}

		/* first collect all children form currentSpecifier without branch conflicts and reduce to default child if available */
		std::set<const Specifier*> children;
		if(defaultChild) {
			if(logger.trace) {
				logger.trace << spaces(depth) << "- \"" << defaultChild->getId() << "\" (default child detected)\n";
			}
			if(!hasBranchConflict(buildSpecifiers, *defaultChild)) {
				children.insert(defaultChild);
			}
			else if(logger.trace) {
				logger.trace << spaces(depth) << "- \"" << defaultChild->getId() << "\" (branch conflict detected)\n";
			}
		}
		else {
			for(auto childRef : allChildrenRef) {
				/* skip optional specifier */
				if(childRef.get().getAvailability() == Specifier::isOptional) {
					if(logger.trace) {
						logger.trace << spaces(depth) << "- \"" << childRef.get().getId() << "\" (optional child detected)\n";
					}
					continue;
				}

				/* skip child with branch conflict */
				if(hasBranchConflict(buildSpecifiers, childRef.get())) {
					if(logger.trace) {
						logger.trace << spaces(depth) << "- \"" << childRef.get().getId() << "\" (branch conflict detected)\n";
					}
					continue;
				}

				if(childRef.get().getType() == Specifier::isUndefined && specifiers.getBranches(childRef.get()).empty()) {
					newSpecifiers.push_back(childRef);
				}
				else {
					children.insert(&childRef.get());
				}
			}
		}

		if(logger.trace) {
			logger.trace << spaces(depth) << "Children (filtered):";
			for(auto child : children) {
				logger.trace << " \"" << child->getId() << "\"";
			}
			logger.trace << "\n";
		}

		std::set<std::set<const Specifier*>> childrenCombinations = createChildCombinations(buildSpecifiers, children);

		if(logger.trace) {
			logger.trace << spaces(depth) << "Combinations:\n";
			for(auto childrenCombination : childrenCombinations) {
				logger.trace << spaces(depth) << "-";
				for(auto child : childrenCombination) {
					logger.trace << " \"" << child->getId() << "\"";
				}
				logger.trace << "\n";
			}
			logger.trace << "\n";
		}

		if(childrenCombinations.empty()) {
			continue;
		}

		if(childrenCombinations.size() == 1) {
			const std::set<const Specifier*>& newChildren = *childrenCombinations.begin();

			for(const Specifier* newChild : newChildren) {
				newSpecifiers.push_back(std::cref(*newChild));
			}

			continue;
		}

		for(const auto& newChildren : childrenCombinations) {
			std::list<std::reference_wrapper<const Specifier>> newSpecifiersBranch = newSpecifiers;
			for(const Specifier* newChild : newChildren) {
				newSpecifiersBranch.push_back(std::cref(*newChild));
			}

			std::set<std::set<const Specifier*>> partResults = createBuildSpecifiers(buildSpecifiers, std::move(newSpecifiersBranch), depth + 1);
			for(const auto& partResult : partResults) {
				rv.insert(partResult);
			}
		}
		break;
	}

	rv.insert(buildSpecifiers);
	return rv;
}

std::set<std::set<const Specifier*>> Solver::createChildCombinations(const std::set<const Specifier*>& buildSpecifiers, std::set<const Specifier*> childrenSpecifiers) const {
	std::set<std::set<const Specifier*>> rv;
	rv.insert(std::set<const Specifier*>());

	for(auto childSpecifier: childrenSpecifiers) {
		/* **************************************** *
		 * Double entries in rv with childSpecifier *
		 * **************************************** */

		std::set<std::set<const Specifier*>> withChild = rv;
		for(const auto& specifiers : withChild) {
			std::set<const Specifier*> newBuildSpecifiers = buildSpecifiers;
			for(const Specifier* specifier : specifiers) {
				newBuildSpecifiers.insert(specifier);
			}

			if(hasBranchConflict(newBuildSpecifiers, *childSpecifier)) {
				continue;
			}

			std::set<const Specifier*> newCombination = specifiers;
			newCombination.insert(childSpecifier);
			rv.insert(newCombination);
		}

	}
	return rv;
}

const Specifier* Solver::findDefaultChild(const Specifier& specifier) const {
	std::vector<std::reference_wrapper<const Specifier>> children = specifier.getChildren();

	for(const auto& refChild : children) {
		if(refChild.get().getAvailability() == Specifier::isDefault) {
			return &refChild.get();
		}
	}

	return nullptr;
}

bool Solver::hasBranchConflict(const std::set<const Specifier*>& buildSpecifiers, const Specifier& newSpecifier) const {
	/* check if specifier exists already */
	if(buildSpecifiers.find(&newSpecifier) != buildSpecifiers.end()) {
		return false;
	}

	for(auto specifierRequired : buildSpecifiers) {
		const std::set<const Specifier*>& branches = specifiers.getBranches(specifierRequired->getId());
		if(branches.find(&newSpecifier) != branches.end()) {
			return true;
		}
	}

	return false;
}

void Solver::dump(const std::set<const Specifier*> specifiers) const {
	std::map<std::string, std::reference_wrapper<const Specifier>> specifiersMap;
	for(const Specifier* specifier : specifiers) {
		if(specifier) {
			specifiersMap.insert(std::make_pair(specifier->getId(), std::cref(*specifier)));
		}
	}

	for(const auto& specifier : specifiersMap) {
		switch(specifier.second.get().getType()) {
		case Specifier::Type::isArchitecture:
			logger.trace << " A:";
			break;

		case Specifier::Type::isProfile:
			logger.trace << " P:";
			break;

		case Specifier::Type::isUndefined:
			logger.trace << " U:";
			break;
		}

		logger.trace << " \"" << specifier.first << "\"";
	}
	logger.trace << "\n";
}

void Solver::dump(const std::set<std::set<const Specifier*>>& specifiersSet) const {
	int i=0;
	for(const std::set<const Specifier*>& specifiers : specifiersSet) {
		logger.trace << "#" << ++i << ":";
		dump(specifiers);
	}
}


} /* namespace solver */
} /* namespace architectures */
} /* namespace transformer */
#endif
