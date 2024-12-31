#ifndef TRANSFORMER_CONFIG_BUILD_DEPENDENCY_H_
#define TRANSFORMER_CONFIG_BUILD_DEPENDENCY_H_

#include <string>

namespace transformer {
namespace config {
namespace build {

struct Dependency {
	enum Type {
		dtStatic, dtDynamic
	};

	Type type;

	std::string name;
	std::string variant;

	std::string version;
	std::string versionSolved;

};

} /* namespace build */
} /* namespace config */
} /* namespace transformer */

#endif /* TRANSFORMER_CONFIG_BUILD_DEPENDENCY_H_ */
