#include <transformer/build/Solver.h>
#include <transformer/model/Variant.h>
#include <transformer/model/Dependency.h>
#include <transformer/Logger.h>

#include <esl/system/Stacktrace.h>

#include <iostream>

namespace transformer {
namespace build {

namespace {
Logger logger("transformer::build::Solver");
}

Solver::Solver(repository::Database& aDatabase, const model::Variant& variant)
: database(aDatabase),
  architectures(variant.getArchitecturesEffective())
{
	for(model::Dependency& dependency : variant.getDependenciesEffective()) {
		requiredOrdered.emplace_back(dependency.id);
		required.emplace(dependency.id, dependency);
	}
}

SolvedDescriptors Solver::solve() {
	std::vector<SolvedDescriptor> result;

	if(requiredOrdered.empty()) {
		return SolvedDescriptors(result);
	}

	result = solveInternal();

	if(result.empty()) {
		throw esl::system::Stacktrace::add(std::runtime_error("cannot solve descriptor"));
	}

	return SolvedDescriptors(result);
}

std::vector<SolvedDescriptor> Solver::solveInternal() {
	std::vector<SolvedDescriptor> result;

	while(!requiredOrdered.empty()) {
		model::Dependency dependency;
		{
			auto iter = required.find(requiredOrdered.front());
			if(iter == std::end(required)) {
				throw esl::system::Stacktrace::add(std::runtime_error("ERROR: This should never happen. Cannot find dependency \"" + requiredOrdered.front() + "\" in \"required\"."));
			}
			dependency = iter->second;
			requiredOrdered.pop_front();
			required.erase(iter);
		}

		std::cout << "Solving \""  << dependency.id << "\"\n";

		/* get list of "solvedDiscreptors" according to "dependency". */
		std::vector<SolvedDescriptor> solvedDescriptors = createReducedOrderedSolvedDescirptors(dependency);

		if(solvedDescriptors.empty()) {
			std::cout << "für den aktuell betrachteten Strang konnte keine Lösung gefunden werden, da für die Dependency \"" << dependency.id << "\" kein passender Descriptor gefunden wurde.\n";
			// empty list
			return result;
		}

		for(auto& solvedDescriptor : solvedDescriptors) {

			if(!solvedDescriptor.getVariantName().empty() && !solvedDescriptor.getDescriptor().hasSpecialVariant(solvedDescriptor.getVariantName())) {
				std::cerr << "ERROR: This shoud not happen: Descriptor \"" << solvedDescriptor.getArtefactId() << "[" << solvedDescriptor.getArtefactVersion() << "] has no variant \"" << solvedDescriptor.getVariantName() << "\"\n";
				continue;
			}

			std::list<std::string> availableOrderedSaved;
			std::map<std::string, SolvedDescriptor> availableSaved;

			std::list<std::string> requiredOrderedSaved;
			std::map<std::string, model::Dependency> requiredSaved;

			availableOrderedSaved = availableOrdered;
			availableSaved = available;
			availableAddOrReduce(solvedDescriptor);

			/* if "solvedDescriptor" was already available in "available" then continue with next required dependency */
			if(availableSaved.size() == available.size()) {
				break;
			}

			requiredOrderedSaved = requiredOrdered;
			requiredSaved = required;
			for(model::Dependency& dependency : solvedDescriptor.getVariant().getDependenciesEffective()) {
				requiredAddOrReduce(dependency);
			}

			result = solveInternal();
			if(!result.empty()) {
				return result;
			}

			/* reject this path, continue and try with next "solvedDescriptor" */
			required = requiredSaved;
			requiredOrdered = requiredOrderedSaved;

			available = availableSaved;
			availableOrdered = availableOrderedSaved;
		}
	}

	for(const auto& artefactName : availableOrdered) {
		auto iter = available.find(artefactName);
		if(iter == std::end(available)) {
			throw esl::system::Stacktrace::add(std::runtime_error("ERROR: This should never happen. Cannon find artefactName \"" + artefactName + "\" in map \"available\"."));
		}

		/* reduce solved descriptor last time to real used library type */
		if(iter->second.hasStatic) {
			iter->second.hasDynamic = false;
		}

		result.emplace_back(iter->second);
	}

	return result;
}

std::vector<SolvedDescriptor> Solver::createOrderedSolvedDescirptors(const model::Dependency& dependency, bool reduceToAvailableVersion) const {
	std::vector<SolvedDescriptor> solvedDescriptors;

	std::map<model::VersionNumber, std::reference_wrapper<const model::Descriptor>> descriptorsOrderedByVersionNumber;
	{
		std::set<std::string> artefactVersions;

		if(reduceToAvailableVersion) {
			/* find "dependcy.name" in "availableSolvedDescriptorByName" */
			auto iter = available.find(dependency.id);
			if(iter != std::end(available)) {
				artefactVersions = { iter->second.getArtefactVersion() };
			}
		}

		if(artefactVersions.empty()) {
			artefactVersions = database.getArtefactVersions(dependency.id, dependency.versionRanges);
		}

		for(const auto& artefactVersion : artefactVersions) {
			const model::Descriptor* descriptor = database.getDescriptor(dependency.id, artefactVersion);
			if(descriptor == nullptr) {
				throw esl::system::Stacktrace::add(std::runtime_error("should not happen: cannot load descriptor \"" + dependency.id + "[" + artefactVersion + "]\""));
			}
			descriptorsOrderedByVersionNumber.emplace(model::VersionNumber(artefactVersion), *descriptor);
		}
	}

	for(const auto& entry : descriptorsOrderedByVersionNumber) {
		// there is a special variant forced to use
		if(!dependency.variantName.empty()) {
			// if descriptor does not have this variant then skip this artefact version
			if(!entry.second.get().hasSpecialVariant(dependency.variantName)) {
				// continue with next version
				continue;
			}
			// add this forced variant into the list and continue with next version
			solvedDescriptors.emplace_back(entry.second.get().getVariant(dependency.variantName), dependency.variantName);
			continue;
		}

		// fill all available variants to the list

		// if descriptor only has a default variant, then add default variant into the list
		if(!entry.second.get().hasSpecialVariants()) {
			solvedDescriptors.emplace_back(entry.second.get().getVariant(""), "");
			// continue with next version
			continue;
		}

		// if there is a special default variant, then put this variant first into the list
		if(!entry.second.get().getDefaultVariantName().empty()) {
			solvedDescriptors.emplace_back(entry.second.get().getVariant(entry.second.get().getDefaultVariantName()), entry.second.get().getDefaultVariantName());
		}

		// put all other variants into the list "solvedDescriptors"
		for(const auto& dependencyVariantName : entry.second.get().getSpecialVariantNames()) {
			// skip this entry if this is the default variant that has been added to the list a few lines before
			if(entry.second.get().getDefaultVariantName() == dependencyVariantName) {
				continue;
			}
			solvedDescriptors.emplace_back(entry.second.get().getVariant(dependencyVariantName), dependencyVariantName);
		}
	}

	return solvedDescriptors;
}

std::vector<SolvedDescriptor> Solver::createReducedOrderedSolvedDescirptors(const model::Dependency& dependency) const {
	std::vector<SolvedDescriptor> result;

	/* find "dependcy.name" in "availableSolvedDescriptorByName" */
	auto iter = available.find(dependency.id);

	std::vector<SolvedDescriptor> solvedDescriptors = createOrderedSolvedDescirptors(dependency, true);

	logger.debug << "Solved descriptors for dependency \"" << dependency.id << "\":\n";
	for(auto& solvedDescriptor : solvedDescriptors) {
		logger.debug << "- id: \"" << solvedDescriptor.getArtefactId() << "\" / name: \"" << solvedDescriptor.getArtefactName() << "\"\n";

		/* reduce "solvedDescriptor" according to allowed options forced in "dependency" */
		solvedDescriptor.hasDynamic = solvedDescriptor.hasDynamic && (dependency.allowedTypeRepositoryDynamic || dependency.allowedTypeSystemDynamic);
		solvedDescriptor.hasStatic = solvedDescriptor.hasStatic && dependency.allowedTypeRepositoryStatic;

		/* check if "solvedDescriptor" is available already in "availableSolvedDescriptorByName" */
		if(iter != std::end(available)) {
			const SolvedDescriptor& availableDescriptor = iter->second;

			/* skip this "solvedDescriptor" is it is a different version in "availableSolvedDescriptorByName" */
			if(availableDescriptor.getArtefactVersion() != solvedDescriptor.getArtefactVersion()) {
				logger.debug << "  available version \"" << availableDescriptor.getArtefactVersion() << "\" != required version \"" << solvedDescriptor.getArtefactVersion() << "\"\n";
				continue;
			}
			logger.debug << "  required version \"" << solvedDescriptor.getArtefactVersion() << "\" found!\n";

			/* reduce "solvedDescriptor" according to allowed options reduced already in "availableSolvedDescriptorByName" */
			solvedDescriptor.hasStatic = solvedDescriptor.hasStatic && availableDescriptor.hasStatic;
			solvedDescriptor.hasDynamic = solvedDescriptor.hasDynamic && availableDescriptor.hasDynamic;
			solvedDescriptor.hasSource = solvedDescriptor.hasSource && availableDescriptor.hasSource;
		}

		/* skip "solvedDescriptor" if it does nothing provide (no dynamic, no static, no source) */
		if(!solvedDescriptor.hasDynamic && !solvedDescriptor.hasStatic/* && !solvedDescriptor.hasSource*/) {
			logger.debug << "  required version is neither dynamic nor static!\n";
			continue;
		}

		/* check if "solvedDescriptor" is compatible to required architectures if it does not provide sources */
		if(solvedDescriptor.hasSource == false) {
			for(const auto& architectureTarget: architectures) {
				bool found = false;
				for(const auto& architectureActual: solvedDescriptor.getArchitectures()) {
					// TODO: Hier anstatt auf "Gleichheit" auf "Kompatiblität" prüfen
					if(architectureActual == architectureTarget) {
						found = true;
						break;
					}
				}
				if(found == false) {
					std::cout << "Architecture \"" << architectureTarget << "\" missing in descriptor \"" << solvedDescriptor.getArtefactId() << "\", version \"" << solvedDescriptor.getArtefactVersion() << "\".\n";
					continue;
				}
			}
		}

		logger.debug << "  add solved descriptor!\n";
		result.emplace_back(solvedDescriptor);
	}

	return result;
}

void Solver::availableAddOrReduce(const SolvedDescriptor& solvedDescriptor) {
	auto iter = available.find(solvedDescriptor.getArtefactId());
	if(iter != std::end(available)) {

		/* check if version of "solvedDescriptor" does not match */
		if(iter->second.getArtefactVersion() != solvedDescriptor.getArtefactVersion()) {
			throw esl::system::Stacktrace::add(std::runtime_error("ERROR: This should never happen: version does not match."));
		}

		/* reduce available entry in "available" according to "solvedDescriptor"  */
		iter->second.hasStatic = iter->second.hasStatic && solvedDescriptor.hasStatic;
		iter->second.hasDynamic = iter->second.hasDynamic && solvedDescriptor.hasDynamic;
		iter->second.hasSource = iter->second.hasSource && solvedDescriptor.hasSource;
	}
	else {
		availableOrdered.emplace_back(solvedDescriptor.getArtefactId());
		available.emplace(solvedDescriptor.getArtefactId(), solvedDescriptor);
	}
}

void Solver::requiredAddOrReduce(const model::Dependency& dependency) {
	auto iter = required.find(dependency.id);
	if(iter != std::end(required)) {
		/* reduce available entry in "required" according to "dependency"  */
		iter->second.allowedTypeRepositoryStatic = iter->second.allowedTypeRepositoryStatic && dependency.allowedTypeRepositoryStatic;
		iter->second.allowedTypeRepositoryDynamic = iter->second.allowedTypeRepositoryDynamic && dependency.allowedTypeRepositoryDynamic;
		iter->second.allowedTypeSystemDynamic = iter->second.allowedTypeSystemDynamic && dependency.allowedTypeSystemDynamic;
	}
	else {
		requiredOrdered.emplace_back(dependency.id);
		required.emplace(dependency.id, dependency);
	}
	// TODO
}

} /* namespace build */
} /* namespace transformer */
