#ifndef BIG_DATA
#define BIG_DATA
#include <memory>

struct bigData 
{
	size_t capacity;
	std::shared_ptr<unsigned int> bigObjectPointer;
	bigData(unsigned int* a, size_t _capacity);
	void swap(bigData &other) noexcept;
};

#endif
