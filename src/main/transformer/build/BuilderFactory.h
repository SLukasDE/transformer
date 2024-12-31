#ifndef TRANSFORMER_BUILD_BUILDERFACTORY_H_
#define TRANSFORMER_BUILD_BUILDERFACTORY_H_

#include <string>
#include <map>
#include <memory>
#include <transformer/model/Descriptor.h>
#include <transformer/build/Builder.h>
#include <transformer/build/Sources.h>
#include <transformer/build/BuildManagerVariant.h>
//#include <transformer/build/SolvedDescriptors.h>

namespace transformer {
namespace build {

class BuilderFactory {
public:
	BuilderFactory();
	virtual ~BuilderFactory();

	virtual bool hasArchitecture(const std::string& architecture) const = 0;
	virtual std::unique_ptr<Builder> createBuilder(const model::Descriptor& descriptor, std::string architecture, const Sources& sources, const BuildManager& buildManager, const std::vector<std::string>& variants) const = 0;
};

} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_BUILDERFACTORY_H_ */
