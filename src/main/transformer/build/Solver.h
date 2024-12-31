#ifndef TRANSFORMER_BUILD_SOLVER_H_
#define TRANSFORMER_BUILD_SOLVER_H_

#include <vector>
#include <string>
#include <map>
#include <list>
#include <memory>
#include <transformer/build/SolvedDescriptor.h>
#include <transformer/build/SolvedDescriptors.h>
#include <transformer/repository/Database.h>
#include <transformer/model/Descriptor.h>
#include <transformer/model/Dependency.h>
#include <transformer/model/VersionRanges.h>

namespace transformer {
namespace build {

class Solver {
public:
	Solver(repository::Database& database, const model::Variant& variant);

	SolvedDescriptors solve();

private:
	repository::Database& database;
	std::vector<std::string> architectures;

	std::list<std::string> availableOrdered;
	std::map<std::string, SolvedDescriptor> available;

	std::list<std::string> requiredOrdered;
	std::map<std::string, model::Dependency> required;

	std::vector<SolvedDescriptor> solveInternal();
	void availableAddOrReduce(const SolvedDescriptor& solvedDescriptor);
	void requiredAddOrReduce(const model::Dependency& dependency);

	/* Just creates an ordered list of SolvedDescriptors according to dependency.
	 * SolvedDescriptors are not reduced or otherwise patched.
	 * It's just a list of ALL available Entries that are specified by the dependency if "reduceToAvailableVersion" == false.
	 * If "reduceToAvailableVersion" == true, it is the same as "false" if "dependency.name" is not already available in "availableSolvedDescriptorByName".
	 * Otherwhise it contains only one enty with this verison availabe in "availableSolvedDescriptorByName".
	 */
	std::vector<SolvedDescriptor> createOrderedSolvedDescirptors(const model::Dependency& dependency, bool reduceToAvailableVersion = false) const;

	// this is an extended version of "createOrderedSolvedDescirptors" but it patches this descriptors and rejects descriptors that does not fit to the current context
	std::vector<SolvedDescriptor> createReducedOrderedSolvedDescirptors(const model::Dependency& dependency) const;
};

} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_SOLVER_H_ */
