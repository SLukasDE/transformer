#ifndef TRANSFORMER_BUILD_GCC_VARIANTBUILDER_H_
#define TRANSFORMER_BUILD_GCC_VARIANTBUILDER_H_

#include <string>
#include <vector>
#include <functional>
#include <transformer/model/Descriptor.h>
#include <transformer/files/Source.h>
#include <transformer/build/BuildManager.h>
#include <transformer/build/SolvedDescriptor.h>
#include <transformer/build/SolvedDescriptors.h>
#include <transformer/build/gcc/BuildCompile.h>

namespace transformer {
namespace build {
namespace gcc {

class Builder;

class VariantBuilder {
public:
	VariantBuilder(const Builder& builder, std::string variantName);
	virtual ~VariantBuilder();

	int onlyDependencies();
	int onlyGenerateSources();
	int onlyCompile();
	int onlyTest();
	int onlyLink();
	int onlySite(); // only documentation
	int onlyPackage();
	int onlyProvide();

private:
	const Builder& builder;
	const std::string variantName;

	const BuildManager& getBuildManager() const;
	const model::Descriptor& getDescriptor() const;
	const std::string& getVariantName() const;
	const std::string& getArchitecture() const;

	int onlyDependency(const model::Variant& variant, const std::string& variantName, const SolvedDescriptors& solvedDescriptors, bool buildExecutable = false);

	// installs generators and generates sources
	int onlyGenerateSources(const model::Variant& variant, const std::string& variantName);
	int onlyLinkStatic(const model::Variant& variant, const std::string& variantName, const SolvedDescriptors& solvedDescriptors);
	int onlyLinkDynamic(const model::Variant& variant, const std::string& variantName, const SolvedDescriptors& solvedDescriptors);
	int onlyLinkExecutable(const model::Variant& variant, const std::string& variantName, const SolvedDescriptors& solvedDescriptors, bool isTest = false);

	void addCompileSourcesMain(BuildCompile& buildCompile, const model::Variant& variant, const std::string& variantName, const SolvedDescriptors& solvedDescriptors);
	void addCompileSourcesTest(BuildCompile& buildCompile, const model::Variant& variant, const std::string& variantName, const SolvedDescriptors& solvedDescriptors);
	void addCompileSources(BuildCompile& buildCompile, const model::Variant& variant, const std::string& variantName, const SolvedDescriptors& solvedDescriptors);

	const SolvedDescriptors& getSolvedDescriptors(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName) const;

	std::vector<std::reference_wrapper<const files::Source>> getSourcesSourceMain() const;
	std::vector<std::reference_wrapper<const files::Source>> getSourcesSourceTest() const;
};

} /* namespace gcc */
} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_GCC_VARIANTBUILDER_H_ */
