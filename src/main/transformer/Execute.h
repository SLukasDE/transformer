#ifndef TRANSFORMER_EXECUTE_H_
#define TRANSFORMER_EXECUTE_H_

#include <transformer/Config.h>

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

#include <vector>
#include <utility>
#include <string>

namespace transformer {

// goal::Goal braucht keine Sources im Konstruktor mehr (entsprechende addRule-Methoden mit Strings löschen)
// dann ist der Builder quasi überflüssig - das kann man durch Goals schnell erreichen

class Execute final {
public:
	Execute() = delete;

	static void mkdir(const boost::filesystem::path& path);
	static void remove(const boost::filesystem::path& path);
	static void removeAll(const boost::filesystem::path& path);
	static void copy(const boost::filesystem::path& fromPath, const boost::filesystem::path& toPath);
	static void copyFile(const boost::filesystem::path& fromPath, const boost::filesystem::path& toPath);

	static void run(const std::string& command);
	static void run(const std::string& command, int& exitCode);
	static int run(std::vector<std::string> textCommandList);

	static int tar(const boost::filesystem::path& tarFile, const boost::filesystem::path& fromPath);
	static int untar(const boost::filesystem::path& tarFile, const boost::filesystem::path& toPath);

//private:
	static void addLineToScript(const std::string& line);

};

} /* namespace transformer */

#endif /* TRANSFORMER_EXECUTE_H_ */
