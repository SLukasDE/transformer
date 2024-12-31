#include <transformer/build/SolvedDescriptors.h>

#include <esl/system/Stacktrace.h>

#include <set>
#include <list>
namespace transformer {
namespace build {

SolvedDescriptors::SolvedDescriptors(std::vector<SolvedDescriptor> aSolvedDescriptors)
: solvedDescriptors(std::move(aSolvedDescriptors))
{
}
/*
SolvedDescriptors::~SolvedDescriptors() {
	// TODO Auto-generated destructor stub
}
*/

const std::vector<SolvedDescriptor>& SolvedDescriptors::getElements() const {
	return solvedDescriptors;
}

const SolvedDescriptor& SolvedDescriptors::getSolvedDescriptor(const std::string& artefactId) const {
	const SolvedDescriptor* result = nullptr;

	for(const auto& solvedDescriptor : solvedDescriptors) {
		if(solvedDescriptor.getArtefactId() == artefactId) {
			result = &solvedDescriptor;
			break;
		}
	}

	if(result == nullptr) {
		throw esl::system::Stacktrace::add(std::runtime_error("cannot find solved descriptor with artefactId = \"" + artefactId + "\""));
	}

	return *result;
}

std::vector<std::reference_wrapper<const SolvedDescriptor>> SolvedDescriptors::getSolvedDescirptorsOfDependency(const std::string& artefactId) const {
	std::vector<std::reference_wrapper<const SolvedDescriptor>> result;
	std::set<std::string> artefactIdsProcessed;
	std::list<std::reference_wrapper<const SolvedDescriptor>> queue;

//	if(solvedDescriptors.empty()) {
//		return result;
//	}
	bool isRootArtefactId = true;
	for(const auto& solvedDescriptor : solvedDescriptors) {
		result.emplace_back(solvedDescriptor);
		if(solvedDescriptor.getArtefactId() == artefactId) {
			isRootArtefactId = false;
//			break;
		}
	}

	if(isRootArtefactId == false) {
		result.clear();

		/* init queue */
		artefactIdsProcessed.insert(artefactId);
		queue.emplace_back(getSolvedDescriptor(artefactId));

		while(!queue.empty()) {
			const SolvedDescriptor& solvedDescriptor = queue.front().get();
			queue.pop_front();

			auto dependencies = solvedDescriptor.getVariant().getDependenciesEffective();
			for(const auto& dependency : dependencies) {
				if(artefactIdsProcessed.find(dependency.id) != std::end(artefactIdsProcessed)) {
					continue;
				}

				const SolvedDescriptor& dependencySolvedDescriptor = getSolvedDescriptor(dependency.id);

				artefactIdsProcessed.insert(dependency.id);
				queue.emplace_back(dependencySolvedDescriptor);
				result.emplace_back(dependencySolvedDescriptor);
			}
		}
	}

	return result;
}

} /* namespace build */
} /* namespace transformer */
