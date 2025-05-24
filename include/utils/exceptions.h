#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

class OrderException : public std::runtime_error {
public:
    explicit OrderException(const std::string& msg) : std::runtime_error(msg) {}
};

class PaymentException : public std::runtime_error {
public:
    explicit PaymentException(const std::string& msg) : std::runtime_error(msg) {}
};

#endif