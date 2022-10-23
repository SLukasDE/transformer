#ifndef TRANSFORMER_PLUGIN_H_
#define TRANSFORMER_PLUGIN_H_

#include <esl/plugin/Registry.h>

namespace transformer {

class Plugin final {
public:
	Plugin() = delete;
	static void install(esl::plugin::Registry& registry, const char* data);
};

} /* namespace transformer */

#endif /* TRANSFORMER_PLUGIN_H_ */
