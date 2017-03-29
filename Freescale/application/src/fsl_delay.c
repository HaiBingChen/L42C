#include "fsl_delay.h"


static inline void fsl_delay_us(unsigned int volatile number_of_us)
{
	unsigned int i=0;
	unsigned int j=0;

	for(j=0; j<number_of_us; j++)
	{
		for(i=0; i<44; i++)
		{
			__asm("nop");
		}
	}
}

/*lint --e{438} "Variable not used" */
void fsl_delay_ms(unsigned  volatile number_of_ms)
{
    while(number_of_ms != 0)
    {
        number_of_ms--;
        fsl_delay_us(999);
    }
}
