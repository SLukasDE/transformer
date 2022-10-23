#ifndef TRANSFORMER_ARCHITECTURES_DRIVER_H_
#define TRANSFORMER_ARCHITECTURES_DRIVER_H_

#include <esl/object/Object.h>

#include <string>

namespace transformer {
namespace architectures {

class Driver : public esl::object::Object {
public:
	virtual void addSetting(const std::string& key, const std::string& value) = 0;
};

} /* namespace architectures */
} /* namespace transformer */

#endif /* TRANSFORMER_ARCHITECTURES_DRIVER_H_ */
