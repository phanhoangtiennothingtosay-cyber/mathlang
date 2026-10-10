#include "mathlang/math.hpp"

#include <cmath>
#include <limits>

namespace mathlang::math {

namespace {

Result success(double value) {
    return {
        true,
        value,
        {}
    };
}

Result failure(const char* message) {
    return {
        false,
        0.0,
        message
    };
}

} // namespace

Result add(double a, double b) {
    const double value = a + b;
    if (!std::isfinite(value)) return failure("Addition result is not finite.");
    return success(value);
}

Result subtract(double a, double b) {
    const double value = a - b;
    if (!std::isfinite(value)) return failure("Subtraction result is not finite.");
    return success(value);
}

Result multiply(double a, double b) {
    const double value = a * b;
    if (!std::isfinite(value)) return failure("Multiplication result is not finite.");
    return success(value);
}

Result divide(double a, double b) {
    if (b == 0.0) {
        return failure("Division by zero is undefined.");
    }

    const double value = a / b;
    if (!std::isfinite(value)) return failure("Division result is not finite.");
    return success(value);
}

Result modulo(double a, double b) {
    if (b == 0.0) {
        return failure("Modulo by zero is undefined.");
    }

    const double value = std::fmod(a, b);
    if (!std::isfinite(value)) return failure("Modulo result is not finite.");
    return success(value);
}

Result factorial(double x) {
    if (!std::isfinite(x)) {
        return failure(
            "Factorial requires a finite number."
        );
    }

    if (x < 0.0) {
        return failure(
            "Factorial is undefined for negative integers."
        );
    }

    if (std::floor(x) != x) {
        return failure(
            "Factorial requires a non-negative integer."
        );
    }

    if (x > 170.0) {
        return failure(
            "Factorial result exceeds double range."
        );
    }

    double result = 1.0;

    for (double i = 2.0; i <= x; i += 1.0) {
        result *= i;
    }

    return success(result);
}

Result equal(double a, double b) {
    return success(a == b ? 1.0 : 0.0);
}

Result not_equal(double a, double b) {
    return success(a != b ? 1.0 : 0.0);
}

Result less(double a, double b) {
    return success(a < b ? 1.0 : 0.0);
}

Result greater(double a, double b) {
    return success(a > b ? 1.0 : 0.0);
}

Result less_equal(double a, double b) {
    return success(a <= b ? 1.0 : 0.0);
}

Result greater_equal(double a, double b) {
    return success(a >= b ? 1.0 : 0.0);
}

} // namespace mathlang::math
