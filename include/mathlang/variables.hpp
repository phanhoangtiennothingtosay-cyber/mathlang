#pragma once

#include "mathlang/math.hpp"

#include <cstddef>
#include <string>
#include <unordered_map>

namespace mathlang {

class Variables {
public:
    bool define(const std::string& name, const math::Number& value);
    bool set(const std::string& name, const math::Number& value);
    bool define_or_set(const std::string& name, const math::Number& value);
    bool exists(const std::string& name) const;
    bool get(const std::string& name, math::Number& value) const;
    bool erase(const std::string& name);
    void clear();
    std::size_t size() const noexcept;
    bool empty() const noexcept;
private:
    std::unordered_map<std::string, math::Number> values_;
};

} // namespace mathlang
