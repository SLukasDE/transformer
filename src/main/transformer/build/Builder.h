#ifndef TRANSFORMER_BUILD_BUILDER_H_
#define TRANSFORMER_BUILD_BUILDER_H_

#include <string>
#include <vector>
#include <map>
#include <transformer/model/Descriptor.h>
#include <transformer/build/Sources.h>

namespace transformer {
namespace build {

class VariantBuilder;
class BuildManager;

class Builder {
	friend class VariantBuilder;
public:
	Builder(const model::Descriptor& descriptor, std::string architecture, const BuildManager& buildManager, const Sources& sources);
	virtual ~Builder();

	virtual int onlyDependencies() = 0;
	virtual int onlyGenerateSources() = 0;
	virtual int onlyCompile() = 0;
	virtual int onlyTest() = 0;
	virtual int onlyLink() = 0;
	virtual int onlySite() = 0; // only documentation
	virtual int onlyPackage() = 0;
	virtual int onlyProvide() = 0;

	const BuildManager& getBuildManager() const;
	const model::Descriptor& getDescriptor() const;
	const std::string& getArchitecture() const;
	const Sources& getSources() const;

	const model::Descriptor& getGeneratorDescriptor(const model::Generator& generator) const;

private:
	const BuildManager& buildManager;
	const model::Descriptor& descriptor;
	const std::string architecture;
	const Sources& sources;
};

} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_BUILDER_H_ */
