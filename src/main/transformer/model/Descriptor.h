#ifndef TRANSFORMER_MODEL_DESCRIPTOR_H_
#define TRANSFORMER_MODEL_DESCRIPTOR_H_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <optional>

//#include <transformer/model/Solved.h>
#include <transformer/model/DependencySolved.h>
#include <transformer/model/SystemPath.h>
#include <transformer/model/Variant.h>
#include <transformer/model/Version.h>
#include <transformer/repository/Repository.h>

#include <vector> // because signature of definition of "vector<string> Variant::getGenerators()"

namespace transformer {

namespace repository {
class DescriptorReader;
}

namespace model {

class Descriptor {
	friend class repository::DescriptorReader;
public:
	Descriptor();
	Descriptor(const Descriptor&) = delete;
	Descriptor(Descriptor&&) noexcept = delete;
	Descriptor& operator=(const Descriptor&) = delete;
	Descriptor& operator=(Descriptor&&) noexcept = delete;

	const std::string& getArtefactId() const;
	const std::string& getArtefactName() const;
	const bool hasCommonVersion() const;
	const std::string& getArtefactVersion() const;
	const std::string& getApiVersion() const;

	const std::string& getBaseName() const;
	const std::string& getBaseVersion() const;
	const Descriptor* getBaseDescriptor() const;

	bool hasBuildDir() const;
	const std::string& getBuildDir() const;
	std::string getBuildDirEffective() const;

	std::set<std::string> getAllArchitectures() const;

	bool hasSpecialVariants() const;
	const std::string& getDefaultVariantName() const;
	std::vector<std::string> getSpecialVariantNames() const;
	/* only checks if variantName exists in map of variants. It does not check if name is defaultVariantName */
	bool hasSpecialVariant(const std::string& variantName) const;
	void addSpecialVariant(const std::string& variantName);

	/* returns default variant if variantName == ""
	 * returns named variant from map if variantName != "" or default variant if it does not exists in map AND variantName == this->variantName
	 */
	const Variant& getVariant(const std::string& variantName) const;
//	const std::map<std::string, std::unique_ptr<Variant>>& getSpecialVariants() const;

	bool isDefaultVariant(const Variant& variant) const;

	/* only checks if variantName exists in map of variants. It does not check if name is defaultVariantName */
	bool hasSolved(const std::string& variantName) const;
	void addSolved(const std::string& variantName);
	const std::map<std::string, std::vector<DependencySolved>>& getSolved() const;

	void setSystemPath(std::string libraryName, std::string libraryPath, std::string includePath);
	const std::optional<SystemPath>& getSystemPath() const;

private:
	std::string artefactId;
	std::string artefactName;
	std::string variantName;
	bool commonVersion = false;
	std::string artefactVersion;
	std::string apiVersion;

	std::string baseName;
	std::string baseArtefactVersion;

	std::string buildDir;

	Variant variant; // default definition
	std::map<std::string, std::unique_ptr<Variant>> variants; // named variants

	std::map<std::string, std::vector<DependencySolved>> solved;

	const Descriptor* baseDescriptor = nullptr;

	// beim Laden der Descriptoren werden auch von einem System-Repository (schaut nach, was auf dem OS installiert ist) sog. "virtuelle Descriptoren"  generiert.
	// Zum Auflösen des "best match" sowie des "possible match" Descriptor ist es erforderlich zu wissen ob es ein System- oder (echter) Repository-Descriptor ist.
	std::optional<SystemPath> systemPaths;

};

} /* namespace model */
} /* namespace transformer */

#endif /* TRANSFORMER_MODEL_DESCRIPTOR_H_ */
