#ifndef TRANSFORMER_BUILD_BUILDMANAGERVARIANT_H_
#define TRANSFORMER_BUILD_BUILDMANAGERVARIANT_H_

#include <transformer/model/Variant.h>
#include <transformer/model/VersionRange.h>
#include <transformer/build/SolvedDescriptor.h>
#include <transformer/build/SolvedDescriptors.h>
#include <transformer/build/SolvedStatus.h>
#include <transformer/build/Sources.h>
#include <transformer/repository/Database.h>

#include <string>

namespace transformer {
namespace build {

class BuildManager;
class BuildManagerVariant {
public:
	BuildManagerVariant(BuildManager& buildManager, std::string variantName);
//	virtual ~BuildManagerVariant();

	const SolvedDescriptors& getSolvedDescriptors(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName) const;

private:
	BuildManager& buildManager;
	const std::string variantName;
	repository::Database& database;
	const Sources& sources;
	const model::Variant& variant;
	const std::vector<std::string> architectures;

	void loadGenerator(const model::Generator& generator);
#if 1
	void loadDependency(const model::Variant& variant, const std::string& variantName, SolvedStatus& solvedStatus, bool loadStaticLib, bool loadDynamicLib);
#else
	void loadDependency(const SolvedDescriptor& solvedDescriptor, bool isBinaryArtefactNeeded);
#endif
	void loadDependencyDescriptor(const model::Descriptor& descriptor, SolvedStatus& solvedStatus);
	void loadDependencyHeaders(const model::Variant& variant, const std::string& variantName, SolvedStatus& solvedStatus);
	void loadDependencySources(const model::Variant& variant, const std::string& variantName, SolvedStatus& solvedStatus);
	void loadDependencyGeneratedSources(const model::Variant& variant, const std::string& variantName, SolvedStatus& solvedStatus);
	void loadDependencyGenerators(const model::Variant& variant, SolvedStatus& solvedStatus);
	void loadDependencyLibStatic(const model::Variant& variant, const std::string& variantName, SolvedStatus& solvedStatus);
	void loadDependencyLibDynamic(const model::Variant& variant, const std::string& variantName, SolvedStatus& solvedStatus);
	void loadDependencyExecutable(const model::Variant& variant, const std::string& variantName, SolvedStatus& solvedStatus);
};

} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_BUILDMANAGERVARIANT_H_ */
