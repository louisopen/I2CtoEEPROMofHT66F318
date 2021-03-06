//___________________________________________________________________
//___________________________________________________________________
// Description: 系統初始化相關程序
//  Copyright@: 2019 BY Louis Huang / https://github.com/louisopen/
//   File Name: Sys_Init.c
//Targer Board: MK8002D
//    MCU Body: HT66F317 HT66F318-28ssop
//      Author: Louis Huang
//        Date: 2019/05/18
//     Version: V00 on Hardware V10
//     History:
//___________________________________________________________________
//___________________________________________________________________
#include "common.h"

unsigned char count_2sec;
volatile __byte_type	system_flag;

//___________________________________________________________________
//Function: Time base timer task process
//   INPUT: 
//	  NOTE:
//___________________________________________________________________
void Task_500ms()
{	//TimeBase0
	//if(TB0_int_flag==0) return;
	//TB0_int_flag=0;
	//TimeBase1
	if(TB1_int_flag==0) return;
	TB1_int_flag=0;
	
	count_2sec++;
	if(count_2sec < 4) return;
	count_2sec=0;
	if(toggle_led==0)
	{
		toggle_led=1;
		LED_slow=1;	//_pa0=1;
	}
	else
	{
		toggle_led=0;
		LED_slow=0;	//_pa0=0;
	}
}

//___________________________________________________________________
//Function: Key scan process
//   INPUT: 
//	  NOTE: 
//___________________________________________________________________
void Key_Scan()
{
	//GCC_NOP();
	_nop();
}	

//___________________________________________________________________
//Function: MCU WDT復位初始化
//   INPUT: 
//  OUTPUT: 
//	  NOTE: 
//___________________________________________________________________
void TimerInitial()
{
	//_tmpc = 0b00000000;			//binding IO output? (TP0 /TP1 /TP2)	
	
	//TimeModule0 can using STM 16bits(ht66f318) MuFunction0 ISR
	_tm0c0 = 0b00000000; 		//fsys/4
	_tm0c1 = 0xC1; 				//Compare with CCRA
	//_tm0al = 0x90; 				//CCRA比較值設置16bits(ht66f318) @HIRC8M
	//_tm0ah = 0x01;				//5KHz for Buzzer or other
	_tm0al = 0x58; 				//CCRA比較值設置16bits(ht66f318) @HIRC12M
	_tm0ah = 0x02;				//5KHz for Buzzer or other
	_tm0rp = 0x00;				//CCRP比較值只能設置高8bits(ht66f318)
	_t0ae = 1;					//interrupt for CCRA
	//_t0pe = 1;					//interrupt for CCRP
	_mf0e = 1;					//Multifunction 0 interrupt enable
	_t0on = 1;
	/*
	//TimeModule1 can using PTM 10bits (ht66f317,ht66f318) MuFunction1 ISR
	_tm1c0 = 0B00010000;  	//0001 0000	Pause, TM Clock = fsys, PT1ON=Off state
	_tm1c1 = 0B10101000;	//1010 1000 PWM_Mode, Active high, Non-invert,counter clear when comparator P match
	_tm1al = 0x0C;				//CCRA比較值設置10bits
	_tm1ah = 0x00;
	_tm1rpl = 0x30;				//CCRP有10bit ht66f317/ht66f318
	_tm1rph = 0x00;				//CCRPH只有2bit ht66f317/ht66f318
	//_t1ae = 1;					//interrupt for CCRA
	//_t1pe = 1;					//interrupt for CCRP
	//_mf1e = 1;					//Multifunction 1 interrupt enable
	_t1on = 1;				//_tm1c0.3
	_t1cp = 1;				//enable PA7 TP1(TMPC.1=1) binding TM1 
	*/	/*
	//TimeModule2 can using CTM 16bits(ht66f318) MuFunction1 ISR
	_tm2c0 = 0b00000000; 		//fsys/4
	_tm2c1 = 0xC1; 				//Compare with CCRA
	_tm2al = 0x48; 				//CCRA比較值設置16bits(ht66f318)
	_tm2ah = 0x01;				//3KHz for Buzzer
	_tm2rp = 0x00;				//CCRP比較值只能設置高8bits(ht66f318)
	_t2ae = 1;					//interrupt for CCRA
	//_t2pe = 1;					//interrupt for CCRP
	_mf1e = 1;					//Multifunction 1 interrupt enable
	_t2on = 1;
	*/
}

//___________________________________________________________________
//Function: MCU WDT復位初始化
//   INPUT: 
//  OUTPUT: 
//	  NOTE: 
//___________________________________________________________________
void WDT_ResetInit()	//WDT 溢出復位
{
 	//Uart_Init();		//UART port	
	//Enable_ADC();		//ADC convert 12bit
	TimeBaseInitial();	//TimerBase0/1 module
	
	TimerInitial();		//Timer n module
	
	init_i2c();			//I2C port	
  	_nop();
  	//_pgc0 = 0;
	//_pg0 = 0;
	//_pbs1 = 0b11111111;
	_emi  = 1;	//enable global interrupt
}
//___________________________________________________________________
//Function: MCU POWER UP
//   INPUT: 
//	  NOTE: 
//___________________________________________________________________
void PowerOn_Init() //power up or reset pin normal
{	
  	//SETHXT();		//fH source is external Hi speed
    Fsys_select();	//Option: Internal RC 8MHz
  	
	//LVR low voltage reset select
	//SETLVR_Voltage1_9();
	SETLVR_Voltage2_55();
	//SETLVR_Voltage3_15();

	//GPIO configurations
	GPIO_Init();

	//WDT select & initial
	SETWDTtime8192ms();		//WDT timer enable.
}
//___________________________________________________________________
//Function: Enter to HLAT mode
//   INPUT: 
//	  NOTE: 
//___________________________________________________________________
void ReadyToHalt()
{
	//Disable_ADC();	
	//Uart_off();
	i2c_off();			//I2C port	
	_t1on  = 0;
	//_regc = 0x00;
	//_fsiden = 1;
	_halt();
	//////////////////////Wake up/////////////////////////////
	//Uart_Init();			//UART port	
	//Enable_ADC();			//ADC port	
	init_i2c();				//I2C port	
}
//HALT
//SLEEPMode0	@(IDLEN==0  & LVDEN==Disable & WDT Disable )
//Stop:CPU,Fsub,Fs,Disable WDT,LVDEN must Disable

//SLEEPMode1	@(IDLEN==0  & (LVDEN==Disable || WDT Disable) )
//Stop:CPU,Fsub,Fs,
//Run :Enable WDT or LVDEN

//IDLEMode0		@(IDLEN==1  & FSYSON==0 )
//Stop:CPU,Fs
//Run :Fsub

//IDLEMode1		@(IDLEN==1  & FSYSON==1 )
//Stop:CPU
//Run :Fs,Fsub

//___________________________________________________________________
//Function: GPIO initial
//   INPUT: 
//	  NOTE: Base scription
//___________________________________________________________________
void GPIO_Init()
{
	_acerl=0B00000001;	//binding IO  pb0,pb1,pb2,pa4,pa5,pa6,pa7,pb3 see you.
	_scomc=0B00000000;	//binding IO  ...SCOMEN,pb5,pb6,pa3,pa1 see you
	_cpc=0B00001001;	//binding IO  (pb5,pb6,pa3) see you
	
	_pac = 0b10000101;		//PA0/PA3 timer LED, PA7 binding TP1(TM1) Capture input
	//_papu = 0b11111111;	//default is 0(disable).
	//_pawu = 0b00000000; 	//default is 0(disable).
	_pa = 0x00;

	_pbc = 0b11110001;		//pb0 for an0, pb1 for Test, pb2 for buzzer, PB3(TP2) for PWM
	//_pbpu = 0b11111110;	//default is 0(disable).	
	_pb = 0x01;
	
	//_pcc = 0xff;
	_pcpu = 0b11110000;		//default is 0(disable).	
	//_pc = 0;
	
	/*
	//only for ht66f318
	_pdc = 0;
	_pdpu = 0;		
	_pd = 0;
	*/	
}
//___________________________________________________________________
//Function: Ram_Init
//   INPUT: 
//  OUTPUT: 
//	  NOTE:   
//___________________________________________________________________
void Ram_Init()
{
	_bp = 1;
	_mp1 = 0xA0;	//HT66F318 only
	for(_tblp = 0x00;_tblp < 96;_tblp++) //HT66F318 96x2
	{
		_iar1 = 0;
		_mp1++;
	}	
	_bp = 0;
	//_mp1 = 0x80;	//HT66F317 bank0(80h~FFh) 128bytes
	_mp1 = 0xA0;	//HT66F318 bank0(A0h~FFh),bank1(A0h~FFh) 192bytes
	//for(_tblp = 0x00;_tblp < 128;_tblp++)	//HT66F317 128
	for(_tblp = 0x00;_tblp < 96;_tblp++) //HT66F318 96x2
	{
		_iar1 = 0;
		_mp1++;
	}		
}