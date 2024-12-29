#ifndef TRANSFORMER_CONFIG_H_
#define TRANSFORMER_CONFIG_H_

#include <string>

//#define NO_OPENESL
//#define DRY_RUN := parallelCount == 0

namespace transformer {

struct Config final {
	Config() = delete;

	static std::string scriptFile;
	static unsigned short parallelCount;
	static std::string eslProcessImplementation;
};

} /* namespace transformer */

#endif /* TRANSFORMER_CONFIG_H_ */
