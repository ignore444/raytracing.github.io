//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "rtweekend.h"

#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdlib.h>



// #_PI_2_01 : Estimating Pi
void main_Estimating_Pi() 
{
	int N = 1000;
	int inside_circle = 0;
	for (int i = 0; i < N; i++) 
    {
		auto x = random_double(-1, 1);
		auto y = random_double(-1, 1);
		if (x * x + y * y < 1)
        {
			inside_circle++;
        }
	}
	std::cout << std::fixed << std::setprecision(12);
	std::cout << "Estimate of Pi = " << 4 * double(inside_circle) / N << '\n';
}

// #_PI_2_02 : Showing Convergence
void main_Showing_Convergence() 
{
	unsigned long long inside_circle = 0;
    unsigned long long runs = 0;
	std::cout << std::fixed << std::setprecision(12);
	while (true) 
    {
		runs++;
		auto x = random_double(-1, 1);
		auto y = random_double(-1, 1);
		if (x * x + y * y < 1)
        {
			inside_circle++;
        }

		if (runs % 10000000 == 0)
        {
			std::cout << "Estimate of Pi = " << 4 * double(inside_circle) / runs << '\n';
        }
	}
}

// #_PI_2_03 : Stratified Samples
void main_Stratified_Samples()
{
    int inside_circle = 0;
    int inside_circle_stratified = 0;
    int sqrt_N = 10000;

    for (int i = 0; i < sqrt_N; i++) 
    {
        for (int j = 0; j < sqrt_N; j++) 
        {
            auto x = random_double(-1,1);
            auto y = random_double(-1,1);
            if (x*x + y*y < 1)
            {
                inside_circle++;
            }

            // Stratified Sample
            // 2 * ( i / sqrt_N )               : [0,2] 를 n 칸으로 나눔
            // 2 * ( i / sqrt_N ) - 1           : [-1,1]로 바꿈
            // 2 * ( { i + [0,1) } / sqrt_N )   : [0,2] 를 n 칸으로 나누고, 그 안에서 random하게 위치 지정
            x = 2*((i + random_double()) / sqrt_N) - 1;
            y = 2*((j + random_double()) / sqrt_N) - 1;
            if (x*x + y*y < 1)
            {
                inside_circle_stratified++;
            }
        }
    }

    auto N = static_cast<double>(sqrt_N) * sqrt_N;
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "Regular    Estimate of Pi = " << 4 * double(inside_circle) / (sqrt_N*sqrt_N) << '\n';
    std::cout << "Stratified Estimate of Pi = " << 4 * double(inside_circle_stratified) / (sqrt_N*sqrt_N) << '\n';
}

void main()
{
    main_Stratified_Samples();
}