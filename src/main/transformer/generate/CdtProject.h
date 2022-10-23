#ifndef TRANSFORMER_GENERATE_CDTPROJECT_H_
#define TRANSFORMER_GENERATE_CDTPROJECT_H_

#include <string>
#include <set>
#include <ostream>
#include <transformer/build/BuildManager.h>

namespace transformer {
namespace generate {

class CdtProject {
public:
	CdtProject(const build::BuildManager& buildManager, std::string architecture);

	void generate(const std::set<std::string>& variantNames);

private:
	const build::BuildManager& buildManager;
	const std::string architecture;
	int xmlIdCounter;
	const std::string projectPath;

	struct ReferencedXmlIds {
		ReferencedXmlIds(int& xmlIdCounter, const std::string& variantName);
		std::string xmlName;
		std::string uiName;
		int idBuildmanagerConfig;
		int idBuildmanagerToolGnuCompilerCpp;
		int idBuildmanagerToolGnuCompilerCppInput;
		int idBuildmanagerToolGnuCompilerC;
		int idBuildmanagerToolGnuCompilerCInput;
	};

	void generateProject();
	void generateCProject(const std::set<std::string>& variantNames);
	ReferencedXmlIds generateCProjectVariant(std::ostream& os, const std::string& projectPath, const model::Variant& variant, const std::string& variantName);
};

} /* namespace generate */
} /* namespace transformer */

#endif /* TRANSFORMER_GENERATE_CDTPROJECT_H_ */
