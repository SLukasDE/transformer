#ifndef TRANSFORMER_CONFIG_ARCHITECTURES_DRIVER_H_
#define TRANSFORMER_CONFIG_ARCHITECTURES_DRIVER_H_

#include <string>
#include <vector>
#include <utility>

namespace transformer {
namespace config {
namespace architectures {

struct Driver {
	std::string name;
	std::vector<std::pair<std::string, std::string>> settings;
};

} /* namespace architectures */
} /* namespace config */
} /* namespace transformer */

#endif /* TRANSFORMER_CONFIG_ARCHITECTURES_DRIVER_H_ */
