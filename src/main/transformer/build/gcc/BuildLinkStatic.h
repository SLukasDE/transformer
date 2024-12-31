#ifndef TRANSFORMER_BUILD_GCC_BUILDLINKSTATIC_H_
#define TRANSFORMER_BUILD_GCC_BUILDLINKSTATIC_H_

#include <string>

#include <transformer/build/gcc/BuildLink.h>
#include <transformer/build/gcc/Builder.h>
#include <transformer/files/Source.h>

namespace transformer {
namespace build {
namespace gcc {

class BuildLinkStatic : public BuildLink {
public:
	BuildLinkStatic(const Builder& builder, const files::Source& targetSource, std::string targetName);
	~BuildLinkStatic();

	int run() const override;

private:
};

} /* namespace gcc */
} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_GCC_BUILDLINKSTATIC_H_ */
