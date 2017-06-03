#include "bigData.h"

void bigData::swap(bigData &other) noexcept
{
	using std::swap;
	swap(capacity, other.capacity);
	swap(bigObjectPointer, other.bigObjectPointer);
}

bigData::bigData(unsigned int* a, size_t _capacity) : capacity(_capacity), bigObjectPointer(a) {}