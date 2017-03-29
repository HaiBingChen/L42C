#ifndef _TURNUP_RECOG_H_
#define _TURNUP_RECOG_H_

//#define _PC_DEBUG_ALG_  //PC 模拟

#ifdef _PC_DEBUG_ALG_
#define _PC_DEBUG_WMY_
#endif


/*****************************************************************************
* 类型定义
*****************************************************************************/

typedef signed   char   int8;     //!< Signed 8 bit integer
typedef unsigned char   uint8;    //!< Unsigned 8 bit integer

typedef signed   short  int16;    //!< Signed 16 bit integer
typedef unsigned short  uint16;   //!< Unsigned 16 bit integer

typedef signed   long   int32;    //!< Signed 32 bit integer
typedef unsigned long   uint32;   //!< Unsigned 32 bit integer


typedef enum
{
	GT_DEFAULT = 0,			//无抬手
	GT_HAND_UP = 1,			//抬手
	GT_HAND_DOWN = 2,		//放手
	GT_HAND_ROTATION = 3	//转腕
}GESTURE_TYPE;


/*****************************************************************************
* 对外接口
*****************************************************************************/

void vGesture_Recog_Init_ALG(void); //抬手转腕亮屏初始化函数

void vGesture_Recog_ALG(int16 iacc[3]);//抬手转腕监测函数

uint8 ucGetGestureType_ALG(void);//获取抬手转腕标识，无抬手--0；抬手--1；放手--2，转腕--3；


#endif
