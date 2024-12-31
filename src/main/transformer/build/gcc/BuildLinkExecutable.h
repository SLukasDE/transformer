#ifndef TRANSFORMER_BUILD_GCC_BUILDLINKEXECUTABLE_H_
#define TRANSFORMER_BUILD_GCC_BUILDLINKEXECUTABLE_H_

#include <string>
#include <transformer/build/gcc/BuildLink.h>
#include <transformer/build/gcc/Builder.h>
#include <transformer/files/Source.h>

namespace transformer {
namespace build {
namespace gcc {

class BuildLinkExecutable : public BuildLink {
public:
	BuildLinkExecutable(const Builder& builder, const files::Source& targetSource, std::string targetName, bool isTest = false);
	~BuildLinkExecutable();

	int run() const override;

private:
	const std::string& execCxxOpts;
};

} /* namespace gcc */
} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_GCC_BUILDLINKEXECUTABLE_H_ */
