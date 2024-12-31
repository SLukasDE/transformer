#include <transformer/build/BuildBase.h>

namespace transformer {
namespace build {

BuildBase::BuildBase(const files::Source& aTargetSource)
: targetSource(aTargetSource)
{
}

BuildBase::~BuildBase() {
}

const files::Source& BuildBase::getTargetSource() const {
	return targetSource;
}

} /* namespace build */
} /* namespace transformer */
