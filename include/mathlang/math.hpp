#pragma once

#include <string>

namespace mathlang::math {

struct Result {
    bool valid = false;
    double value = 0.0;
    std::string error;
};

Result add(double a, double b);
Result subtract(double a, double b);
Result multiply(double a, double b);
Result divide(double a, double b);
Result modulo(double a, double b);

Result factorial(double x);

Result equal(double a, double b);
Result not_equal(double a, double b);
Result less(double a, double b);
Result greater(double a, double b);
Result less_equal(double a, double b);
Result greater_equal(double a, double b);

} // namespace mathlang::math
