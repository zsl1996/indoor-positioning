器材:
	战舰STM32F103，OV7670等摄像头.
	
硬件资源:
	1,DS0(连接在PB5)
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面)
	3,ALIENTEK 2.8/3.5/4.3/7寸TFTLCD模块(通过FSMC驱动,FSMC_NE4接LCD片选/A10接RS)
	4,按键KEY0(PE4)/KEY1(PE3)/KEY2(PE2)/KEY_UP(PA0,也称之为WK_UP)
	5,TPAD电容触摸按键(右下角LOGO,即TPAD,连接在PA1)
	6,外部中断8(PA8,用于检测OV7670的帧信号)
	7,定时器6(用于打印摄像头帧率)
	8,ALIENTEK OV7670摄像头模块,连接关系为:
		OV7670模块 ----------- STM32开发板
		 OV_D0~D7  -----------   PC0~7
		 OV_SCL    ------------  PD3
		 OV_SDA    ------------  PG13
		 OV_VSYNC  ------------  PA8
		 FIFO_RRST -----------   PG14
		 FIFO_OE   -----------   PG15
		 FIFO_WRST ------------  PD6
		 FIFO_WEN  ------------  PB3
		 FIFO_RCLK ------------  PB4	  
	可以借助USMART调试,调用SCCB_RD_Reg和SCCB_WR_Reg等函数,实现OV7670寄存器读写.

注意事项:
	初始需要在指定位置确认物体颜色特征





































