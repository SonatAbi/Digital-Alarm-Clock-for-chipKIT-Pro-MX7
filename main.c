///////////////////////////////////////
//  Project: Lab 4 Spring 2017 ECE 2534
//  Author: John Mert
//  Description: Alarm clock with an accelerometer 

#define _PLIB_DISABLE_LEGACY
#include <stdio.h>
#include <stdbool.h>
#include <plib.h>
#include "PmodOLED.h"
#include "OledChar.h"
#include "OledGrph.h"
#include "delay.h"
#include "myBoardConfigFall2016.h"
#include "myAccelerometer.h"

#define NUMBER_OF_MILLISECONDS_PER_OLED_UPDATE 100
#define LED_MASK 0xF000
#define INT_INPUT (1 << 8)
#define LED_TIMER_MASK 0xC000
#define LED_BUTTON1_MASK 0x1000
#define LED_BUTTON2_MASK 0x2000
#define ACC_SPI_CLK_DIVIDER 10

void initAll();
void initTimer2();
void initExtInt();
void initINT();
void clock(unsigned int *clkWkDay, unsigned int *clkMonth, unsigned int *clkDay, unsigned int *clkHour, unsigned int *clkMin, unsigned int *clkSec, bool clk);
void disp(unsigned int alrWkDay, unsigned int alrMonth, unsigned int alrDay, 
        unsigned int alrHour, unsigned int alrMin, unsigned int alrSec,
        unsigned int clkWkDay, unsigned int clkMonth, unsigned int clkDay,
        unsigned int clkHour, unsigned int clkMin, unsigned int clkSec,
        bool clk, bool alarm);
void weekDay(unsigned int wkDay, char *weekday);

//Global variables for ISR
volatile bool newADXLInterrupt = false;
volatile unsigned int timer2_ms_value = 0;

void __ISR(_EXTERNAL_1_VECTOR, IPL7AUTO) _EXTERNAL_HANDLER(void)
{
    newADXLInterrupt = true;
    INTClearFlag(INT_INT1);
}
void __ISR(_TIMER_2_VECTOR, IPL4AUTO) _Timer2Handler(void) {
    timer2_ms_value++; // Increment the millisecond counter.
    INTClearFlag(INT_T2); // Acknowledge the interrupt source by clearing its flag.
}


const SpiChannel ch = SPI_CHANNEL3; //going to be reading from this channel, will never change so set to const
/*  PORTGSET = (1 << 12);
    PORTGSET = (1 << 13);
    PORTGSET = (1 << 14);
    PORTGSET = (1 << 15);
    short xyzArray[3];
    getAccelData(ch, xyzArray);
    displayAccelData(xyzArray);
 */
int main()
{    
    initAll();
    
    bool clk = true;
    bool alarm = false;
    
    unsigned int clkWkDay = 1;
    unsigned int clkMonth = 1;
    unsigned int clkDay = 1;
    unsigned int clkHour = 0;
    unsigned int clkMin = 0;
    unsigned int clkSec = 0;
    
    unsigned int alrWkDay = 1;
    unsigned int alrMonth = 1;
    unsigned int alrDay = 1;
    unsigned int alrHour = 0;
    unsigned int alrMin = 0;
    unsigned int alrSec = 0;
    
    //enum {BASE, ALRSET, CLKSET} mode = BASE;
	while(1)
	{
        clock(&clkWkDay, &clkMonth, &clkDay, &clkHour, &clkMin, &clkSec, clk);
        disp(alrWkDay, alrMonth, alrDay, alrHour, alrMin, alrSec, clkWkDay, clkMonth, clkDay, clkHour, clkMin, clkSec, clk, alarm);
	}
	return 0;
}

void disp(unsigned int alrWkDay, unsigned int alrMonth, unsigned int alrDay, 
        unsigned int alrHour, unsigned int alrMin, unsigned int alrSec,
        unsigned int clkWkDay, unsigned int clkMonth, unsigned int clkDay,
        unsigned int clkHour, unsigned int clkMin, unsigned int clkSec,
        bool clk, bool alarm){
    ////////////////////Alarm Display/////////////////////////
    OledSetCursor(0, 0);
    OledPutString("ALARM");
    OledSetCursor(0, 7);
    OledPutString("SU");
    OledSetCursor(0, 10);
    char alrMonthStr[2];
    sprintf(alrMonthStr, "%02d", alrMonth);
    OledPutString(alrMonthStr);
    OledSetCursor(0, 12);
    OledPutString("/");
    OledSetCursor(0, 13);
    char alrDayStr[2];
    sprintf(alrDayStr, "%02d", alrDay);
    OledPutString(alrDayStr);
    OledSetCursor(1, 0);
    if(alarm == true){
        OledPutString("ON");
    }
    else{
        OledPutString("OFF");
    }
    OledSetCursor(1, 5);
    char alrHourStr[2];
    sprintf(alrHourStr, "%02d", alrHour);
    OledPutString(alrHourStr);
    OledSetCursor(1, 7);
    OledPutString(":");
    OledSetCursor(1, 8);
    char alrMinStr[2];
    sprintf(alrMinStr, "%02d", alrMin);
    OledPutString(alrMinStr);
    OledSetCursor(1, 10);
    OledPutString(":");
    OledSetCursor(1, 11);
    char alrSecStr[2];
    sprintf(alrSecStr, "%02d", alrSec);
    OledPutString(alrSecStr);
    ///////////////////////////Clock Display/////////////////////
    OledSetCursor(2, 0);
    OledPutString("CLOCK");
    OledSetCursor(2, 7);
    OledPutString("SU");
    OledSetCursor(2, 10);
    char clkMonthStr[2];
    sprintf(clkMonthStr, "%02d", clkMonth);
    OledPutString(clkMonthStr);
    OledSetCursor(2, 12);
    OledPutString("/");
    OledSetCursor(2, 13);
    char clkDayStr[2];
    sprintf(clkDayStr, "%02d", clkDay);
    OledPutString(clkDayStr);
    OledSetCursor(3, 0);
    if(alarm == true){
        OledPutString("ON");
    }
    else{
        OledPutString("OFF");
    }
    OledSetCursor(3, 5);
    char clkHourStr[2];
    sprintf(clkHourStr, "%02d", clkHour);
    OledPutString(clkHourStr);
    OledSetCursor(3, 7);
    OledPutString(":");
    OledSetCursor(3, 8);
    char clkMinStr[2];
    sprintf(clkMinStr, "%02d", clkMin);
    OledPutString(clkMinStr);
    OledSetCursor(3, 10);
    OledPutString(":");
    OledSetCursor(3, 11);
    char clkSecStr[2];
    sprintf(clkSecStr, "%02d", clkSec);
    OledPutString(clkSecStr);
}

void weekDay(unsigned int wkDay, char *weekday){
    int mod = wkDay %7;
    if(mod == 1) weekday = "MO";
    if(mod == 2) weekday = "TU";
    if(mod == 3) weekday = "WE";
    if(mod == 4) weekday = "TH";
    if(mod == 5) weekday = "FR";
    if(mod == 6) weekday = "SA";
    if(mod == 0) weekday = "SU";
}

void clock(unsigned int *clkWkDay, unsigned int *clkMonth, unsigned int *clkDay, unsigned int *clkHour, unsigned int *clkMin, unsigned int *clkSec, bool clk){
    if((timer2_ms_value >= 1000) && clk == true){
        timer2_ms_value = 0;
        (*clkSec)++;
        if(*clkSec == 60){
            *clkSec = 0;
            (*clkMin)++;
            if(*clkMin == 60){
                *clkMin = 0;
                (*clkHour)++;
                if(*clkHour == 24){
                    *clkHour = 0;
                    (*clkDay)++;
                    (*clkWkDay)++;
                        
                    if( (*clkMonth == 1 || *clkMonth == 3 || *clkMonth == 5 || *clkMonth == 7 || *clkMonth == 8 || *clkMonth == 10 || *clkMonth == 12) && *clkDay == 32){
                        *clkDay = 1;
                        (*clkMonth)++;
                        if(*clkMonth == 13){
                            *clkMonth = 1;
                        }
                    }
                    if( (*clkMonth == 4 || *clkMonth == 6 || *clkMonth == 9 || *clkMonth == 11) && *clkDay == 31){
                        *clkDay = 1;
                        (*clkMonth)++;
                    }
                    if( (*clkMonth == 2) && *clkDay == 29){
                        *clkDay = 1;
                        (*clkMonth)++;
                    }
                }
            }
        }
    }
}

void initAll() //initialize timers, interrupts, accelerometer, SPI and configures interrupt
{
    // Initialize GPIO for LEDs
    TRISGCLR = LED_MASK; // For LEDs: configure PortG pins for output
    ODCGCLR = LED_MASK; // For LEDs: configure as normal output (not open drain)
    LATGCLR = LED_MASK; // Turn all LEDs off
    TRISESET = INT_INPUT;
    
    //Initialize External interrupt and Timer 2
    initINT();
    initExtInt();
    initTimer2();
    
    // Initialize Timer1 and OLED for display
    DelayInit();
    OledInit();
    
    //initialize stuff for the accelerometer
    initAccMasterSPI(ch);
    initAccelerometer(ch);

    OledClearBuffer();
}

void initTimer2() {
    // Configure Timer 2 to request a real-time interrupt once per millisecond.
    // The period of Timer 2 is (16 * 625)/(10 MHz) = 1ms.
    OpenTimer2(T2_ON | T2_IDLE_CON | T2_SOURCE_INT | T2_PS_1_16 | T2_GATE_OFF, 624);
    
    // Setup Timer 2 interrupts
    INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_4);
    INTClearFlag(INT_T2);
    INTEnable(INT_T2, INT_ENABLED);
}

void initExtInt(){
    //interrupt configuration
    INTSetVectorPriority(INT_EXTERNAL_1_VECTOR, EXT_INT_PRI_7);
    INTClearFlag(INT_INT1);
    //mINT1SetEdgeMode(1);
    INTEnable(INT_INT1, INT_ENABLED);
}


void initINT()
{
    // This is a multi-vector setup
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    
    // Let the interrupts happen
    INTEnableInterrupts();
}