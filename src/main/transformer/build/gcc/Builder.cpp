#include <transformer/build/gcc/Builder.h>
#include <transformer/build/gcc/VariantBuilder.h>
#include <transformer/build/SolvedDescriptor.h>

namespace transformer {
namespace build {
namespace gcc {

const std::string Builder::execCxx = "g++";
const std::string Builder::execCxxOptsCompileMain = "-std=c++17 -fPIC -Wall -fmessage-length=0 -O0";
const std::string Builder::execCxxOptsCompileTest = "-std=c++17 -fPIC -Wall -fmessage-length=0 -O0 -g3 -fprofile-arcs -ftest-coverage";
const std::string Builder::execCxxOptsLinkDynamic = "-shared";
const std::string Builder::execCxxOptsLinkExecutableMain = "";
const std::string Builder::execCxxOptsLinkExecutableTest = "-lgcov --coverage";
const std::string Builder::execAr = "ar";
const std::string Builder::execArOpts = "-rs";

Builder::Builder(const model::Descriptor& descriptor, std::string architecture, const Sources& sources, const BuildManager& buildManager, const std::vector<std::string>& variants)
: build::Builder(descriptor, std::move(architecture), buildManager, sources)
{
	for(const auto& variantName : variants) {
		std::unique_ptr<VariantBuilder> variantBuilder(new VariantBuilder(*this, variantName));
		variantBuilderByVariantName.emplace(variantName, std::move(variantBuilder));
	}
}

Builder::~Builder() {
}

int Builder::onlyDependencies() {
	int exitCode = 0;

	for(const auto& variantNameVariantBuilder : variantBuilderByVariantName) {
		exitCode = variantNameVariantBuilder.second->onlyDependencies();
		if(exitCode != 0) {
			break;
		}
	}

	return exitCode;
}

int Builder::onlyGenerateSources() {
	int exitCode = 0;

	for(const auto& variantNameVariantBuilder : variantBuilderByVariantName) {
		exitCode = variantNameVariantBuilder.second->onlyGenerateSources();
		if(exitCode != 0) {
			break;
		}
	}

	return exitCode;
}

int Builder::onlyCompile() {
	int exitCode = 0;

	for(const auto& variantNameVariantBuilder : variantBuilderByVariantName) {
		exitCode = variantNameVariantBuilder.second->onlyCompile();
		if(exitCode != 0) {
			break;
		}
	}

	return exitCode;
}

int Builder::onlyTest() {
	int exitCode = 0;

	for(const auto& variantNameVariantBuilder : variantBuilderByVariantName) {
		exitCode = variantNameVariantBuilder.second->onlyTest();
		if(exitCode != 0) {
			break;
		}
	}

	return exitCode;
}

int Builder::onlyLink() {
	int exitCode = 0;

	for(const auto& variantNameVariantBuilder : variantBuilderByVariantName) {
		exitCode = variantNameVariantBuilder.second->onlyLink();
		if(exitCode != 0) {
			break;
		}
	}

	return exitCode;
}

int Builder::onlySite() {
	int exitCode = 0;

	for(const auto& variantNameVariantBuilder : variantBuilderByVariantName) {
		exitCode = variantNameVariantBuilder.second->onlySite();
		if(exitCode != 0) {
			break;
		}
	}

	return exitCode;
}

int Builder::onlyPackage() {
	int exitCode = 0;

	for(const auto& variantNameVariantBuilder : variantBuilderByVariantName) {
		exitCode = variantNameVariantBuilder.second->onlyPackage();
		if(exitCode != 0) {
			break;
		}
	}

	return exitCode;
}

int Builder::onlyProvide() {
	int exitCode = 0;

	for(const auto& variantNameVariantBuilder : variantBuilderByVariantName) {
		exitCode = variantNameVariantBuilder.second->onlyProvide();
		if(exitCode != 0) {
			break;
		}
	}

	return exitCode;
}

const std::string& Builder::getExecCxx() const {
	return execCxx;
}

const std::string& Builder::getExecCxxOptsCompileMain() const {
	return execCxxOptsCompileMain;
}

const std::string& Builder::getExecCxxOptsCompileTest() const {
	return execCxxOptsCompileTest;
}

const std::string& Builder::getExecCxxOptsLinkDynamic() const {
	return execCxxOptsLinkDynamic;
}

const std::string& Builder::getExecCxxOptsLinkExecutableMain() const {
	return execCxxOptsLinkExecutableMain;
}

const std::string& Builder::getExecCxxOptsLinkExecutableTest() const {
	return execCxxOptsLinkExecutableTest;
}

const std::string& Builder::getExecAr() const {
	return execAr;
}

const std::string& Builder::getExecArOpts() const {
	return execArOpts;
}

} /* namespace gcc */
} /* namespace build */
} /* namespace transformer */
