#ifndef TRANSFORMER_BUILD_SOLVEDDESCRIPTOR_H_
#define TRANSFORMER_BUILD_SOLVEDDESCRIPTOR_H_

#include <utility>
#include <string>
#include <transformer/model/Variant.h>
#include <string>
#include <vector>
#include <transformer/model/Descriptor.h>
#include <transformer/model/Variant.h>
#include <transformer/build/Sources.h>
#include <transformer/files/Source.h>

namespace transformer {
namespace build {

class SolvedDescriptor {
public:
	SolvedDescriptor(const model::Variant& variant, std::string variantName);

	const std::string& getArtefactId() const;
	const std::string& getArtefactName() const;
	const std::string& getArtefactVersion() const;
	const std::string& getApiVersion() const;

	const model::Descriptor& getDescriptor() const;
	const model::Variant& getVariant() const;
	const std::string& getVariantName() const;

	const std::vector<std::string>& getArchitectures() const;
//	bool hasArchitecture(const std::string& architecture) const;

	bool hasStatic;
	bool hasDynamic;
//	bool hasExecutable;
	bool hasSource;

private:
	const model::Variant& variant;
	const std::string variantName;
};

} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_SOLVEDDESCRIPTOR_H_ */
