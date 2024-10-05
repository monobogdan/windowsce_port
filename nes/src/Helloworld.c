#include "mrc_base.h"
#include "mrc_win.h"

#include "Helloworld.h"

void RUN_WaveNES(void);
int8 KEY1,KEY2,KEY3,KEY4,KEY5,KEY6,KEY7,KEY8;

int32 mrc_init(void)
{
	mrc_LCDCanSleep(0);
	KEY1=KEY2=KEY3=KEY4=KEY5=KEY6=KEY7=KEY8=0;
    RUN_WaveNES();
	return MR_SUCCESS;
}

int32 mrc_appEvent(int32 code, int32 p0, int32 param1)
{
		if(MR_KEY_PRESS == code)
	{
		switch(p0)
		{
		case MR_KEY_SOFTRIGHT:
			
			mrc_exit();
			break;
		case MR_KEY_SELECT:
			 KEY5=1;
			 break;
        case MR_KEY_SOFTLEFT:  
			 KEY6=1;
			 break;
		case MR_KEY_LEFT:
			 KEY2=1;
			 break;
		case MR_KEY_RIGHT:
			 KEY1=1;
			 break;
		case MR_KEY_UP:
             KEY4=1;
			 break;
        case MR_KEY_DOWN:
			 KEY3=1;
			 break;
		case MR_KEY_1:
			 KEY7=1;
			 break;
		case MR_KEY_2:
			 KEY8=1;
			 break;
		}
	}
	else 
	{
      KEY1=KEY2=KEY3=KEY4=KEY5=KEY6=KEY7=KEY8=0;
	}
	return MR_SUCCESS;
}


int32 mrc_pause(void)
{
	return 0;
}

int32 mrc_resume(void)
{
	
	return 0;
}

int32 mrc_exitApp(void)
{
	return 0;
}
int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1)
{
return 0;
}
int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4,int32 p5)
{
return 0;
}