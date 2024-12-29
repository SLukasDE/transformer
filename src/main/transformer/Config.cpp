#include <transformer/Config.h>

namespace transformer {

unsigned short Config::parallelCount = 20;
std::string Config::scriptFile;
//std::string Config::scriptFile = "tbuild_script.sh";

#ifdef NO_OPENESL
std::string Config::eslProcessImplementation = "transformer/system/ProcessSystem";
#else
std::string Config::eslProcessImplementation = "esl/system/ZSProcess";
#endif

} /* namespace transformer */
