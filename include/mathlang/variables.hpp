#pragma once

#include <string>
#include <unordered_map>

namespace mathlang {

class Variables {
public:
    bool define(
        const std::string& name,
        double value
    );

    bool set(
        const std::string& name,
        double value
    );

    bool exists(
        const std::string& name
    ) const;

    bool get(
        const std::string& name,
        double& value
    ) const;

    void clear();

private:
    std::unordered_map<std::string, double> values_;
};

} // namespace mathlang
