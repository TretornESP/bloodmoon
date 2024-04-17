#include "math.h"

double math_pow(double base, double exponent) {
    double result = 1;
    for (int i = 0; i < exponent; i++) {
        result *= base;
    }
    return result;
}
double math_sqrt(double x) {
    double result = 1;
    for (int i = 0; i < 10; i++) {
        result = 0.5 * (result + x / result);
    }
    return result;

}
double math_cbrt(double x) {
    double result = 1;
    for (int i = 0; i < 10; i++) {
        result = (1.0 / 3.0) * ((2 * result) + (x / (result * result)));
    }
    return result;

}
double math_hypot(double x, double y) {
    return sqrt(pow(x, 2) + pow(y, 2));
}
double math_exp(double x) {
    double result = 1;
    for (int i = 0; i < 10; i++) {
        result += pow(x, i) / factorial(i);
    }
    return result;
}
double math_factorial(double x) {
    double result = 1;
    for (int i = 1; i <= x; i++) {
        result *= i;
    }
    return result;
}