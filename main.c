#include "main.h"
// Noi khai bao hang so
#define     LED     PORTD
#define     ON      1
#define     OFF     0

#define     INIT_SYSTEM         0
#define     ENTER_PASSWORD      1
#define     CHECK_PASSWORD      2
#define     UNLOCK_DOOR         3
#define     WRONG_PASSWORD      4

#define     MENU                5
#define     CHOOSE_SLOT_PASS    6
#define     CHANGE_PASSWORD     7
#define     OLD_PASSWORD        8
#define     CHECK_OLD_PASSWORD  9
#define     NEW_PASSWORD        10
#define     CONFIRM_PASSWORD    11
#define     CONFIRM_SUCCESS     12
#define     CONFIRM_FAIL        13


#define     SET_RFID            14
#define     CHECK_PASS_RFID     15
//#define     CHECK_FAIL          16
#define     CHECK_SUCCESS       17
#define     RFID_INPUT          18
#define     RFID_SUCCESS        19

#define     DELETE_RFID         20


// Noi khai bao bien toan cuc
unsigned char arrayMapOfOutput [8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned char statusOutput[8] = {0,0,0,0,0,0,0,0};
// Khai bao cac ham co ban IO
void init_system(void);
void delay_ms(int value);
void OpenOutput(int index);
void CloseOutput(int index);
void TestOutput(void);
void ReverseOutput(int index);
void Test_KeyMatrix();
//Chuong trinh Password Door
unsigned char arrayMapOfNumber [16] = {1,2,3,'A',4,5,6,'B',
                                       7,8,9,'C','*',0,'E','D'};
unsigned char arrayMapOfPassword [5][4]= {
  {1,2,3,4},
  {1,1,1,1},
  {3,3,3,3},
  {4,5,6,3},
  {2,3,2,3},
};

unsigned char RfID[14] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

char *stringPointer;

int passOkFlag = 0;
int passConfirmFlag = 0;
int indexOfMenu = 3;
int passwordSlot;

unsigned char arrayPassword[4];
unsigned char statusPassword = INIT_SYSTEM;
unsigned char indexOfNumber = 0;
unsigned char timeDelay = 0;
unsigned char tempCount = 0;

unsigned char arrayOfNewPassword[4];
unsigned char arrayOfConfirmPassword[4];

void App_PasswordDoor();
unsigned char CheckPassword();
unsigned char isButtonNumber();
unsigned char numberValue;
unsigned char isButtonEnter();
unsigned char isButtonScroll();
unsigned char ValidateNewPassword();
unsigned char CheckPasswordWithSlot(int slot);
unsigned char CheckRfID();
void ApplyPassword();
void InputRfID();
void ClearRfID();
void UnlockDoor();
void LockDoor();
////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
////////////////////////////////////////////////////////////////////
void main(void)
{
	unsigned int k = 0;
	init_system();
        //TestOutput();
	while (1)
	{
//        LcdClearS();
        while (!flag_timer3);
        flag_timer3 = 0;
        // thuc hien cac cong viec duoi day
        scan_key_matrix_with_uart();
        App_PasswordDoor();
        
//        if(isButtonEnter()){
//            LcdPrintCharS(0,0,'*');
//        }
//        if(isButtonNumber()){
//            for(tempCount = 0; tempCount < 10; tempCount ++){
//                if(key_code[tempCount]==1){
//                    LcdPrintNumS(1,0,tempCount);
//                }
//            }
//        }
        
//            if(tagRecdFlag == 1){
//                LcdPrintCharS(0,2, 's');
//                tagRecdFlag = 0;
//                for(tempCount = 0; tempCount <10; tempCount++){
//                    LcdPrintCharS(1,tempCount, tagRX[tempCount+1]);
//                }
//            }
        DisplayLcdScreen();
	}
}
// Hien thuc cac module co ban cua chuong trinh
void delay_ms(int value)
{
	int i,j;
	for(i=0;i<value;i++)
		for(j=0;j<238;j++);
}

void init_system(void)
{
    TRISB = 0x00;		//setup PORTB is output
    TRISD = 0x00;
    init_lcd();
    LcdClearS();
    LED = 0x00;
	init_interrupt();
    delay_ms(1000);
    init_timer0(4695);//dinh thoi 1ms sai so 1%
    init_timer1(9390);//dinh thoi 2ms
	init_timer3(46950);//dinh thoi 10ms
	SetTimer0_ms(2);
    SetTimer1_ms(10);
	SetTimer3_ms(50); //Chu ky thuc hien viec xu ly input,proccess,output
    init_key_matrix_with_uart();
    init_uart();
    CloseOutput(1);
}

void OpenOutput(int index) 
{
	if (index >= 0 && index <= 7)
	{
		LED = LED | arrayMapOfOutput[index];
	}

}

void CloseOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		LED = LED & ~arrayMapOfOutput[index];
	}
}

void ReverseOutput(int index)
{
    if (statusOutput[index]  == ON)
    {
        CloseOutput(index);
        statusOutput[index] = OFF;
    }
    else
    {
        OpenOutput(index);
        statusOutput[index] = ON;
    }
}

void TestOutput(void)
{
	int k;
	for (k=0;k<=7 ;k++ )
	{
		OpenOutput(k);
		delay_ms(500);
		CloseOutput(k);
		delay_ms(500);
	}
}
unsigned char isButtonMotorOn()
{
    if (key_code[2] == 1)
        return 1;
    else
        return 0;
}

unsigned char isButtonMotorOff()
{
    if (key_code[3] == 1)
        return 1;
    else
        return 0;
}

void App_PasswordDoor()
{
    switch (statusPassword)
    {
        case INIT_SYSTEM:
            LcdPrintStringS(0,0,"PRESS A FOR PASS");
            LcdPrintStringS(1,0,"                ");
            LockDoor();
            if(tagRecdFlag == 1){
                if(CheckRfID()==1){
                    UnlockDoor();
                    statusPassword = MENU;
                }else{
                    LcdPrintStringS(0,0,"WRONG RFID!     ");
                }
                tagRecdFlag = 0;
            }
            if (isButtonEnter())
            {
                indexOfNumber = 0;
                timeDelay = 0;
                statusPassword = ENTER_PASSWORD;
            }
            break;
        case ENTER_PASSWORD:
            LcdPrintStringS(0,0,"ENTER PASSWORD  ");
            timeDelay ++;
            if (isButtonNumber())
            {
                LcdPrintStringS(1,indexOfNumber,"*");
                arrayPassword [indexOfNumber] = numberValue;
                indexOfNumber ++;
                timeDelay = 0;
            }
            if (indexOfNumber >= 4)
                statusPassword = CHECK_PASSWORD;
            if (timeDelay >= 100){
                tagRecdFlag = 0;
                statusPassword = INIT_SYSTEM;
            }
            if (isButtonEnter()){
                tagRecdFlag = 0;
                statusPassword = INIT_SYSTEM;
            }
            break;
        case CHECK_PASSWORD:
            timeDelay = 0;
            if (CheckPassword())
                statusPassword = UNLOCK_DOOR;
            else
                statusPassword = WRONG_PASSWORD;
            break;
        case UNLOCK_DOOR:
            timeDelay = 0;
            LcdPrintStringS(0,0,"OPENING DOOR    ");
            UnlockDoor();
            statusPassword = MENU;
            
            break;
        case WRONG_PASSWORD:
            timeDelay ++;
            LcdPrintStringS(0,0,"PASSWORD WRONG  ");
            if (timeDelay >= 40){
                tagRecdFlag = 0;
                statusPassword = INIT_SYSTEM;
            }
            break;
        case MENU:
            timeDelay ++;
            if(indexOfMenu == 0){
                stringPointer = "CHANGE PASSWORD ";
            } else if (indexOfMenu == 1){
                stringPointer = "SET RFID        ";
            } else if (indexOfMenu == 2){
                stringPointer = "DELETE RFID     ";
            } else if (indexOfMenu == 3){
                stringPointer = "LOCK DOOR       ";
            }
            LcdPrintStringS(0,0,stringPointer);
            LcdPrintStringS(1,0,"A:Enter   B:Mode");    
            // scroll the menu list on lcd screen
            if(isButtonScroll()){
                if(indexOfMenu == 3)indexOfMenu = 0;
                else indexOfMenu++;
                timeDelay = 0;
            }
            // enter the mode which is chosen
            if(isButtonEnter()){
                timeDelay = 0;
                switch(indexOfMenu){
                    case 3:
                        statusPassword = INIT_SYSTEM;
                        tagRecdFlag = 0;
                        break;
                    case 0:
                        passwordSlot = 0;
                        statusPassword = CHOOSE_SLOT_PASS;
                        break;
                    case 1:
                        LcdPrintStringS(1,0,"                ");
                        indexOfNumber = 0;
                        statusPassword = SET_RFID;
                        break;
                    case 2:
                        statusPassword = DELETE_RFID;
                        break;
                }
            }
            if(timeDelay >= 200){
                statusPassword = INIT_SYSTEM;
                tagRecdFlag = 0;
            }
            break;
        case CHOOSE_SLOT_PASS:
            timeDelay++;
            switch(passwordSlot){
                case 0:
                    stringPointer = "SLOT 1          ";
                    break;
                case 1:
                    stringPointer = "SLOT 2          ";
                    break;
                case 2:
                    stringPointer = "SLOT 3          ";
                    break;
                case 3:
                    stringPointer = "SLOT 4          ";
                    break;
                case 4:
                    stringPointer = "SLOT 5          ";
                    break;
            }
            LcdPrintStringS(0,0,stringPointer);
            if(isButtonScroll()){
                if(passwordSlot == 3)passwordSlot = 0;
                else passwordSlot++;
                timeDelay = 0;
            }
            // enter the mode which is chosen
            if(isButtonEnter()){
                timeDelay = 0;
                indexOfNumber = 0;
                LcdPrintStringS(1,0,"                ");
                statusPassword = OLD_PASSWORD;
            }
            if(timeDelay >= 200){
                timeDelay = 0;
                statusPassword = MENU;
            }
            break;
            
        case OLD_PASSWORD:
            timeDelay ++;
            LcdPrintStringS(0,0,"OLD PASSWORD    ");
            if (isButtonNumber())
            {
                LcdPrintStringS(1,indexOfNumber,"*");
                arrayPassword [indexOfNumber] = numberValue;
                indexOfNumber ++;
                timeDelay = 0;
            }
            if (indexOfNumber >= 4){
                statusPassword = CHECK_OLD_PASSWORD;
                timeDelay = 0;
                indexOfNumber = 0;
            }
            
            //TIME OUT
            if(timeDelay >= 200){
                timeDelay = 0;
                indexOfNumber = 0;
                statusPassword = MENU;
            }
            break;
        case CHECK_OLD_PASSWORD:
            timeDelay++;
            if(CheckPasswordWithSlot(passwordSlot)){
                LcdPrintStringS(1,0,"                ");
                statusPassword = NEW_PASSWORD;
            }else{
                LcdPrintStringS(0,0,"WRONG PASSWORD! ");
                LcdPrintStringS(1,0,"                ");
                if(timeDelay >= 10){
                    statusPassword = OLD_PASSWORD;
                    timeDelay = 0;
                }
            }
            break;
        case NEW_PASSWORD:
            timeDelay++;    
            LcdPrintStringS(0,0,"NEW PASSWORD    ");
            if (isButtonNumber())
            {
                LcdPrintStringS(1,indexOfNumber,"*");
                arrayOfNewPassword[indexOfNumber] = numberValue;
                indexOfNumber ++;
                timeDelay = 0;
            }
            if(indexOfNumber >= 4){
                LcdPrintStringS(1,0,"                ");
                statusPassword = CONFIRM_PASSWORD;
                timeDelay = 0;
                indexOfNumber = 0;
            }
            if(timeDelay >= 200){
                statusPassword = MENU;
                timeDelay = 0;
                indexOfNumber = 0;
            }
            break;
        case CONFIRM_PASSWORD:
            timeDelay++;
            LcdPrintStringS(0,0,"CONFIRM PASSWORD");
            if (isButtonNumber())
            {
                LcdPrintStringS(1,indexOfNumber,"*");
                arrayOfConfirmPassword[indexOfNumber] = numberValue;
                indexOfNumber ++;
                timeDelay = 0;
            }
            if(indexOfNumber >= 4){
                if(ValidateNewPassword()) {
                    ApplyPassword();
                    statusPassword = CONFIRM_SUCCESS;
                    timeDelay = 0;
                }
                else{
                    statusPassword = CONFIRM_FAIL;
                    timeDelay = 0;
                }
                indexOfNumber = 0;
            }
            if(timeDelay >= 200){
                statusPassword = MENU;
                timeDelay = 0;
                indexOfNumber = 0;
            }
            break;
        case CONFIRM_SUCCESS:
            timeDelay++;
            LcdPrintStringS(0,0,"SUCCESSFUL!     ");
            LcdPrintStringS(1,0,"                ");
            if(timeDelay>=20){
                statusPassword = MENU;
                timeDelay = 0;
            }
            break;
        case CONFIRM_FAIL:
            timeDelay++;
            LcdPrintStringS(0,0,"NOT THE SAME!   ");
            LcdPrintStringS(1,0,"                ");
            if(timeDelay>=20){
                statusPassword = NEW_PASSWORD;
                timeDelay = 0;
            }
            break;
        case SET_RFID:
            timeDelay ++;
            LcdPrintStringS(0,0,"ENTER PASSWORD  ");
            
            if (isButtonNumber())
            {
                LcdPrintStringS(1,indexOfNumber,"*");
                arrayPassword [indexOfNumber] = numberValue;
                indexOfNumber ++;
                timeDelay = 0;
            }
            if (indexOfNumber >= 4){
                statusPassword = CHECK_PASS_RFID;
                timeDelay = 0;
                indexOfNumber = 0;
            }
            
            //TIME OUT
            if(timeDelay >= 200){
                timeDelay = 0;
                indexOfNumber = 0;
                statusPassword = MENU;
            }
            break;
        case CHECK_PASS_RFID:
            timeDelay++;
            if(CheckPassword()){
                LcdPrintStringS(1,0,"                ");
                tagRecdFlag = 0;
                statusPassword = CHECK_SUCCESS;
            }else{
                LcdPrintStringS(0,0,"WRONG PASSWORD! ");
                LcdPrintStringS(1,0,"                ");
                if(timeDelay >= 10){
                    statusPassword = MENU;
                    timeDelay = 0;
                }
            }
            break;
        case CHECK_SUCCESS:
            timeDelay++;
            LcdPrintStringS(0,0,"SCAN THE CARD   ");
            if(tagRecdFlag == 1){
                tagRecdFlag = 0;
                statusPassword = RFID_INPUT;
                timeDelay = 0;
            }
            if(timeDelay > 200){
                statusPassword = MENU;
                timeDelay = 0;
            }
            break;
        case RFID_INPUT:
            timeDelay++;
            InputRfID();
            LcdPrintStringS(0,0,"SUCCESSFUL!     ");
            if(timeDelay>=20){
                
                statusPassword = MENU;
                timeDelay = 0;
            }
            break;
        case DELETE_RFID:
            timeDelay++;
            ClearRfID();
            LcdPrintStringS(0,0,"DELETE SUCCESS! ");
            if(timeDelay>=20){
                statusPassword = MENU;
                timeDelay = 0;
            }
            break;
        default:
            break;

    }
}

void ApplyPassword(){
    int i=0;
    for(i = 0; i<4; i++){
        arrayMapOfPassword[passwordSlot][i]= arrayOfConfirmPassword[i];
    }
}

unsigned char ValidateNewPassword()
{
    
    unsigned char i;
    for(i = 0; i<4; i++){
        if(arrayOfNewPassword[i] != arrayOfConfirmPassword[i]) return 0;
    }
    return 1;
}

unsigned char CheckPasswordWithSlot(int slot)
{
    
    unsigned char i;
    for(i = 0; i<4; i++){
        if (arrayPassword[i] != arrayMapOfPassword[slot][i]) return 0;
    }
    return 1;
}


unsigned char CheckPassword()
{
    unsigned char i,j;
    unsigned result = 1;
    for (i=0;i<5;i++)
    {
        result = 1;
        for (j=0;j<4;j++)
        {
            if (arrayPassword[j] != arrayMapOfPassword[i][j])
                result = 0;
        }
        if (result == 1)
            return (i+1);
    }

}
unsigned char isButtonNumber()
{
    unsigned char i;
    for (i = 0; i<=7; i++)
        if (key_code[i] == 1)
        {
            numberValue = arrayMapOfNumber[i];
            return 1;
        }
    return 0;
}

unsigned char isButtonEnter()
{
    if (key_code[3] == 1)
        return 1;
    else
        return 0;
}

unsigned char isButtonScroll()
{
    if (key_code[7] == 1)
        return 1;
    else
        return 0;
}

void InputRfID(){
    unsigned char i;
    for (i = 0; i<14; i++)
        RfID[i]=tagRX[i];
}

void ClearRfID(){
    unsigned char i;
    for (i = 0; i<14; i++)
        RfID[i]=0x00;
}

unsigned char CheckRfID(){
    unsigned char i;
    for (i = 0; i<14; i++)
        if(RfID[i]!=tagRX[i])return 0;
    return 1;
}

void UnlockDoor()
{
    CloseOutput(0);
    
    
    //OpenOutput(4);
}
void LockDoor()
{
    OpenOutput(0);
    //CloseOutput(4);
}
