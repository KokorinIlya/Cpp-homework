#include "myVector.h"
#include "deleter.h"

const size_t UINT_SIZE = sizeof(unsigned int);

size_t getCapacityForxpansion(const size_t n)
{
	if (n == 0) 
	{
		return 4;
	}
	return n << 1;
}


bool compareData(const unsigned int* __restrict first, const unsigned int* __restrict second, size_t n)
{
	for (size_t i = 0; i < n; i++)
	{
		if (*(first + i) != *(second + i))
		{
			return false;
		}
	}
	return true;
}

void copyData(unsigned int* __restrict first, const unsigned int* __restrict second, size_t n)
{
	for (size_t i = 0; i < n; i++)
	{
		*(first + i) = *(second + i);
	}
}

void swapAnyData(dataStorage &a, dataStorage &b) noexcept //first is small, second is bigData
{
	unsigned int temp[SMALL_OBJECT_SIZE];
	copyData(temp, a.smallObject, SMALL_OBJECT_SIZE);
	new(&a.bigObject) bigData(b.bigObject);
	b.bigObject.~bigData();
	copyData(b.smallObject, temp, SMALL_OBJECT_SIZE);
}

unsigned int* makeNewData(const unsigned int* src, size_t size, size_t capacity)
{
	unsigned int* res = static_cast<unsigned int*>(operator new(capacity * UINT_SIZE));
	copyData(res, src, size);
	memset(res + size, 0, (capacity - size) * UINT_SIZE);
	return res;
}

myVector myVector::operator=(myVector const &other)
{
	myVector temp(other);
	swap(temp);
	return *this;
}

size_t myVector::size() const
{
	return vectorSize;
}

myVector::myVector() : vectorSize(0), isBig(false), arrayPointer(data.smallObject)
{
	memset(arrayPointer, 0, SMALL_OBJECT_SIZE * UINT_SIZE);
}

myVector::myVector(size_t needSize) : myVector() 
{
	reserve(needSize);
	memset(arrayPointer, 0, needSize * UINT_SIZE);
	vectorSize = needSize;
}

myVector::~myVector()
{
	if (isBig)
	{
		data.bigObject.~bigData();
	}
}

myVector::myVector(std::initializer_list<unsigned int> const& list) : myVector() {
	reserve(list.size());
	vectorSize = list.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		arrayPointer[i] = *(list.begin() + i);
	}
}

size_t myVector::capacityGetter() const
{
	if (isBig) 
	{
		return data.bigObject.capacity;
	}
	return SMALL_OBJECT_SIZE;
}

void myVector::reserve(size_t _capacity)
{
	if (_capacity > capacityGetter())
	{
		capacitySetter(_capacity);
	}
}

inline void myVector::vectorFactory(size_t capacity)
{
	data.bigObject.bigObjectPointer.reset(makeNewData(arrayPointer, vectorSize, capacity), deleter());
	data.bigObject.capacity = capacity;
	arrayPointer = data.bigObject.bigObjectPointer.get();
}

inline void myVector::makeNewVector()
{
	if (isBig && !data.bigObject.bigObjectPointer.unique())
	{
		//vectorFactory(vectorSize);
		data.bigObject.bigObjectPointer.reset(makeNewData(arrayPointer, vectorSize, vectorSize), deleter());
		arrayPointer = data.bigObject.bigObjectPointer.get();
		data.bigObject.capacity = vectorSize;
	}
}

inline void myVector::capacitySetter(size_t _capacity)
{
	if (!isBig)
	{
		isBig = true;
		new(&data.bigObject) bigData(makeNewData(arrayPointer, vectorSize, _capacity), _capacity);
		arrayPointer = data.bigObject.bigObjectPointer.get();
	} 
	else 
	{
		vectorFactory(_capacity);
	}
}

unsigned int const& myVector::operator[](size_t ind) const 
{
	return arrayPointer[ind];
}

unsigned int& myVector::operator[](size_t ind)
{
	makeNewVector();
	return arrayPointer[ind];
}

void myVector::resize(size_t _size) 
{
	reserve(_size);
	memset(arrayPointer + vectorSize, 0, vectorSize - _size);
	vectorSize = _size;
}

unsigned int* myVector::getData() const
{
	return arrayPointer;
}

bool myVector::empty() const
{
	return vectorSize == 0;
}

void myVector::pop_back() 
{
	makeNewVector();
	vectorSize--;
}

void myVector::push_back(const unsigned int x) 
{
	if (capacityGetter() < vectorSize + 1)
	{
		reserve(getCapacityForxpansion(vectorSize));
	}
	makeNewVector();
	arrayPointer[vectorSize] = x;
	vectorSize++;
}

unsigned int myVector::back() const 
{
	return arrayPointer[vectorSize - 1];
}

bool operator==(const myVector &a, const myVector &b) 
{
	if (a.vectorSize != b.vectorSize)
	{
		return false;
	}
	return compareData(a.arrayPointer, b.arrayPointer, a.size());
}

myVector::myVector(myVector const &other) : vectorSize(other.vectorSize), isBig(other.isBig) 
{
	if (other.isBig) 
	{
		new (&data.bigObject) bigData(other.data.bigObject);
		arrayPointer = data.bigObject.bigObjectPointer.get();
	} 
	else
	{
		copyData(data.smallObject, other.data.smallObject, SMALL_OBJECT_SIZE);
		arrayPointer = data.smallObject;
	}
}

void myVector::swap(myVector &other) noexcept
{
	using std::swap;
	if (isBig == other.isBig)
	{
		if (!isBig) 
		{
			for (size_t i = 0; i < SMALL_OBJECT_SIZE; i++) 
			{
				swap(data.smallObject[i], other.data.smallObject[i]);
			}
		}
		else 
		{
			swap(data.bigObject, other.data.bigObject);
			swap(other.arrayPointer, arrayPointer);
		}
	}
	else
	{
		swap(isBig, other.isBig);
		if (!isBig)  //before swap(isBig, other.isBig): isBig == true => this is small
		{
			swapAnyData(other.data, data);
			arrayPointer = data.smallObject;
			other.arrayPointer = other.data.bigObject.bigObjectPointer.get();
		}
		else 
		{
			swapAnyData(data, other.data);
			other.arrayPointer = other.data.smallObject;
			arrayPointer = data.bigObject.bigObjectPointer.get();
		}
	}
	swap(vectorSize, other.vectorSize);
}
