#ifndef TRANSFORMER_BUILD_GCC_BUILDCOMPILE_H_
#define TRANSFORMER_BUILD_GCC_BUILDCOMPILE_H_

#include <filesystem>
#include <map>
#include <string>

#include <transformer/build/gcc/BuildBase.h>
#include <transformer/build/gcc/Builder.h>
#include <transformer/files/Source.h>


namespace transformer {
namespace build {
namespace gcc {

class BuildCompile : public BuildBase {
public:
	BuildCompile(const Builder& builder, const files::Source& targetSource, bool isTest = false);
	~BuildCompile();

	int run() const override;

	void addIncludePath(const std::filesystem::path& path);
	void addSystemIncludePath(const std::filesystem::path& path);

	void addDefine(const std::string& key, const std::string& value);

	void addSource(const files::Source& source) override;

private:
	std::map<std::filesystem::path, bool> isSystemIncludeByPath;
	std::map<std::string, std::string> defines;

	const std::string& execCxxOpts;
};

} /* namespace gcc */
} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_GCC_BUILDCOMPILE_H_ */
