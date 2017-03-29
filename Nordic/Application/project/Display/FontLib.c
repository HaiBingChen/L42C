
#include "FontLib.h"


const uint8 FontASCII9x16[][18] = {
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*" ",0*/
	{0x00,0x00,0x00,0xF8,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x30,0x00,0x00,0x00,0x00},/*"!",1*/
	{0x00,0x18,0x0E,0x06,0x18,0x0E,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*""",2*/
	{0x40,0x40,0xF8,0x40,0x40,0x40,0xF8,0x40,0x00,0x04,0x3F,0x05,0x04,0x04,0x3F,0x05,0x04,0x00},/*"#",3*/
	{0x00,0x70,0xD8,0x88,0xFC,0x08,0x28,0x30,0x00,0x00,0x18,0x28,0x20,0xFF,0x21,0x32,0x1C,0x00},/*"$",4*/
	{0xF0,0x08,0x08,0xF0,0x80,0x70,0x08,0x00,0x00,0x00,0x21,0x19,0x06,0x1F,0x21,0x21,0x1E,0x00},/*"%",5*/
	{0x00,0xF0,0x08,0x88,0x70,0x00,0x00,0x00,0x00,0x1E,0x21,0x23,0x24,0x18,0x2D,0x23,0x21,0x10},/*"&",6*/
	{0x10,0x16,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"'",7*/
	{0x00,0x00,0x00,0x00,0xE0,0x18,0x04,0x02,0x00,0x00,0x00,0x00,0x00,0x07,0x18,0x20,0x40,0x00},/*"(",8*/
	{0x00,0x02,0x04,0x18,0xE0,0x00,0x00,0x00,0x00,0x00,0x40,0x20,0x18,0x07,0x00,0x00,0x00,0x00},/*")",9*/
	{0x00,0x40,0x40,0x80,0xF0,0x80,0x40,0x40,0x00,0x00,0x02,0x02,0x01,0x0F,0x01,0x02,0x02,0x00},/*"*",10*/
	{0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x01},/*"+",11*/
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xB0,0x70,0x00,0x00,0x00,0x00,0x00,0x00},/*",",12*/
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01},/*"-",13*/
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00},/*".",14*/
	{0x00,0x00,0x00,0x00,0x00,0x80,0x60,0x18,0x04,0x00,0x40,0x30,0x0C,0x03,0x01,0x00,0x00,0x00},/*"/",15*/
	{0x00,0xE0,0x30,0x08,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x18,0x20,0x20,0x20,0x18,0x0F,0x00},/*"0",16*/
	{0x00,0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},/*"1",17*/
	{0x00,0x60,0x10,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x30,0x28,0x24,0x22,0x22,0x21,0x38,0x00},/*"2",18*/
	{0x00,0x30,0x08,0x88,0x88,0x88,0x58,0x30,0x00,0x00,0x18,0x20,0x20,0x20,0x20,0x11,0x0E,0x00},/*"3",19*/
	{0x00,0x00,0x80,0x40,0x20,0xF8,0x00,0x00,0x00,0x00,0x07,0x04,0x04,0x24,0x3F,0x24,0x24,0x00},/*"4",20*/
	{0x00,0xF8,0x08,0x88,0x88,0x88,0x88,0x08,0x00,0x00,0x19,0x21,0x20,0x20,0x20,0x10,0x0F,0x00},/*"5",21*/
	{0x00,0xE0,0x10,0x88,0x88,0x88,0x98,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x20,0x11,0x0E,0x00},/*"6",22*/
	{0x00,0x38,0x08,0x08,0x88,0x68,0x18,0x08,0x00,0x00,0x00,0x00,0x3E,0x01,0x00,0x00,0x00,0x00},/*"7",23*/
	{0x00,0x70,0x98,0x88,0x08,0x08,0x98,0x70,0x00,0x00,0x1E,0x11,0x20,0x21,0x21,0x32,0x1C,0x00},/*"8",24*/
	{0x00,0xE0,0x18,0x08,0x08,0x08,0x10,0xE0,0x00,0x00,0x00,0x31,0x22,0x22,0x22,0x19,0x07,0x00},/*"9",25*/
	{0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00},/*":",26*/
	{0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x60,0x00,0x00,0x00,0x00},/*";",27*/
	{0x00,0x00,0x80,0x40,0x40,0x20,0x10,0x08,0x00,0x00,0x01,0x02,0x04,0x08,0x08,0x10,0x20,0x00},/*"<",28*/
	{0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00},/*"=",29*/
	{0x00,0x08,0x10,0x20,0x40,0x40,0x80,0x00,0x00,0x00,0x20,0x10,0x08,0x08,0x04,0x02,0x01,0x00},/*">",30*/
	{0x00,0x70,0x50,0x08,0x08,0x08,0x98,0x70,0x00,0x00,0x00,0x00,0x30,0x36,0x01,0x00,0x00,0x00},/*"?",31*/
	{0xC0,0x30,0x98,0x48,0x28,0xE8,0x10,0xE0,0x00,0x07,0x18,0x27,0x24,0x24,0x23,0x14,0x0B,0x00},/*"@",32*/
	{0x00,0x00,0x00,0xE0,0x18,0xF0,0x00,0x00,0x00,0x20,0x38,0x27,0x02,0x02,0x02,0x27,0x38,0x20},/*"A",33*/
	{0x08,0xF8,0x88,0x88,0x88,0xC8,0x70,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x20,0x31,0x1E,0x00},/*"B",34*/
	{0xC0,0x30,0x10,0x08,0x08,0x08,0x18,0x38,0x00,0x0F,0x18,0x20,0x20,0x20,0x20,0x10,0x08,0x00},/*"C",35*/
	{0x08,0xF8,0x08,0x08,0x08,0x10,0x30,0xC0,0x00,0x20,0x3F,0x20,0x20,0x20,0x10,0x18,0x07,0x00},/*"D",36*/
	{0x08,0xF8,0x88,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x20,0x20,0x23,0x30,0x08,0x00},/*"E",37*/
	{0x08,0xF8,0x88,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x00,0x00,0x03,0x00,0x00,0x00},/*"F",38*/
	{0xC0,0x30,0x10,0x08,0x08,0x08,0x18,0x38,0x00,0x07,0x18,0x20,0x20,0x20,0x20,0x22,0x1E,0x02},/*"G",39*/
	{0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x00,0x20,0x3F,0x21,0x01,0x01,0x21,0x3F,0x20,0x00},/*"H",40*/
	{0x00,0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},/*"I",41*/
	{0x00,0x00,0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0xC0,0x80,0x80,0x80,0x40,0x3F,0x00,0x00,0x00},/*"J",42*/
	{0x08,0xF8,0x08,0x80,0x40,0x28,0x18,0x08,0x00,0x20,0x3F,0x21,0x00,0x03,0x2C,0x30,0x20,0x00},/*"K",43*/
	{0x08,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x20,0x20,0x18,0x00},/*"L",44*/
	{0x08,0xF8,0x78,0x80,0x00,0xC0,0x78,0xF8,0x08,0x20,0x3F,0x20,0x07,0x3C,0x03,0x20,0x3F,0x20},/*"M",45*/
	{0x08,0xF8,0x38,0xC0,0x00,0x08,0xF8,0x08,0x00,0x20,0x3F,0x20,0x00,0x07,0x18,0x3F,0x00,0x00},/*"N",46*/
	{0xC0,0x30,0x08,0x08,0x08,0x08,0x30,0xC0,0x00,0x07,0x18,0x20,0x20,0x20,0x20,0x18,0x07,0x00},/*"O",47*/
	{0x08,0xF8,0x08,0x08,0x08,0x08,0x98,0xF0,0x00,0x20,0x3F,0x21,0x01,0x01,0x01,0x00,0x00,0x00},/*"P",48*/
	{0xC0,0x30,0x08,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x18,0x24,0x24,0x2C,0x50,0x48,0x27,0x00},/*"Q",49*/
	{0x08,0xF8,0x88,0x88,0x88,0x88,0x70,0x00,0x00,0x20,0x3F,0x20,0x00,0x03,0x0C,0x30,0x20,0x00},/*"R",50*/
	{0x00,0x70,0x98,0x88,0x08,0x08,0x18,0x38,0x00,0x00,0x38,0x30,0x20,0x21,0x21,0x32,0x1C,0x00},/*"S",51*/
	{0x00,0x18,0x08,0x08,0xF8,0x08,0x08,0x18,0x00,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00},/*"T",52*/
	{0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,0x00},/*"U",53*/
	{0x08,0x38,0xC8,0x00,0x00,0x00,0xE8,0x18,0x08,0x00,0x00,0x01,0x0E,0x30,0x0E,0x01,0x00,0x00},/*"V",54*/
	{0x08,0xF8,0x00,0x88,0xF8,0x08,0x00,0xF8,0x08,0x00,0x07,0x3C,0x07,0x00,0x07,0x3C,0x03,0x00},/*"W",55*/
	{0x08,0x18,0x68,0x80,0xC0,0x28,0x18,0x08,0x00,0x20,0x30,0x2C,0x03,0x06,0x28,0x30,0x20,0x00},/*"X",56*/
	{0x08,0x18,0x68,0x80,0x00,0x80,0x68,0x18,0x08,0x00,0x00,0x00,0x21,0x3F,0x20,0x00,0x00,0x00},/*"Y",57*/
	{0x10,0x08,0x08,0x08,0x88,0x68,0x18,0x08,0x00,0x20,0x30,0x2C,0x23,0x20,0x20,0x30,0x08,0x00},/*"Z",58*/
	{0x00,0x00,0x00,0x00,0xFE,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x7F,0x40,0x40,0x40,0x00},/*"[",59*/
	{0x00,0x04,0x18,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x0C,0x30,0xC0,0x00},/*"\",60*/
	{0x00,0x02,0x02,0x02,0xFE,0x00,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x7F,0x00,0x00,0x00,0x00},/*"]",61*/
	{0x00,0x00,0x04,0x02,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"^",62*/
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},/*"_",63*/
	{0x00,0x00,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"`",64*/
	{0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3F,0x20,0x00},/*"a",65*/
	{0x08,0xF8,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x3F,0x11,0x20,0x20,0x20,0x11,0x1F,0x00},/*"b",66*/
	{0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x0E,0x11,0x20,0x20,0x20,0x20,0x11,0x00},/*"c",67*/
	{0x00,0x00,0x00,0x80,0x80,0x80,0x88,0xF8,0x00,0x00,0x1E,0x11,0x20,0x20,0x20,0x10,0x3F,0x20},/*"d",68*/
	{0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x0F,0x13,0x22,0x22,0x22,0x22,0x13,0x00},/*"e",69*/
	{0x00,0x80,0x80,0xF0,0x88,0x88,0x88,0x18,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,0x00},/*"f",70*/
	{0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x6B,0x94,0x94,0x94,0x94,0x93,0x60,0x00},/*"g",71*/
	{0x08,0xF8,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x00,0x20,0x3F,0x20},/*"h",72*/
	{0x00,0x00,0x80,0x98,0x98,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},/*"i",73*/
	{0x00,0x00,0x00,0x00,0x80,0x98,0x98,0x00,0x00,0x00,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00},/*"j",74*/
	{0x08,0xF8,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x26,0x02,0x0D,0x30,0x20,0x20,0x00},/*"k",75*/
	{0x00,0x00,0x08,0x08,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},/*"l",76*/
	{0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x3F,0x20,0x00,0x3F,0x20,0x00,0x3F,0x20},/*"m",77*/
	{0x80,0x80,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x00,0x20,0x3F,0x20},/*"n",78*/
	{0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x0E,0x11,0x20,0x20,0x20,0x11,0x0E,0x00},/*"o",79*/
	{0x80,0x80,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x80,0xFF,0x91,0x20,0x20,0x20,0x11,0x0F,0x00},/*"p",80*/
	{0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x1E,0x11,0x20,0x20,0x20,0x91,0xFF,0x80},/*"q",81*/
	{0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x20,0x20,0x3F,0x21,0x21,0x00,0x00,0x01,0x00},/*"r",82*/
	{0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x33,0x22,0x24,0x24,0x24,0x28,0x19,0x00},/*"s",83*/
	{0x00,0x80,0x80,0xE0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x20,0x20,0x10,0x00,0x00},/*"t",84*/
	{0x80,0x80,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x10,0x3F,0x20},/*"u",85*/
	{0x80,0x80,0x80,0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x01,0x06,0x18,0x20,0x18,0x06,0x01,0x00},/*"v",86*/
	{0x80,0x80,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x0F,0x30,0x1E,0x03,0x1C,0x30,0x0F,0x00},/*"w",87*/
	{0x00,0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x31,0x3B,0x04,0x2B,0x31,0x20,0x00},/*"x",88*/
	{0x00,0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x81,0xCE,0x70,0x0E,0x01,0x00,0x00},/*"y",89*/
	{0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x21,0x30,0x28,0x24,0x22,0x21,0x38,0x00},/*"z",90*/
	{0x00,0x00,0x00,0x80,0x80,0x7C,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x40,0x40,0x00},/*"{",91*/
	{0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00},/*"|",92*/
	{0x00,0x02,0x02,0x7C,0x80,0x80,0x00,0x00,0x00,0x00,0x40,0x40,0x3F,0x00,0x00,0x00,0x00,0x00},/*"}",93*/
	{0x00,0x06,0x03,0x01,0x02,0x02,0x04,0x06,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"~",94*/
};


#ifdef FONT_LARTIN
const uint8 FontLartin9x16[][18] = {
	/*00C0*/{0x00,0x00,0x00,0xe1,0x1a,0xe0,0x00,0x00,0x00,0x20,0x38,0x27,0x02,0x02,0x02,0x27,0x3c,0x20,},
	/*00C1*/{0x00,0x00,0x00,0xe0,0x1a,0xe1,0x00,0x00,0x00,0x20,0x38,0x27,0x02,0x02,0x02,0x27,0x3c,0x20,},
	/*00C2*/{0x00,0x00,0x00,0xe2,0x19,0xe2,0x00,0x00,0x00,0x20,0x38,0x27,0x02,0x02,0x02,0x27,0x3c,0x20},
	/*00C3*/{0x00,0x00,0x02,0xe1,0x1a,0xe1,0x00,0x00,0x00,0x20,0x38,0x27,0x02,0x02,0x02,0x27,0x3c,0x20},
	/*00C4*/{0x00,0x00,0x00,0xe2,0x18,0xe2,0x00,0x00,0x00,0x20,0x38,0x27,0x02,0x02,0x02,0x27,0x3c,0x20},
	/*00C5*/{0x00,0x00,0x00,0xe2,0x1d,0xe2,0x00,0x00,0x00,0x20,0x38,0x27,0x02,0x02,0x02,0x27,0x3c,0x20},
	/*00C6*/{0x00,0x00,0xe0,0x18,0xfc,0x44,0x44,0xf4,0x08,0x38,0x27,0x22,0x22,0x3f,0x20,0x20,0x31,0x08},
	/*00C7*/{0xe0,0x18,0x08,0x04,0x04,0x04,0x0c,0x1c,0x00,0x07,0x0c,0x10,0x10,0xb0,0x50,0x08,0x04,0x00},
	/*00C8*/{0x08,0xf8,0x88,0x89,0x8a,0xe8,0x08,0x10,0x00,0x20,0x3f,0x20,0x20,0x20,0x23,0x30,0x08,0x00},
	/*00C9*/{0x08,0xf8,0x88,0x88,0x8a,0xe9,0x08,0x10,0x00,0x20,0x3f,0x20,0x20,0x20,0x23,0x30,0x08,0x00},
	/*00CA*/{0x08,0xf8,0x88,0x8a,0x89,0xea,0x08,0x10,0x00,0x20,0x3f,0x20,0x20,0x20,0x23,0x30,0x08,0x00},
	/*00CB*/{0x08,0xf8,0x88,0x8a,0x88,0xea,0x08,0x10,0x00,0x20,0x3f,0x20,0x20,0x20,0x23,0x30,0x08,0x00},
	/*00CC*/{0x00,0x00,0x08,0x09,0xfa,0x08,0x08,0x00,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*00CD*/{0x00,0x00,0x08,0x08,0xfa,0x09,0x08,0x00,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*00CE*/{0x00,0x00,0x08,0x0a,0xf9,0x0a,0x08,0x00,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*00CF*/{0x00,0x00,0x08,0x0a,0xf8,0x0a,0x08,0x00,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*00D0*/{0x08,0xf8,0x08,0x08,0x08,0x10,0x30,0xc0,0x00,0x21,0x3f,0x21,0x21,0x20,0x10,0x18,0x07,0x00},
	/*00D1*/{0x08,0xf8,0x3a,0xc1,0x02,0x09,0xf8,0x08,0x00,0x20,0x3f,0x20,0x00,0x07,0x18,0x3f,0x00,0x00},
	/*00D2*/{0xc0,0x30,0x08,0x09,0x0a,0x08,0x30,0xc0,0x00,0x07,0x18,0x20,0x20,0x20,0x20,0x18,0x07,0x00},
	/*00D3*/{0xc0,0x30,0x08,0x08,0x0a,0x09,0x30,0xc0,0x00,0x07,0x18,0x20,0x20,0x20,0x20,0x18,0x07,0x00},
	/*00D4*/{0xc0,0x30,0x08,0x0a,0x09,0x0a,0x30,0xc0,0x00,0x07,0x18,0x20,0x20,0x20,0x20,0x18,0x07,0x00},
	/*00D5*/{0xc0,0x30,0x0a,0x09,0x0a,0x09,0x30,0xc0,0x00,0x07,0x18,0x20,0x20,0x20,0x20,0x18,0x07,0x00},
	/*00D6*/{0xc0,0x30,0x08,0x0a,0x08,0x0a,0x30,0xc0,0x00,0x07,0x18,0x20,0x20,0x20,0x20,0x18,0x07,0x00},
	/*00D7*/{0x08,0x18,0x68,0x80,0xc0,0x28,0x18,0x08,0x00,0x20,0x30,0x2c,0x03,0x06,0x28,0x30,0x20,0x00},
	/*00D8*/{0xc0,0x30,0x08,0x08,0x08,0x88,0x70,0xe0,0x10,0x17,0x18,0x24,0x22,0x21,0x20,0x18,0x07,0x00},
	/*00D9*/{0x08,0xf8,0x08,0x01,0x02,0x00,0x08,0xf8,0x08,0x00,0x1f,0x20,0x20,0x20,0x20,0x20,0x1f,0x00},
	/*00DA*/{0x08,0xf8,0x08,0x00,0x02,0x01,0x08,0xf8,0x08,0x00,0x1f,0x20,0x20,0x20,0x20,0x20,0x1f,0x00},
	/*00DB*/{0x08,0xf8,0x08,0x02,0x01,0x02,0x08,0xf8,0x08,0x00,0x1f,0x20,0x20,0x20,0x20,0x20,0x1f,0x00},
	/*00DC*/{0x08,0xf8,0x08,0x02,0x00,0x02,0x08,0xf8,0x08,0x00,0x1f,0x20,0x20,0x20,0x20,0x20,0x1f,0x00},
	/*00DD*/{0x00,0x08,0x38,0xc8,0x02,0xc9,0x38,0x08,0x00,0x00,0x00,0x00,0x20,0x3f,0x20,0x00,0x00,0x00},
	/*00DE*/{0x08,0xf8,0x48,0x40,0x40,0x80,0x80,0x00,0x00,0x20,0x3f,0x28,0x08,0x08,0x04,0x04,0x03,0x00},
	/*00DF*/{0x00,0xe0,0x10,0x08,0x88,0x48,0x30,0x00,0x00,0x20,0x3f,0x20,0x00,0x08,0x09,0x06,0x00,0x00},
	/*00E0*/{0x00,0x00,0x80,0x90,0xa0,0x80,0x00,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3f,0x20,0x00},
	/*00E1*/{0x00,0x00,0x80,0x80,0xa0,0x90,0x00,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3f,0x20,0x00},
	/*00E2*/{0x00,0x00,0x80,0xa0,0x90,0xa0,0x00,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3f,0x20,0x00},
	/*00E3*/{0x00,0x00,0xa0,0x90,0xa0,0x90,0x00,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3f,0x20,0x00},
	/*00E4*/{0x00,0x00,0x80,0xa0,0x80,0xa0,0x00,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3f,0x20,0x00},
	/*00E5*/{0x00,0x00,0x80,0x90,0xa8,0x90,0x00,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3f,0x20,0x00},
	/*00E6*/{0x00,0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x19,0x24,0x22,0x22,0x3f,0x22,0x22,0x22,0x13},
	/*00E7*/{0x00,0x00,0x80,0x40,0x40,0x40,0x40,0x80,0x00,0x00,0x07,0x08,0x10,0xb0,0x50,0x10,0x08,0x00},
	/*00E8*/{0x00,0x00,0x00,0x90,0xa0,0x80,0x80,0x00,0x00,0x00,0x0f,0x13,0x22,0x22,0x22,0x22,0x13,0x00},
	/*00E9*/{0x00,0x00,0x00,0x80,0xa0,0x90,0x80,0x00,0x00,0x00,0x0f,0x13,0x22,0x22,0x22,0x22,0x13,0x00},
	/*00EA*/{0x00,0x00,0x00,0xa0,0x90,0xa0,0x80,0x00,0x00,0x00,0x0f,0x13,0x22,0x22,0x22,0x22,0x13,0x00},
	/*00EB*/{0x00,0x00,0x00,0xa0,0x80,0xa0,0x80,0x00,0x00,0x00,0x0f,0x13,0x22,0x22,0x22,0x22,0x13,0x00},
	/*00EC*/{0x00,0x00,0x80,0x99,0x9a,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*00ED*/{0x00,0x00,0x80,0x98,0x9a,0x01,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*00EE*/{0x00,0x00,0x80,0x9a,0x99,0x02,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*00EF*/{0x00,0x00,0x80,0x9a,0x98,0x02,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*00F0*/{0x00,0x00,0x08,0xc8,0xa8,0x90,0x28,0xc0,0x00,0x00,0x0e,0x11,0x20,0x20,0x20,0x11,0x0f,0x00},
	/*00F1*/{0x80,0x80,0x20,0x90,0xa0,0x90,0x80,0x00,0x00,0x20,0x3f,0x21,0x00,0x00,0x00,0x20,0x3f,0x20},
	/*00F2*/{0x00,0x00,0x00,0x90,0xa0,0x80,0x00,0x00,0x00,0x00,0x0e,0x11,0x20,0x20,0x20,0x11,0x0e,0x00},
	/*00F3*/{0x00,0x00,0x00,0x80,0xa0,0x90,0x00,0x00,0x00,0x00,0x0e,0x11,0x20,0x20,0x20,0x11,0x0e,0x00},
	/*00F4*/{0x00,0x00,0x00,0xa0,0x90,0xa0,0x00,0x00,0x00,0x00,0x0e,0x11,0x20,0x20,0x20,0x11,0x0e,0x00},
	/*00F5*/{0x00,0x00,0x20,0x90,0xa0,0x90,0x00,0x00,0x00,0x00,0x0e,0x11,0x20,0x20,0x20,0x11,0x0e,0x00},
	/*00F6*/{0x00,0x00,0x00,0xa0,0x80,0xa0,0x00,0x00,0x00,0x00,0x0e,0x11,0x20,0x20,0x20,0x11,0x0e,0x00},
	/*00F7*/{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x15,0x04,0x04,0x04,0x00},
	/*00F8*/{0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x80,0x00,0x00,0x2e,0x11,0x28,0x24,0x22,0x11,0x0e,0x00},
	/*00F9*/{0x80,0x80,0x00,0x10,0x20,0x00,0x80,0x80,0x00,0x00,0x1f,0x20,0x20,0x20,0x20,0x10,0x3f,0x20},
	/*00FA*/{0x80,0x80,0x00,0x00,0x20,0x10,0x80,0x80,0x00,0x00,0x1f,0x20,0x20,0x20,0x20,0x10,0x3f,0x20},
	/*00FB*/{0x80,0x80,0x00,0x20,0x10,0x20,0x80,0x80,0x00,0x00,0x1f,0x20,0x20,0x20,0x20,0x10,0x3f,0x20},
	/*00FC*/{0x80,0x80,0x00,0x20,0x00,0x20,0x80,0x80,0x00,0x00,0x1f,0x20,0x20,0x20,0x20,0x10,0x3f,0x20},
	/*00FD*/{0x00,0x40,0xc0,0x40,0x10,0x48,0xc0,0x40,0x00,0x00,0x00,0x80,0xc7,0x78,0x07,0x00,0x00,0x00},
	/*00FE*/{0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x3f,0x2a,0x0a,0x0a,0x04,0x00,0x00,0x00},
	/*00FF*/{0x00,0x40,0xc0,0x50,0x00,0x50,0xc0,0x40,0x00,0x00,0x00,0x80,0xc7,0x78,0x07,0x00,0x00,0x00},
	/*0100*/{0x00,0x00,0x00,0xe2,0x1a,0xe2,0x00,0x00,0x00,0x20,0x38,0x27,0x02,0x02,0x02,0x27,0x3c,0x20},
	/*0101*/{0x00,0x00,0x80,0xa0,0xa0,0xa0,0x00,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3f,0x20,0x00},
	/*0102*/{0x00,0x00,0x01,0xe2,0x1a,0xe1,0x00,0x00,0x00,0x20,0x38,0x27,0x02,0x02,0x02,0x27,0x3c,0x20},
	/*0103*/{0x00,0x00,0x90,0xa0,0xa0,0x90,0x00,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3f,0x20,0x00},
	/*0104*/{0x00,0x00,0x80,0x70,0x0c,0x70,0x80,0x00,0x00,0x10,0x1c,0x13,0x01,0x01,0x01,0x53,0xbc,0x10},
	/*0105*/{0x00,0x80,0x40,0x40,0x40,0x40,0x80,0x00,0x00,0x00,0x0c,0x12,0x11,0x11,0x51,0xbf,0x10,0x00},
	/*0106*/{0xc0,0x30,0x10,0x08,0x0a,0x09,0x18,0x38,0x00,0x0f,0x18,0x20,0x20,0x20,0x20,0x10,0x08,0x00},
	/*0107*/{0x00,0x00,0x00,0x80,0xa0,0x90,0x80,0x00,0x00,0x00,0x0e,0x11,0x20,0x20,0x20,0x20,0x11,0x00},
	/*0108*/{0xc0,0x30,0x10,0x0a,0x09,0x0a,0x18,0x38,0x00,0x0f,0x18,0x20,0x20,0x20,0x20,0x10,0x08,0x00},
	/*0109*/{0x00,0x00,0x00,0xa0,0x90,0xa0,0x80,0x00,0x00,0x00,0x0e,0x11,0x20,0x20,0x20,0x20,0x11,0x00},
	/*010A*/{0xc0,0x30,0x10,0x08,0x0a,0x08,0x18,0x38,0x00,0x0f,0x18,0x20,0x20,0x20,0x20,0x10,0x08,0x00},
	/*010B*/{0x00,0x00,0x00,0x80,0xa0,0x80,0x80,0x00,0x00,0x00,0x0e,0x11,0x20,0x20,0x20,0x20,0x11,0x00},
	/*010C*/{0xc0,0x30,0x10,0x09,0x0a,0x09,0x18,0x38,0x00,0x0f,0x18,0x20,0x20,0x20,0x20,0x10,0x08,0x00},
	/*010D*/{0x00,0x00,0x00,0x90,0xa0,0x90,0x80,0x00,0x00,0x00,0x0e,0x11,0x20,0x20,0x20,0x20,0x11,0x00},
	/*010E*/{0x08,0xf8,0x08,0x09,0x0a,0x11,0x30,0xc0,0x00,0x20,0x3f,0x20,0x20,0x20,0x10,0x18,0x07,0x00},
	/*010F*/{0x00,0x00,0x80,0x80,0x88,0xf8,0x00,0x50,0x20,0x1e,0x11,0x20,0x20,0x10,0x3f,0x20,0x00,0x00},
	/*0110*/{0x18,0xf8,0x08,0x08,0x08,0x10,0x30,0xc0,0x00,0x20,0x3f,0x21,0x21,0x20,0x10,0x18,0x07,0x00},
	/*0111*/{0x00,0x00,0x00,0x80,0x80,0x80,0xa8,0xf8,0x20,0x00,0x1e,0x11,0x20,0x20,0x20,0x10,0x3f,0x20},
	/*0112*/{0x08,0xf8,0x88,0x8a,0x8a,0xea,0x08,0x10,0x00,0x20,0x3f,0x20,0x20,0x20,0x23,0x30,0x08,0x00},
	/*0113*/{0x00,0x00,0x00,0xa0,0xa0,0xa0,0x80,0x00,0x00,0x00,0x0f,0x13,0x22,0x22,0x22,0x22,0x13,0x00},
	/*0114*/{0x08,0xf8,0x89,0x8a,0x8a,0xe9,0x08,0x10,0x00,0x20,0x3f,0x20,0x20,0x20,0x23,0x30,0x08,0x00},
	/*0115*/{0x00,0x00,0x10,0xa0,0xa0,0x90,0x80,0x00,0x00,0x00,0x0f,0x13,0x22,0x22,0x22,0x22,0x13,0x00},
	/*0116*/{0x08,0xf8,0x88,0x88,0x8a,0xe8,0x08,0x10,0x00,0x20,0x3f,0x20,0x20,0x20,0x23,0x30,0x08,0x00},
	/*0117*/{0x00,0x00,0x00,0x80,0xa0,0x80,0x80,0x00,0x00,0x00,0x0f,0x13,0x22,0x22,0x22,0x22,0x13,0x00},
	/*0118*/{0x04,0xfc,0x44,0x44,0x44,0xf4,0x04,0x08,0x00,0x10,0x1f,0x10,0x10,0x10,0x51,0xb8,0x04,0x00},
	/*0119*/{0x00,0x80,0x80,0x40,0x40,0x40,0x40,0x80,0x00,0x00,0x07,0x09,0x11,0x11,0x51,0xb1,0x09,0x00},
	/*011A*/{0x08,0xf8,0x88,0x89,0x8a,0xe9,0x08,0x10,0x00,0x20,0x3f,0x20,0x20,0x20,0x23,0x30,0x08,0x00},
	/*011B*/{0x00,0x00,0x00,0x90,0xa0,0x90,0x80,0x00,0x00,0x00,0x0f,0x13,0x22,0x22,0x22,0x22,0x13,0x00},
	/*011C*/{0xc0,0x30,0x10,0x0a,0x09,0x0a,0x18,0x38,0x00,0x07,0x18,0x20,0x20,0x20,0x20,0x22,0x1e,0x02},
	/*011D*/{0x00,0x00,0x80,0xa0,0x90,0xa0,0x80,0x80,0x00,0x00,0x6b,0x94,0x94,0x94,0x94,0x93,0x60,0x00},
	/*011E*/{0xc0,0x30,0x11,0x0a,0x0a,0x09,0x18,0x38,0x00,0x07,0x18,0x20,0x20,0x20,0x20,0x22,0x1e,0x02},
	/*011F*/{0x00,0x00,0x90,0xa0,0xa0,0x90,0x80,0x80,0x00,0x00,0x6b,0x94,0x94,0x94,0x94,0x93,0x60,0x00},
	/*0120*/{0xc0,0x30,0x10,0x08,0x0a,0x08,0x18,0x38,0x00,0x07,0x18,0x20,0x20,0x20,0x20,0x22,0x1e,0x02},
	/*0121*/{0x00,0x00,0x80,0x80,0xa0,0x80,0x80,0x80,0x00,0x00,0x6b,0x94,0x94,0x94,0x94,0x93,0x60,0x00},
	/*0122*/{0xe0,0x18,0x08,0x04,0x04,0x04,0x0c,0x1c,0x00,0x03,0x0c,0x10,0x10,0xb0,0x50,0x11,0x0f,0x01},
	/*0123*/{0x00,0x00,0x80,0x90,0xa0,0x80,0x80,0x80,0x00,0x00,0x6b,0x94,0x94,0x94,0x94,0x93,0x60,0x00},
	/*0124*/{0x08,0xf8,0x08,0x02,0x01,0x0a,0xf8,0x08,0x00,0x20,0x3f,0x21,0x01,0x01,0x21,0x3f,0x20,0x00},
	/*0125*/{0x08,0xf8,0x08,0x82,0x81,0x82,0x00,0x00,0x00,0x20,0x3f,0x21,0x00,0x00,0x20,0x3f,0x20,0x00},
	/*0126*/{0x28,0xf8,0x28,0x20,0x20,0x28,0xf8,0x28,0x00,0x20,0x3f,0x21,0x01,0x01,0x21,0x3f,0x20,0x00},
	/*0127*/{0x28,0xf8,0x28,0x80,0x80,0x80,0x00,0x00,0x00,0x20,0x3f,0x21,0x00,0x00,0x20,0x3f,0x20,0x00},
	/*0128*/{0x00,0x00,0x0a,0x09,0xfa,0x09,0x08,0x00,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*0129*/{0x00,0x00,0x82,0x99,0x9a,0x01,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*012A*/{0x00,0x00,0x08,0x0a,0xfa,0x0a,0x08,0x00,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*012B*/{0x00,0x00,0x80,0x9a,0x9a,0x02,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*012C*/{0x00,0x00,0x09,0x0a,0xfa,0x09,0x08,0x00,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*012D*/{0x00,0x00,0x81,0x9a,0x9a,0x01,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*012E*/{0x00,0x00,0x04,0x04,0xfc,0x04,0x04,0x00,0x00,0x00,0x00,0x10,0x50,0xbf,0x10,0x10,0x00,0x00},
	/*012F*/{0x00,0x00,0x40,0x58,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x50,0xbf,0x10,0x10,0x00,0x00},
	/*0130*/{0x00,0x00,0x08,0x08,0xfa,0x08,0x08,0x00,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*0131*/{0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*0132*/{0x08,0xf8,0x08,0x00,0x08,0x08,0xf8,0x08,0x08,0x20,0xff,0xa0,0x80,0x80,0x40,0x3f,0x00,0x00},
	/*0133*/{0x80,0x98,0x98,0x00,0x00,0x00,0x80,0x98,0x98,0x20,0x20,0x3f,0x20,0xe0,0x80,0x80,0x80,0x7f},
	/*0134*/{0x00,0x00,0x00,0x08,0x0a,0xf9,0x0a,0x08,0x00,0xc0,0x80,0x80,0x80,0x40,0x3f,0x00,0x00,0x00},
	/*0135*/{0x00,0x00,0x00,0x00,0x82,0x99,0x9a,0x00,0x00,0x00,0x00,0xc0,0x80,0x80,0x80,0x7f,0x00,0x00},
	/*0136*/{0x04,0xfc,0x84,0x40,0xa0,0x14,0x0c,0x04,0x00,0x10,0x1f,0x10,0xa0,0x41,0x16,0x18,0x10,0x00},
	/*0137*/{0x04,0xfc,0x00,0x00,0xc0,0x40,0x40,0x00,0x00,0x10,0x1f,0x13,0xa1,0x46,0x18,0x10,0x10,0x00},
	/*0138*/{0x08,0xf8,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3f,0x26,0x02,0x0d,0x30,0x20,0x20,0x00},
	/*0139*/{0x08,0xf8,0x08,0x00,0x02,0x01,0x00,0x00,0x00,0x20,0x3f,0x20,0x20,0x20,0x20,0x20,0x18,0x00},
	/*013A*/{0x00,0x00,0x08,0x08,0xfa,0x01,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*013B*/{0x04,0xfc,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x1f,0x10,0x10,0xb0,0x50,0x10,0x0c,0x00},
	/*013C*/{0x00,0x00,0x04,0x04,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x10,0xbf,0x50,0x10,0x00,0x00},
	/*013D*/{0x08,0xf8,0x08,0x00,0xa0,0x40,0x00,0x00,0x00,0x20,0x3f,0x20,0x20,0x20,0x20,0x20,0x18,0x00},
	/*013E*/{0x00,0x00,0x08,0x08,0xf8,0x00,0xa0,0x40,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*013F*/{0x08,0xf8,0x08,0x00,0x40,0x00,0x00,0x00,0x00,0x20,0x3f,0x20,0x20,0x20,0x20,0x20,0x18,0x00},
	/*0140*/{0x00,0x00,0x08,0x08,0xf8,0x00,0x40,0x00,0x00,0x00,0x00,0x20,0x20,0x3f,0x20,0x20,0x00,0x00},
	/*0141*/{0x08,0xf8,0x08,0x80,0x40,0x00,0x00,0x00,0x00,0x24,0x3f,0x21,0x20,0x20,0x20,0x20,0x18,0x00},
	/*0142*/{0x00,0x00,0x08,0x08,0xf8,0x80,0x40,0x00,0x00,0x00,0x00,0x24,0x22,0x3f,0x20,0x20,0x00,0x00},
	/*0143*/{0x08,0xf8,0x38,0xc2,0x01,0x08,0xf8,0x08,0x00,0x20,0x3f,0x20,0x00,0x07,0x18,0x3f,0x00,0x00},
	/*0144*/{0x80,0x80,0x00,0xa0,0x90,0x80,0x80,0x00,0x00,0x20,0x3f,0x21,0x00,0x00,0x00,0x20,0x3f,0x20},
	/*0145*/{0x04,0xfc,0x1c,0x60,0x80,0x04,0xfc,0x04,0x00,0x10,0x1f,0x10,0x00,0xa3,0x4c,0x1f,0x00,0x00},
	/*0146*/{0x40,0xc0,0x80,0x40,0x40,0x40,0x40,0x80,0x00,0x10,0x1f,0x10,0x00,0xa0,0x40,0x10,0x1f,0x10},
	/*0147*/{0x08,0xf8,0x38,0xc1,0x02,0x09,0xf8,0x08,0x00,0x20,0x3f,0x20,0x00,0x07,0x18,0x3f,0x00,0x00},
	/*0148*/{0x80,0x80,0x00,0x90,0xa0,0x90,0x80,0x00,0x00,0x20,0x3f,0x21,0x00,0x00,0x00,0x20,0x3f,0x20},
	/*0149*/{0x80,0xa8,0x10,0x80,0x80,0x80,0x80,0x00,0x00,0x20,0x3f,0x21,0x00,0x00,0x00,0x20,0x3f,0x20},
	/*014A*/{0x04,0xfc,0x1c,0x60,0x80,0x04,0xfc,0x04,0x00,0x10,0x1f,0x10,0x40,0x83,0x8c,0x7f,0x00,0x00},
	/*014B*/{0x40,0xc0,0x80,0x40,0x40,0x40,0x40,0x80,0x00,0x10,0x1f,0x10,0x00,0x40,0x80,0x90,0x7f,0x10},
	/*014C*/{0xc0,0x30,0x08,0x0a,0x0a,0x0a,0x30,0xc0,0x00,0x07,0x18,0x20,0x20,0x20,0x20,0x18,0x07,0x00},
	/*014D*/{0x00,0x00,0x00,0xa0,0xa0,0xa0,0x00,0x00,0x00,0x00,0x0e,0x11,0x20,0x20,0x20,0x11,0x0e,0x00},
	/*014E*/{0xc0,0x30,0x09,0x0a,0x0a,0x09,0x30,0xc0,0x00,0x07,0x18,0x20,0x20,0x20,0x20,0x18,0x07,0x00},
	/*014F*/{0x00,0x00,0x10,0xa0,0xa0,0x90,0x00,0x00,0x00,0x00,0x0e,0x11,0x20,0x20,0x20,0x11,0x0e,0x00},
	/*0150*/{0xc0,0x30,0x0a,0x09,0x08,0x0a,0x31,0xc0,0x00,0x07,0x18,0x20,0x20,0x20,0x20,0x18,0x07,0x00},
	/*0151*/{0x00,0x00,0x20,0x90,0x80,0xa0,0x10,0x00,0x00,0x00,0x0e,0x11,0x20,0x20,0x20,0x11,0x0e,0x00},
	/*0152*/{0xc0,0x30,0x08,0x08,0xf8,0x88,0x88,0x88,0xd0,0x07,0x18,0x20,0x20,0x3f,0x20,0x20,0x30,0x09},
	/*0153*/{0x00,0x00,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x0e,0x11,0x20,0x20,0x1f,0x22,0x22,0x22,0x13},
	/*0154*/{0x08,0xf8,0x88,0x88,0x8a,0x89,0x70,0x00,0x00,0x20,0x3f,0x20,0x00,0x03,0x0c,0x30,0x20,0x00},
	/*0155*/{0x80,0x80,0x80,0x00,0x20,0x90,0x80,0x80,0x00,0x20,0x20,0x3f,0x21,0x21,0x00,0x00,0x01,0x00},
	/*0156*/{0x04,0xfc,0x44,0x44,0xc4,0x44,0x38,0x00,0x00,0x10,0x1f,0x10,0xa0,0x41,0x06,0x18,0x10,0x00},
	/*0157*/{0x40,0x40,0xc0,0x80,0x80,0x40,0x40,0xc0,0x00,0x10,0x10,0x1f,0x10,0x00,0xa0,0x40,0x00,0x00},
	/*0158*/{0x08,0xf8,0x88,0x89,0x8a,0x89,0x70,0x00,0x00,0x20,0x3f,0x20,0x00,0x03,0x0c,0x30,0x20,0x00},
	/*0159*/{0x80,0x80,0x80,0x10,0x20,0x90,0x80,0x80,0x00,0x20,0x20,0x3f,0x21,0x21,0x00,0x00,0x01,0x00},
	/*015A*/{0x00,0x70,0x98,0x88,0x0a,0x09,0x18,0x38,0x00,0x00,0x38,0x30,0x20,0x21,0x21,0x32,0x1c,0x00},
	/*015B*/{0x00,0x00,0x80,0x80,0xa0,0x90,0x80,0x80,0x00,0x00,0x33,0x22,0x24,0x24,0x24,0x28,0x19,0x00},
	/*015C*/{0x00,0x70,0x98,0x8a,0x09,0x0a,0x18,0x38,0x00,0x00,0x38,0x30,0x20,0x21,0x21,0x32,0x1c,0x00},
	/*015D*/{0x00,0x00,0x80,0xa0,0x90,0xa0,0x80,0x80,0x00,0x00,0x33,0x22,0x24,0x24,0x24,0x28,0x19,0x00},
	/*015E*/{0x00,0x38,0x4c,0x44,0x84,0x84,0x0c,0x1c,0x00,0x00,0x1c,0x18,0x10,0xb0,0x50,0x19,0x0e,0x00},
	/*015F*/{0x00,0x80,0x40,0x40,0x40,0x40,0x40,0xc0,0x00,0x00,0x19,0x11,0x12,0xb2,0x52,0x14,0x0c,0x00},
	/*0160*/{0x00,0x70,0x98,0x89,0x0a,0x09,0x18,0x38,0x00,0x00,0x38,0x30,0x20,0x21,0x21,0x32,0x1c,0x00},
	/*0161*/{0x00,0x00,0x80,0x90,0xa0,0x90,0x80,0x80,0x00,0x00,0x33,0x22,0x24,0x24,0x24,0x28,0x19,0x00},
	/*0162*/{0x00,0x0c,0x04,0x04,0xfc,0x04,0x04,0x0c,0x00,0x00,0x00,0x00,0x10,0xbf,0x50,0x00,0x00,0x00},
	/*0163*/{0x00,0x40,0x40,0xf0,0x40,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xb0,0x50,0x08,0x00,0x00},
	/*0164*/{0x00,0x18,0x08,0x09,0xfa,0x09,0x08,0x18,0x00,0x00,0x00,0x00,0x20,0x3f,0x20,0x00,0x00,0x00},
	/*0165*/{0x00,0x80,0x80,0xe0,0x80,0x80,0x00,0x50,0x20,0x00,0x00,0x00,0x1f,0x20,0x20,0x10,0x00,0x00},
	/*0166*/{0x00,0x18,0x88,0x88,0xf8,0x88,0x88,0x18,0x00,0x00,0x00,0x00,0x20,0x3f,0x20,0x00,0x00,0x00},
	/*0167*/{0x00,0x80,0x80,0xe0,0x80,0x80,0x00,0x00,0x00,0x00,0x04,0x04,0x1f,0x24,0x24,0x10,0x00,0x00},
	/*0168*/{0x08,0xf8,0x0a,0x01,0x02,0x01,0x08,0xf8,0x08,0x00,0x1f,0x20,0x20,0x20,0x20,0x20,0x1f,0x00},
	/*0169*/{0x80,0x80,0x20,0x10,0x20,0x10,0x80,0x80,0x00,0x00,0x1f,0x20,0x20,0x20,0x20,0x10,0x3f,0x20},
	/*016A*/{0x08,0xf8,0x08,0x02,0x02,0x02,0x08,0xf8,0x08,0x00,0x1f,0x20,0x20,0x20,0x20,0x20,0x1f,0x00},
	/*016B*/{0x80,0x80,0x00,0x20,0x20,0x20,0x80,0x80,0x00,0x00,0x1f,0x20,0x20,0x20,0x20,0x10,0x3f,0x20},
	/*016C*/{0x08,0xf8,0x09,0x02,0x02,0x01,0x08,0xf8,0x08,0x00,0x1f,0x20,0x20,0x20,0x20,0x20,0x1f,0x00},
	/*016D*/{0x80,0x80,0x10,0x20,0x20,0x10,0x80,0x80,0x00,0x00,0x1f,0x20,0x20,0x20,0x20,0x10,0x3f,0x20},
	/*016E*/{0x08,0xf8,0x08,0x02,0x05,0x02,0x08,0xf8,0x08,0x00,0x1f,0x20,0x20,0x20,0x20,0x20,0x1f,0x00},
	/*016F*/{0x80,0x80,0x00,0x10,0x28,0x10,0x80,0x80,0x00,0x00,0x1f,0x20,0x20,0x20,0x20,0x10,0x3f,0x20},
	/*0170*/{0x08,0xf8,0x0a,0x01,0x00,0x02,0x09,0xf8,0x08,0x00,0x1f,0x20,0x20,0x20,0x20,0x20,0x1f,0x00},
	/*0171*/{0x80,0x80,0x20,0x10,0x00,0x20,0x90,0x80,0x00,0x00,0x1f,0x20,0x20,0x20,0x20,0x10,0x3f,0x20},
	/*0172*/{0x04,0xfc,0x04,0x00,0x00,0x00,0x04,0xfc,0x04,0x00,0x0f,0x10,0x50,0xb0,0x10,0x10,0x0f,0x00},
	/*0173*/{0x40,0xc0,0x00,0x00,0x00,0x00,0x40,0xc0,0x00,0x00,0x0f,0x10,0x50,0xb0,0x10,0x08,0x1f,0x10},
	/*0174*/{0x08,0xf8,0x00,0x8a,0xf9,0x0a,0x00,0xf8,0x08,0x00,0x07,0x3c,0x07,0x00,0x07,0x3c,0x03,0x00},
	/*0175*/{0x80,0x80,0x00,0xa0,0x90,0xa0,0x00,0x80,0x80,0x00,0x0f,0x30,0x1e,0x03,0x1c,0x30,0x0f,0x00},
	/*0176*/{0x00,0x08,0x38,0xca,0x01,0xca,0x38,0x08,0x00,0x00,0x00,0x00,0x20,0x3f,0x20,0x00,0x00,0x00},
	/*0177*/{0x00,0x40,0xc0,0x50,0x08,0x50,0xc0,0x40,0x00,0x00,0x00,0x80,0xc7,0x78,0x07,0x00,0x00,0x00},
	/*0178*/{0x00,0x08,0x38,0xca,0x00,0xca,0x38,0x08,0x00,0x00,0x00,0x00,0x20,0x3f,0x20,0x00,0x00,0x00},
	/*0179*/{0x10,0x08,0x08,0x08,0x8a,0x69,0x18,0x08,0x00,0x20,0x30,0x2c,0x23,0x20,0x20,0x30,0x08,0x00},
	/*017A*/{0x00,0x80,0x80,0x80,0xa0,0x90,0x80,0x80,0x00,0x00,0x21,0x30,0x28,0x24,0x22,0x21,0x38,0x00},
	/*017B*/{0x10,0x08,0x08,0x08,0x8a,0x68,0x18,0x08,0x00,0x20,0x30,0x2c,0x23,0x20,0x20,0x30,0x08,0x00},
	/*017C*/{0x00,0x80,0x80,0x80,0xa0,0x80,0x80,0x80,0x00,0x00,0x21,0x30,0x28,0x24,0x22,0x21,0x38,0x00},
	/*017D*/{0x10,0x08,0x08,0x09,0x8a,0x69,0x18,0x08,0x00,0x20,0x30,0x2c,0x23,0x20,0x20,0x30,0x08,0x00},
	/*017E*/{0x00,0x80,0x80,0x90,0xa0,0x90,0x80,0x80,0x00,0x00,0x21,0x30,0x28,0x24,0x22,0x21,0x38,0x00},
	/*017F*/{0x00,0x00,0x00,0xc0,0x20,0x20,0x20,0x40,0x00,0x00,0x80,0x80,0xff,0x80,0x80,0x00,0x00,0x00},
};
#endif

#ifdef FONT_RUSSION
const uint8 FontRussion9x16[][18] = {
	/*0410*/{0x00,0x00,0x00,0xe0,0x18,0xe0,0x00,0x00,0x00,0x20,0x38,0x27,0x02,0x02,0x02,0x27,0x3c,0x20},
	/*0411*/{0x08,0xf8,0x88,0x88,0x88,0x88,0x08,0x18,0x00,0x20,0x3f,0x20,0x20,0x20,0x20,0x31,0x1e,0x00},
	/*0412*/{0x08,0xf8,0x88,0x88,0x88,0xc8,0x70,0x00,0x00,0x20,0x3f,0x20,0x20,0x20,0x20,0x31,0x1e,0x00},
	/*0413*/{0x08,0xf8,0x08,0x08,0x08,0x08,0x08,0x18,0x00,0x20,0x3f,0x20,0x20,0x00,0x00,0x00,0x00,0x00},
	/*0414*/{0x00,0x08,0x08,0xf8,0x08,0x08,0xf8,0x08,0x00,0x30,0x08,0x0c,0x0b,0x08,0x08,0x0f,0x08,0x30},
	/*0415*/{0x08,0xf8,0x88,0x88,0x88,0xe8,0x08,0x10,0x00,0x20,0x3f,0x20,0x20,0x20,0x23,0x30,0x08,0x00},
	/*0416*/{0x08,0x38,0xc0,0x08,0xf8,0x08,0xc0,0x38,0x08,0x20,0x38,0x06,0x21,0x3f,0x21,0x06,0x38,0x20},
	/*0417*/{0x38,0x10,0x08,0x08,0x08,0x08,0x90,0x60,0x00,0x18,0x10,0x20,0x21,0x21,0x21,0x12,0x0c,0x00},
	/*0418*/{0x08,0xf8,0x08,0x00,0xc0,0x38,0xf8,0x08,0x00,0x20,0x3f,0x18,0x07,0x00,0x20,0x3f,0x20,0x00},
	/*0419*/{0x08,0xf8,0x09,0x02,0xc2,0x39,0xf8,0x08,0x00,0x20,0x3f,0x18,0x07,0x00,0x20,0x3f,0x20,0x00},
	/*041A*/{0x08,0xf8,0x08,0x80,0x40,0x20,0x10,0x20,0x00,0x20,0x3f,0x21,0x00,0x03,0x1c,0x20,0x10,0x00},
	/*041B*/{0x00,0x08,0x08,0xf8,0x08,0x08,0x08,0xf8,0x08,0x80,0x80,0x40,0x3f,0x00,0x20,0x20,0x3f,0x20},
	/*041C*/{0x08,0xf8,0x78,0x80,0x00,0xc0,0x78,0xf8,0x08,0x20,0x3f,0x20,0x07,0x3c,0x03,0x20,0x3f,0x20},
	/*041D*/{0x08,0xf8,0x08,0x00,0x00,0x08,0xf8,0x08,0x00,0x20,0x3f,0x21,0x01,0x01,0x21,0x3f,0x20,0x00},
	/*041E*/{0xc0,0x30,0x08,0x08,0x08,0x08,0x30,0xc0,0x00,0x07,0x18,0x20,0x20,0x20,0x20,0x18,0x07,0x00},
	/*041F*/{0x08,0xf8,0x08,0x08,0x08,0x08,0xf8,0x08,0x00,0x20,0x3f,0x20,0x00,0x00,0x20,0x3f,0x20,0x00},
	/*0420*/{0x08,0xf8,0x08,0x08,0x08,0x08,0x98,0xf0,0x00,0x20,0x3f,0x21,0x01,0x01,0x01,0x00,0x00,0x00},
	/*0421*/{0xc0,0x30,0x10,0x08,0x08,0x08,0x18,0x38,0x00,0x0f,0x18,0x20,0x20,0x20,0x20,0x10,0x08,0x00},
	/*0422*/{0x00,0x18,0x08,0x08,0xf8,0x08,0x08,0x18,0x00,0x00,0x00,0x00,0x20,0x3f,0x20,0x00,0x00,0x00},
	/*0423*/{0x00,0x08,0x38,0xc8,0x00,0xc8,0x38,0x08,0x00,0x00,0x00,0x00,0x20,0x3f,0x20,0x00,0x00,0x00},
	/*0424*/{0x80,0x60,0x10,0x10,0xf8,0x10,0x10,0x60,0x80,0x03,0x0c,0x10,0x10,0x3f,0x10,0x10,0x0c,0x03},
	/*0425*/{0x08,0x18,0x68,0x80,0xc0,0x28,0x18,0x08,0x00,0x20,0x30,0x2c,0x03,0x06,0x28,0x30,0x20,0x00},
	/*0426*/{0x08,0xf8,0x08,0x00,0x00,0x08,0xf8,0x08,0x00,0x20,0x3f,0x20,0x20,0x20,0x20,0x3f,0x20,0x40},
	/*0427*/{0x08,0xf8,0x08,0x00,0x00,0x08,0xf8,0x08,0x00,0x00,0x00,0x01,0x01,0x01,0x21,0x3f,0x20,0x00},
	/*0428*/{0x08,0xf8,0x00,0x08,0xf8,0x08,0x00,0xf8,0x08,0x20,0x3f,0x20,0x20,0x3f,0x20,0x20,0x3f,0x20},
	/*0429*/{0x08,0xf8,0x00,0x08,0xf8,0x08,0x00,0xf8,0x08,0x20,0x3f,0x20,0x20,0x3f,0x20,0x20,0x3f,0x60},
	/*042A*/{0x18,0x08,0xf8,0x88,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x3f,0x20,0x20,0x20,0x20,0x31,0x1e},
	/*042B*/{0x08,0xf8,0x00,0x80,0x80,0x00,0x08,0xf8,0x08,0x00,0x3f,0x11,0x20,0x20,0x1f,0x20,0x3f,0x20},
	/*042C*/{0x08,0xf8,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x3f,0x11,0x20,0x20,0x20,0x11,0x1f,0x00},
	/*042D*/{0x38,0x08,0x08,0x08,0x08,0x08,0x10,0xe0,0x00,0x10,0x30,0x20,0x21,0x21,0x21,0x11,0x0f,0x00},
	/*042E*/{0x08,0xf8,0x08,0xc0,0x30,0x08,0x08,0x30,0xc0,0x20,0x3f,0x21,0x07,0x18,0x20,0x20,0x18,0x07},
	/*042F*/{0x00,0x00,0x70,0x88,0x88,0x88,0x88,0xf8,0x08,0x00,0x20,0x30,0x0c,0x03,0x00,0x20,0x3f,0x20},
	/*0430*/{0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3f,0x20,0x00},
	/*0431*/{0x00,0xc0,0x20,0x90,0x90,0x90,0x08,0x00,0x00,0x00,0x0f,0x11,0x20,0x20,0x20,0x11,0x0e,0x00},
	/*0432*/{0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x20,0x3f,0x24,0x24,0x24,0x27,0x18,0x00},
	/*0433*/{0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x20,0x3f,0x20,0x00,0x00,0x00,0x01,0x00,0x00},
	/*0434*/{0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x18,0x14,0x13,0x10,0x10,0x1f,0x10,0x20},
	/*0435*/{0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x0f,0x13,0x22,0x22,0x22,0x22,0x13,0x00},
	/*0436*/{0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x80,0x80,0x20,0x31,0x0a,0x24,0x3f,0x24,0x0a,0x31,0x20},
	/*0437*/{0x00,0x80,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1b,0x31,0x20,0x24,0x24,0x24,0x1b,0x00},
	/*0438*/{0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x20,0x3f,0x10,0x0c,0x02,0x21,0x3f,0x20,0x00},
	/*0439*/{0x80,0x80,0x90,0x20,0x20,0x90,0x80,0x80,0x00,0x20,0x3f,0x10,0x0c,0x02,0x21,0x3f,0x20,0x00},
	/*043A*/{0x80,0x80,0x80,0x00,0x00,0x80,0x00,0x00,0x00,0x20,0x3f,0x24,0x0a,0x11,0x20,0x21,0x18,0x00},
	/*043B*/{0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x40,0x3f,0x00,0x00,0x20,0x3f,0x20},
	/*043C*/{0x80,0x80,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x3f,0x01,0x06,0x38,0x0c,0x03,0x00,0x3f,0x00},
	/*043D*/{0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x20,0x3f,0x24,0x04,0x04,0x24,0x3f,0x20,0x00},
	/*043E*/{0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x0e,0x11,0x20,0x20,0x20,0x11,0x0e,0x00},
	/*043F*/{0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x3f,0x20,0x00,0x00,0x20,0x3f,0x20,0x00},
	/*0440*/{0x80,0x80,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x80,0xff,0x91,0x20,0x20,0x20,0x11,0x0f,0x00},
	/*0441*/{0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x0e,0x11,0x20,0x20,0x20,0x20,0x11,0x00},
	/*0442*/{0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x01,0x00,0x20,0x3f,0x20,0x00,0x01,0x00},
	/*0443*/{0x00,0x40,0xc0,0x40,0x00,0x40,0xc0,0x40,0x00,0x00,0x00,0x80,0xc7,0x78,0x07,0x00,0x00,0x00},
	/*0444*/{0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x0e,0x11,0x11,0x3f,0x11,0x11,0x0e,0x00},
	/*0445*/{0x00,0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x31,0x3b,0x04,0x2b,0x31,0x20,0x00},
	/*0446*/{0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x20,0x3f,0x20,0x20,0x20,0x20,0x3f,0x40,0x00},
	/*0447*/{0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x03,0x04,0x04,0x04,0x24,0x3f,0x20,0x00},
	/*0448*/{0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x80,0x80,0x20,0x3f,0x20,0x20,0x3f,0x20,0x20,0x3f,0x20},
	/*0449*/{0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x80,0x80,0x20,0x3f,0x20,0x20,0x3f,0x20,0x20,0x3f,0xc0},
	/*044A*/{0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x01,0x00,0x20,0x3f,0x24,0x24,0x24,0x18,0x00},
	/*044B*/{0x80,0x80,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x20,0x3f,0x24,0x24,0x24,0x18,0x20,0x3f,0x20},
	/*044C*/{0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x3f,0x24,0x24,0x24,0x24,0x18,0x00},
	/*044D*/{0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x10,0x31,0x20,0x20,0x24,0x24,0x15,0x0e,0x00},
	/*044E*/{0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x00,0x00,0x20,0x3f,0x24,0x0e,0x11,0x20,0x20,0x11,0x0e},
	/*044F*/{0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x23,0x14,0x0c,0x04,0x24,0x3f,0x20},
};

// Special
const uint8 FontRussionSpecial9x16[][18] = {
	/*0415+0308*/{0x08,0xf8,0x8a,0x88,0x88,0xea,0x08,0x10,0x00,0x20,0x3f,0x20,0x20,0x20,0x23,0x30,0x08,0x00},
	/*0418+0306*/{0xf8,0xf8,0x01,0x02,0xc2,0x71,0xf8,0xf8,0x00,0x3f,0x3f,0x0c,0x07,0x01,0x00,0x3f,0x3f,0x00},
	/*0419+0306*/{0xf8,0xf9,0x02,0x02,0xc1,0x72,0xfa,0xf9,0x00,0x3f,0x3f,0x0c,0x07,0x01,0x00,0x3f,0x3f,0x00},
	/*0435+0308*/{0x00,0x00,0x00,0xa0,0x80,0xa0,0x80,0x00,0x00,0x00,0x0f,0x13,0x22,0x22,0x22,0x22,0x13,0x00},
	/*0438+0306*/{0x80,0x80,0x10,0x20,0x20,0x10,0x80,0x80,0x00,0x3f,0x3f,0x18,0x0c,0x06,0x03,0x3f,0x3f,0x00},
	/*0439+0306*/{0x80,0x90,0x20,0x20,0x10,0x20,0xa0,0x90,0x00,0x3f,0x3f,0x18,0x0c,0x06,0x03,0x3f,0x3f,0x00},
	/*039B*/	 {0x00,0x00,0x00,0xe0,0x18,0xe0,0x00,0x00,0x00,0x20,0x38,0x27,0x00,0x00,0x00,0x27,0x3c,0x20},
};
#endif

#ifdef FONT_CHINESE
const uint16 Gb2312_AddrTable[83]={	
/*0x4Exx*/0x0000,/*0x4Fxx*/0x0258,	
/*0x50xx*/0x0438,/*0x51xx*/0x0530,/*0x52xx*/0x06c4,/*0x53xx*/0x0848,/*0x54xx*/0x0a44,/*0x55xx*/0x0c94,/*0x56xx*/0x0e40,/*0x57xx*/0x0f68,
/*0x58xx*/0x10f8,/*0x59xx*/0x11cc,/*0x5Axx*/0x1360,/*0x5Bxx*/0x145c,/*0x5Cxx*/0x15e4,/*0x5Dxx*/0x1768,/*0x5Exx*/0x1848,/*0x5Fxx*/0x19c4,
/*0x60xx*/0x1b7c,/*0x61xx*/0x1d54,/*0x62xx*/0x1e20,/*0x63xx*/0x2040,/*0x64xx*/0x21ec,/*0x65xx*/0x2314,/*0x66xx*/0x2474,/*0x67xx*/0x25b4,
/*0x68xx*/0x2774,/*0x69xx*/0x28e0,/*0x6Axx*/0x29e8,/*0x6Bxx*/0x2a6c,/*0x6Cxx*/0x2b88,/*0x6Dxx*/0x2d98,/*0x6Exx*/0x2f58,/*0x6Fxx*/0x30a8,
/*0x70xx*/0x3188,/*0x71xx*/0x3294,/*0x72xx*/0x3364,/*0x73xx*/0x34bc,/*0x74xx*/0x35e0,/*0x75xx*/0x36dc,/*0x76xx*/0x3894,/*0x77xx*/0x39f8,
/*0x78xx*/0x3b48,/*0x79xx*/0x3cac,/*0x7Axx*/0x3dcc,/*0x7Bxx*/0x3ee4,/*0x7Cxx*/0x4058,/*0x7Dxx*/0x4144,/*0x7Exx*/0x416c,/*0x7Fxx*/0x42e4,
/*0x80xx*/0x44b4,/*0x81xx*/0x46a4,/*0x82xx*/0x47f8,/*0x83xx*/0x49f0,/*0x84xx*/0x4bd0,/*0x85xx*/0x4d0c,/*0x86xx*/0x4dec,/*0x87xx*/0x4f30,
/*0x88xx*/0x5070,/*0x89xx*/0x51a4,/*0x8Axx*/0x5270,/*0x8Bxx*/0x5290,/*0x8Cxx*/0x5408,/*0x8Dxx*/0x5538,/*0x8Exx*/0x572c,/*0x8Fxx*/0x5814,
/*0x90xx*/0x599c,/*0x91xx*/0x5b6c,/*0x92xx*/0x5c98,/*0x93xx*/0x5ca8,/*0x94xx*/0x5cbc,/*0x95xx*/0x5e88,/*0x96xx*/0x6088,/*0x97xx*/0x624c,
/*0x98xx*/0x633c,/*0x99xx*/0x6420,/*0x9Axx*/0x64e8,/*0x9Bxx*/0x6618,/*0x9Cxx*/0x6670,/*0x9Dxx*/0x678c,/*0x9Exx*/0x678c,/*0x9Fxx*/0x691c,
/*0xA0xx*/0x69ac
};
#endif

#ifdef FONT_JAPANESE

#endif
