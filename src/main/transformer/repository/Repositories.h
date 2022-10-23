#ifndef TRANSFORMER_REPOSITORY_REPOSITORIES_H_
#define TRANSFORMER_REPOSITORY_REPOSITORIES_H_

#include <transformer/repository/Repository.h>
#include <vector>
#include <memory>

namespace transformer {
namespace repository {

using Repositories = std::vector<std::unique_ptr<Repository>>;

} /* namespace repository */
} /* namespace transformer */

#endif /* TRANSFORMER_REPOSITORY_REPOSITORIES_H_ */
