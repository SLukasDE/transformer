#include <transformer/build/Builder.h>
#include <transformer/build/BuildManager.h>

namespace transformer {
namespace build {

Builder::Builder(const model::Descriptor& aDescriptor, std::string aArchitecture, const BuildManager& aBuildManager, const Sources& aSources)
: buildManager(aBuildManager),
  descriptor(aDescriptor),
  architecture(std::move(aArchitecture)),
  sources(aSources)
{
}

Builder::~Builder() {
}

const BuildManager& Builder::getBuildManager() const {
	return buildManager;
}

const model::Descriptor& Builder::getDescriptor() const {
	return descriptor;
}

const std::string& Builder::getArchitecture() const {
	return architecture;
}

const Sources& Builder::getSources() const {
	return sources;
}

const model::Descriptor& Builder::getGeneratorDescriptor(const model::Generator& generator) const {
	return buildManager.getGeneratorDescriptor(generator);
}

} /* namespace build */
} /* namespace transformer */
