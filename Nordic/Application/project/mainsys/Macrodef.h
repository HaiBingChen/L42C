#include "Typedef.h"


#define RTC_TIMEOUT (1)
#define DISPLAY_WATCH_TIME_TYPE (0)

typedef union 
{
  uint32 u32PMD;
  struct {
	uint32 PMD0:1;
	uint32 PMD1:1;
	uint32 PMD2:1;
	uint32 PMD3:1;
	uint32 PMD4:1;
	uint32 PMD5:1;
	uint32 PMD6:1;
	uint32 PMD7:1;
	uint32 PMD8:1;
	uint32 PMD9:1;
	uint32 PMD10:1;
	uint32 PMD11:1;
	uint32 PMD12:1;
	uint32 PMD13:1;
	uint32 PMD14:1;
	uint32 PMD15:1;
	uint32 PMD16:1;
	uint32 PMD17:1;
	uint32 PMD18:1;
	uint32 PMD19:1;
	uint32 PMD20:1; 
	uint32 PMD21:1; 
	uint32 PMD22:1;
	uint32 PMD23:1;
	uint32 PMD24:1;
	uint32 PMD25:1;
	uint32 PMD26:1;
	uint32 PMD27:1;
	uint32 PMD28:1;
	uint32 PMD29:1;
	uint32 PMD30:1;
	uint32 PMD31:1;
  } PMD;
}DATA_CONFIG_T;  

//协议函数结构
typedef uint8 (*pHandlerFn)(uint8 *);
typedef struct
{
  uint8 DataCmd;
  pHandlerFn pFn;
} DATA_CMD_T;

//时间界面设置结构体
typedef struct 
{
  uint8	DateFormat; 
  uint8 	TimeFormat; 
  uint8 	BatFormat; 
  uint8 	LunarFormat; 
  uint8 	RotateFormat; 
  uint8	BackgroundStyle; 
  uint8 	HeartRateFormat; 
  uint8 	UserNameFormat; 
}TIME_ORDER;

//设置值
typedef struct 
{
  uint8 	Backlight;
  uint8 	Volume;
  uint8	Language;
  uint8     DistanceFormat;
  uint8     UserHabit;
}SET_VALUE;

//用户信息
typedef struct 
{
  uint8 UserSex;      //男/女？
  uint8 UserAge;      //岁数
  uint8 UserHeight;   //身高 1cm  
  uint16 UserWeight;  //体重 10g
}USER_INFO_DATA_T;


//目标设置
typedef struct 
{
  uint16	Goal;
  uint8 	MainGoalFlag;
}DAILY_GOAL;

//其他信息提醒
typedef enum 
{
	NOTICE_MISSEDCALL=0x00,
	NOTICE_SMS,
	NOTICE_SOCIAL,
	NOTICE_EMAIL,	
	NOTICE_SCHEDULEL,
	NOTICE_INCOMING_CALL,
	NOTICE_CALL_END	
}NOTICE_E;

//提醒事件结构体
#define NOTE_SIZE	9
typedef struct
{
  uint8 Id;
  uint8 Hour;
  uint8 Minute;
  uint8 RemindTimes;
  uint8 RdSwitch;
} REMIND_DATA_T;

typedef struct
{
  REMIND_DATA_T RD;
  uint8 SecondRemind;
  uint8 page;
} REMIND_T;


//运动数据结构体
typedef struct 
{
  uint16 SportIdx;
  uint32 SportTimeStamp;
  uint32 SportSteps;
  uint32 SportKcal;
  uint32 SportDistance;
  uint32 SportDuration;
}SPORT_DATA_T;

//运动数据结构体
typedef struct 
{
  uint16 Idx;
  uint32 TimeStamp;
  uint32 Steps;
  uint32 Kcal;
  uint32 Distance;
  uint32 Duration;
  uint32 Hr;
}REC_WORK_DATA_T;

typedef struct
{ 
  uint16  SleepIdx;
  uint32  SleepTimeStamp;   //时间戳
  uint8    SleepType;         //类型
}SLEEP_DATA_T;

typedef struct
{
  uint8 BeginHour;
  uint8 BeginMinute;
  uint8 EndHour;
  uint8 EndMinute;
  uint8 SleepCycle;
}AUTO_SLEEP_DATA_T;

typedef struct
{
	uint16 HeartRateIdx;
	uint32 HeartRateTimeStamp;
	uint8  HeartRateVal;
}HEARTRATE_DATA_T;

//接收命令列表
typedef enum 
{
	BT_CLOSE=0x00,
	BT_OPEN=0x01,	
	BT_CONNECTED=0x02,
	BT_DISCONNECT=0x03,
}BT_STATE_E;


#define MAX_MONTH                        12
#define MAX_HOUR_IN_24H             23
#define MAX_MINUTE                       59
#define MAX_SECOND                       59

#define VALID_YEAR_MIN			2012
#define VALID_YEAR_MAX			2050


//响应枚举
typedef enum 
{
      E_RSP_OK=0x00,
      E_RSP_FAIL=0x01,
      E_RSP_PARSE_ERROR=0x02,
      E_RSP_NULL=0xFF
}RSP_VAULE_E;


//band时间结构体
typedef struct 
{
  uint16   Year;  //小端表示
  uint8	Month;
  uint8	Day;
  uint8	Hour;
  uint8	Minute;
  uint8	Second;
}BAND_TIME_MSG_DATA_T;


typedef struct 
{
  uint8	SrSwitch;
  uint8	SrDayFrequency;
  uint8	SrFrequency;
  uint8     SrBeginHour;
  uint8 	SrBeginMinute;
  uint8     SrEndHour;
  uint8     SrEndMinute;
  uint8	SrStepThresholdL;
  uint8	SrStepThresholdH;
}SIT_REMIAND_DATA_T;


typedef enum
{
  REMIND_EAT=0x00,
  REMIND_MEDICINE,
  REMIND_DRINKING,
  REMIND_SLEEP,
  REMIND_WAKEUP,
  REMIND_MOVING,
  REMIND_MEETING,
  REMIND_USER
}REMIND_TYPE_T;


//incoming call notify struct
typedef struct 
{
  uint8     name_type;
  uint8	name_len;
}INCALL_DATA_T;

typedef struct
{
  uint16 x; 
  uint16 y; 
  uint16 z;
} SENSOR_DATA_T;

typedef struct
{
uint8   TitleLen; 
uint8   MessageLen; 
uint8   DateLen;
} ANCS_SMS_HEADER;

typedef enum 
{
    USB_PINS=0x00,
    BC5_PINS,
    KL17_PINS
}UartPins;

typedef uint8 (*pBc5HandlerFn)(uint8 *,uint8);
typedef struct
{
  uint8 Bc5Cmd;
  pBc5HandlerFn pFn;
} BC5_DATA_CMD_T;

typedef enum
{
	SENSOR_NULL=0x00,
	SENSOR_LIS3DH,
	SENSOR_BMA250E,
}Gsensor;

typedef struct
{
    uint16_t day;      // 0-30
    uint16_t month;    // 0-11
    uint16_t year;    // 2000+
    uint16_t total;
    uint16_t Awake;
    uint16_t Light;
    uint16_t Deep;	
} SleepMapData;

typedef enum
{
	Rw_Ready=0x00,
	Rw_go,
	Rw_TimerRun,
	Rw_TimerStop,
}RecordWork;
