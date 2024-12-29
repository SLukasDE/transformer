#if 0
#ifndef TRANSFORMER_ARCHITECTURES_SOLVER_SOLVER_H_
#define TRANSFORMER_ARCHITECTURES_SOLVER_SOLVER_H_

#include <transformer/architectures/Specifiers.h>
#include <transformer/architectures/Specifier.h>
#include <transformer/architectures/Architecture.h>

#include <string>
#include <set>
#include <map>
#include <list>

namespace transformer {
namespace architectures {
namespace solver {

class Solver {
public:
	Solver(const Specifiers& specifiers, const std::set<std::string>& requiredSpecifierIds);

	std::map<std::set<std::string>, architectures::Architecture> getBuildArchitectures() const noexcept;

private:
	std::set<std::set<const Specifier*>> createBuildSpecifiers(std::set<const Specifier*> buildSpecifiers, std::list<std::reference_wrapper<const Specifier>> newSpecifiers, std::size_t depth = 0) const;

	/* creates based on buildSpecifiers a set of all combinations of childrenSpecifiers with respect to branch conflicts. */
	std::set<std::set<const Specifier*>> createChildCombinations(const std::set<const Specifier*>& buildSpecifiers, std::set<const Specifier*> childrenSpecifiers) const;

	const Specifier* findDefaultChild(const Specifier& specifier) const;

	/* returns true if adding newSpecifier would result into a branch conflict with existing specifiers of buildSpecifiers.
	 * returns false if adding newSpecifier would not result into a conflicting branch.
	 */
	bool hasBranchConflict(const std::set<const Specifier*>& buildSpecifiers, const Specifier& newSpecifier) const;

	void dump(const std::set<const Specifier*> specifiers) const;
	void dump(const std::set<std::set<const Specifier*>>& specifiersSet) const;

	const Specifiers& specifiers;
	std::map<std::set<std::string>, architectures::Architecture> buildArchitectures;
};

} /* namespace solver */
} /* namespace architectures */
} /* namespace transformer */

#endif /* TRANSFORMER_ARCHITECTURES_SOLVER_SOLVER_H_ */
#endif

