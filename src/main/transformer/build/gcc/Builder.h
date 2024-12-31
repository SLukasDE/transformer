#ifndef TRANSFORMER_BUILD_GCC_BUILDER_H_
#define TRANSFORMER_BUILD_GCC_BUILDER_H_

#include <transformer/build/Builder.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <transformer/model/Descriptor.h>
#include <transformer/build/BuildManagerVariant.h>
#include <transformer/build/Sources.h>
#include <transformer/build/SolvedDescriptors.h>

namespace transformer {
namespace build {
namespace gcc {

class VariantBuilder;
class Builder: public build::Builder {
public:
	Builder(const model::Descriptor& descriptor, std::string architecture, const Sources& sources, const BuildManager& buildManager, const std::vector<std::string>& variants);
	~Builder();

	int onlyDependencies() override;
	int onlyGenerateSources() override;
	int onlyCompile() override;
	int onlyTest() override;
	int onlyLink() override;
	int onlySite() override; // only documentation
	int onlyPackage() override;
	int onlyProvide() override;

	const std::string& getExecCxx() const;
	const std::string& getExecCxxOptsCompileMain() const;
	const std::string& getExecCxxOptsCompileTest() const;
	const std::string& getExecCxxOptsLinkDynamic() const;
	const std::string& getExecCxxOptsLinkExecutableMain() const;
	const std::string& getExecCxxOptsLinkExecutableTest() const;
	const std::string& getExecAr() const;
	const std::string& getExecArOpts() const;

private:
	std::map<std::string, std::unique_ptr<VariantBuilder>> variantBuilderByVariantName;

	static const std::string execCxx;
	static const std::string execCxxOptsCompileMain;
	static const std::string execCxxOptsCompileTest;
	static const std::string execCxxOptsLinkDynamic;
	static const std::string execCxxOptsLinkExecutableMain;
	static const std::string execCxxOptsLinkExecutableTest;
	static const std::string execAr;
	static const std::string execArOpts;
};

} /* namespace gcc */
} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_GCC_BUILDER_H_ */
