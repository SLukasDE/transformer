#ifndef TRANSFORMER_CONFIG_ARCHITECTURES_LANGUAGE_H_
#define TRANSFORMER_CONFIG_ARCHITECTURES_LANGUAGE_H_

#include <transformer/config/architectures/Driver.h>
#include <transformer/config/architectures/Profile.h>

#include <string>
#include <map>
#include <memory>

namespace transformer {
namespace config {
namespace architectures {

struct Language {
	std::string name;

	std::unique_ptr<Driver> driver;

	std::map<std::string, std::unique_ptr<Profile>> profiles;
};

} /* namespace architectures */
} /* namespace config */
} /* namespace transformer */

#endif /* TRANSFORMER_CONFIG_ARCHITECTURES_LANGUAGE_H_ */
