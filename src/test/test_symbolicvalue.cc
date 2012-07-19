#include "../symbolicvalue.h"

#include <iostream>

int main ()
{
	SymbolicValue wild (32);
	SymbolicValue one  (32, 1);

	if ((wild.svassert(one))) std::cout << "pass" << std::endl;
	else                      std::cout << "fail" << std::endl;


	SymbolicValue wild2 = wild;

	if (wild.svassert(wild2)) std::cout << "pass" << std::endl;
	else                      std::cout << "fail" << std::endl;


	SymbolicValue notWild = ~wild;

	if (wild.svassert(notWild)) std::cout << "fail" << std::endl;
	else                        std::cout << "pass" << std::endl;

	SymbolicValue notWildPlusOne = notWild + one;

	if (wild.svassert(notWildPlusOne)) std::cout << "pass" << std::endl;
	else                               std::cout << "fail" << std::endl;

	SymbolicValue two  (32, 2);
	SymbolicValue lt2 = wild.cmpLtu(two);

	if (lt2.svassert(one)) std::cout << "pass" << std::endl;
	else                   std::cout << "fail" << std::endl;
	if (lt2.svassert(two)) std::cout << "fail" << std::endl;
	else                   std::cout << "pass" << std::endl;

	return 0;
}