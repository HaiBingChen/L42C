#include "7816_test.h"


static void print_buf(uint8_t * string, uint8_t * buffer, uint32_t len)
{
	int i;
	PRINTF("CMD: %s\r\n", string);
	PRINTF("return len: %d\r\n", len);
	PRINTF("return data: \r\n");
	for(i=0; i<len; i++)
	{
		PRINTF("0x%x  ", *(buffer + i));
	}
	PRINTF("\r\n");
	PRINTF("\r\n");
}

u1 test_7816(void) 
{ 
	u1 bufatr[32],len; 
	u1 buf2[256]; 
	u2 len2; 

	SCD_POWER_ON(bufatr,&len,0,0); 
	print_buf((uint8_t *)"ATR  ", bufatr, len);

	
	SCD_APDU_CMD((u1 *)"\x00\xA4\x04\x00\x0C\xF0\x4B\x44\x44\x49\x4B\x4C\x10\x01\x28\x01\x01 ",17,buf2,&len2);	
	print_buf((uint8_t *)"\x00\xA4\x04\x00\x0C\xF0\x4B\x44\x44\x49\x4B\x4C\x10\x01\x28\x01\x01  ", buf2, len2);
	//SCD_DelayMs(200);  
	
	SCD_APDU_CMD((u1 *)"\x00\x01\x00\x00\x01\x01 ",6,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB0 0x95 0x0A 0x0A  ", buf2, len2);
	//SCD_DelayMs(200); 	

	SCD_APDU_CMD((u1 *)"\x00\x01\x00\x00\x02\x01\x02 ",7,buf2,&len2);	
	print_buf((uint8_t *)"0x80 0x5C 0x00 0x02 0x04  ", buf2, len2);
	//SCD_DelayMs(200); 	

	SCD_APDU_CMD((u1 *)"\x00\xB2\x01\xC4\x17",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x01 0xC4 0x17  ", buf2, len2);
	//SCD_DelayMs(200); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x02\xC4\x17",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x02 0xC4 0x17  ", buf2, len2);
	//SCD_DelayMs(200); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x03\xC4\x17",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x03 0xC4 0x17  ", buf2, len2);
	//SCD_DelayMs(200); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x04\xC4\x17",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x04 0xC4 0x17  ", buf2, len2);
	//SCD_DelayMs(200); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x05\xC4\x17",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x05 0xC4 0x17  ", buf2, len2);
	//SCD_DelayMs(200); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x06\xC4\x17",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x06 0xC4 0x17  ", buf2, len2);
	//SCD_DelayMs(200); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x07\xC4\x17",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x07 0xC4 0x17  ", buf2, len2);
	//SCD_DelayMs(200); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x08\xC4\x17",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x08 0xC4 0x17  ", buf2, len2);
	//SCD_DelayMs(200); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x09\xC4\x17",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x09 0xC4 0x17  ", buf2, len2);
	//SCD_DelayMs(200); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x0A\xC4\x17",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x0A 0xC4 0x17  ", buf2, len2);
	//SCD_DelayMs(200); 
	
	SCD_POWER_OFF(); 

	return 0; 
}


u1 test_7816_2(void) 
{ 
	u1 bufatr[32],len; 
	u1 buf2[256]; 
	u2 len2; 

	SCD_POWER_ON(bufatr,&len,0,0); 


	SCD_APDU_CMD((u1 *)"\x00\xA4\x04\x00\x08\xA0\x00\x00\x03\x33\x01\x01\x06",13,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xA4 0x04 0x00 0x08 0xA0 0x00 0x00 0x03 0x33 0x01 0x01 0x06", buf2, len2);
	//SCD_DelayMs(400); 		
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x01\x1C\x18",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x01 0x1C 0x18  ", buf2, len2);
	//SCD_DelayMs(400); 	

	SCD_APDU_CMD((u1 *)"\x80\xCA\x9F\x79\x09",5,buf2,&len2);	
	print_buf((uint8_t *)"0x80 0xCA 0x9F 0x079 0x09  ", buf2, len2);
	//SCD_DelayMs(400); 	

	SCD_APDU_CMD((u1 *)"\x00\xB2\x01\x5C\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x01 0x5C 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x02\x5C\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x02 0x5C 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x03\x5C\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x03 0x5C 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x04\x5C\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x04 0x5C 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x05\x5C\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x05 0x5C 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x06\x5C\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x06 0x5C 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x07\x5C\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x07 0x5C 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x08\x5C\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x08 0x5C 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x09\x5C\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x09 0x5C 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x0A\x5C\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x0A 0x5C 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 
	
	///////////////////////////////////////////////////////
	SCD_APDU_CMD((u1 *)"\x00\xB2\x01\x64\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x01 0x64 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x02\x64\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x03 0x64 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x03\x64\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x03 0x64 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 	
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x04\x64\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x04 0x64 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 

	SCD_APDU_CMD((u1 *)"\x00\xB2\x05\x64\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x05 0x64 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 

	SCD_APDU_CMD((u1 *)"\x00\xB2\x06\x64\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x06 0x64 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x07\x64\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x07 0x64 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x08\x64\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x08 0x64 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x09\x64\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x09 0x64 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 
	
	SCD_APDU_CMD((u1 *)"\x00\xB2\x0A\x64\x2c",5,buf2,&len2);	
	print_buf((uint8_t *)"0x00 0xB2 0x0A 0x64 0x2c  ", buf2, len2);
	//SCD_DelayMs(400); 
	
	SCD_POWER_OFF(); 

	return 0; 
}

u1 test_7816_3(void) 
{ 
	u1 bufatr[32]={0},len; 
	u1 buf2[256]={0}; 
	u2 len2; 
	u1 status = 0;
	

	SCD_POWER_ON(bufatr,&len,0,0); 
	print_buf((uint8_t *)"bufatr:  ", bufatr, len);
	
	
	status =  SCD_APDU_LYCMD((u1 *)"\x00\x01\x01\x00\x00", 5, buf2, &len2);
	print_buf((uint8_t *)"0x00 0x01 0x01 0x00 0x00  ", &buf2[0], len2);
	PRINTF("status: %d\r\n", status);
	
	status = SCD_APDU_LYCMD((u1 *)"\x00\x02\x01\x00\x00", 5, buf2, &len2);
	print_buf((uint8_t *)"0x00 0x02 0x01 0x00 0x00  ", &buf2[0], len2);
	PRINTF("status: %d\r\n", status);
	
	status = SCD_APDU_LYCMD((u1 *)"\x00\x04\x01\x00\x00", 5, buf2,&len2);
	print_buf((uint8_t *)"0x00 0x04 0x01 0x00 0x00  ",  &buf2[0],len2);
	PRINTF("status: %d\r\n", status);
	
	status = SCD_APDU_LYCMD((u1 *)"\x00\x05\x01\x00\x00", 5, buf2,&len2);
	print_buf((uint8_t *)"0x00 0x05 0x01 0x00 0x00  ",  &buf2[0],len2);
	PRINTF("status: %d\r\n", status);
	////////////////////////////////////////////////
	
	status =  SCD_APDU_LYCMD((u1 *)"\x00\x01\x02\x00\x00", 5, buf2, &len2);
	print_buf((uint8_t *)"0x00 0x01 0x02 0x00 0x00  ", &buf2[0], len2);
	PRINTF("status: %d\r\n", status);
	
	status = SCD_APDU_LYCMD((u1 *)"\x00\x02\x02\x00\x00", 5, buf2,&len2);
	print_buf((uint8_t *)"0x00 0x02 0x02 0x00 0x00  ", &buf2[0],len2);
	PRINTF("status: %d\r\n", status);

	status = SCD_APDU_LYCMD((u1 *)"\x00\x03\x02\x00\x00", 5, buf2,&len2);
	print_buf((uint8_t *)"0x00 0x03 0x02 0x00 0x00  ", &buf2[0],len2);
	PRINTF("status: %d\r\n", status);
	
	SCD_POWER_OFF(); 
	return 0; 
}
