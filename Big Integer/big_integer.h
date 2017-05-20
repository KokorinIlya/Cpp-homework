//
// Created by Илья Кокорин on 30.04.2017.
//
#ifndef BIGINT_BIG_INTEGER_H
#define BIGINT_BIG_INTEGER_H

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include <set>
#include <algorithm>
#include "big_integer_exceptions.h"


struct big_integer
{
public:
	big_integer(); //конструктор по умолчанию, инициализирующий число нулём
	big_integer(big_integer const& other); // конструктор копирования
	big_integer(int a); // конструктор от int
	big_integer(unsigned int x); //конструктор от unsigned int
	big_integer(unsigned long long x); //конструктор от unsigned long long
	explicit big_integer(std::string const& str); //explicit-конструктор от строки

												  //деструктор не нужен, мы можем положиться на стандартный, так как не работаем с динамической памятью

	big_integer& operator=(big_integer const& other); //Оператор присваивания

													  //Арифметические операции c операцией присваивания
	big_integer& operator+=(big_integer const& other);
	big_integer& operator-=(big_integer const& other);
	big_integer& operator*=(big_integer const& other);
	big_integer& operator/=(big_integer const& other);
	big_integer& operator%=(big_integer const& other);

	//деление от инта с присваиванием
	big_integer& operator/=(int other);
	big_integer& operator/=(unsigned int other);


	//битовые операции с присваиванием
	big_integer& operator&=(big_integer const& other);
	big_integer& operator|=(big_integer const& other);
	big_integer& operator^=(big_integer const& other);

	//сдвиги с присваиванием
	big_integer& operator<<=(int shift);
	big_integer& operator>>=(int shift);

	//std::string toString(); // преобразование в строку, член класса
	friend std::string to_string(big_integer const& a); //функция, возвращающая строковое представление числа, не член класса

														//операции сравнения
	friend bool operator==(big_integer const& a, big_integer const& b);
	friend bool operator!=(big_integer const& a, big_integer const& b);
	friend bool operator<(big_integer const& a, big_integer const& b);
	friend bool operator>(big_integer const& a, big_integer const& b);
	friend bool operator<=(big_integer const& a, big_integer const& b);
	friend bool operator>=(big_integer const& a, big_integer const& b);

	//арфиметические операции
	friend big_integer operator+(big_integer const& a, big_integer const& b);
	friend big_integer operator-(big_integer const& a, big_integer const& b);
	friend big_integer operator*(big_integer const& a, big_integer const& b);
	friend big_integer operator/(big_integer const& a, big_integer const& b);
	friend big_integer operator%(big_integer const& a, big_integer const& b);


	//операции деления на инт
	friend big_integer operator/(big_integer const& a, int b);
	friend big_integer operator/(big_integer const& a, unsigned int b);
	friend int operator%(big_integer const& a, unsigned int b);

	//битовые операции
	friend big_integer operator&(big_integer const& a, big_integer const& b);
	friend big_integer operator|(big_integer const& a, big_integer const& b);
	friend big_integer operator^(big_integer const& a, big_integer const& b);

	//сдвиги
	friend big_integer operator<<(big_integer const& a, int shift);
	friend big_integer operator>>(big_integer const& a, int shift);

	//унарные операторы
	friend big_integer operator-(big_integer const& a);
	friend big_integer operator+(big_integer const& a); // серьёзно? унарный плюс?
	friend big_integer operator~(big_integer const& a);

	// операторы инкремента
	big_integer& operator++();       // преффиксный оператор инкремента
	big_integer operator++(int notNeeded);     // постфиксный оператор инкремента.

											   // операторы декремента
	big_integer& operator--();       // преффиксный оператор декремента.
	big_integer operator--(int notNeeded);     // постфиксный оператор декремента.


	//операторы ввода и вывода числа
	friend std::istream& operator>>(std::istream& out, big_integer& a);
	friend std::ostream& operator<<(std::ostream& in, big_integer const& a);

	//swap как член класса и как внешняя функция
	void swap(big_integer& other) noexcept;
	friend void swap(big_integer& a, big_integer& b) noexcept;

	bool isNegative() const;
	big_integer absoluteValue() const;

	void print() const; //used for debug, delete before release

private:
	void setSign(bool s);
	unsigned int getDigit(size_t i) const;
	void normalize(); //отбратсывание лишних 2^32 - 1 у отрицательного и лишних нулей у положительногшо
	big_integer(std::vector<unsigned int> const& T, bool _sign); //конструктор от vector и sign, деталь реализации
	friend big_integer abstractBitOperation(big_integer const& first, big_integer const& second, char mode);
	friend big_integer transformStringToBigInteger(std::string const& str);
	friend big_integer getHighDigits(big_integer const& number, size_t n);
	bool isMoreThan0();

	//данные:
	bool sign;
	std::vector <unsigned int> data;
};

#endif //BIGINT_BIG_INTEGER_H
