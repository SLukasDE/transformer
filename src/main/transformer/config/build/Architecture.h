#ifndef TRANSFORMER_CONFIG_BUILD_ARCHITECTURE_H_
#define TRANSFORMER_CONFIG_BUILD_ARCHITECTURE_H_

#include <string>
#include <set>

namespace transformer {
namespace config {
namespace build {

struct Architecture {
	std::set<std::string> features;
};

} /* namespace build */
} /* namespace config */
} /* namespace transformer */

#endif /* TRANSFORMER_CONFIG_BUILD_ARCHITECTURE_H_ */
