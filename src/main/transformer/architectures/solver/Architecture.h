#ifndef TRANSFORMER_ARCHITECTURES_SOLVER_ARCHITECTURE_H_
#define TRANSFORMER_ARCHITECTURES_SOLVER_ARCHITECTURE_H_

#include <transformer/architectures/Specifier.h>
#include <transformer/architectures/Architecture.h>

#include <string>
#include <vector>
#include <functional>

namespace transformer {
namespace architectures {
namespace solver {

class Architecture {
public:
	Architecture(const std::vector<std::reference_wrapper<const Specifier>>& specifiers);
	Architecture(const std::vector<std::reference_wrapper<const Specifier>>& specifiers, const std::string& language);

	const architectures::Architecture& getArchitecture() const noexcept;

private:
	architectures::Architecture architecture;
};

} /* namespace solver */
} /* namespace architectures */
} /* namespace transformer */

#endif /* TRANSFORMER_ARCHITECTURES_SOLVER_ARCHITECTURE_H_ */
