#ifndef TRANSFORMER_ARCHITECTURES_CONFIG_H_
#define TRANSFORMER_ARCHITECTURES_CONFIG_H_

#include <transformer/architectures/Specifiers.h>
#include <transformer/config/architectures/Architectures.h>

#include <filesystem>
#include <istream>
#include <ostream>

namespace transformer {
namespace architectures {

class Config {
public:
	Config(Specifiers& specifiers);

	void load(const std::filesystem::path& path);
	void load(std::istream& iStream);

	void save(std::ostream& oStream);

private:
	void load();

	Specifiers& specifiers;
	config::architectures::Architectures configArchitectures;
};

} /* namespace architectures */
} /* namespace transformer */

#endif /* TRANSFORMER_ARCHITECTURES_CONFIG_H_ */
