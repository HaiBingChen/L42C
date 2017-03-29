#ifndef _TURNUP_RECOG_H_
#define _TURNUP_RECOG_H_

//#define _PC_DEBUG_ALG_  //PC ģ��

#ifdef _PC_DEBUG_ALG_
#define _PC_DEBUG_WMY_
#endif


/*****************************************************************************
* ���Ͷ���
*****************************************************************************/

typedef signed   char   int8;     //!< Signed 8 bit integer
typedef unsigned char   uint8;    //!< Unsigned 8 bit integer

typedef signed   short  int16;    //!< Signed 16 bit integer
typedef unsigned short  uint16;   //!< Unsigned 16 bit integer

typedef signed   long   int32;    //!< Signed 32 bit integer
typedef unsigned long   uint32;   //!< Unsigned 32 bit integer


typedef enum
{
	GT_DEFAULT = 0,			//��̧��
	GT_HAND_UP = 1,			//̧��
	GT_HAND_DOWN = 2,		//����
	GT_HAND_ROTATION = 3	//ת��
}GESTURE_TYPE;


/*****************************************************************************
* ����ӿ�
*****************************************************************************/

void vGesture_Recog_Init_ALG(void); //̧��ת��������ʼ������

void vGesture_Recog_ALG(int16 iacc[3]);//̧��ת���⺯��

uint8 ucGetGestureType_ALG(void);//��ȡ̧��ת���ʶ����̧��--0��̧��--1������--2��ת��--3��


#endif
