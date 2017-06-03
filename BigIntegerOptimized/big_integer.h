#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include "vector/myVector.h"
#include <string>
#include "big_integer_exceptions.h"

struct big_integer
{
	big_integer();
	big_integer(big_integer const& other);
	big_integer(int a);
	big_integer(unsigned int a);
	explicit big_integer(std::string const& str);

	big_integer& operator=(big_integer const& other);

	big_integer abs() const;
	big_integer& operator+=(big_integer const& other);
	big_integer& operator-=(big_integer const& other);
	big_integer& operator*=(big_integer const& other);
	big_integer& operator/=(big_integer const& other);
	big_integer& operator%=(big_integer const& other);

	big_integer& operator&=(big_integer const& other);
	big_integer& operator|=(big_integer const& other);
	big_integer& operator^=(big_integer const& other);

	big_integer& operator++();
	big_integer operator++(int notNeeded);


	big_integer& operator--();
	big_integer operator--(int notNeeded);

	big_integer& operator<<=(int shift);
	big_integer& operator>>=(int shift);

	friend big_integer operator-(big_integer const& a);
	friend big_integer operator+(big_integer const& a);
	friend big_integer operator~(big_integer const& a);


	friend bool operator==(big_integer const& a, big_integer const& b);
	friend bool operator!=(big_integer const& a, big_integer const& b);
	friend bool operator<(big_integer const& a, big_integer const& b);
	friend bool operator>(big_integer const& a, big_integer const& b);
	friend bool operator<=(big_integer const& a, big_integer const& b);
	friend bool operator>=(big_integer const& a, big_integer const& b);

	friend big_integer operator+(big_integer const &a, big_integer const& b);
	friend big_integer operator-(big_integer const &a, big_integer const& b);
	friend big_integer operator*(big_integer const &a, big_integer const& b);
	friend big_integer operator/(big_integer const &a, big_integer const& b);
	friend big_integer operator%(big_integer const &a, big_integer const& b);
	friend unsigned int operator%(big_integer const& first, unsigned int second);

	friend big_integer operator/(big_integer const &a, unsigned int b);
	friend big_integer operator/(big_integer const &a, int b);
	big_integer& operator/=(int other);
	big_integer& operator/=(unsigned int other);


	friend big_integer operator&(big_integer const &a, big_integer const& b);
	friend big_integer operator|(big_integer const &a, big_integer const& b);
	friend big_integer operator^(big_integer const &a, big_integer const& b);
	friend big_integer operator<<(big_integer const &a, int b);
	friend big_integer operator>>(big_integer const &a, int b);

	void swap(big_integer &other) noexcept;
	bool isNegative() const;
	big_integer(bool new_sign, myVector const &new_data);
	void normalize();
private:
	bool sign;
	myVector data;

	size_t length() const;
	unsigned int getInfDigit(size_t ind) const;
	unsigned int getDigit(size_t ind) const;
	void setSign(bool s);

	friend big_integer abstractBitOperation(big_integer const& first, big_integer const& second, char mode);
	friend big_integer transformStringToBigInteger(std::string const& str);
	friend unsigned int getNextDigit(big_integer const& first, big_integer const& second);
	friend big_integer getHighDigits(big_integer const& number, size_t n);
};

std::istream& operator>>(std::istream& out, big_integer& a);
std::ostream& operator<<(std::ostream& in, big_integer const& a);
std::string to_string(big_integer const& a);



#endif
