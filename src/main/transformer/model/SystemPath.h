#ifndef TRANSFORMER_MODEL_SYSTEMPATH_H_
#define TRANSFORMER_MODEL_SYSTEMPATH_H_

#include <string>

namespace transformer {
namespace model {

struct SystemPath {
	std::string libraryName;
	std::string includePath;
	std::string libraryPath;
};

} /* namespace model */
} /* namespace transformer */

#endif /* TRANSFORMER_MODEL_SYSTEMPATH_H_ */
