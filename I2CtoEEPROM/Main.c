//___________________________________________________________________
//___________________________________________________________________
// Description: Main program
//  Copyright@: 2019 BY Louis Huang / https://github.com/louisopen/
//   File Name: main
//Targer Board: MK8002D 
//    MCU Body: HT66F318-28ssop
//      Author: Louis Huang
//        Date: 2019/05/18
//     Version: V00 on Hardware V10
//     History:
//___________________________________________________________________
//___________________________________________________________________
#include "common.h"

//___________________________________________________________________
//___________________________________________________________________
//___________________________________________________________________
void main()
{	//like Arduino setup();
	if(_to ==0 || _pdf ==0)		//power up or reset pin normal.
	{	//上電復位或者是正常情況下的非正常復位
		Ram_Init();		//RAM clear all, first time power up.
		PowerOn_Init();	//第一次上電或正常reset pin 
		WDT_ResetInit();                          								
	}
	else//Watch dog reset 													
	{	//WDT造成溢出的復位
		WDT_ResetInit();	
	}
	//sleep_request=1;					//test sleep request
	//TEST_EEPROM(EEPROM_BUFFER_MAX);	//auto fill 0,1,2,3,~
	tx_iic[0] = Read_EEPROM(EEPROM_START_ADDR);
	tx_iic[1] = Read_EEPROM(EEPROM_START_ADDR+1);
	tx_iic[2] = Read_EEPROM(EEPROM_START_ADDR+2);
	tx_iic[3] = Read_EEPROM(EEPROM_START_ADDR+3);
	tx_iic[4] = Read_EEPROM(EEPROM_START_ADDR+4);
	tx_iic[5] = Read_EEPROM(EEPROM_START_ADDR+5);
	tx_iic[6] = Read_EEPROM(EEPROM_START_ADDR+6);
	tx_iic[7] = Read_EEPROM(EEPROM_START_ADDR+7);
	while(1)	//like Arduino loop() for main loop
	{	
		//GCC_CLRWDT();		//Clear Watchdog
		_clrwdt();			//Clear Watchdog
		//Uart_RXD_Manage();	//Received process/check
		i2c_manage();		//Received process/check
		if(sleep_request)	//Enter sleep mode ?
		{
 			ReadyToHalt();
		}
		else
		{
			Task_500ms();	//TimeBase Task process/check
			Key_Scan();		
		}
	}
}
