
#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>

namespace mathlang {

class Variables {
public:
    bool define(const std::string& name, double value);
    bool set(const std::string& name, double value);
    bool define_or_set(const std::string& name, double value);

    bool exists(const std::string& name) const;
    bool get(const std::string& name, double& value) const;
    bool erase(const std::string& name);

    void clear();

    std::size_t size() const noexcept;
    bool empty() const noexcept;

private:
    std::unordered_map<std::string, double> values_;
};

} // namespace mathlang
