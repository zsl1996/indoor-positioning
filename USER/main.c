#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "string.h"
#include "ov7670.h"
#include "tpad.h"
#include "timer.h"
#include "exti.h"
#include "usmart.h

u16 color;
const u8*LMODE_TBL[5] = { "Auto","Sunny","Cloudy","Office","Home" };							//5种光照模式	    
const u8*EFFECTS_TBL[7] = { "Normal","Negative","B&W","Redish","Greenish","Bluish","Antique" };	//7种特效 
int XX2[17] = { 0,4,8,18,36,57,71,89,107,125,140,164,178,194,215,224,240 };
int YY2[17] = { 21,41,61,73,90,107,126,143,160,180,197,213,230,248,266,283,300 };
int XX3[17] = { 55,69,86,100,118,141,156,173,191,207,220,232,236,237,238,239,240 };
int YY3[17] = { 33,50,67,76,96,112,129,148,163,183,199,217,234,250,269,286,303 };
u8 XY1[8], XY2[8], XY3[8];
u8 xy[14];
extern u8 ov_sta;	//在exit.c里 面定义
extern u8 ov_frame;	//在timer.c里面定义		 
int x1 = 0, y1 = 0, n1 = 0, x2 = 0, y2 = 0, n2 = 0, x3 = 0, y3 = 0, n3 = 0, flagXY = 0;
int hang = 0, line = 0;

int zhaozhi(int a[], int x) {
	int i = 0;
	for (i = 0; i <= 16; i++) {
		if ((a[i] <= x && a[i + 1] >= x) || (a[i] >= x && a[i + 1] <= x)) {
			return i;
		}
	}
	return i;
}

void xian(u8 a[], int x, int y, int b[], int c[]) {
	int i = 0, j = 0;
	float z = x, k = y;
	i = zhaozhi(b, x);
	j = zhaozhi(c, y);
	z = i + (b[i] - z) / (b[i] - b[i + 1]);
	k = j + (k - c[j]) / (c[j + 1] - c[j]);
	z = z * 5 - 40;
	k = k * 5 - 40;
	a[11] = 32;
	if (z <= 20 && z >= -20 && k <= 20 && k >= -20) {
		a[12] = 65;
	}
	if (z > 20 && z >= k && z + k >= 0) {
		a[12] = 67;
	}
	if (z < -20 && z < k&&z + k < 0) {
		a[12] = 69;
	}
	if (k > 20 && z < k&&z + k >= 0) {
		a[12] = 66;
	}
	if (k < -20 && z >= k && z + k < 0) {
		a[12] = 68;
	}

	if (z < 0) {
		a[0] = 45;
		z = -z;
	}
	else a[0] = 32;
	x = 10 * z;
	i = x / 100;
	a[1] = i + 48;
	x = x - 100 * i;
	i = x / 10;
	a[2] = i + 48;
	x = x - 10 * i;
	i = x;
	a[3] = 46;
	a[4] = i + 48;

	a[5] = 32;

	if (k < 0) {
		a[6] = 45;
		k = -k;
	}
	else a[6] = 32;
	x = 10 * k;
	i = x / 100;
	a[7] = i + 48;
	x = x - 100 * i;
	i = x / 10;
	a[8] = i + 48;
	x = x - 10 * i;
	i = x;
	a[9] = 46;
	a[10] = i + 48;
}

void xianshi(u8 a[], int x, int y) {
	int i = 0;
	i = x / 100;
	a[0] = i + 48;
	x = x - 100 * i;
	i = x / 10;
	a[1] = i + 48;
	x = x - 10 * i;
	i = x;
	a[2] = i + 48;

	a[3] = 32;

	i = y / 100;
	a[4] = i + 48;
	y = y - 100 * i;
	i = y / 10;
	a[5] = i + 48;
	y = y - 10 * i;
	i = y;
	a[6] = i + 48;
}

void clearXY(void) {
	x1 = 0; y1 = 0; n1 = 0; x2 = 0; y2 = 0; n2 = 0; x3 = 0; y3 = 0; n3 = 0; flagXY = 0; hang = 0; line = 0;
}

int ASpoint(int x, int y, int m, int n) {
	if (x <= m + 40 && x >= m - 40 && y <= n + 40 && y >= n - 40) {
		return 1;
	}
	else return 0;
}
void camera_refresh(void)
{
	u32 j;

	if (ov_sta == 2)//有帧中断更新？
	{
		clearXY();
		LCD_Scan_Dir(U2D_L2R);		//从上到下,从左到右  
		LCD_WriteRAM_Prepare();     //开始写入GRAM	
		OV7670_RRST = 0;				//开始复位读指针 
		OV7670_RCK_L;
		OV7670_RCK_H;
		OV7670_RCK_L;
		OV7670_RRST = 1;				//复位读指针结束 
		OV7670_RCK_H;
		for (j = 0; j < 240 * 320; j++)
		{
			OV7670_RCK_L;
			color = GPIOC->IDR & 0XFF;	//读数据
			OV7670_RCK_H;
			color <<= 8;
			OV7670_RCK_L;
			color |= GPIOC->IDR & 0XFF;	//读数据
			OV7670_RCK_H;
			LCD->LCD_RAM = color;
			if (hang == 320) {
				line++;
				hang = 0;
			}
			if (color == 0xFFFF) {
				if (flagXY == 0) {
					flagXY = 1;
					x1 = line;
					y1 = hang;
					n1++;
				}
				if (flagXY == 1) {
					if (ASpoint(x1 / n1, y1 / n1, line, hang)) {
						x1 = x1 + line;
						y1 = y1 + hang;
						n1++;
					}
					else {
						flagXY = 2;
						x2 = line;
						y2 = hang;
						n2 = 1;
					}
				}
				else {
					if (flagXY == 2) {
						if (ASpoint(x1 / n1, y1 / n1, line, hang)) {
							x1 = x1 + line;
							y1 = y1 + hang;
							n1++;
						}
						else {
							if (ASpoint(x2 / n2, y2 / n2, line, hang)) {
								x2 = x2 + line;
								y2 = y2 + hang;
								n2++;
							}
							else {
								flagXY = 3;
								x3 = line;
								y3 = hang;
								n3 = 1;
							}
						}
					}
					else {
						if (flagXY == 3) {
							if (ASpoint(x1 / n1, y1 / n1, line, hang)) {
								x1 = x1 + line;
								y1 = y1 + hang;
								n1++;
							}
							else {
								if (ASpoint(x2 / n2, y2 / n2, line, hang)) {
									x2 = x2 + line;
									y2 = y2 + hang;
									n2++;
								}
								else {
									if (ASpoint(x3 / n3, y3 / n3, line, hang)) {
										x3 = x3 + line;
										y3 = y3 + hang;
										n3++;
									}
								}
							}
						}
					}
				}
			}
			hang++;
		}
		if (flagXY >= 1) LCD_Draw_Circle(y1 / n1, x1 / n1, 5);
		if (flagXY >= 2) LCD_Draw_Circle(y2 / n2, x2 / n2, 7);
		if (flagXY == 3) LCD_Draw_Circle(y3 / n3, x3 / n3, 9);


		ov_sta = 0;					//清零帧中断标记
		ov_frame++;
		LCD_Scan_Dir(DFT_SCAN_DIR);	//恢复默认扫描方向 
	}
}


int main(void)
{
	u8 key;
	u8 lightmode = 0, saturation = 2, brightness = 2, contrast = 2;
	u8 effect = 0;
	u8 i = 0;
	u8 msgbuf[15];				//消息缓存区
	u8 tm = 0;

	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级

	LCD_Init();		 //初始化LCD  


	POINT_COLOR = RED;			//设置字体为红色 
	LCD_ShowString(30, 50, 200, 16, 16, "WarShip STM32");
	LCD_ShowString(30, 70, 200, 16, 16, "OV7670 TEST");
	LCD_ShowString(30, 90, 200, 16, 16, "ATOM@ALIENTEK");
	LCD_ShowString(30, 110, 200, 16, 16, "2015/1/18");
	LCD_ShowString(30, 130, 200, 16, 16, "KEY0:Light Mode");
	LCD_ShowString(30, 150, 200, 16, 16, "KEY1:Saturation");
	LCD_ShowString(30, 170, 200, 16, 16, "KEY2:Brightness");
	LCD_ShowString(30, 190, 200, 16, 16, "KEY_UP:Contrast");
	LCD_ShowString(30, 210, 200, 16, 16, "TPAD:Effects");
	LCD_ShowString(30, 230, 200, 16, 16, "OV7670 Init...");

	while (OV7670_Init())//初始化OV7670
	{
		LCD_ShowString(30, 230, 200, 16, 16, "OV7670 Error!!");
		delay_ms(200);
		LCD_Fill(30, 230, 239, 246, WHITE);
		delay_ms(200);
	}
	LCD_ShowString(30, 230, 200, 16, 16, "OV7670 Init OK");
	delay_ms(1500);
	OV7670_Light_Mode(lightmode);
	OV7670_Color_Saturation(saturation);
	OV7670_Brightness(brightness);
	OV7670_Contrast(contrast);
	OV7670_Special_Effects(0);
	//	TIM6_Int_Init(10000,7199);			//10Khz计数频率,1秒钟中断									  
	EXTI8_Init();						//使能定时器捕获
	OV7670_Window_Set(10, 176, 240, 320);	//设置窗口	  
	OV7670_CS = 0;
	LCD_Clear(BLACK);
	while (1)
	{

		camera_refresh();//更新显示
		if (flagXY >= 1)
		{
			xianshi(XY1, x1 / n1, y1 / n1);
			LCD_ShowString(10, 260, 200, 16, 16, XY1);
		}
		if (flagXY >= 2)
		{
			xianshi(XY2, x2 / n2, y2 / n2);
			LCD_ShowString(10, 280, 200, 16, 16, XY2);
		}
		if (flagXY >= 3)
		{
			xianshi(XY3, x3 / n3, y3 / n3);
			LCD_ShowString(10, 300, 200, 16, 16, XY3);
		}
		if (flagXY == 2 && x1 / n1 <= 120 && ((x2 / n2 - x1 / n1) < 90) && (x1 / n1 >= 20)) xian(xy, x1 / n1, y1 / n1, XX2, YY2);
		else {
			if ((flagXY <= 1)) {
				xian(xy, x1 / n1, y1 / n1, XX3, YY3);
				xy[0] = 45;
				xy[1] = 3 + 48;
				xy[2] = 7 + 48;
				xy[3] = 46;
				xy[4] = 5 + 48;
			}
			else {
				if (flagXY == 2 && x1 / n1 <= 120 && ((x2 / n2 - x1 / n1) < 90) && (x1 / n1 < 20)) {
					xian(xy, x2 / n2, y2 / n2, XX3, YY3);
				}
				else
					xian(xy, x2 / n2, y2 / n2, XX2, YY2);
			}
		}
		LCD_ShowString(120, 300, 200, 16, 16, xy);

		delay_ms(200);

	}
}













