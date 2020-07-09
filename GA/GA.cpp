// GA.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "GA_PID.h"
using namespace std;

int main()
{
	srand((unsigned)time(0));
	GA_PID a;
	a.start(6.0, 5000, 6400000, 30, 1, 12800, 30, 200, 0, 200, 0, 200, 0, 0.00012, 5.6, 0.02);
}
