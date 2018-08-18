#include <stdio.h>
#include <math.h>
#include <iostream>

#include "compare.h"


int main()
{

    s a[100];
    int b = 10;
    for (int i = 0; i < 11; i++) a[i].a = 50 * i + 1;
    int* p = &b;
    srand(time(NULL));
    double m_alpha = 80.0f;
    a[0].a = m_alpha / 10.0f;
    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            int32_t r = rand();
            double beta = (double)(r % (RAND_MAX - 1) + 1) / (double)RAND_MAX;
            //double beta = (double)(rand() % 10) / (10) ;
            a[j].b = pow(beta, m_alpha / a[j].a);
            a[j].r = r;
        }
        std::partial_sort(a, a+11 , a+ 11, RealEcpmCompare(p));
        for (int i = 0; i < 11; i++) printf("(%d %.5lf) ", a[i].a, a[i].b, a[i].r);
        printf("\n");
    }
    if (0.0005 >= 0.0f) printf("yes \n");
    if (0.0000 <= 0.0f) printf("no \n");

    char szOrbits[] = "686.00097 365.00024";
    char* pEnd;
    float f1, f2;
    f1 = strtof (szOrbits, &pEnd);
    f2 = strtof (pEnd, NULL);
    printf ("One martian year takes %lf %lf Earth years.\n", f1 ,f2);
    printf ("%d.\n", INT_MAX);
    printf ("%d.\n", RAND_MAX);
    printf ("%.100lf.\n", (double)1/RAND_MAX);
    printf ("%lf.\n", pow(0.9, 50));

    long l[2];
    l[0]=100;
    l[1]=100;
    printf("%ld, %f\n", l[0],(double)l[0]);
    return 0;
}
