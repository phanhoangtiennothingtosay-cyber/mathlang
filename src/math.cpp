#include "mathlang/math.hpp"

#include <algorithm>
#include <cctype>
#include <exception>
#include <string>
#include <utility>

namespace mathlang::math {
namespace {

Result success(Number value) { return {true, std::move(value), {}}; }
Result failure(const std::string& message) { return {false, Number(0), message}; }

bool finite(const Number& value) { return boost::multiprecision::isfinite(value); }

std::string normalized_name(std::string_view name) {
    std::string result(name);
    for (char& c : result) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return result;
}

Result checked(Number value, const char* message) {
    if (!finite(value)) return failure(message);
    return success(std::move(value));
}

bool integer_value(const Number& value) {
    return finite(value) && boost::multiprecision::floor(value) == value;
}

} // namespace

Result add(const Number& a, const Number& b) { return checked(a + b, "Addition result is not finite."); }
Result subtract(const Number& a, const Number& b) { return checked(a - b, "Subtraction result is not finite."); }
Result multiply(const Number& a, const Number& b) { return checked(a * b, "Multiplication result is not finite."); }

Result divide(const Number& a, const Number& b) {
    if (b == 0) return failure("Division by zero is undefined.");
    return checked(a / b, "Division result is not finite.");
}

Result modulo(const Number& a, const Number& b) {
    if (b == 0) return failure("Modulo by zero is undefined.");
    return checked(boost::multiprecision::fmod(a, b), "Modulo result is not finite.");
}

Result power(const Number& a, const Number& b) {
    if (a == 0 && b < 0) return failure("Zero cannot be raised to a negative power.");
    try {
        return checked(boost::multiprecision::pow(a, b),
                       "Power result is not a finite real number.");
    } catch (const std::exception&) {
        return failure("Power is undefined for these real-number operands.");
    }
}

Result factorial(const Number& x) {
    if (!finite(x)) return failure("Factorial requires a finite number.");
    if (x < 0) return failure("Factorial requires a non-negative integer.");
    if (!integer_value(x)) return failure("Factorial requires a whole number; fractional values are not supported.");
    if (x > 10000) return failure("Factorial input is too large (maximum: 10000).");

    const unsigned int n = x.convert_to<unsigned int>();
    Number result = 1;
    for (unsigned int i = 2; i <= n; ++i) result *= i;
    return checked(std::move(result), "Factorial result is not finite.");
}

Result call_function(std::string_view function_name, const std::vector<Number>& args) {
    const std::string name = normalized_name(function_name);
    const auto arity_error = [&]() { return failure("Function '" + name + "' received the wrong number of arguments."); };
    const auto one = [&]() { return args.size() == 1; };
    const auto two = [&]() { return args.size() == 2; };
    const auto three = [&]() { return args.size() == 3; };

    try {
        if (name == "abs") { if (!one()) return arity_error(); return success(boost::multiprecision::abs(args[0])); }
        if (name == "sqrt") {
            if (!one()) return arity_error();
            if (args[0] < 0) return failure("sqrt() requires a value greater than or equal to zero.");
            return checked(boost::multiprecision::sqrt(args[0]), "sqrt() result is not finite.");
        }
        if (name == "cbrt") {
            if (!one()) return arity_error();
            const Number magnitude = boost::multiprecision::pow(boost::multiprecision::abs(args[0]), Number(1) / 3);
            return success(args[0] < 0 ? -magnitude : magnitude);
        }
        if (name == "exp") { if (!one()) return arity_error(); return checked(boost::multiprecision::exp(args[0]), "exp() result is not finite."); }
        if (name == "ln" || name == "log") {
            if (args.size() != 1 && !(name == "log" && two())) return arity_error();
            if (args[0] <= 0) return failure(name + "() requires a positive input.");
            const Number numerator = boost::multiprecision::log(args[0]);
            if (name == "log" && two()) {
                if (args[1] <= 0 || args[1] == 1) return failure("log(value, base) requires base > 0 and base != 1.");
                return checked(numerator / boost::multiprecision::log(args[1]), "log() result is not finite.");
            }
            return checked(numerator, "log() result is not finite.");
        }
        if (name == "log10") { if (!one()) return arity_error(); if (args[0] <= 0) return failure("log10() requires a positive input."); return checked(boost::multiprecision::log10(args[0]), "log10() result is not finite."); }
        if (name == "log2") { if (!one()) return arity_error(); if (args[0] <= 0) return failure("log2() requires a positive input."); return checked(boost::multiprecision::log(args[0]) / boost::multiprecision::log(Number(2)), "log2() result is not finite."); }
        if (name == "sin") { if (!one()) return arity_error(); return checked(boost::multiprecision::sin(args[0]), "sin() result is not finite."); }
        if (name == "cos") { if (!one()) return arity_error(); return checked(boost::multiprecision::cos(args[0]), "cos() result is not finite."); }
        if (name == "tan") {
            if (!one()) return arity_error();
            const Number cosine = boost::multiprecision::cos(args[0]);
            // Inputs closer than the PI literal's meaningful precision to a pole
            // are treated as undefined instead of printing a misleading huge value.
            if (boost::multiprecision::abs(cosine) < Number("1e-50"))
                return failure("tan() is undefined at or extremely close to an odd multiple of pi/2.");
            return checked(boost::multiprecision::tan(args[0]), "tan() is undefined for this input.");
        }
        if (name == "asin") { if (!one()) return arity_error(); if (args[0] < -1 || args[0] > 1) return failure("asin() input must be between -1 and 1."); return checked(boost::multiprecision::asin(args[0]), "asin() result is not finite."); }
        if (name == "acos") { if (!one()) return arity_error(); if (args[0] < -1 || args[0] > 1) return failure("acos() input must be between -1 and 1."); return checked(boost::multiprecision::acos(args[0]), "acos() result is not finite."); }
        if (name == "atan") { if (!one()) return arity_error(); return checked(boost::multiprecision::atan(args[0]), "atan() result is not finite."); }
        if (name == "sinh") { if (!one()) return arity_error(); return checked(boost::multiprecision::sinh(args[0]), "sinh() result is not finite."); }
        if (name == "cosh") { if (!one()) return arity_error(); return checked(boost::multiprecision::cosh(args[0]), "cosh() result is not finite."); }
        if (name == "tanh") { if (!one()) return arity_error(); return checked(boost::multiprecision::tanh(args[0]), "tanh() result is not finite."); }
        if (name == "asinh") { if (!one()) return arity_error(); return checked(boost::multiprecision::asinh(args[0]), "asinh() result is not finite."); }
        if (name == "acosh") { if (!one()) return arity_error(); if (args[0] < 1) return failure("acosh() requires an input greater than or equal to 1."); return checked(boost::multiprecision::acosh(args[0]), "acosh() result is not finite."); }
        if (name == "atanh") { if (!one()) return arity_error(); if (args[0] <= -1 || args[0] >= 1) return failure("atanh() input must be strictly between -1 and 1."); return checked(boost::multiprecision::atanh(args[0]), "atanh() result is not finite."); }
        if (name == "floor") { if (!one()) return arity_error(); return success(boost::multiprecision::floor(args[0])); }
        if (name == "ceil" || name == "ceiling") { if (!one()) return arity_error(); return success(boost::multiprecision::ceil(args[0])); }
        if (name == "round") { if (!one()) return arity_error(); return success(boost::multiprecision::round(args[0])); }
        if (name == "trunc") { if (!one()) return arity_error(); return success(boost::multiprecision::trunc(args[0])); }
        if (name == "sign") { if (!one()) return arity_error(); return success(args[0] < 0 ? Number(-1) : (args[0] > 0 ? Number(1) : Number(0))); }
        if (name == "min") { if (!two()) return arity_error(); return success(std::min(args[0], args[1])); }
        if (name == "max") { if (!two()) return arity_error(); return success(std::max(args[0], args[1])); }
        if (name == "pow") { if (!two()) return arity_error(); return power(args[0], args[1]); }
        if (name == "hypot") { if (!two()) return arity_error(); return checked(boost::multiprecision::sqrt(args[0] * args[0] + args[1] * args[1]), "hypot() result is not finite."); }
        if (name == "atan2") { if (!two()) return arity_error(); return checked(boost::multiprecision::atan2(args[0], args[1]), "atan2() result is not finite."); }
        if (name == "clamp") {
            if (!three()) return arity_error();
            if (args[1] > args[2]) return failure("clamp(value, lower, upper) requires lower <= upper.");
            return success(std::max(args[1], std::min(args[0], args[2])));
        }
        if (name == "deg") { if (!one()) return arity_error(); return success(args[0] * 180 / Number(PI_DECIMAL)); }
        if (name == "rad") { if (!one()) return arity_error(); return success(args[0] * Number(PI_DECIMAL) / 180); }
        if (name == "fact" || name == "factorial") { if (!one()) return arity_error(); return factorial(args[0]); }
    } catch (const std::exception&) {
        return failure("Function '" + name + "' failed: input is outside its supported real-number domain.");
    }
    return failure("Unknown math function: " + name + ".");
}

Result equal(const Number& a, const Number& b) { return success(a == b ? Number(1) : Number(0)); }
Result not_equal(const Number& a, const Number& b) { return success(a != b ? Number(1) : Number(0)); }
Result less(const Number& a, const Number& b) { return success(a < b ? Number(1) : Number(0)); }
Result greater(const Number& a, const Number& b) { return success(a > b ? Number(1) : Number(0)); }
Result less_equal(const Number& a, const Number& b) { return success(a <= b ? Number(1) : Number(0)); }
Result greater_equal(const Number& a, const Number& b) { return success(a >= b ? Number(1) : Number(0)); }

} // namespace mathlang::math
