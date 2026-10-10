
#include "mathlang/variables.hpp"

namespace mathlang {

bool Variables::define(const std::string& name, double value) {
    if (name.empty()) return false;
    return values_.emplace(name, value).second;
}

bool Variables::set(const std::string& name, double value) {
    auto it = values_.find(name);
    if (it == values_.end()) return false;
    it->second = value;
    return true;
}

bool Variables::define_or_set(const std::string& name, double value) {
    if (name.empty()) return false;
    values_[name] = value;
    return true;
}

bool Variables::exists(const std::string& name) const {
    return values_.find(name) != values_.end();
}

bool Variables::get(const std::string& name, double& value) const {
    auto it = values_.find(name);
    if (it == values_.end()) return false;
    value = it->second;
    return true;
}

bool Variables::erase(const std::string& name) {
    return values_.erase(name) > 0;
}

void Variables::clear() {
    values_.clear();
}

std::size_t Variables::size() const noexcept {
    return values_.size();
}

bool Variables::empty() const noexcept {
    return values_.empty();
}

} // namespace mathlang
