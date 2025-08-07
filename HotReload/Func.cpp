#include "Func.h"

int test2()
{
    return 200;
}

REVOLVER_EXPOSE int test()
{
    return 1 * test2();
}