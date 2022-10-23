#ifndef TRANSFORMER_CONFIG_ARCHITECTURES_ARCHITECTURE_H_
#define TRANSFORMER_CONFIG_ARCHITECTURES_ARCHITECTURE_H_

#include <transformer/config/architectures/Language.h>

#include <string>
#include <set>
#include <map>
#include <memory>

namespace transformer {
namespace config {
namespace architectures {

struct Architecture {
	std::set<std::string> ids;

	std::map<std::string, std::unique_ptr<Language>> languages;
};

} /* namespace architectures */
} /* namespace config */
} /* namespace transformer */

#endif /* TRANSFORMER_CONFIG_ARCHITECTURES_ARCHITECTURE_H_ */
