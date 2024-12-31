#include <transformer/build/gcc/BuilderFactory.h>
#include <transformer/build/gcc/Builder.h>

namespace transformer {
namespace build {
namespace gcc {

BuilderFactory::BuilderFactory()
: build::BuilderFactory()
{
}

BuilderFactory::~BuilderFactory() {
}

bool BuilderFactory::hasArchitecture(const std::string& architecture) const {
	return architecture == "linux-gcc";
}

std::unique_ptr<build::Builder> BuilderFactory::createBuilder(const model::Descriptor& descriptor, std::string architecture, const Sources& sources, const BuildManager& buildManager, const std::vector<std::string>& variants) const {
//std::unique_ptr<build::Builder> BuilderFactory::createBuilder(const model::Descriptor& descriptor, std::string architecture, const Sources& sources, std::map<std::string, SolvedDescriptors> solvedDescriptorsByVariantName) const {
	return std::unique_ptr<build::Builder>(new Builder(descriptor, architecture, sources, buildManager, variants));
}

} /* namespace gcc */
} /* namespace build */
} /* namespace transformer */
