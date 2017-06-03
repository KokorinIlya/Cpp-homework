#ifndef MY_DATA
#define MY_DATA

#include "bigData.h"

const size_t SMALL_OBJECT_SIZE = 4;

union dataStorage 
{
	unsigned int smallObject[SMALL_OBJECT_SIZE];
	bigData bigObject;
	dataStorage() {}
	~dataStorage() {}
};
#endif
