#ifndef TRANSFORMER_CONFIG_EXCEPTION_H_
#define TRANSFORMER_CONFIG_EXCEPTION_H_

#include <transformer/config/Parser.h>

#include <stdexcept>

namespace transformer {
namespace config {

class Exception : public std::runtime_error {
public:
	Exception(const Parser& parser);
	explicit Exception(const Parser& parser, const std::string& message);
	explicit Exception(const Parser& parser, const char* message);

	std::size_t getLineNo() const;
	const std::string& getLine() const;

private:
	std::size_t lineNo;
	std::string line;
};

} /* namespace config */
} /* namespace transformer */

#endif /* TRANSFORMER_CONFIG_EXCEPTION_H_ */
