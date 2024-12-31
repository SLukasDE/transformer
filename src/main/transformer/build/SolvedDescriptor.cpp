#include <transformer/build/SolvedDescriptor.h>

namespace transformer {
namespace build {

SolvedDescriptor::SolvedDescriptor(const model::Variant& aVariant, std::string aVariantName)
: hasStatic(aVariant.isProvidedStaticEffective()),
  hasDynamic(aVariant.isProvidedDynamicEffective()),
//  hasExecutable(aVariant.isProvidedExecutableEffective()),
  hasSource(aVariant.isProvidedSourceEffective()),
  variant(aVariant),
  variantName(std::move(aVariantName))
{
}

const std::string& SolvedDescriptor::getArtefactId() const {
	return getDescriptor().getArtefactId();
}

const std::string& SolvedDescriptor::getArtefactName() const {
	return getDescriptor().getArtefactName();
}

const std::string& SolvedDescriptor::getArtefactVersion() const {
	return getDescriptor().getArtefactVersion();
}

const std::string& SolvedDescriptor::getApiVersion() const {
	return getDescriptor().getApiVersion();
}

const model::Descriptor& SolvedDescriptor::getDescriptor() const {
	return variant.getDescriptor();
}

const model::Variant& SolvedDescriptor::getVariant() const {
	return variant;
}

const std::string& SolvedDescriptor::getVariantName() const {
	return variantName;
}

const std::vector<std::string>& SolvedDescriptor::getArchitectures() const {
	return variant.getArchitecturesEffective();
}
/*
bool SolvedDescriptor::hasArchitecture(const std::string& architecture) const {
	const std::vector<std::string>& architectures = getArchitectures();

	for(const auto& architectureFound : architectures) {
		if(architecture == architectureFound) {
			return true;
		}
	}

	return false;
}
*/
} /* namespace build */
} /* namespace transformer */
