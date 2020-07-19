// GA.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "GA_PID.h"
using namespace std;

int main()
{
	srand((unsigned)time(0));
	GA_PID a;
	a.start(6.0, 5000, 6400000, 100, 1000, 1, 12800, 100, 0, 100, 0, 100, 0, 2.666, 13.357, 0.02);
	// giá điện kinh doanh 2.666 VND / kWh
	// giá nước kinh doanh 13.357 VNG / m3
}
