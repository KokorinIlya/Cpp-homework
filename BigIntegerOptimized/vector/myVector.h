#ifndef MY_VECTOR
#define MY_VECTOR

#include <memory>
#include "myData.h"
#include <cstring>

struct myVector {
public:
	myVector();
	explicit myVector(size_t needSize);
	myVector(myVector const &other);
	myVector::myVector(std::initializer_list<unsigned int> const& list);

	~myVector();

	size_t size() const;
	bool empty() const;

	unsigned int& operator[](size_t ind);
	unsigned int const& operator[](size_t ind) const;

	myVector operator=(myVector const &other);

	void pop_back();
	void push_back(const unsigned int x);
	unsigned int back() const;

	void swap(myVector &other) noexcept;

	unsigned int* getData() const;

	friend bool operator==(const myVector &a, const myVector &b);

private:
	size_t vectorSize;
	dataStorage data;
	bool isBig;
	unsigned int* arrayPointer;

	void reserve(size_t cap);

	size_t capacityGetter() const;
	void capacitySetter(size_t _capacity);

	void makeNewVector();
	void vectorFactory(size_t capacity);
};

#endif 

