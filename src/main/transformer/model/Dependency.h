#ifndef TRANSFORMER_MODEL_DEPENDENCY_H_
#define TRANSFORMER_MODEL_DEPENDENCY_H_

#include <transformer/model/VersionRanges.h>
#include <string>

namespace transformer {
namespace model {

struct Dependency {
	std::string id;
	std::string variantName;
	VersionRanges versionRanges;
	bool allowedTypeRepositoryStatic = false;
	bool allowedTypeRepositoryDynamic = false;
	bool allowedTypeSystemDynamic = false;
};

} /* namespace model */
} /* namespace transformer */

#endif /* TRANSFORMER_MODEL_DEPENDENCY_H_ */
