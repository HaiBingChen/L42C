/*******************************************************************************
 *
		 Example_Code

               upper_level_xxx.h - i2c high level functions
 *
*******************************************************************************/

#ifndef UPPER_LEVER_XXX
#define	UPPER_LEVER_XXX

void CommsIQS_Read(unsigned char i2c_addr, unsigned char read_addr, unsigned char *data, unsigned char NoOfBytes);
void CommsIQS_Write(unsigned char i2c_addr, unsigned char write_addr, unsigned char *data, unsigned char NoOfBytes);

#endif	/* I2C_HIGH_LEVEL_H */



