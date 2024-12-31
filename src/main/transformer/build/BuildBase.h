#ifndef TRANSFORMER_BUILD_BUILDBASE_H_
#define TRANSFORMER_BUILD_BUILDBASE_H_

#include <string>
#include <transformer/files/Source.h>

namespace transformer {
namespace build {

class BuildBase {
public:
	BuildBase(const files::Source& targetSource);
	virtual ~BuildBase();

	const files::Source& getTargetSource() const;

	virtual int run() const = 0;

private:
	const files::Source& targetSource;
};

} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_BUILDBASE_H_ */
