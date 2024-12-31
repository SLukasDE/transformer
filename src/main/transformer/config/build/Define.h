#ifndef TRANSFORMER_CONFIG_BUILD_DEFINE_H_
#define TRANSFORMER_CONFIG_BUILD_DEFINE_H_

#include <string>

namespace transformer {
namespace config {
namespace build {

struct Define {
	std::string key;
	std::string define;
	bool isPublic = false;
};

} /* namespace build */
} /* namespace config */
} /* namespace transformer */

#endif /* TRANSFORMER_CONFIG_BUILD_DEFINE_H_ */
