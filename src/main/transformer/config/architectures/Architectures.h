#ifndef TRANSFORMER_CONFIG_ARCHITECTURES_ARCHITECTURES_H_
#define TRANSFORMER_CONFIG_ARCHITECTURES_ARCHITECTURES_H_

#include <transformer/config/architectures/Architecture.h>
#include <transformer/config/Parser.h>

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS


#include <string>
#include <istream>
#include <map>
#include <set>
#include <functional>
#include <memory>

namespace transformer {
namespace config {
namespace architectures {

class Architectures {
public:
	Architectures();

	const std::vector<std::vector<std::string>>& getBranches() const noexcept;
	const std::set<std::string>& getDefaults() const noexcept;
	const std::set<std::string>& getOptionals() const noexcept;
	const std::map<std::set<std::string>, std::unique_ptr<Architecture>>& getArchitectures() const noexcept;

	void load(const boost::filesystem::path& path);
	void load(std::istream& iStream);

private:
	void load(Parser& parser);

	std::vector<std::vector<std::string>> branches;
	std::set<std::string> defaults;
	std::set<std::string> optionals;

	std::map<std::set<std::string>, std::unique_ptr<Architecture>> architectures;

	Architecture* activeArchitecture = nullptr;
};

} /* namespace architectures */
} /* namespace config */
} /* namespace transformer */

#endif /* TRANSFORMER_CONFIG_ARCHITECTURES_ARCHITECTURES_H_ */
