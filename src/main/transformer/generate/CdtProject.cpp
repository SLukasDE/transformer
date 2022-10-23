#include <transformer/generate/CdtProject.h>
//#include <transformer/build/Builder.h>
#include <transformer/build/SolvedDescriptor.h>
#include <transformer/build/SolvedDescriptors.h>
#include <esl/system/Stacktrace.h>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <ctime>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS


namespace transformer {
namespace generate {

namespace {
int createXmlIdCounter() {
	std::srand(std::time(nullptr)); // use current time as seed for random generator
	return std::rand();
}

std::string toLower(std::string value) {
	std::transform(value.begin(), value.end(), value.begin(),
	    [](unsigned char c){ return std::tolower(c); });
	return value;
}
}

CdtProject::ReferencedXmlIds::ReferencedXmlIds(int& xmlIdCounter, const std::string& variantName)
: xmlName("release"),
//  xmlName(variantName.empty() ? "default" : "variant" + toLower(variantName)),
  uiName(variantName.empty() ? "default" : "[" + toLower(variantName) + "]"),
/* idBuildmanagerConfig(1095216325), // needed for reference */
  idBuildmanagerConfig(++xmlIdCounter),
/* idBuildmanagerToolGnuCompilerCpp = 528377259), // needed for reference */
  idBuildmanagerToolGnuCompilerCpp(++xmlIdCounter),
/* idBuildmanagerToolGnuCompilerCppInput = 1356643451), // needed for reference */
  idBuildmanagerToolGnuCompilerCppInput(++xmlIdCounter),
/* idBuildmanagerToolGnuCompilerC(2094625749), // needed for reference */
  idBuildmanagerToolGnuCompilerC(++xmlIdCounter),
/* idBuildmanagerToolGnuCompilerCInput = 1963835028) // needed for reference */
  idBuildmanagerToolGnuCompilerCInput(++xmlIdCounter)
{
}

CdtProject::CdtProject(const build::BuildManager& aBuildManager, std::string aArchitecture)
: buildManager(aBuildManager),
  architecture(std::move(aArchitecture)),
  xmlIdCounter(createXmlIdCounter()),
  projectPath(boost::filesystem::current_path().leaf().generic_string())
{
}

void CdtProject::generate(const std::set<std::string>& variantNames) {
//	buildManager.initializeDependencies();
	generateProject();
	generateCProject(variantNames);
}

void CdtProject::generateProject() {
	std::ofstream os(".project");

	os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	os << "<projectDescription>\n";
	os << " <name>" << buildManager.getDescriptor().getArtefactName() << "</name>\n";
	os << " <comment></comment>\n";
	os << " <projects>\n";
	os << " </projects>\n";
	os << " <buildSpec>\n";
	os << "  <buildCommand>\n";
	os << "   <name>org.eclipse.cdt.managedbuilder.core.genmakebuilder</name>\n";
	os << "   <triggers>clean,full,incremental,</triggers>\n";
	os << "   <arguments>\n";
	os << "   </arguments>\n";
	os << "  </buildCommand>\n";
	os << "  <buildCommand>\n";
	os << "   <name>org.eclipse.cdt.managedbuilder.core.ScannerConfigBuilder</name>\n";
	os << "   <triggers>full,incremental,</triggers>\n";
	os << "   <arguments>\n";
	os << "   </arguments>\n";
	os << "  </buildCommand>\n";
	os << " </buildSpec>\n";
	os << " <natures>\n";
	os << "  <nature>org.eclipse.cdt.core.cnature</nature>\n";
	os << "  <nature>org.eclipse.cdt.core.ccnature</nature>\n";
	os << "  <nature>org.eclipse.cdt.managedbuilder.core.managedBuildNature</nature>\n";
	os << "  <nature>org.eclipse.cdt.managedbuilder.core.ScannerConfigNature</nature>\n";
	os << " </natures>\n";
	os << "</projectDescription>\n";
	os.close();
}

void CdtProject::generateCProject(const std::set<std::string>& variantNamesOption) {
	std::ofstream os(".cproject");

	os << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
	os << "<?fileVersion 4.0.0?><cproject storage_type_id=\"org.eclipse.cdt.core.XmlProjectDescriptionStorage\">\n";
	os << " <storageModule moduleId=\"org.eclipse.cdt.core.settings\">\n";

	std::vector<ReferencedXmlIds> referencedXmlIds;
	if(buildManager.getDescriptor().getDefaultVariantName().empty()) {
		referencedXmlIds.emplace_back(generateCProjectVariant(os, projectPath, buildManager.getDescriptor().getVariant(""), ""));
	}

	std::vector<std::string> variantNames;
	if(variantNamesOption.empty()) {
		if(buildManager.getDescriptor().getDefaultVariantName().empty()) {
			variantNames.emplace_back("");
		}
		std::vector<std::string> tmpVariantNames = buildManager.getDescriptor().getSpecialVariantNames();
		for(const auto& variantName : tmpVariantNames) {
			variantNames.emplace_back(variantName);
		}
	}
	else {
		for(const auto& variantName : variantNamesOption) {
			variantNames.emplace_back(variantName);
		}
	}

	for(const auto& variantName : variantNames) {
		referencedXmlIds.emplace_back(generateCProjectVariant(os, projectPath, buildManager.getDescriptor().getVariant(variantName), variantName));
	}

	int idBuildmanagerTargetGnuExe = ++xmlIdCounter;
	/*
	int idBuildmanagerTargetGnuExe = 832821992;
	*/

	os << " </storageModule>\n";
	os << " <storageModule moduleId=\"cdtBuildSystem\" version=\"4.0.0\">\n";
	os << "  <project id=\"transformer-test.cdt.managedbuild.target.gnu.cross.exe." << idBuildmanagerTargetGnuExe << "\" name=\"Executable\" projectType=\"cdt.managedbuild.target.gnu.cross.exe\"/>\n";
	os << " </storageModule>\n";
	os << " <storageModule moduleId=\"scannerConfiguration\">\n";
	os << "  <autodiscovery enabled=\"true\" problemReportingEnabled=\"true\" selectedProfileId=\"\"/>\n";
	for(const auto& refXmlIds : referencedXmlIds) {
		os << "  <scannerConfigBuildInfo instanceId=\"cdt.managedbuild.config.gnu.cross.exe." << refXmlIds.xmlName << "." << refXmlIds.idBuildmanagerConfig << ";cdt.managedbuild.config.gnu.cross.exe." << refXmlIds.xmlName << "." << refXmlIds.idBuildmanagerConfig << ".;cdt.managedbuild.tool.gnu.cross.cpp.compiler." << refXmlIds.idBuildmanagerToolGnuCompilerCpp << ";cdt.managedbuild.tool.gnu.cpp.compiler.input." << refXmlIds.idBuildmanagerToolGnuCompilerCppInput << "\">\n";
		os << "    <autodiscovery enabled=\"true\" problemReportingEnabled=\"true\" selectedProfileId=\"\"/>\n";
		os << "  </scannerConfigBuildInfo>\n";
		os << "  <scannerConfigBuildInfo instanceId=\"cdt.managedbuild.config.gnu.cross.exe." << refXmlIds.xmlName << "." << refXmlIds.idBuildmanagerConfig << ";cdt.managedbuild.config.gnu.cross.exe." << refXmlIds.xmlName << "." << refXmlIds.idBuildmanagerConfig << ".;cdt.managedbuild.tool.gnu.cross.c.compiler." << refXmlIds.idBuildmanagerToolGnuCompilerC << ";cdt.managedbuild.tool.gnu.c.compiler.input." << refXmlIds.idBuildmanagerToolGnuCompilerCInput << "\">\n";
		os << "    <autodiscovery enabled=\"true\" problemReportingEnabled=\"true\" selectedProfileId=\"\"/>\n";
		os << "  </scannerConfigBuildInfo>\n";
	}
	os << " </storageModule>\n";
	os << " <storageModule moduleId=\"org.eclipse.cdt.core.LanguageSettingsProviders\"/>\n";
	os << " <storageModule moduleId=\"refreshScope\" versionNumber=\"2\">\n";
	os << "  <configuration configurationName=\"Debug\">\n";

	/***************************
	 * set debug configuration *
	 ***************************/
	os << "    <resource resourceType=\"PROJECT\" workspacePath=\"/" << projectPath << "\"/>\n";

	os << "  </configuration>\n";
	os << " </storageModule>\n";
	os << " <storageModule moduleId=\"org.eclipse.cdt.make.core.buildtargets\"/>\n";
	os << "</cproject>\n";
	os.close();
}

CdtProject::ReferencedXmlIds CdtProject::generateCProjectVariant(std::ostream& os, const std::string& projectPath, const model::Variant& variant, const std::string& variantName) {
	ReferencedXmlIds referencedXmlIds(xmlIdCounter, variantName);
	const build::SolvedDescriptors& solvedDescriptors = buildManager.getSolvedDescriptors(buildManager.getDescriptor().getArtefactId(), buildManager.getDescriptor().getArtefactVersion(), variantName);

	const std::string buildCommand = "transformer";
	const std::string buildCommandArgBuild = "link variant=" + variantName + " architecture=" + architecture;
	const std::string buildCommandArgClean = "clean";

	int idBuildmanagerToolchain = ++xmlIdCounter;
	int idBuildmanagerTargetPlatform = ++xmlIdCounter;
	int idBuildmanagerBuilderGnuCross = ++xmlIdCounter;

	int idGnuCOptimizationLevel = ++xmlIdCounter;
	int idGnuCDebuggingLevel = ++xmlIdCounter;

	int idGnuCppOptimizationLevel = ++xmlIdCounter;
	int idGnuCppDebuggingLevel = ++xmlIdCounter;
	int idGnuCppIncludePaths = ++xmlIdCounter;
	int idGnuCppPreprocesorDef = ++xmlIdCounter;

	int idBuildmanagerToolGnuLinkerC = ++xmlIdCounter;
	int idBuildmanagerToolGnuLinkerCpp = ++xmlIdCounter;

	int idGnuCppLinkerPath = ++xmlIdCounter;
	int idGnuCppLinkerLibs = ++xmlIdCounter;
	int idBuildmanagerToolGnuCppLinkerInput = ++xmlIdCounter;

	int idBuildmanagerToolGnuArchiver = ++xmlIdCounter;

	int idBuildmanagerToolGnuAssembler = ++xmlIdCounter;
	int idBuildmanagerToolGnuAssemblerInput = ++xmlIdCounter;

/*
	int idBuildmanagerDebugToolchain = 493939884;
	int idBuildmanagerTargetPlatform = 997556457;
	int idBuildmanagerBuilderGnuCross = 804452212;

	int idGnuCOptimizationLevel = 1281868805;
	int idGnuCDebuggingLevel = 565613923;

	int idGnuCppOptimizationLevel = 978548782;
	int idGnuCppDebuggingLevel = 1090030378;
	int idGnuCppIncludePaths = 1454130198;
	int idGnuCppPreprocesorDef = 179177778;

	int idBuildmanagerToolGnuLinkerC = 1759195913;
	int idBuildmanagerToolGnuLinkerCpp = 151307716;

	int idGnuCppLinkerPath = 611373827;
	int idBuildmanagerToolGnuCppLinkerInput = 1426097354;

	int idBuildmanagerToolGnuArchiver = 1326608228;

	int idBuildmanagerToolGnuAssembler = 196446353;
	int idBuildmanagerToolGnuAssemblerInput = 606473109;
*/

	os << "  <cconfiguration id=\"cdt.managedbuild.config.gnu.cross.exe." << referencedXmlIds.xmlName << "." << referencedXmlIds.idBuildmanagerConfig << "\">\n";
	os << "    <storageModule buildSystemId=\"org.eclipse.cdt.managedbuilder.core.configurationDataProvider\" id=\"cdt.managedbuild.config.gnu.cross.exe." << referencedXmlIds.xmlName << "." << referencedXmlIds.idBuildmanagerConfig << "\" moduleId=\"org.eclipse.cdt.core.settings\" name=\"" << referencedXmlIds.uiName << "\">\n";
	os << "     <externalSettings/>\n";
	os << "     <extensions>\n";
	os << "      <extension id=\"org.eclipse.cdt.core.ELF\" point=\"org.eclipse.cdt.core.BinaryParser\"/>\n";
	os << "      <extension id=\"org.eclipse.cdt.core.GASErrorParser\" point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	os << "      <extension id=\"org.eclipse.cdt.core.GmakeErrorParser\" point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	os << "      <extension id=\"org.eclipse.cdt.core.GLDErrorParser\" point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	os << "      <extension id=\"org.eclipse.cdt.core.CWDLocator\" point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	os << "      <extension id=\"org.eclipse.cdt.core.GCCErrorParser\" point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	os << "     </extensions>\n";
	os << "    </storageModule>\n";
	os << "    <storageModule moduleId=\"cdtBuildSystem\" version=\"4.0.0\">\n";
	os << "     <configuration artifactName=\"${ProjName}\" buildArtefactType=\"org.eclipse.cdt.build.core.buildArtefactType.exe\" buildProperties=\"org.eclipse.cdt.build.core.buildArtefactType=org.eclipse.cdt.build.core.buildArtefactType.exe,org.eclipse.cdt.build.core.buildType=org.eclipse.cdt.build.core.buildType." << referencedXmlIds.xmlName << "\" cleanCommand=\"rm -rf\" description=\"\" id=\"cdt.managedbuild.config.gnu.cross.exe." << referencedXmlIds.xmlName << "." << referencedXmlIds.idBuildmanagerConfig << "\" name=\"" << referencedXmlIds.uiName << "\" optionalBuildProperties=\"\" parent=\"cdt.managedbuild.config.gnu.cross.exe." << referencedXmlIds.xmlName << "\">\n";
	os << "      <folderInfo id=\"cdt.managedbuild.config.gnu.cross.exe." << referencedXmlIds.xmlName << "." << referencedXmlIds.idBuildmanagerConfig << ".\" name=\"/\" resourcePath=\"\">\n";
	os << "       <toolChain id=\"cdt.managedbuild.toolchain.gnu.cross.exe." << referencedXmlIds.xmlName << "." << idBuildmanagerToolchain << "\" name=\"Cross GCC\" superClass=\"cdt.managedbuild.toolchain.gnu.cross.exe." << referencedXmlIds.xmlName << "\">\n";
	os << "        <targetPlatform archList=\"all\" binaryParser=\"org.eclipse.cdt.core.ELF\" id=\"cdt.managedbuild.targetPlatform.gnu.cross." << idBuildmanagerTargetPlatform << "\" isAbstract=\"false\" osList=\"all\" superClass=\"cdt.managedbuild.targetPlatform.gnu.cross\"/>\n";

	/******************
	 * set build path *
	 ******************/

	os << "        <builder buildPath=\"${workspace_loc:/" << projectPath << "}\" cleanBuildTarget=\"" << buildCommandArgClean << "\" command=\"" << buildCommand << "\" id=\"cdt.managedbuild.builder.gnu.cross." << idBuildmanagerBuilderGnuCross << "\" incrementalBuildTarget=\"" << buildCommandArgBuild << "\" keepEnvironmentInBuildfile=\"false\" managedBuildOn=\"false\" name=\"Gnu Make Builder\" superClass=\"cdt.managedbuild.builder.gnu.cross\"/>\n";

	os << "        <tool id=\"cdt.managedbuild.tool.gnu.cross.c.compiler." << referencedXmlIds.idBuildmanagerToolGnuCompilerC << "\" name=\"Cross GCC Compiler\" superClass=\"cdt.managedbuild.tool.gnu.cross.c.compiler\">\n";
	os << "         <option defaultValue=\"gnu.c.optimization.level.none\" id=\"gnu.c.compiler.option.optimization.level." << idGnuCOptimizationLevel << "\" name=\"Optimization Level\" superClass=\"gnu.c.compiler.option.optimization.level\" useByScannerDiscovery=\"false\" valueType=\"enumerated\"/>\n";
	os << "         <option defaultValue=\"gnu.c.debugging.level.max\" id=\"gnu.c.compiler.option.debugging.level." << idGnuCDebuggingLevel << "\" name=\"Debug Level\" superClass=\"gnu.c.compiler.option.debugging.level\" useByScannerDiscovery=\"false\" valueType=\"enumerated\"/>\n";
	os << "         <inputType id=\"cdt.managedbuild.tool.gnu.c.compiler.input." << referencedXmlIds.idBuildmanagerToolGnuCompilerCInput << "\" superClass=\"cdt.managedbuild.tool.gnu.c.compiler.input\"/>\n";
	os << "        </tool>\n";
	os << "        <tool id=\"cdt.managedbuild.tool.gnu.cross.cpp.compiler." << referencedXmlIds.idBuildmanagerToolGnuCompilerCpp << "\" name=\"Cross G++ Compiler\" superClass=\"cdt.managedbuild.tool.gnu.cross.cpp.compiler\">\n";
	os << "         <option id=\"gnu.cpp.compiler.option.optimization.level." << idGnuCppOptimizationLevel << "\" name=\"Optimization Level\" superClass=\"gnu.cpp.compiler.option.optimization.level\" useByScannerDiscovery=\"false\" value=\"gnu.cpp.compiler.optimization.level.none\" valueType=\"enumerated\"/>\n";
	os << "         <option defaultValue=\"gnu.cpp.compiler.debugging.level.max\" id=\"gnu.cpp.compiler.option.debugging.level." << idGnuCppDebuggingLevel << "\" name=\"Debug Level\" superClass=\"gnu.cpp.compiler.option.debugging.level\" useByScannerDiscovery=\"false\" valueType=\"enumerated\"/>\n";
	os << "         <option IS_BUILTIN_EMPTY=\"false\" IS_VALUE_EMPTY=\"false\" id=\"gnu.cpp.compiler.option.include.paths." << idGnuCppIncludePaths << "\" name=\"Include paths (-I)\" superClass=\"gnu.cpp.compiler.option.include.paths\" useByScannerDiscovery=\"false\" valueType=\"includePath\">\n";

	/*********************
	 * add include paths *
	 *********************/

	// add own sources
	for(const auto& pathSource :variant.getSourcesMainDirEffective()) {
		os << "          <listOptionValue builtIn=\"false\" value=\"&quot;${workspace_loc:/" << projectPath << "/" << pathSource << "}&quot;\"/>\n";
	}
	// add generated sources
	std::map<std::string, model::Generator> generators = variant.getGeneratorsEffective();
	for(const auto generatorEntry : generators) {
		boost::filesystem::path pathGeneratedSources = buildManager.getSources().getPathBuildGenerated(buildManager.getDescriptor().getArtefactId(), buildManager.getDescriptor().getArtefactVersion(), variantName, architecture, generatorEntry.first);
		os << "          <listOptionValue builtIn=\"false\" value=\"&quot;${workspace_loc:/" << projectPath << "/" << pathGeneratedSources.string() << "}&quot;\"/>\n";
	}

	// add dependency headers
	for(const auto& solvedDescriptor : solvedDescriptors.getElements()) {
		if(solvedDescriptor.getDescriptor().getSystemPath()) {
			if(!solvedDescriptor.getDescriptor().getSystemPath()->includePath.empty()) {
				os << "          <listOptionValue builtIn=\"false\" value=\"" << solvedDescriptor.getDescriptor().getSystemPath()->includePath << "\"/>\n";
			}
		}
		else {
			boost::filesystem::path path = buildManager.getSources().getPathBuildHeaders(solvedDescriptor.getArtefactId(), solvedDescriptor.getDescriptor().getArtefactVersion(), solvedDescriptor.getVariantName());
			os << "          <listOptionValue builtIn=\"false\" value=\"&quot;${workspace_loc:/" << projectPath << "/" << path.string() << "}&quot;\"/>\n";

			// and add generated sources of this dependency
			std::map<std::string, model::Generator> generators = solvedDescriptor.getVariant().getGeneratorsEffective();
			for(const auto generatorEntry : generators) {
				boost::filesystem::path pathGeneratedSources = buildManager.getSources().getPathBuildGenerated(solvedDescriptor.getArtefactId(), solvedDescriptor.getDescriptor().getArtefactVersion(), solvedDescriptor.getVariantName(), architecture, generatorEntry.first);
				os << "          <listOptionValue builtIn=\"false\" value=\"&quot;${workspace_loc:/" << projectPath << "/" << pathGeneratedSources.string() << "}&quot;\"/>\n";
			}
		}
	}

	os << "         </option>\n";
	os << "         <option IS_BUILTIN_EMPTY=\"false\" IS_VALUE_EMPTY=\"false\" id=\"gnu.cpp.compiler.option.preprocessor.def." << idGnuCppPreprocesorDef << "\" name=\"Defined symbols (-D)\" superClass=\"gnu.cpp.compiler.option.preprocessor.def\" useByScannerDiscovery=\"false\" valueType=\"definedSymbols\">\n";

	/***************
	 * add defines *
	 ***************/

	std::map<std::string, std::string> defines;

	// add private defines
	defines = variant.getDefinesPrivateEffective();
	for(const auto& defineEntry : defines) {
		os << "          <listOptionValue builtIn=\"false\" value=\"" << defineEntry.second << "\"/>\n";
	}

	// add public defines
	defines = variant.getDefinesPublicEffective();
	for(const auto& defineEntry : defines) {
		os << "          <listOptionValue builtIn=\"false\" value=\"" << defineEntry.second << "\"/>\n";
	}

	os << "         </option>\n";
	os << "         <inputType id=\"cdt.managedbuild.tool.gnu.cpp.compiler.input." << referencedXmlIds.idBuildmanagerToolGnuCompilerCppInput << "\" superClass=\"cdt.managedbuild.tool.gnu.cpp.compiler.input\"/>\n";
	os << "        </tool>\n";
	os << "        <tool id=\"cdt.managedbuild.tool.gnu.cross.c.linker." << idBuildmanagerToolGnuLinkerC << "\" name=\"Cross GCC Linker\" superClass=\"cdt.managedbuild.tool.gnu.cross.c.linker\"/>\n";
	os << "        <tool id=\"cdt.managedbuild.tool.gnu.cross.cpp.linker." << idBuildmanagerToolGnuLinkerCpp << "\" name=\"Cross G++ Linker\" superClass=\"cdt.managedbuild.tool.gnu.cross.cpp.linker\">\n";
	os << "         <option IS_BUILTIN_EMPTY=\"false\" IS_VALUE_EMPTY=\"false\" id=\"gnu.cpp.link.option.paths." << idGnuCppLinkerPath << "\" name=\"Library search path (-L)\" superClass=\"gnu.cpp.link.option.paths\" valueType=\"libPaths\">\n";

	/*********************
	 * add library paths *
	 *********************/

	// add library path from system dependency
	for(const auto& solvedDescriptor : solvedDescriptors.getElements()) {
		if(solvedDescriptor.getDescriptor().getSystemPath()) {
			if(!solvedDescriptor.getDescriptor().getSystemPath()->libraryPath.empty()) {
				os << "          <listOptionValue builtIn=\"false\" value=\"" << solvedDescriptor.getDescriptor().getSystemPath()->libraryPath << "\"/>\n";
			}
		}
	}
/*
	// add path to resulting executable
	{
		boost::filesystem::path path = getSources().getPathBuildLinkExecutable(getDescriptor().getArtefactId(), getDescriptor().getArtefactVersion(), variantName, architecture);
		os << "          <listOptionValue builtIn=\"false\" value=\"&quot;${workspace_loc:/" << projectPath << "/" << path.string() << "}&quot;\"/>\n";
	}
*/
	os << "         </option>\n";
	os << "         <option IS_BUILTIN_EMPTY=\"false\" IS_VALUE_EMPTY=\"false\" id=\"gnu.cpp.link.option.libs." << idGnuCppLinkerLibs << "\" superClass=\"gnu.cpp.link.option.libs\" valueType=\"libs\">\n";

	/*****************
	 * add libraries *
	 *****************/

	// add libraries from system dependency
	for(const auto& solvedDescriptor : solvedDescriptors.getElements()) {
		if(solvedDescriptor.getDescriptor().getSystemPath()) {
			if(!solvedDescriptor.getDescriptor().getSystemPath()->libraryName.empty()) {
				os << "          <listOptionValue builtIn=\"false\" srcPrefixMapping=\"\" srcRootPath=\"\" value=\"" << solvedDescriptor.getDescriptor().getSystemPath()->libraryName << "\"/>\n";
			}
		}
	}
	os << "         </option>\n";
	os << "         <inputType id=\"cdt.managedbuild.tool.gnu.cpp.linker.input." << idBuildmanagerToolGnuCppLinkerInput << "\" superClass=\"cdt.managedbuild.tool.gnu.cpp.linker.input\">\n";
	os << "          <additionalInput kind=\"additionalinputdependency\" paths=\"$(USER_OBJS)\"/>\n";
	os << "          <additionalInput kind=\"additionalinput\" paths=\"$(LIBS)\"/>\n";
	os << "         </inputType>\n";
	os << "        </tool>\n";
	os << "        <tool id=\"cdt.managedbuild.tool.gnu.cross.archiver." << idBuildmanagerToolGnuArchiver << "\" name=\"Cross GCC Archiver\" superClass=\"cdt.managedbuild.tool.gnu.cross.archiver\"/>\n";
	os << "        <tool id=\"cdt.managedbuild.tool.gnu.cross.assembler." << idBuildmanagerToolGnuAssembler << "\" name=\"Cross GCC Assembler\" superClass=\"cdt.managedbuild.tool.gnu.cross.assembler\">\n";
	os << "         <inputType id=\"cdt.managedbuild.tool.gnu.assembler.input." << idBuildmanagerToolGnuAssemblerInput << "\" superClass=\"cdt.managedbuild.tool.gnu.assembler.input\"/>\n";
	os << "        </tool>\n";
	os << "       </toolChain>\n";
	os << "      </folderInfo>\n";
	os << "      <sourceEntries>\n";

	/********************
	 * add source paths *
	 ********************/

	// add own sources
	for(const auto& pathSource :variant.getSourcesMainDirEffective()) {
		os << "       <entry flags=\"VALUE_WORKSPACE_PATH\" kind=\"sourcePath\" name=\"" << pathSource << "\"/>\n";
	}

	os << "      </sourceEntries>\n";
	os << "     </configuration>\n";
	os << "    </storageModule>\n";
	os << "    <storageModule moduleId=\"org.eclipse.cdt.core.externalSettings\"/>\n";
	os << "  </cconfiguration>\n";

	return referencedXmlIds;
}

} /* namespace generate */
} /* namespace transformer */
