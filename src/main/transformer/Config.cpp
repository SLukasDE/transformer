#include <transformer/Config.h>

namespace transformer {

unsigned short Config::parallelCount = 20;
std::string Config::scriptFile;
//std::string Config::scriptFile = "tbuild_script.sh";

#ifdef NO_ESLX

#ifdef USE_BOOST_PROCESS
std::string Config::eslProcessImplementation = "transformer/system/ProcessBoost";
#else
std::string Config::eslProcessImplementation = "transformer/system/ProcessSystem";
#endif

#else
std::string Config::eslProcessImplementation = "eslx/system/Process";
#endif

} /* namespace transformer */
