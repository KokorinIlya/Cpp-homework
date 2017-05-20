//
// Created by Илья Кокорин on 30.04.2017.
//

#ifndef BIGINT_BIG_INTEGER_IMPL
#define BIGINT_BIG_INTEGER_IMPL

#include "big_integer.h"

using namespace std;

const unsigned long long numberOfDigits = 32;
const unsigned long long base = 1ll << numberOfDigits;

set <char> knownBitModes{ '&', '|', '^' };

string setCharToString(set <char> x)
{
	string res = "";
	for (set <char>::iterator it = x.begin(); it != x.end(); it++)
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

big_integer::big_integer() : big_integer(0) {}

big_integer::big_integer(big_integer const& other)
{
	data = other.data;
	sign = other.sign;
}

bool big_integer::isNegative() const
{
	return sign;
}

void big_integer::normalize()
{                                //если число < 0 и представлено в двоичном дополнительном коде, нам не нужна куча единиц в начале
								 //аналогично с числом > 0, дополненным в начале кучей нулей
	while (data.size() > 1 &&
		(isNegative() && data.back() == UINT_MAX ||
			!isNegative() && data.back() == 0))
	{
		data.pop_back();
	}
}

big_integer::big_integer(int x)
{
	sign = (x < 0);
	data = vector<unsigned int>{static_cast<unsigned int>(x)};
}

big_integer::big_integer(unsigned long long x)
{
	sign = false;
	unsigned int first = static_cast<unsigned int>(x & (base - 1));
	unsigned int second = static_cast<unsigned int>(x >> numberOfDigits);
	data = vector<unsigned int>{ first, second };
}

big_integer::big_integer(unsigned int x)
{
	sign = false;
	data = vector <unsigned int>{ x };
}

big_integer::big_integer(vector<unsigned int> const& _init, bool _sign) : data(_init), sign(_sign)
{
	normalize();
}

void big_integer::swap(big_integer& other) noexcept
{
	std::swap(sign, other.sign);
	std::swap(data, other.data);
}

void swap(big_integer& a, big_integer& b) noexcept
{
	std::swap(a.sign, b.sign);
	std::swap(a.data, b.data);
}

big_integer& big_integer::operator=(big_integer const& other)
{
	big_integer temp(other);
	swap(temp);
	return *this;
}

bool operator==(big_integer const& a, big_integer const& b)
{
	return a.data == b.data && a.sign == b.sign;
}

bool operator!=(big_integer const& a, big_integer const& b)
{
	return !(a == b);
}

unsigned int big_integer::getDigit(size_t i) const
{
	if (i < data.size())
	{
		return data[i];
	}
	if (isNegative())
	{
		return base - 1;
	}
	return 0;
}

big_integer operator+(big_integer const& first, big_integer const& second)
{
	unsigned long long carry = 0;
	size_t size = max(first.data.size(), second.data.size()) + 2;
	vector<unsigned int> temp(size);
	for (size_t i = 0; i < size; i++)
	{
		unsigned long long curValue = carry + first.getDigit(i) + second.getDigit(i);
		carry = curValue >> numberOfDigits; //equals carry = (sum / 2^32)
		temp[i] = static_cast<unsigned int>(curValue & (base - 1)); // equals temp[i] = (sum % 2^32)
																	//cast is safe, base - 1 == 2^32 - 1 => curValue & (base - 1) <= 2^32 - 1 => safe cast to UInt
	}
	bool sgn = ((temp[size - 1] >> (numberOfDigits - 1)) > 0);
	return big_integer(temp, sgn); //construct from <vector, bool>
}

big_integer& big_integer::operator+=(big_integer const& other)
{
	big_integer temp = *this + other;
	swap(temp);
	return *this;
}

big_integer operator+(big_integer const& a)
{
	return big_integer(a);
}

big_integer operator~(big_integer const& a)
{
	size_t size = a.data.size();
	vector <unsigned int> temp(size);
	for (size_t i = 0; i < size; i++)
	{
		temp[i] = ~a.getDigit(i);
	}
	return big_integer(temp, !a.isNegative());
}

big_integer operator-(big_integer const& a)
{
	return (~a) + 1;
}

big_integer operator-(big_integer const& first, big_integer const& second)
{
	return first + (-second);
}

big_integer& big_integer::operator-=(big_integer const& other)
{
	big_integer temp = *this - other;
	swap(temp);
	return *this;
}

big_integer abstractBitOperation(big_integer const& first, big_integer const& second, char mode)
{
	size_t size = max(first.data.size(), second.data.size());
	vector<unsigned int> temp(size);
	for (size_t i = 0; i < size; i++)
	{
		temp[i] = doBitOperation(first.getDigit(i), second.getDigit(i), mode);
	}
	return big_integer(temp, doBitOperation(first.isNegative(), second.isNegative(), mode));
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

big_integer& big_integer::operator++() //преффиксный инкремент
{
	*this += 1;
	return *this;
}

big_integer big_integer::operator++(int notNeeded) //постфиксный инкремент
{
	big_integer temp = *this + 1; //temp = x + 1, this = x
	swap(temp); // temp = x, this = x + 1
	return temp; // return x, change this
}

big_integer& big_integer:: operator--() //преффиксный декремент
{
	*this -= 1;
	return *this;
}

big_integer big_integer::operator--(int notNeeded) //постфиксный декркмент
{
	big_integer temp = *this - 1; //temp = x - 1, this = x
	swap(temp); // temp = x, this = x - 1
	return temp; // return x, change this
}

big_integer& big_integer::operator&=(big_integer const& other)
{
	big_integer temp = *this & other;
	swap(temp);
	return *this;
}

big_integer& big_integer::operator|=(big_integer const& other)
{
	big_integer temp = *this | other;
	swap(temp);
	return *this;
}

big_integer& big_integer::operator^=(big_integer const& other)
{
	big_integer temp = *this ^ other;
	swap(temp);
	return *this;
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
	size_t size = static_cast<size_t>(first.data.size() + div + 1);
	vector <unsigned int> temp(size);
	temp[div] = static_cast<unsigned int>(first.getDigit(0) << mod);
	for (size_t i = static_cast<size_t>(div + 1); i < size; i++)
	{
		unsigned int firstDigit = first.getDigit(static_cast<unsigned int>(i - div));
		unsigned int secondDigit = first.getDigit(static_cast<unsigned int>(i - div - 1));
		temp[i] = static_cast<unsigned int>(static_cast<unsigned long long>(firstDigit << mod)
			| static_cast<unsigned long long>(secondDigit) >> (numberOfDigits - mod));
	}
	return big_integer(temp, first.isNegative());
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
	if (first.data.size() < div)
	{
		vector <unsigned int> res;
		return big_integer(res, false);
	}
	size_t size = static_cast<size_t>(first.data.size() - div);
	//cast is safe, size >= div
	vector<unsigned int> temp(size);
	for (size_t i = 0; i < size; i++)
	{
		unsigned int firstDigit = first.getDigit(static_cast<unsigned int>(i + div));
		unsigned int secondDigit = first.getDigit(static_cast<unsigned int>(i + div + 1));
		temp[i] = static_cast<unsigned int>((static_cast<unsigned long long>(firstDigit) >> mod)
			| (static_cast<unsigned long long>(secondDigit) << (numberOfDigits - mod)));
	}
	return big_integer(temp, first.isNegative());
}

big_integer& big_integer::operator<<=(int shift)
{
	big_integer temp = *this << shift;
	swap(temp);
	return *this;
}

big_integer& big_integer::operator>>=(int shift)
{
	big_integer temp = *this >> shift;
	swap(temp);
	return *this;
}

void big_integer::print() const //used for debug
{
	for (int i = data.size() - 1; i >= 0; i--)
	{
		cout << data[i] << " ";
	}
	cout << endl;
}

big_integer big_integer::absoluteValue() const
{
	if (!isNegative())
	{
		return *this;
	}
	return -(*this);
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

bool operator>(big_integer const& a, big_integer const& b) //a > b <=> b < a
{
	return b < a;
}

bool operator>=(big_integer const& a, big_integer const& b) //a >= b <=> !(a < b)
{
	return !(a < b);
}

bool operator<=(big_integer const& a, big_integer const& b) //a <= b <=> !(a > b) => !(b < a)
{
	return !(b < a);
}

void big_integer::setSign(bool s)
{
	if (s != isNegative())
	{
		*this = -*this;
	}
}

big_integer operator*(big_integer const& first, big_integer const& second) {
	bool sign = first.isNegative() ^ second.isNegative();
	big_integer absFirst(first.absoluteValue());
	big_integer absSecond(second.absoluteValue());
	vector<unsigned int> temp(absFirst.data.size() + absSecond.data.size() + 1);
	if (absFirst.data.size() > absSecond.data.size())
	{
		swap(absFirst, absSecond);
	}
	for (size_t i = 0; i < absFirst.data.size(); i++)
	{
		unsigned long long carry = 0;
		for (size_t j = 0; j < absSecond.data.size(); j++)
		{
			size_t k = i + j;
			unsigned long long tmp =
				static_cast<unsigned long long>(absFirst.getDigit(i)) * absSecond.getDigit(j);
			//к k-ому разряду(k == i + j) нужно прибавить результат first[i] * second[j]
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
	big_integer result(temp, false);
	result.setSign(sign);
	return result;
}

big_integer& big_integer::operator*=(big_integer const& other)
{
	big_integer temp = *this * other;
	swap(temp);
	return *this;
}

const vector <unsigned int> bases = 
{
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
	big_integer absFirst(first.absoluteValue());
	bool isNeg = first.isNegative();
	unsigned long long div = static_cast<unsigned long long>(second);
	size_t size = absFirst.data.size();
	vector<unsigned int> temp(size);
	unsigned long long carry = 0;
	for (int i = size - 1; i >= 0; i--)
	{
		unsigned long long cur = absFirst.getDigit(static_cast<size_t>(i)) + carry * base;
		temp[i] = static_cast<unsigned int>(cur / div);
		carry = cur % div;
	}
	big_integer T(temp, false);
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
	big_integer temp = *this / b;
	swap(temp);
	return *this;
}

big_integer& big_integer::operator/=(unsigned int b)
{
	big_integer temp = *this / b;
	swap(temp);
	return *this;
}

big_integer getHighDigits(big_integer const& number, size_t n) //получает вектор, состоящие из последних n цифр данного
															   // и конструирует из него бигинт, если цифр < n, добивает ведущими нулями
{
	vector<unsigned int> temp(n);
	for (size_t i = 0; i < temp.size(); i++)
	{
		if (i < number.data.size())
		{
			temp[temp.size() - i - 1] = number.getDigit(number.data.size() - i - 1);
		}
	}
	return big_integer(temp, false);
}


unsigned int getNextDigit(big_integer const& first, big_integer const& second)
{
	big_integer divident = getHighDigits(first, 5); //будем делить 5 цифр на 4
	big_integer divisor = getHighDigits(second, 4);
	big_integer copyDivident(divident);
	big_integer copyDivisor(divisor);
	if (divident < divisor) //деление целочисленное, так что x / y == 0, если y > x
	{
		return 0;
	}
	if (divisor.getDigit(divisor.data.size() - 1) < base / 2)
	{
		unsigned int multiplier = (base / 2) / divisor.getDigit(divisor.data.size() - 1) + 2;
		divident *= multiplier;
		divisor *= multiplier;
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
			while (dig > 0 && dig * copyDivisor > copyDivident)
			{
				dig--;
			}
			if (dig > 0)
			{
				return dig;
			}
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
	big_integer absFirst(first.absoluteValue());
	big_integer absSecond(second.absoluteValue());
	if (absFirst < absSecond)
	{
		return big_integer(0);
	}
	if (absSecond.data.size() == 1) //если можно поделить длинное на короткое
	{
		big_integer result = absFirst / absSecond.getDigit(0); //bigint / unsigned int
		result.setSign(sign);
		return result;
	}
	size_t size = absFirst.data.size() - absSecond.data.size();
	if (absFirst < absSecond << static_cast<unsigned int>(size) * numberOfDigits)
	{
		size--;
	}
	vector<unsigned int> temp;
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
	reverse(temp.begin(), temp.end());
	big_integer result(temp, false);
	result.setSign(sign);
	return result;
}

big_integer& big_integer::operator/=(big_integer const& other)
{
	big_integer temp = *this / other;
	swap(temp);
	return *this;
}

big_integer& big_integer::operator%=(big_integer const& other)
{
	big_integer temp = *this % other;
	swap(temp);
	return *this;
}

big_integer operator%(big_integer const& first, big_integer const& second)
{
	return first - (first / second) * second;
}

int operator%(big_integer const& first, unsigned int second)
{
	if (second == 0)
	{
		throw EvaluateException("Division by zero");
	}
	big_integer absFirst(first.absoluteValue());
	size_t size = first.data.size();
	int carry = 0;
	for (int i = size - 1; i >= 0; i--)
	{
		unsigned long long cur = absFirst.getDigit(static_cast<size_t >(i)) + carry * base; //cast is safe: i >= 0
		carry = static_cast<int>(cur % second);
	}
	return carry;
}

bool big_integer::isMoreThan0()
{
	if (data.size() > 1)
	{
		return true;
	}
	return (getDigit(0) != 0);
}

string to_string(big_integer const& arg)
{
	string result;
	big_integer absArg = arg.absoluteValue();
	bool isNeg = arg.isNegative();
	while (absArg.isMoreThan0())
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

#endif