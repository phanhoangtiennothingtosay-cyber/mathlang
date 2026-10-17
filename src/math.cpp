#include "mathlang/math.hpp"

#include <algorithm>
#include <cctype>
#include <exception>
#include <string>
#include <utility>

namespace mathlang::math {
namespace {

Result success(Number value) {
    return {true, std::move(value), {}};
}

Result failure(const std::string& message) {
    return {false, Number(0), message};
}

bool finite(const Number& value) {
    return boost::multiprecision::isfinite(value);
}

std::string normalized_name(std::string_view name) {
    std::string result(name);

    for (char& c : result) {
        c = static_cast<char>(
            std::tolower(static_cast<unsigned char>(c)));
    }

    return result;
}

Result checked(Number value, const std::string& message) {
    if (!finite(value)) {
        return failure(message);
    }

    return success(std::move(value));
}

bool integer_value(const Number& value) {
    return finite(value) &&
           boost::multiprecision::floor(value) == value;
}

} // namespace

// ============================================================
// Basic arithmetic
// ============================================================

Result add(const Number& a, const Number& b) {
    return checked(a + b, "Addition result is not finite.");
}

Result subtract(const Number& a, const Number& b) {
    return checked(a - b, "Subtraction result is not finite.");
}

Result multiply(const Number& a, const Number& b) {
    return checked(a * b, "Multiplication result is not finite.");
}

Result divide(const Number& a, const Number& b) {
    if (b == 0) {
        return failure("Division by zero is undefined.");
    }

    return checked(a / b, "Division result is not finite.");
}

Result modulo(const Number& a, const Number& b) {
    if (b == 0) {
        return failure("Modulo by zero is undefined.");
    }

    return checked(
        boost::multiprecision::fmod(a, b),
        "Modulo result is not finite.");
}

Result power(const Number& a, const Number& b) {
    if (a == 0 && b < 0) {
        return failure(
            "Zero cannot be raised to a negative power.");
    }

    try {
        return checked(
            boost::multiprecision::pow(a, b),
            "Power result is not a finite real number.");
    } catch (const std::exception&) {
        return failure(
            "Power is undefined for these real-number operands.");
    }
}

// ============================================================
// Factorial
// ============================================================

Result factorial(const Number& x) {
    if (!finite(x)) {
        return failure("Factorial requires a finite number.");
    }

    if (x < 0 || !integer_value(x)) {
        return failure(
            "Factorial requires a non-negative integer.");
    }

    if (x > 10000) {
        return failure(
            "Factorial input is too large (maximum: 10000).");
    }

    const unsigned int n = x.convert_to<unsigned int>();

    Number result = 1;

    for (unsigned int i = 2; i <= n; ++i) {
        result *= i;
    }

    return checked(
        std::move(result),
        "Factorial result is not finite.");
}

// ============================================================
// Standard mathematical functions
//
// Function names are case-insensitive.
// Example: Pi(), PI(), and pi() are equivalent.
// ============================================================

Result call_function(
    std::string_view function_name,
    const std::vector<Number>& args) {

    const std::string name = normalized_name(function_name);

    const auto arity_error = [&]() {
        return failure(
            "Function '" + name +
            "' received the wrong number of arguments.");
    };

    const auto zero = [&]() { return args.empty(); };
    const auto one = [&]() { return args.size() == 1; };
    const auto two = [&]() { return args.size() == 2; };
    const auto three = [&]() { return args.size() == 3; };

    try {
        // ----------------------------------------------------
        // Constants: Pi(), E(), Tau(), GoldenRatio()
        // ----------------------------------------------------

        if (name == "pi") {
            if (!zero()) return arity_error();
            return success(Number(PI_DECIMAL));
        }

        if (name == "tau") {
            if (!zero()) return arity_error();
            return success(Number(PI_DECIMAL) * 2);
        }

        if (name == "e") {
            if (!zero()) return arity_error();

            return success(Number(
                "2.718281828459045235360287471352662497757247093699959574966967"));
        }

        if (name == "goldenratio" ||
            name == "golden_ratio") {
            if (!zero()) return arity_error();

            return checked(
                (Number(1) +
                 boost::multiprecision::sqrt(Number(5))) / 2,
                "GoldenRatio() result is not finite.");
        }

        // ----------------------------------------------------
        // Absolute value and roots
        // ----------------------------------------------------

        if (name == "abs") {
            if (!one()) return arity_error();

            return success(
                boost::multiprecision::abs(args[0]));
        }

        if (name == "sqrt") {
            if (!one()) return arity_error();

            if (args[0] < 0) {
                return failure(
                    "Sqrt() requires a non-negative input.");
            }

            return checked(
                boost::multiprecision::sqrt(args[0]),
                "Sqrt() result is not finite.");
        }

        if (name == "cbrt") {
            if (!one()) return arity_error();

            const Number magnitude =
                boost::multiprecision::pow(
                    boost::multiprecision::abs(args[0]),
                    Number(1) / 3);

            return checked(
                args[0] < 0 ? -magnitude : magnitude,
                "Cbrt() result is not finite.");
        }

        if (name == "root") {
            if (!two()) return arity_error();

            const Number& radicand = args[0];
            const Number& degree = args[1];

            if (degree == 0) {
                return failure(
                    "Root(value, degree) requires a non-zero degree.");
            }

            if (radicand == 0 && degree < 0) {
                return failure(
                    "Zero cannot have a negative root degree.");
            }

            if (radicand < 0) {
                if (!integer_value(degree)) {
                    return failure(
                        "A negative radicand requires an odd integer root degree.");
                }

                const Number remainder =
                    boost::multiprecision::abs(
                        boost::multiprecision::fmod(
                            degree, Number(2)));

                if (remainder != 1) {
                    return failure(
                        "A negative radicand requires an odd integer root degree.");
                }
            }

            const Number magnitude =
                boost::multiprecision::pow(
                    boost::multiprecision::abs(radicand),
                    Number(1) / degree);

            return checked(
                radicand < 0 ? -magnitude : magnitude,
                "Root() result is not a finite real number.");
        }

        // ----------------------------------------------------
        // Exponential and logarithmic functions
        // ----------------------------------------------------

        if (name == "exp") {
            if (!one()) return arity_error();

            return checked(
                boost::multiprecision::exp(args[0]),
                "Exp() result is not finite.");
        }

        if (name == "ln" || name == "log") {
            if (args.size() != 1 &&
                !(name == "log" && two())) {
                return arity_error();
            }

            if (args[0] <= 0) {
                return failure(
                    "Logarithm requires a positive input.");
            }

            const Number numerator =
                boost::multiprecision::log(args[0]);

            if (name == "log" && two()) {
                if (args[1] <= 0 || args[1] == 1) {
                    return failure(
                        "Log(value, base) requires base > 0 and base != 1.");
                }

                return checked(
                    numerator /
                        boost::multiprecision::log(args[1]),
                    "Log() result is not finite.");
            }

            return checked(
                numerator,
                "Logarithm result is not finite.");
        }

        if (name == "log10") {
            if (!one()) return arity_error();

            if (args[0] <= 0) {
                return failure(
                    "Log10() requires a positive input.");
            }

            return checked(
                boost::multiprecision::log10(args[0]),
                "Log10() result is not finite.");
        }

        if (name == "log2") {
            if (!one()) return arity_error();

            if (args[0] <= 0) {
                return failure(
                    "Log2() requires a positive input.");
            }

            return checked(
                boost::multiprecision::log(args[0]) /
                    boost::multiprecision::log(Number(2)),
                "Log2() result is not finite.");
        }

        // ----------------------------------------------------
        // Trigonometric functions: radians
        // ----------------------------------------------------

        if (name == "sin") {
            if (!one()) return arity_error();

            return checked(
                boost::multiprecision::sin(args[0]),
                "Sin() result is not finite.");
        }

        if (name == "cos") {
            if (!one()) return arity_error();

            return checked(
                boost::multiprecision::cos(args[0]),
                "Cos() result is not finite.");
        }

        if (name == "tan") {
            if (!one()) return arity_error();

            const Number cosine =
                boost::multiprecision::cos(args[0]);

            if (boost::multiprecision::abs(cosine) <
                Number("1e-50")) {
                return failure(
                    "Tan() is undefined at or extremely close to a pole.");
            }

            return checked(
                boost::multiprecision::tan(args[0]),
                "Tan() result is not finite.");
        }

        if (name == "asin" || name == "acos") {
            if (!one()) return arity_error();

            if (args[0] < -1 || args[0] > 1) {
                return failure(
                    "Asin()/Acos() input must be between -1 and 1.");
            }

            if (name == "asin") {
                return checked(
                    boost::multiprecision::asin(args[0]),
                    "Asin() result is not finite.");
            }

            return checked(
                boost::multiprecision::acos(args[0]),
                "Acos() result is not finite.");
        }

        if (name == "atan") {
            if (!one()) return arity_error();

            return checked(
                boost::multiprecision::atan(args[0]),
                "Atan() result is not finite.");
        }

        // ----------------------------------------------------
        // Trigonometric functions: degrees
        //
        // SinDeg(30) = 0.5
        // Also accepts SinD(30) and Sin_Deg(30).
        // ----------------------------------------------------

        if (name == "sind" ||
            name == "sindeg" ||
            name == "sin_deg") {
            if (!one()) return arity_error();

            const Number radians =
                args[0] * Number(PI_DECIMAL) / 180;

            return checked(
                boost::multiprecision::sin(radians),
                "SinDeg() result is not finite.");
        }

        if (name == "cosd" ||
            name == "cosdeg" ||
            name == "cos_deg") {
            if (!one()) return arity_error();

            const Number radians =
                args[0] * Number(PI_DECIMAL) / 180;

            return checked(
                boost::multiprecision::cos(radians),
                "CosDeg() result is not finite.");
        }

        if (name == "tand" ||
            name == "tandeg" ||
            name == "tan_deg") {
            if (!one()) return arity_error();

            const Number radians =
                args[0] * Number(PI_DECIMAL) / 180;

            const Number cosine =
                boost::multiprecision::cos(radians);

            if (boost::multiprecision::abs(cosine) <
                Number("1e-50")) {
                return failure(
                    "TanDeg() is undefined at this angle.");
            }

            return checked(
                boost::multiprecision::tan(radians),
                "TanDeg() result is not finite.");
        }

        if (name == "asind" ||
            name == "asindeg" ||
            name == "asin_deg") {
            if (!one()) return arity_error();

            if (args[0] < -1 || args[0] > 1) {
                return failure(
                    "AsinDeg() input must be between -1 and 1.");
            }

            return checked(
                boost::multiprecision::asin(args[0]) *
                    180 / Number(PI_DECIMAL),
                "AsinDeg() result is not finite.");
        }

        if (name == "acosd" ||
            name == "acosdeg" ||
            name == "acos_deg") {
            if (!one()) return arity_error();

            if (args[0] < -1 || args[0] > 1) {
                return failure(
                    "AcosDeg() input must be between -1 and 1.");
            }

            return checked(
                boost::multiprecision::acos(args[0]) *
                    180 / Number(PI_DECIMAL),
                "AcosDeg() result is not finite.");
        }

        if (name == "atand" ||
            name == "atandeg" ||
            name == "atan_deg") {
            if (!one()) return arity_error();

            return checked(
                boost::multiprecision::atan(args[0]) *
                    180 / Number(PI_DECIMAL),
                "AtanDeg() result is not finite.");
        }

        // ----------------------------------------------------
        // Hyperbolic functions
        // ----------------------------------------------------

        if (name == "sinh") {
            if (!one()) return arity_error();

            return checked(
                boost::multiprecision::sinh(args[0]),
                "Sinh() result is not finite.");
        }

        if (name == "cosh") {
            if (!one()) return arity_error();

            return checked(
                boost::multiprecision::cosh(args[0]),
                "Cosh() result is not finite.");
        }

        if (name == "tanh") {
            if (!one()) return arity_error();

            return checked(
                boost::multiprecision::tanh(args[0]),
                "Tanh() result is not finite.");
        }

        if (name == "asinh") {
            if (!one()) return arity_error();

            return checked(
                boost::multiprecision::asinh(args[0]),
                "Asinh() result is not finite.");
        }

        if (name == "acosh") {
            if (!one()) return arity_error();

            if (args[0] < 1) {
                return failure(
                    "Acosh() requires an input greater than or equal to 1.");
            }

            return checked(
                boost::multiprecision::acosh(args[0]),
                "Acosh() result is not finite.");
        }

        if (name == "atanh") {
            if (!one()) return arity_error();

            if (args[0] <= -1 || args[0] >= 1) {
                return failure(
                    "Atanh() input must be strictly between -1 and 1.");
            }

            return checked(
                boost::multiprecision::atanh(args[0]),
                "Atanh() result is not finite.");
        }

        // ----------------------------------------------------
        // Rounding and sign
        // ----------------------------------------------------

        if (name == "floor") {
            if (!one()) return arity_error();

            return success(
                boost::multiprecision::floor(args[0]));
        }

        if (name == "ceil" || name == "ceiling") {
            if (!one()) return arity_error();

            return success(
                boost::multiprecision::ceil(args[0]));
        }

        if (name == "round") {
            if (!one()) return arity_error();

            return success(
                boost::multiprecision::round(args[0]));
        }

        if (name == "trunc") {
            if (!one()) return arity_error();

            return success(
                boost::multiprecision::trunc(args[0]));
        }

        if (name == "sign") {
            if (!one()) return arity_error();

            return success(
                args[0] < 0 ? Number(-1)
                : args[0] > 0 ? Number(1)
                              : Number(0));
        }

        // ----------------------------------------------------
        // Multi-argument functions
        // ----------------------------------------------------

        if (name == "min") {
            if (!two()) return arity_error();

            return success(
                std::min(args[0], args[1]));
        }

        if (name == "max") {
            if (!two()) return arity_error();

            return success(
                std::max(args[0], args[1]));
        }

        if (name == "pow") {
            if (!two()) return arity_error();

            return power(args[0], args[1]);
        }

        if (name == "hypot") {
            if (!two()) return arity_error();

            return checked(
                boost::multiprecision::sqrt(
                    args[0] * args[0] +
                    args[1] * args[1]),
                "Hypot() result is not finite.");
        }

        if (name == "atan2") {
            if (!two()) return arity_error();

            return checked(
                boost::multiprecision::atan2(
                    args[0], args[1]),
                "Atan2() result is not finite.");
        }

        if (name == "clamp") {
            if (!three()) return arity_error();

            if (args[1] > args[2]) {
                return failure(
                    "Clamp(value, lower, upper) requires lower <= upper.");
            }

            return success(
                std::max(args[1],
                         std::min(args[0], args[2])));
        }

        // ----------------------------------------------------
        // Angle conversions
        //
        // Deg(x): radians -> degrees
        // Rad(x): degrees -> radians
        // ----------------------------------------------------

        if (name == "deg") {
            if (!one()) return arity_error();

            return checked(
                args[0] * 180 / Number(PI_DECIMAL),
                "Deg() result is not finite.");
        }

        if (name == "rad") {
            if (!one()) return arity_error();

            return checked(
                args[0] * Number(PI_DECIMAL) / 180,
                "Rad() result is not finite.");
        }

        // ----------------------------------------------------
        // Factorial functions
        // ----------------------------------------------------

        if (name == "fact" || name == "factorial") {
            if (!one()) return arity_error();

            return factorial(args[0]);
        }

    } catch (const std::exception&) {
        return failure(
            "Function '" + name +
            "' failed: input is outside its supported domain.");
    }

    return failure("Unknown math function: " + name + ".");
}

// ============================================================
// Numeric comparisons
// The current runtime represents Boolean results separately.
// These functions retain the established math::Result API.
// ============================================================

Result equal(const Number& a, const Number& b) {
    return success(a == b ? Number(1) : Number(0));
}

Result not_equal(const Number& a, const Number& b) {
    return success(a != b ? Number(1) : Number(0));
}

Result less(const Number& a, const Number& b) {
    return success(a < b ? Number(1) : Number(0));
}

Result greater(const Number& a, const Number& b) {
    return success(a > b ? Number(1) : Number(0));
}

Result less_equal(const Number& a, const Number& b) {
    return success(a <= b ? Number(1) : Number(0));
}

Result greater_equal(const Number& a, const Number& b) {
    return success(a >= b ? Number(1) : Number(0));
}

} // namespace mathlang::math