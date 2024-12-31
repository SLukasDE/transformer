#ifndef TRANSFORMER_CONFIG_BUILD_BUILD_H_
#define TRANSFORMER_CONFIG_BUILD_BUILD_H_

#include <transformer/config/build/Variant.h>
#include <transformer/config/Parser.h>

#include <filesystem>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <istream>
#include <functional>
#include <memory>

namespace transformer {
namespace config {
namespace build {

class Build {
public:
	Build();

	void load(const std::filesystem::path path);
	void load(std::istream& iStream);

private:
	void load(Parser& parser);
	Variant& getVariant(const std::string& variantName);

	std::string id;
	std::string name;
	std::string buildDirectory = "build";

	std::vector<std::unique_ptr<Variant>> variants;
	std::map<std::string, std::reference_wrapper<Variant>> variantByName;
	std::reference_wrapper<Variant> activeVariant;
};

} /* namespace build */
} /* namespace config */
} /* namespace transformer */

#endif /* TRANSFORMER_CONFIG_BUILD_BUILD_H_ */
