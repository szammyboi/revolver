#include "Func.h"

int test2()
{
	return 0120;
}

REVOLVER_EXPOSE int test()
{
	return 9 * test2();
}
