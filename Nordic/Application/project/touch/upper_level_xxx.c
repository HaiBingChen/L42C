/*********************************************************************************************************************

		 Example_Code

               upper_level_xxx.c - i2c high level functions

********************************************************************************************************************/
#include "headfile.h"









void CommsIQS_Read(unsigned char i2c_addr, unsigned char read_addr, unsigned char *data, unsigned char NoOfBytes)
{
    unsigned char i;

    CommsIQSxxx_send((i2c_addr << 1) + 0x00);      // device address + write
    CommsIQSxxx_send(read_addr);                   // IQS address-command
    CommsIQSxxx_repeat_start();
    CommsIQSxxx_send((i2c_addr << 1) + 0x01);      // device address + read
    if (NoOfBytes > 1)
    {
        for (i = 0; i < NoOfBytes - 1; i++)
            data[i] = CommsIQSxxx_read_ack();      // all bytes except las must be followed by ACK
    }
    data[NoOfBytes-1] = CommsIQSxxx_read_nack();   // last byte read must be followed by a NACK
}

void CommsIQS_Write(unsigned char i2c_addr, unsigned char write_addr, unsigned char *data, unsigned char NoOfBytes)
{
    unsigned char i;

    CommsIQSxxx_send((i2c_addr << 1) + 0x00);      // device address + write
    CommsIQSxxx_send(write_addr);                  // IQS address-command
    for (i = 0; i < NoOfBytes; i++)
        CommsIQSxxx_send(data[i]);
}





