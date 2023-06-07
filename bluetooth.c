#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include <time.h>
#include "TM4C123.h"
#include <stdlib.h>
#include "inc/hw_gpio.h"
#include "inc/tm4c123gh6pm.h"
void HC05_init(void); // Initialize UART5 module for HC-05
char Bluetooth_Read(void); //Receive data from Rx5 pin
void Bluetooth_Write(unsigned char data); // Transmit data to Hc05 from Tx5 pin
void Bluetooth_Write_String(char *str); // Transmit a string to HC-05 over Tx5 pin 
int main(void)
{
	  
	HC05_init(); // call HC05_init() to initialze UART5 of TM4C123GH6PM
	
	/* Set PF1, PF2 and PF3 as digital output pins, only using PF1 but having PF2/3 open for other options*/
	
		SYSCTL->RCGCGPIO |= 0x20;		//enable clock for GPIOF
    GPIO_PORTF_DIR_R |= 0x0E;   //set PF1, PF2 and PF3 as digital output pin
    GPIO_PORTF_DEN_R |= 0x0E;   // CON PF1, PF2 and PF3 as digital GPIO pins
  	SysCtlDelay(16000000); 
		GPIO_PORTF_PUR_R = 0x11;		//enable pull up
	
	while(1)
	{
		char c = Bluetooth_Read();  //get character from UART5
		

    if( c=='R'){								//Transmit/Receive Character R to turn on PF1 pin and RED LED 
			GPIO_PORTF_DATA_R |=(1<<1);
			Bluetooth_Write_String("Red LED on, Coffee is brewing\n");
		}
		else if( c=='r'){						//Transmite/Receive Character r to turn off PF1 and RED LED
			GPIO_PORTF_DATA_R &=~(1<<1);
			Bluetooth_Write_String("Red LED off, Coffee Meachine off\n");
		}
	}
}

void HC05_init(void)
{
		SYSCTL->RCGCUART |= 0x20;		//enable clock to UART5 
    SYSCTL->RCGCGPIO |= 0x10;	//enable clock to PORTE (PE4 and PE5)
    SysCtlDelay(1600000);
	
    //initialize UART0
    UART5->CTL = 0;         // UART5 module disbable
    UART5->IBRD = 104;      // for 9600 baud rate, integer = 104
    UART5->FBRD = 11;       // for 9600 baud rate, fractional = 11
    UART5->CC = 0;          // select system clock
    UART5->LCRH = 0x60;     // data lenght 8-bit
    UART5->CTL = 0x301;     // Enable UART5 module, Rx and Tx
	
    GPIOE->DEN = 0x30;      // set PE4 and PE5 as digital
    GPIOE->AFSEL = 0x30;    // Use PE4,PE5 alternate function
    GPIOE->AMSEL = 0;    		// Turn off analog function
    GPIOE->PCTL = 0x00110000;     // configure PE4 and PE5 for UART
}
char Bluetooth_Read(void)  
{
    char data;
	  while((UART5->FR & (1<<4)) != 0); // wait until Rx buffer is not full
    data = UART5->DR ;  							// before giving it another byte
    return (unsigned char) data; 
}

void Bluetooth_Write(unsigned char data)  
{
    while((UART5->FR & (1<<5)) != 0);  	//wait until Tx buffer not full
    UART5->DR = data;                  	// before giving it another byte
}

void Bluetooth_Write_String(char *str)		//send string to hc05
{
  while(*str)
	{
		Bluetooth_Write(*(str++));
	}
}
