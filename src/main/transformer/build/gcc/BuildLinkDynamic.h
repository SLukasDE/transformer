#ifndef TRANSFORMER_BUILD_GCC_BUILDLINKDYNAMIC_H_
#define TRANSFORMER_BUILD_GCC_BUILDLINKDYNAMIC_H_

#include <string>
#include <transformer/build/gcc/BuildLink.h>
#include <transformer/build/gcc/Builder.h>
#include <transformer/files/Source.h>

namespace transformer {
namespace build {
namespace gcc {

class BuildLinkDynamic : public BuildLink {
public:
	BuildLinkDynamic(const Builder& builder, const files::Source& targetSource, std::string targetName, std::string soName);
	~BuildLinkDynamic();

	int run() const override;

	const std::string& getSoName() const;

private:
	const std::string soName;
};

} /* namespace gcc */
} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_GCC_BUILDLINKDYNAMIC_H_ */
