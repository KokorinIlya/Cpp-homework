#pragma once
#ifndef BIGINT_BIG_INTEGER_EXCEPTIONS
#define BIGINT_BIG_INTEGER_EXCEPTIONS

#include <stdexcept>

struct ParsingException : public std::runtime_error
{
	ParsingException(const std::string& message) : runtime_error(message) {}
};

struct EvaluateException : public std::runtime_error
{
	EvaluateException(const std::string& message) : runtime_error(message) {}
};

struct UnknownModeException : public std::invalid_argument
{
	UnknownModeException(const std::string& message) : invalid_argument(message) {};
};
#endif