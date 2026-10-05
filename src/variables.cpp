#include "mathlang/variables.hpp"

namespace mathlang {

bool Variables::define(
    const std::string& name,
    double value
) {
    return values_.emplace(name, value).second;
}

bool Variables::set(
    const std::string& name,
    double value
) {
    const auto it = values_.find(name);

    if (it == values_.end()) {
        return false;
    }

    it->second = value;
    return true;
}

bool Variables::exists(
    const std::string& name
) const {
    return values_.find(name) != values_.end();
}

bool Variables::get(
    const std::string& name,
    double& value
) const {
    const auto it = values_.find(name);

    if (it == values_.end()) {
        return false;
    }

    value = it->second;
    return true;
}

void Variables::clear() {
    values_.clear();
}

} // namespace mathlang
