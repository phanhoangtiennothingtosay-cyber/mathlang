#pragma once

#include <boost/multiprecision/cpp_dec_float.hpp>

#include <string>
#include <string_view>
#include <vector>

namespace mathlang::math {

// Header-only decimal floating point with 100 decimal digits of precision.
// It keeps the 60-decimal-place PI literal meaningful during calculations.
using Number = boost::multiprecision::cpp_dec_float_100;

inline constexpr char PI_DECIMAL[] =
    "3.141592653589793238462643383279502884197169399375105820974944";

struct Result {
    bool valid = false;
    Number value = 0;
    std::string error;
};

Result add(const Number& a, const Number& b);
Result subtract(const Number& a, const Number& b);
Result multiply(const Number& a, const Number& b);
Result divide(const Number& a, const Number& b);
Result modulo(const Number& a, const Number& b);
Result power(const Number& a, const Number& b);
Result factorial(const Number& x);
Result call_function(std::string_view name, const std::vector<Number>& args);

Result equal(const Number& a, const Number& b);
Result not_equal(const Number& a, const Number& b);
Result less(const Number& a, const Number& b);
Result greater(const Number& a, const Number& b);
Result less_equal(const Number& a, const Number& b);
Result greater_equal(const Number& a, const Number& b);

} // namespace mathlang::math
