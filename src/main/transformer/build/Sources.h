#ifndef TRANSFORMER_BUILD_SOURCES_H_
#define TRANSFORMER_BUILD_SOURCES_H_

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <filesystem>

#include <transformer/files/Source.h>
#include <transformer/files/FileEntries.h>
#include <transformer/model/Descriptor.h>
#include <transformer/model/Dependency.h>
#include <transformer/model/Variant.h>

namespace transformer {
namespace build {

class Sources {
public:
	Sources(const model::Descriptor& descriptor);
	~Sources();

	std::vector<std::filesystem::path> getPathesSourceMain(const std::string& variantName) const;
	std::vector<std::filesystem::path> getPathesSourceTest(const std::string& variantName) const;
	std::vector<std::filesystem::path> getPathesHeader(const std::string& variantName) const;

	std::filesystem::path getPathBuild(const std::string& artefactId, const std::string& artefactVersion) const;
//	std::filesystem::path getPathBuildGenerated(const std::string& variantName, const std::string& architecture, const std::string& generatorId) const;

	std::filesystem::path getPathBuildSource(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName) const;
	std::filesystem::path getPathBuildHeaders(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName) const;
	std::filesystem::path getPathBuildCompile(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const;
	std::filesystem::path getPathBuildCompileTest(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const;
	std::filesystem::path getPathBuildLinkStatic(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const;
	std::filesystem::path getPathBuildLinkDynamic(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const;
	std::filesystem::path getPathBuildLinkExecutable(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const;
	std::filesystem::path getPathBuildLinkExecutableTest(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const;
	std::filesystem::path getPathBuildGenerated(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture, const std::string& generatorId) const;

	std::vector<std::reference_wrapper<const files::Source>> getSourcesSourceMain(const std::string& variantName) const;
	std::vector<std::reference_wrapper<const files::Source>> getSourcesSourceTest(const std::string& variantName) const;
	std::vector<std::reference_wrapper<const files::Source>> getSourcesHeader(const std::string& variantName) const;

	const files::Source& getSourceBuildSource(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName) const;
	const files::Source& getSourceBuildHeaders(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName) const;
	const files::Source& getSourceBuildCompile(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const;
	const files::Source& getSourceBuildCompileTest(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const;
	const files::Source& getSourceBuildLinkStatic(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const;
	const files::Source& getSourceBuildLinkDynamic(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const;
	const files::Source& getSourceBuildLinkExecutable(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const;
	const files::Source& getSourceBuildLinkExecutableTest(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const;
	const files::Source& getSourceBuildGenerated(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture, const std::string& generatorId) const;

private:
	const model::Descriptor& descriptor;

	mutable std::vector<std::string> sourceNames;
	mutable std::map<std::string, files::Source> sourcesByName;

	const files::Source& addOrGetSource(const std::filesystem::path& path) const;
	const files::Source& addOrGetSource(const std::string& name, const std::filesystem::path& path) const;
//	std::filesystem::path getPathBuild(const std::string& variantName, const std::string& architecture = "") const;
	std::filesystem::path getPathBuild(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture = "") const;
};

} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_SOURCES_H_ */
