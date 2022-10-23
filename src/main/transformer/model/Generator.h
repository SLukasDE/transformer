#ifndef TRANSFORMER_MODEL_GENERATOR_H_
#define TRANSFORMER_MODEL_GENERATOR_H_

#include <string>
#include <transformer/model/VersionRange.h>

namespace transformer {
namespace model {

class Descriptor;

struct Generator {
	std::string id;
	std::string variantName;
	VersionRange versionRange;

	const std::string& getEffectiveVariantName(const model::Descriptor& generatorDescriptor) const;
};

} /* namespace model */
} /* namespace transformer */

#endif /* TRANSFORMER_MODEL_GENERATOR_H_ */
