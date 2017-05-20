//
// Created by ���� ������� on 30.04.2017.
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
	big_integer(); //����������� �� ���������, ���������������� ����� ����
	big_integer(big_integer const& other); // ����������� �����������
	big_integer(int a); // ����������� �� int
	big_integer(unsigned int x); //����������� �� unsigned int
	big_integer(unsigned long long x); //����������� �� unsigned long long
	explicit big_integer(std::string const& str); //explicit-����������� �� ������

												  //���������� �� �����, �� ����� ���������� �� �����������, ��� ��� �� �������� � ������������ �������

	big_integer& operator=(big_integer const& other); //�������� ������������

													  //�������������� �������� c ��������� ������������
	big_integer& operator+=(big_integer const& other);
	big_integer& operator-=(big_integer const& other);
	big_integer& operator*=(big_integer const& other);
	big_integer& operator/=(big_integer const& other);
	big_integer& operator%=(big_integer const& other);

	//������� �� ���� � �������������
	big_integer& operator/=(int other);
	big_integer& operator/=(unsigned int other);


	//������� �������� � �������������
	big_integer& operator&=(big_integer const& other);
	big_integer& operator|=(big_integer const& other);
	big_integer& operator^=(big_integer const& other);

	//������ � �������������
	big_integer& operator<<=(int shift);
	big_integer& operator>>=(int shift);

	//std::string toString(); // �������������� � ������, ���� ������
	friend std::string to_string(big_integer const& a); //�������, ������������ ��������� ������������� �����, �� ���� ������

														//�������� ���������
	friend bool operator==(big_integer const& a, big_integer const& b);
	friend bool operator!=(big_integer const& a, big_integer const& b);
	friend bool operator<(big_integer const& a, big_integer const& b);
	friend bool operator>(big_integer const& a, big_integer const& b);
	friend bool operator<=(big_integer const& a, big_integer const& b);
	friend bool operator>=(big_integer const& a, big_integer const& b);

	//�������������� ��������
	friend big_integer operator+(big_integer const& a, big_integer const& b);
	friend big_integer operator-(big_integer const& a, big_integer const& b);
	friend big_integer operator*(big_integer const& a, big_integer const& b);
	friend big_integer operator/(big_integer const& a, big_integer const& b);
	friend big_integer operator%(big_integer const& a, big_integer const& b);


	//�������� ������� �� ���
	friend big_integer operator/(big_integer const& a, int b);
	friend big_integer operator/(big_integer const& a, unsigned int b);
	friend int operator%(big_integer const& a, unsigned int b);

	//������� ��������
	friend big_integer operator&(big_integer const& a, big_integer const& b);
	friend big_integer operator|(big_integer const& a, big_integer const& b);
	friend big_integer operator^(big_integer const& a, big_integer const& b);

	//������
	friend big_integer operator<<(big_integer const& a, int shift);
	friend big_integer operator>>(big_integer const& a, int shift);

	//������� ���������
	friend big_integer operator-(big_integer const& a);
	friend big_integer operator+(big_integer const& a); // ��������? ������� ����?
	friend big_integer operator~(big_integer const& a);

	// ��������� ����������
	big_integer& operator++();       // ����������� �������� ����������
	big_integer operator++(int notNeeded);     // ����������� �������� ����������.

											   // ��������� ����������
	big_integer& operator--();       // ����������� �������� ����������.
	big_integer operator--(int notNeeded);     // ����������� �������� ����������.


	//��������� ����� � ������ �����
	friend std::istream& operator>>(std::istream& out, big_integer& a);
	friend std::ostream& operator<<(std::ostream& in, big_integer const& a);

	//swap ��� ���� ������ � ��� ������� �������
	void swap(big_integer& other) noexcept;
	friend void swap(big_integer& a, big_integer& b) noexcept;

	bool isNegative() const;
	big_integer absoluteValue() const;

	void print() const; //used for debug, delete before release

private:
	void setSign(bool s);
	unsigned int getDigit(size_t i) const;
	void normalize(); //������������� ������ 2^32 - 1 � �������������� � ������ ����� � ���������������
	big_integer(std::vector<unsigned int> const& T, bool _sign); //����������� �� vector � sign, ������ ����������
	friend big_integer abstractBitOperation(big_integer const& first, big_integer const& second, char mode);
	friend big_integer transformStringToBigInteger(std::string const& str);
	friend big_integer getHighDigits(big_integer const& number, size_t n);
	bool isMoreThan0();

	//������:
	bool sign;
	std::vector <unsigned int> data;
};

#endif //BIGINT_BIG_INTEGER_H
