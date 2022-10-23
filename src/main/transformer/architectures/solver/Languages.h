#ifndef TRANSFORMER_ARCHITECTURES_SOLVER_LANGUAGES_H_
#define TRANSFORMER_ARCHITECTURES_SOLVER_LANGUAGES_H_

#include <transformer/architectures/Specifiers.h>
#include <transformer/architectures/Specifier.h>

#include <string>
#include <set>
#include <vector>
#include <functional>

namespace transformer {
namespace architectures {
namespace solver {

class Languages {
public:
	Languages(const Specifiers& specifiers);
	Languages(const std::vector<std::reference_wrapper<const Specifier>>& specifiers);

	const std::set<std::string>& getLanguages() const noexcept;

private:
	std::set<std::string> languages;
};

} /* namespace solver */
} /* namespace architectures */
} /* namespace transformer */

#endif /* TRANSFORMER_ARCHITECTURES_SOLVER_LANGUAGES_H_ */
