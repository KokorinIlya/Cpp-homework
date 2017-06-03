#ifndef DELETER
#define DELETER

struct deleter 
{
	void operator() (unsigned int* pointer) const
	{
		operator delete(pointer);
	}
};

#endif
