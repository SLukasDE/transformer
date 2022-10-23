#ifndef TRANSFORMER_MODEL_DEPENDENCYSOLVED_H_
#define TRANSFORMER_MODEL_DEPENDENCYSOLVED_H_

#include <string>

namespace transformer {
namespace model {

struct DependencySolved {
	std::string name;
	std::string variantName;
	std::string versionApi;
	bool allowedTypeRepositoryStatic = false;
	bool allowedTypeRepositoryDynamic = false;
	bool allowedTypeSystemDynamic = false;
};

} /* namespace model */
} /* namespace transformer */

#endif /* TRANSFORMER_MODEL_DEPENDENCYSOLVED_H_ */
