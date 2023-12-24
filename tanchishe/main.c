#include <REGX52.H>
#include <stdlib.h>
#include "Delay.h"
#include "MatrixLED.h"
#include "MatrixKey.h"
#include "Timer0.h"

unsigned char Direction,Length,KeyNum,Speed,Score;
char Snake_x[11] = {0};//蛇身x坐标,理想状态是64，但单片机RAM不够，所以少设一些
char Snake_y[11] = {0};//蛇身y坐标
unsigned char code x[] = {0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};
unsigned char code y[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
unsigned char Food_x;//食物x坐标
unsigned char Food_y;//食物y坐标


unsigned char Independent_Key() //四个独立按键选择模式
{
	static unsigned char KeyNumber_0;
	if(P3_1==0){KeyNumber_0=1;} //简单模式
	if(P3_0==0){KeyNumber_0=2;} //普通模式
	if(P3_2==0){KeyNumber_0=3;} //困难模式
	if(P3_3==0){KeyNumber_0=4;} //娱乐模式
	return KeyNumber_0;
}


void Snake_move()//蛇身移动
{
	unsigned char i;
  for (i = Length + 1; i > 0;i--)
  {
      Snake_x[i] = Snake_x[i - 1];//下一次的x坐标为上一次的x坐标，以实现向前挪动
      Snake_y[i] = Snake_y[i - 1];//同上
  }
	switch(Direction)//方向：1:上2:下3:左4:右
	{
		case 1:(Snake_y[0])--;break;
		case 2:(Snake_y[0])++;break;
		case 3:(Snake_x[0])--;break;
		case 4:(Snake_x[0])++;break;
	}
}


void Direction_key()//检测方向键，控制方向
{
	if(KeyNum)
	{
		switch(KeyNum){
			case 1:if(Direction != 1)Direction = 2;break;//如果方向不是上，也就是方向为左或右，那么朝下
			case 2:if(Direction != 2)Direction = 1;break;//同上
			case 3:if(Direction != 3)Direction = 4;break;//同上
			case 4:if(Direction != 4)Direction = 3;break;//同上
		}			
	}
}


void Food_coordinate()//食物坐标
{	
	Food_x = rand() % 8;
	Food_y = rand() % 8;
}


void Refresh() //刷新蛇和食物的位置
{
	unsigned char i;
	for(i = 0;i < Length;i++)
		{
			MatrixLED_Init();
			MatrixLED_ShowColumn(x[Snake_x[i]],y[Snake_y[i]]);
			MatrixLED_Init();
			MatrixLED_ShowColumn(x[Food_x],y[Food_y]);
		}
}


void Eat_food() //判断是否吃到食物
{
	unsigned char i;
	for(i = 0;i < Length;i++)
	{
		if(Food_x == Snake_x[i] && Food_y == Snake_y[i])
		{
			Length++;
			Food_coordinate();
			Score++;
		}
	}
}


void Game_over() //游戏结束显示效果
{
	unsigned char code Animation[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x7E,0x81,0x81,0x89,0x89,0x4E,0x00,0x1F,
0x68,0x88,0x68,0x1F,0x00,0xFF,0x40,0x20,
0x20,0x40,0xFF,0x00,0xFF,0x91,0x91,0x91,
0x91,0x00,0x00,0x7E,0x81,0x81,0x81,0x81,
0x7E,0x00,0xF0,0x0E,0x01,0x0E,0xF0,0x00,
0xFF,0x91,0x91,0x91,0x91,0x00,0x7F,0x88,
0x8C,0x8A,0x71,0x00,0x00,0x00,0x00,0xF2,
0x97,0x92,0x02,0xF2,0x12,0x12,0x02,0xF2,
0x02,0xF2,0x92,0x92,0x02,0xF2,0x42,0xB2,
0x02,0x02,0x02,0xF2,0x02,0x82,0xF2,0x82,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
	unsigned char i,Offset = 0,Count = 0;
	MatrixLED_Init();
	while(1)
	{
		for(i = 0;i < 8;i++)
		{
			MatrixLED_Show_Gameover(i,Animation[i + Offset]);
		}
		Count++;
		if(Count > 4)
		{
			Count = 0;
			Offset++;
			if(Offset > 88)
			{
				Offset = 0;
			}
		}
		if(Score==0){P2_0=1;}
		if(Score==1){P2_0=0;}
		if(Score==2){P2_0=0;P2_1=0;}
		if(Score==3){P2_0=0;P2_1=0;P2_2=0;}
		if(Score==4){P2_0=0;P2_1=0;P2_2=0;P2_3=0;}
		if(Score==5){P2_0=0;P2_1=0;P2_2=0;P2_3=0;P2_4=0;}
		if(Score==6){P2_0=0;P2_1=0;P2_2=0;P2_3=0;P2_4=0;P2_5=0;}
		if(Score==7){P2_0=0;P2_1=0;P2_2=0;P2_3=0;P2_4=0;P2_5=0;P2_6=0;}
		if(Score==8){P2_0=0;P2_1=0;P2_2=0;P2_3=0;P2_4=0;P2_5=0;P2_6=0;P2_7=0;}
	}		
}


void Snake_death() //判断死亡
{
	
	//撞自己
	unsigned char i;
	for(i = 4;i < Length;i++)
	{
		if(Snake_x[0] == Snake_x[i] && Snake_y[0] == Snake_y[i])
		{			
			Game_over();
		}
	}
	
	//撞墙
		if(Snake_x[0] < 0 || Snake_x[0] > 7 || Snake_y[0] < 0 || Snake_y[0] > 7)
		{
			Game_over();
		}
}


void main()
{
	Length = 2;
	Direction = 4;
	Score=-1;
	while(Independent_Key()==1) //简单模式
	{
			Timer0_Init();
			Speed=200;
			rand();
			KeyNum=Key();
			Direction_key();
			MatrixLED_Init();
			Refresh();
			Eat_food();
			Snake_death();
	}
	while(Independent_Key()==2) //普通模式
	{
			Timer0_Init();
			Speed=150;
			rand();
			KeyNum=Key();
			Direction_key();
			MatrixLED_Init();
			Refresh();
			Eat_food();
			Snake_death();
	}
	while(Independent_Key()==3) //困难模式
	{
			Timer0_Init();
			Speed=100;
			rand();
			KeyNum=Key();
			Direction_key();
			MatrixLED_Init();
			Refresh();
			Eat_food();
			Snake_death();
	}
	while(Independent_Key()==4) //娱乐模式
	{
			Timer0_Init();
			Speed=150;
			rand();
			KeyNum=Key();
			Direction_key();
			MatrixLED_Init();
			Refresh();
			Eat_food();
			Snake_death();
	}
}


void Routine_Timer0() interrupt 1
{
	static unsigned int T0_Count1,T0_Count2,T0_Count3;
	TL0 = 0x18;		
	TH0 = 0xFC;			
	//刷新按键
	T0_Count1++;
	if(T0_Count1 == 50)
	{
		T0_Count1 = 0;
		Circulate_MatrixKey();
	}
	//蛇移动的刷新速度
	T0_Count2++;
	if(T0_Count2 == Speed)
	{	
		T0_Count2 = 0;
		Snake_move();
	}
	//娱乐模式食物刷新
	if(Independent_Key() == 4)
	{
		T0_Count3++;
		if(T0_Count3 == 5000)
		{
			T0_Count3 = 0;		
			Food_coordinate();
			MatrixLED_ShowColumn(x[Food_x],y[Food_y]);		
		}
	}
}