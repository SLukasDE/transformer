#include <transformer/model/Generator.h>
#include <transformer/model/Descriptor.h>

namespace transformer {
namespace model {

const std::string& Generator::getEffectiveVariantName(const model::Descriptor& generatorDescriptor) const {
	return variantName.empty() ? generatorDescriptor.getDefaultVariantName() : variantName;
}

} /* namespace model */
} /* namespace transformer */
