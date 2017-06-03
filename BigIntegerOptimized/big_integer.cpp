#include "big_integer.h"
#include <algorithm>
#include <stdexcept>
#include <cassert>
#include <set>
#include <limits>
#include <climits>

using namespace std;

const unsigned long long numberOfDigits = 32;
const unsigned long long base = 1ll << numberOfDigits;
const unsigned int MAX_DIGIT = UINT32_MAX;

const unsigned int bases[10] = {
	1,
	10,
	100,
	1000,
	10000,
	100000,
	1000000,
	10000000,
	100000000,
	1000000000
};

set <char> knownBitModes{ '&', '|', '^' };

string setCharToString(set <char> x)
{
	string res = "";
	for (set <char>::iterator it = x.begin(); it != x.end(); ++it)
	{
		res += (*it);
		res += " ";
	}
	return res;
}

template <typename digitType>
digitType doBitOperation(digitType a, digitType b, char mode)
{
	if (mode == '&')
	{
		return a & b;
	}
	if (mode == '|')
	{
		return a | b;
	}
	if (mode == '^')
	{
		return a ^ b;
	}
	throw UnknownModeException(static_cast<std::string>("Used unknown mode ") + mode
		+ " try using one of this modes: " + setCharToString(knownBitModes));
}

unsigned int transformCharToDigit(char c)
{
	if (isdigit(c))
	{
		return static_cast<unsigned int>(c - '0');
	}
	string errorMessage = static_cast<string>("Character ") + c + " is not digit";
	throw ParsingException(errorMessage);
}

char transformDigitToChar(unsigned int x)
{
	return static_cast<char>('0' + x);
}

string indexToString(size_t i) noexcept
{
	string res = "";
	while (i > 0)
	{
		unsigned int digit = i % 10;
		i /= 10;
		res += transformDigitToChar(digit);
	}
	reverse(res.begin(), res.end());
	return res;
}

string constructErrorPointer(const string& expr, size_t index)
{
	string res = expr + "\n";
	for (size_t i = 0; i < index; i++)
	{
		res += " ";
	}
	res += "^";
	return res;
}

size_t big_integer::length() const 
{
	return data.size();
}

bool big_integer::isNegative() const 
{
	return sign;
}

big_integer big_integer::abs() const {
	return sign ? -(*this) : *this;
}

void big_integer::swap(big_integer &other) noexcept 
{
	std::swap(data, other.data);
	std::swap(sign, other.sign);
}

unsigned int big_integer::getDigit(size_t ind) const 
{
	return data[ind];
}

unsigned int big_integer::getInfDigit(size_t ind) const
{
	if (ind < length())
	{
		return data[ind];
	}
	 if (sign) 
	{
		return MAX_DIGIT;
	}
	return 0;
}


void big_integer::normalize() 
{
	while (!data.empty() && (!sign && data.back() == 0 || sign && data.back() == MAX_DIGIT)) 
	{
		data.pop_back();
	}
}

big_integer::big_integer(bool _sign, myVector const & _data) : sign(_sign), data(_data)
{
	normalize();
}

big_integer::big_integer() : sign(false), data(0) {}

big_integer::big_integer(big_integer const & other) : sign(other.sign), data(other.data) 
{
	normalize();
}

big_integer::big_integer(int a) : sign(a < 0), data{ static_cast <unsigned int>(a) }
{
	normalize();
}

big_integer::big_integer(unsigned int a) : sign(false), data{a}
{
	normalize();
}

big_integer& big_integer::operator=(big_integer const &other)
{
	big_integer temp(other);
	swap(temp);
	return *this;
}

big_integer& big_integer::operator+=(big_integer const &other) 
{
	return *this = *this + other;
}

big_integer& big_integer::operator-=(big_integer const &other) 
{
	return *this = *this - other;
}

big_integer& big_integer::operator*=(big_integer const &other) 
{
	return *this = *this * other;
}

big_integer& big_integer::operator/=(big_integer const& other)
{
	return *this = *this / other;
}

big_integer& big_integer::operator%=(big_integer const& other)
{
	return *this = *this % other;
}

big_integer& big_integer::operator&=(big_integer const &other)
{
	return *this = *this & other;
}

big_integer& big_integer::operator|=(big_integer const &other) 
{
	return *this = *this | other;
}

big_integer& big_integer::operator^=(big_integer const &other)
{
	return *this = *this ^ other;
}

big_integer& big_integer::operator<<=(int other) 
{
	return *this = *this << other;
}

big_integer& big_integer::operator>>=(int other)
{
	return *this = *this >> other;
}

big_integer operator+(big_integer const& a)
{
	return big_integer(a);
}

big_integer operator-(big_integer const& first) 
{
	return ++(~first);
}

big_integer operator~(big_integer const& a)
{
	size_t size = a.length();
	myVector temp(size);
	for (size_t i = 0; i < size; i++)
	{
		temp[i] = ~a.getInfDigit(i);
	}
	return big_integer(!a.isNegative(), temp);
}


big_integer& big_integer::operator++()
{
	*this += 1;
	return *this;
}

big_integer big_integer::operator++(int notNeeded)
{
	big_integer temp = *this + 1; //temp = x + 1, this = x
	swap(temp); // temp = x, this = x + 1
	return temp; // return x, change this
}

big_integer& big_integer:: operator--()
{
	*this -= 1;
	return *this;
}

big_integer big_integer::operator--(int notNeeded)
{
	big_integer temp = *this - 1; //temp = x - 1, this = x
	swap(temp); // temp = x, this = x - 1
	return temp; // return x, change this
}

big_integer abstractBitOperation(big_integer const& first, big_integer const& second, char mode)
{
	size_t size = max(first.length(), second.length());
	myVector temp(size);
	for (size_t i = 0; i < size; i++)
	{
		temp[i] = doBitOperation(first.getInfDigit(i), second.getInfDigit(i), mode);
	}
	return big_integer(doBitOperation(first.isNegative(), second.isNegative(), mode), temp);
}


big_integer operator&(big_integer const& first, big_integer const& second)
{
	return abstractBitOperation(first, second, '&');
}

big_integer operator|(big_integer const& first, big_integer const& second)
{
	return abstractBitOperation(first, second, '|');
}

big_integer operator^(big_integer const& first, big_integer const& second)
{
	return abstractBitOperation(first, second, '^');
}

big_integer operator<<(big_integer const& first, int shift)
{
	if (shift == 0)
	{
		return big_integer(first);
	}
	if (shift < 0)
	{
		return first >> (-shift);
	}
	unsigned long long mod = shift % numberOfDigits;
	unsigned long long div = shift / numberOfDigits;
	//casts are safe, shift > 0, numberOfDigits > 0
	size_t size = static_cast<size_t>(first.length() + div + 1);
	myVector temp(size);
	temp[div] = static_cast<unsigned int>((first.getInfDigit(0) << mod) & (base - 1));
	temp[div + 1] = static_cast<unsigned int>((first.getInfDigit(0) >> (numberOfDigits - mod)) & (base - 1));
	for (size_t i = static_cast<size_t>(div + 1); i < size; i++)
	{
		unsigned int firstDigit = first.getInfDigit(static_cast<unsigned int>(i - div));
		unsigned int secondDigit = first.getInfDigit(static_cast<unsigned int>(i - div - 1));
		temp[i] = static_cast<unsigned int>(static_cast<unsigned long long>(firstDigit << mod)
			| static_cast<unsigned long long>(secondDigit) >> (numberOfDigits - mod));
	}
	return big_integer(first.isNegative(), temp);
}

big_integer operator>>(big_integer const& first, int shift)
{
	if (shift == 0)
	{
		return big_integer(first);
	}
	if (shift < 0)
	{
		return first << (-shift);
	}
	unsigned long long mod = shift % numberOfDigits;
	unsigned long long div = shift / numberOfDigits;
	//casts are safe, shift > 0, numberOfDigits > 0
	if (first.length() < div)
	{
		myVector res;
		return big_integer(false, res);
	}
	size_t size = static_cast<size_t>(first.length() - div);
	//cast is safe, size >= div
	myVector temp(size);
	for (size_t i = 0; i < size; i++)
	{
		unsigned int firstDigit = first.getInfDigit(static_cast<unsigned int>(i + div));
		unsigned int secondDigit = first.getInfDigit(static_cast<unsigned int>(i + div + 1));
		temp[i] = static_cast<unsigned int>((static_cast<unsigned long long>(firstDigit) >> mod)
			| (static_cast<unsigned long long>(secondDigit) << (numberOfDigits - mod)));
	}
	return big_integer(first.isNegative(), temp);
}

bool operator==(big_integer const &a, big_integer const &b) 
{
	return (a.sign == b.sign) && (a.data == b.data);
}

bool operator!=(big_integer const &a, big_integer const &b) {
	return !(a == b);
}

bool operator<(big_integer const& a, big_integer const& b)
{
	if (a.isNegative() != b.isNegative())
	{
		return a.isNegative() > b.isNegative();
	}
	if (a.isNegative())
	{
		return (-b) < (-a);
	}
	if (a.data.size() != b.data.size())
	{
		return a.data.size() < b.data.size();
	}
	for (int i = a.data.size() - 1; i >= 0; i--)
	{
		unsigned int firstDigit = a.getDigit(static_cast<size_t>(i));
		unsigned int secondDigit = b.getDigit(static_cast<size_t>(i));
		if (firstDigit != secondDigit)
		{
			return firstDigit < secondDigit;
		}
	}
	return false;
}


bool operator>(big_integer const& a, big_integer const& b)
{
	return b < a;
}

bool operator<=(big_integer const& a, big_integer const& b)
{
	return !(a > b);
}

bool operator>=(big_integer const& a, big_integer const& b)
{
	return !(a < b);
}

unsigned int operator%(big_integer const& first, unsigned int second)
{
	if (second == 0)
	{
		throw EvaluateException("Division by zero");
	}
	big_integer absFirst(first.abs());
	size_t size = first.length();
	int carry = 0;
	for (int i = size - 1; i >= 0; i--)
	{
		unsigned long long cur = absFirst.getDigit(static_cast<size_t >(i)) + carry * base; //cast is safe: i >= 0
		carry = static_cast<int>(cur % second);
	}
	return carry;
}

string to_string(big_integer const& arg)
{
	string result;
	big_integer absArg = arg.abs();
	bool isNeg = arg.isNegative();
	while (absArg > 0)
	{
		const unsigned int blockLen = 9;
		const unsigned int block = bases[blockLen];
		int t = absArg % block;
		absArg /= block;
		for (int i = 0; i < blockLen; i++)
		{
			result.push_back(static_cast<char>('0' + t % 10)); //cast is safe: t % 10 < 10 => '0' + t % 10  <= '9'
			t /= 10;
		}
	}
	while (result.size() > 1 && result.back() == '0')
	{
		result.pop_back();
	}
	if (result.empty())
	{
		result.push_back('0');
	}
	if (isNeg)
	{
		result.push_back('-');
	}
	reverse(result.begin(), result.end());
	return result;
}

big_integer operator+(big_integer const& first, big_integer const& second)
{
	unsigned long long carry = 0;
	size_t size = max(first.data.size(), second.data.size()) + 1;
	myVector temp(size);
	for (size_t i = 0; i < size; i++)
	{
		unsigned long long curValue = carry + first.getInfDigit(i) + second.getInfDigit(i);
		carry = curValue >> numberOfDigits; //equals carry = (sum / 2^32)
		temp[i] = static_cast<unsigned int>(curValue & (base - 1)); // equals temp[i] = (sum % 2^32)
																	//cast is safe, base - 1 == 2^32 - 1 => curValue & (base - 1) <= 2^32 - 1 => safe cast to UInt
	}
	bool sgn = ((temp[size - 1] >> (numberOfDigits - 1)) > 0);
	return big_integer(sgn, temp); //construct from <bool, vetor>
}
big_integer operator-(big_integer const &a, big_integer const &b) 
{
	return a + (-b);
}

void big_integer::setSign(bool s)
{
	if (s != sign)
	{
		*this = -*this;
	}
}

big_integer operator*(big_integer const& first, big_integer const& second)
{
	bool sign = first.isNegative() ^ second.isNegative();
	big_integer absFirst(first.abs());
	big_integer absSecond(second.abs());
	myVector temp(absFirst.length() + absSecond.length() + 1);
	if (absFirst.length() > absSecond.length())
	{
		swap(absFirst, absSecond);
	}
	for (size_t i = 0; i < absFirst.length(); i++)
	{
		unsigned long long carry = 0;
		for (size_t j = 0; j < absSecond.length(); j++)
		{
			size_t k = i + j;
			unsigned long long tmp =
				static_cast<unsigned long long>(absFirst.getDigit(i)) * absSecond.getDigit(j);
			unsigned int tmpMod = static_cast<unsigned int>(tmp & (base - 1)); //equals tmp % (2 ^ 32)
																			   //cast is safe: base - 1 == 2 ^ 32 - 1 => x & (2 ^ 32 - 1) < 2 ^ 32 - 1 => safe cast to UInt
			unsigned long long tmpDiv = tmp >> numberOfDigits; //equals tmp / (2 ^ 32)

			unsigned long long sum = carry + temp[k] + tmpMod;
			unsigned int sumMod = static_cast<unsigned int>(sum & (base - 1)); //equals sum % (2 ^ 32)
																			   //cast is safe: base - 1 == 2 ^ 32 - 1 => x & (2 ^ 32 - 1) < 2 ^ 32 - 1 => safe cast to UInt
			unsigned long long sumDiv = sum >> numberOfDigits; //equals sum / (2 ^ 32)

			temp[k] = sumMod;
			carry = tmpDiv + sumDiv;
		}
		unsigned int carryMod = static_cast<unsigned int>(carry & (base - 1));
		unsigned int carryDiv = static_cast<unsigned int>(carry >> numberOfDigits);
		temp[i + absSecond.data.size()] += carryMod;
		temp[i + absSecond.data.size() + 1] += carryDiv;
	}
	big_integer result(false, temp);
	result.setSign(sign);
	return result;
}

unsigned int transformStringToUInt(string const& str, size_t start, size_t finish)
{
	unsigned int res = 0;
	for (size_t i = start; i < finish; i++)
	{
		try
		{
			unsigned int cur = transformCharToDigit(str[i]);
			res *= 10;
			res += cur;
		}
		catch (ParsingException e)
		{
			string newMessage = static_cast<string>(e.what()) + " at position " + indexToString(i) + "\n" + constructErrorPointer(str, i);
			throw ParsingException(newMessage);
		}
	}
	return res;
}

big_integer transformStringToBigInteger(string const& str)
{
	if (str.length() == 0)
	{
		return big_integer(0);
	}
	size_t curIndex = 0;
	bool sign = false;
	big_integer result;
	if (str[0] == '-')
	{
		sign = true;
		curIndex = 1;
	}
	while (curIndex + 9 <= str.size())
	{
		unsigned int cur = transformStringToUInt(str, curIndex, curIndex + 9);
		result *= bases[9];
		result += cur;
		curIndex += 9;
	}
	result *= bases[str.size() - curIndex];
	result += transformStringToUInt(str, curIndex, str.size());
	result.setSign(sign);
	return result;
}


big_integer::big_integer(string const& str)
{
	big_integer temp = transformStringToBigInteger(str);
	swap(temp);
}

big_integer operator/(big_integer const& first, unsigned int second)
{
	if (second == 0)
	{
		throw EvaluateException("Division by zero");
	}
	big_integer absFirst(first.abs());
	bool isNeg = first.isNegative();
	unsigned long long div = static_cast<unsigned long long>(second);
	size_t size = absFirst.length();
	myVector temp(size);
	unsigned long long carry = 0;
	for (int i = size - 1; i >= 0; i--)
	{
		unsigned long long cur = absFirst.getDigit(static_cast<size_t>(i)) + carry * base;
		temp[i] = static_cast<unsigned int>(cur / div);
		carry = cur % div;
	}
	big_integer T(false, temp);
	T.setSign(isNeg);
	return T;
}

big_integer operator/(big_integer const& first, int second)
{
	long long temp = second;
	temp = abs(temp);
	unsigned int div = static_cast<unsigned int> (temp);
	//cast if safe: temp <= 2^31, UINT_MAX = 2^32 - 1
	big_integer res = first / div;
	if (second >= 0)
	{
		return res;
	}
	return -res;
}

big_integer& big_integer::operator/=(int b)
{
	return *this = *this / b;
}

big_integer& big_integer::operator/=(unsigned int b)
{
	return *this = *this / b;
}

big_integer getHighDigits(big_integer const& number, size_t n) //ïîëó÷àåò âåêòîð, ñîñòîÿùèå èç ïîñëåäíèõ n öèôð äàííîãî
															   // è êîíñòðóèðóåò èç íåãî áèãèíò, åñëè öèôð < n, äîáèâàåò âåäóùèìè íóëÿìè
{
	myVector temp(n);
	for (size_t i = 0; i < temp.size(); i++)
	{
		if (i < number.data.size())
		{
			temp[temp.size() - i - 1] = number.getDigit(number.data.size() - i - 1);
		}
	}
	return big_integer(false, temp);
}


unsigned int getNextDigit(big_integer const& first, big_integer const& second)
{
	big_integer divident = getHighDigits(first, 5); //áóäåì äåëèòü 5 öèôð íà 4
	big_integer divisor = getHighDigits(second, 4);
	if (divident < divisor)
	{
		return 0;
	}
	divisor = divisor << numberOfDigits;
	for (size_t i = 0; i <= 1; i++)
	{
		unsigned int dig = 0;
		if (divident >= divisor)
		{
			for (int j = numberOfDigits - 1; j >= 0; j--)
			{
				unsigned int nw = dig + (1 << j);
				if (nw * divisor <= divident)
				{
					dig = nw;
				}
			}
			divident = divident - dig * divisor;
		}
		if (dig > 0)
		{
			/*while (dig > 0 && copyDivisor * dig > copyDivident)
			{
				dig--;
			}
			if (dig > 0)
			{
				return dig;
			}*/
			return dig;
		}
		divisor = divisor >> numberOfDigits;
	}
	return 0;
}


big_integer operator/(big_integer const& first, big_integer const& second) {
	if (second == 0)
	{
		throw EvaluateException("Division by zero");
	}
	bool sign = first.isNegative() ^ second.isNegative();
	big_integer absFirst(first.abs());
	big_integer absSecond(second.abs());
	if (absFirst < absSecond)
	{
		return big_integer(0);
	}
	if (absSecond.data.size() == 1) 
	{
		big_integer result = absFirst / absSecond.getDigit(0); //bigint / unsigned int
		result.setSign(sign);
		return result;
	}
	size_t size = absFirst.data.size() - absSecond.data.size();
	if (absFirst < absSecond << (static_cast<unsigned int>(size) * numberOfDigits))
	{
		size--;
	}
	myVector temp;
	for (int i = size; i >= 0; i--)
	{
		unsigned int dig = getNextDigit(absFirst, absSecond);
		big_integer tmp = dig * absSecond;
		bool needCarry = false;
		for (size_t j = static_cast<size_t>(i); j < absFirst.data.size(); j++) //cast is safe: i >= 0
		{
			long long res = absFirst.getDigit(j) - 1LL * needCarry;
			if (j - i < tmp.data.size())
			{
				res -= tmp.getDigit(j - i);
			}
			needCarry = false;
			if (res < 0)
			{
				needCarry = true;
				res += base;
			}
			absFirst.data[j] = static_cast<unsigned int>(res);
		}
		absFirst.normalize();
		temp.push_back(dig);
	}
	for (size_t i = 0; i < temp.size() / 2; i++) {
		swap(temp[i], temp[temp.size() - 1 - i]);
	}
	big_integer result(false, temp);
	result.setSign(sign);
	return result;
}

big_integer operator%(big_integer const &a, big_integer const& b)
{
	return a - (a / b) * b;
}

istream& operator>>(istream& out, big_integer& a)
{
	string s;
	out >> s;
	a = big_integer(s);
	return out;
}

ostream& operator<<(ostream& in, big_integer const& a)
{
	string s = to_string(a);
	in << s;
	return in;
}