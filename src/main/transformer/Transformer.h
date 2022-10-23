#ifndef TRANSFORMER_TRANSFORMER_H_
#define TRANSFORMER_TRANSFORMER_H_

#include <transformer/architectures/Specifiers.h>
#include <transformer/repository/Database.h>
#include <transformer/build/BuildManager.h>
#include <transformer/build/gcc/BuilderFactory.h>

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS


#include <memory>
#include <vector>
#include <set>
#include <string>

namespace transformer {

class Transformer {
public:
	Transformer(int argc, char **argv);
	~Transformer() = default;

	void loadSettings();
	void loadDescriptor();

	static void printUsage();

	std::size_t checkArguments() const;

	int run();

	static const std::size_t npos;

private:
	void loadArchitectures();

	/* settings */
	boost::filesystem::path localRepoPath;
	std::string buildFile = "tbuild.cfg";

	/* architectures */
	architectures::Specifiers specifiers;

	std::vector<std::string> arguments;

	bool hasOptionArchitectures = false;
	std::set<std::string> optionArchitectures;

	bool hasOptionVariants = false;
	std::set<std::string> optionVariants;

	transformer::repository::Database database;
	std::unique_ptr<model::Descriptor> descriptor;
	std::unique_ptr<build::BuildManager> buildManager;

	build::gcc::BuilderFactory builderFactoryGCC;

	const model::Descriptor& getDescriptor() const;
	build::BuildManager& getBuildManager();
	void printDescriptor() const;
	std::string readValue(std::size_t& index) const;
#if 1
	std::vector<std::string> readValues(std::size_t& index) const;
#else
	std::pair<std::vector<std::string>,std::size_t> readValues(std::size_t& index) const;
	static std::vector<std::string> splitValues(std::string values);
#endif
};

} /* namespace transformer */

#endif /* TRANSFORMER_TRANSFORMER_H_ */
