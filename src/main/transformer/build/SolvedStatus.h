#ifndef TRANSFORMER_BUILD_SOLVEDSTATUS_H_
#define TRANSFORMER_BUILD_SOLVEDSTATUS_H_

namespace transformer {
namespace build {

struct SolvedStatus {
	bool descriptorLoaded = false;
	bool headersLoaded = false;
	bool sourcesLoaded = false;
	bool generatedLoaded = false;
	bool generatorsLoaded = false;
	bool executableLoaded = false;
	bool staticLibLoaded = false;
	bool dynamicLibLoaded = false;
};

} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_SOLVEDSTATUS_H_ */
