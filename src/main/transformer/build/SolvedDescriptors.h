#ifndef TRANSFORMER_BUILD_SOLVEDDESCRIPTORS_H_
#define TRANSFORMER_BUILD_SOLVEDDESCRIPTORS_H_

#include <vector>
#include <string>
#include <functional>
#include <transformer/build/SolvedDescriptor.h>

namespace transformer {
namespace build {

class SolvedDescriptors {
public:
	SolvedDescriptors(std::vector<SolvedDescriptor> solvedDescriptors);
//	virtual ~SolvedDescriptors();

	const std::vector<SolvedDescriptor>& getElements() const;
	const SolvedDescriptor& getSolvedDescriptor(const std::string& artefactId) const;
	std::vector<std::reference_wrapper<const SolvedDescriptor>> getSolvedDescirptorsOfDependency(const std::string& artefactId) const;

private:
	const std::vector<SolvedDescriptor> solvedDescriptors;
};

} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_SOLVEDDESCRIPTORS_H_ */
