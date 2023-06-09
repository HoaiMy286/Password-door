#include "uart.h"
#include "..\timer\timer.h"
#include "..\lcd\lcd.h"

unsigned char dataReceive;

void init_uart()
{

	SPBRGH = 0;  //Baud Rate Generator Register 
	//Baud Rate =  FOSC/(64 ([SPBRGH:SPBRG] + 1)) 
	//when SYNC = 0, BRG16 = 1, BRGH = 0 => Baud Rate = F/[16(SPBRG + 1)]
        // => SPBRG = F/(16*Baud Rate) - 1
        // F = 20,000,000
	SPBRG = 129; //9600
	TXSTAbits.TXEN = 1; //Transmit enabled
	TXSTAbits.BRGH = 0; //Low Speed
	BAUDCONbits.BRG16 = 1; //16-bit Baud Rate Generator

	TXSTAbits.SYNC = 0; //Asynchronous mode
	RCSTAbits.SPEN = 1; //Enables
	RCSTAbits.CREN = 1; //Enables receive

	PIR1bits.RCIF = 0;  // clear interrupt flag uart
	PIE1bits.RCIE = 1;	//enable interrupt uart
	IPR1bits.RCIP = 0;	// Priority: low
	TRISCbits.RC6 = 0; // RC6 output
	TRISCbits.RC7 = 1; // RC7 input
}
void uart_delay_ms(int value)
{
	int i,j;
	for(i=0;i<value;i++)
		for(j=0;j<238;j++);
}
void uart_putchar(unsigned char data)
{
	while(PIR1bits.TXIF == 0);
        TXREG = data;
	PIR1bits.TXIF = 0;
}

void uart_send_str(const char *str)
{
	while(*str)
	{
		uart_putchar(*str);
		*str++;
	}
}

void UartSendString(const rom char *str)
{
	while(*str)
	{
		uart_putchar(*str);
		*str++;
	}
}

unsigned long uart_power_of(int a, int x) {
    char i;
    unsigned long temp = 1;
    for(i = 0; i < x; i++)
            temp *= a;
    return temp;
}

void UartSendNum(long num) 
{
    char num_flag = 0;
    char i;

    if(num == 0) 
    {
        uart_putchar('0');
        return;
    }
    if(num < 0) 
    {
        uart_putchar('-');
        num *= -1;
    }

    for(i = 10; i > 0; i--) 
    {
        if((num / uart_power_of(10, i-1)) != 0) 
        {
            num_flag = 1;
            uart_putchar(num/uart_power_of(10, i-1) + '0');
        }
        else 
        {
            if(num_flag != 0)
                uart_putchar('0');
        }
        num %= uart_power_of(10, i-1);
    }
}

char tagRX[14] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char tagRecdFlag;
char tagComingFlag;
char tagCounter;

void rx_handler(void)
{
    unsigned char rxByte;
    rxByte = RCREG; //Read character received from USART

    if (tagComingFlag == 0 && rxByte == 0x02)
    {
        tagRX[tagCounter] = rxByte;
        tagComingFlag = 1;
        tagCounter++;
    }   else if (tagComingFlag == 1)
        {
            tagRX[tagCounter] = rxByte;
            tagCounter++;
            if (tagCounter == 14)
            {
//                LcdPrintStringS(1,0,"OK");
                tagCounter = 0;
                tagComingFlag = 0;
                tagRecdFlag = 1;
            }
        }   else
            {
                tagComingFlag = 0;
                tagCounter = 0;
            }
}

void uart_isr()
{
//    unsigned char tempReceive;
//    tempReceive = RCREG;
//    LcdPrintStringS(1,0,"   ");
//    LcdPrintNumS(1,0,(((tempReceive)&0xf0))>>4);
//    LcdPrintNumS(1,2,((tempReceive)&0x0f));   
//    LcdPrintStringS(1,4,"            ");
    rx_handler();
}

