#ifndef TRANSFORMER_REPOSITORY_DESCRIPTORWRITER_H_
#define TRANSFORMER_REPOSITORY_DESCRIPTORWRITER_H_

#include <vector>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

#include <transformer/model/Descriptor.h>
#include <transformer/model/Variant.h>
#include <transformer/model/Version.h>
#include <transformer/model/VersionRange.h>
#include <transformer/model/VersionRanges.h>
#include <transformer/model/Dependency.h>
#include <transformer/model/DependencySolved.h>
#include <ostream>

namespace transformer {
namespace repository {

class DescriptorWriter {
public:
	DescriptorWriter(const model::Descriptor& descriptor);
	~DescriptorWriter();

	void write(const boost::filesystem::path path) const;
	void write(std::ostream& os) const;

private:
	void writeVersion(std::ostream& os, const model::Version& version) const;
	void writeVersionRange(std::ostream& os, const model::VersionRange& versionRange) const;
	void writeVersionRanges(std::ostream& os, const model::VersionRanges& versionRanges) const;
	void writeVariant(std::ostream& os, const model::Variant& variant) const;
	void writeSolved(std::ostream& os, const std::vector<model::DependencySolved>& solved) const;
	void writeDependency(std::ostream& os, const model::Dependency& dependency) const;
	void writeDependencySolved(std::ostream& os, const model::DependencySolved& dependencySolved) const;

	const model::Descriptor& descriptor;
};

} /* namespace repository */
} /* namespace transformer */

#endif /* TRANSFORMER_REPOSITORY_DESCRIPTORWRITER_H_ */
