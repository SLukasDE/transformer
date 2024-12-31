#ifndef TRANSFORMER_BUILD_GCC_BUILDERFACTORY_H_
#define TRANSFORMER_BUILD_GCC_BUILDERFACTORY_H_

#include <transformer/build/BuilderFactory.h>
#include <string>
#include <map>
#include <memory>
#include <transformer/model/Descriptor.h>
#include <transformer/build/Builder.h>
#include <transformer/build/Sources.h>
#include <transformer/build/BuildManagerVariant.h>

namespace transformer {
namespace build {
namespace gcc {

class BuilderFactory : public build::BuilderFactory {
public:
	BuilderFactory();
	~BuilderFactory();

	bool hasArchitecture(const std::string& architecture) const override;
	std::unique_ptr<build::Builder> createBuilder(const model::Descriptor& descriptor, std::string architecture, const Sources& sources, const BuildManager& buildManager, const std::vector<std::string>& variants) const override;
};

} /* namespace gcc */
} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_GCC_BUILDERFACTORY_H_ */
