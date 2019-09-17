#include <plib.h>
#include "myAccelerometer.h"
#define READ_COMMAND 1 << 7
#define MULTIPLE_BYTE 1 << 6

// LSB: 625 micro second
// 128 * 625 micro = 80 milli seconds
#define TAP_DURATION 0x10  
#define TAP_DURATION_REGISTER 0x21 

// LSB: 1.25 milli second
// Time after first tap to start looking for the second tap 
// 128 * 1.25  = 160 millisecond
#define TAP_LATENCY 0x20 
#define TAP_LATENCY_REGISTER 0x22 

// LSB: 1.25 milli second
// The length of the window to look for a second tap
// 0x90 * 1.25 = 180 milli second
#define TAP_WINDOW 0x90  
#define TAP_WINDOW_REGISTER 0x23

// LSB: 62.5 mg
// Our choice = 4 g 
#define TAP_THRESHOLD 0x30
#define TAP_THRESHOLD_REGISTER 0x1D 

#define TAP_AXES_REGISTER 0x2A


void setAccelReg(SpiChannel chn, unsigned int address, unsigned int data) 
{
    SpiChnPutC(chn, address);
    SpiChnPutC(chn, data);
    SpiChnGetC(chn);
    SpiChnGetC(chn);
}

unsigned char getAccelReg(SpiChannel chn, unsigned int address)
{
    unsigned char temp;
    SpiChnPutC(chn, address | MULTIPLE_BYTE);
    SpiChnPutC(chn, 0);
    SpiChnGetC(chn);
    temp = SpiChnGetC(chn);
    return temp;
}

void getAccelData(SpiChannel chn, short *xyzArray) 
{
    // create the command to read multiple bytes from Acc
    unsigned char dataArray[6];
    
    SpiChnPutC(chn, READ_COMMAND | MULTIPLE_BYTE | 0x32);
    
    
    int i;
    for(i = 0; i<6; i++){
        SpiChnPutC(chn, 0);
    }

    SpiChnGetC(chn);
    int j;
    for(j = 0; j<6; j++){
        dataArray[j] = SpiChnGetC(chn);
    }
    
    // combine high and low bytes to create a short for each of x, y and z
               
    xyzArray[0] = (dataArray[1] << 8 | dataArray[0]);
    xyzArray[1] = (dataArray[3] << 8 | dataArray[2]);
    xyzArray[2] = (dataArray[5] << 8 | dataArray[4]);
    
 }

/*void displayAccelData(short *xyzArray) 
{
    char buf[17];
    OledSetCursor(0,1);
    int x = xyzArray[0]/100;
    if (x)
        sprintf(buf, "X-Axis: %+2d", x); //display x-axis digital read from accelerometer
    else 
        sprintf(buf, "X-Axis:  0"); //display x-axis digital read from accelerometer
    OledPutString(buf);
    
    
    OledSetCursor(0,2);
    int y = xyzArray[1]/100;
    if (y)
        sprintf(buf, "Y-Axis: %+2d", y); //display y-axis digital read from accelerometer
    else
        sprintf(buf, "Y-Axis:  0"); //display y-axis digital read from accelerometer
    OledPutString(buf);
    
    
    OledSetCursor(0,3);
    int z = xyzArray[2] / 100;
    if (z)
        sprintf(buf, "Z-Axis: %+2d", z); //display z-axis digital read from accelerometer
    else
        sprintf(buf, "Z-Axis:  0"); //display z-axis digital read from accelerometer
    OledPutString(buf);
}   */

void initAccelerometer(SpiChannel chn)
{
    // set the Acc register 0x31 such that range is +/-8g with full resolution.
    // Everything else about register 0x31 is kept at default (0)
    setAccelReg(chn, 0x31, 0x09);
    
    // set the 0x2D register such that Acc starts measuring (measure on)
    // Everything else about register 0x2D is kept at default (0)    
    setAccelReg(chn, 0x2D, 0x08);
    
    // set the 0x2C register such that the Acc outputs results (measurements) at 3.13 Hz rate
    // Note that this speed is just about Acc measuring speed and not SPI
    // Everything else about register 0x2C is kept at default (0) 
    setAccelReg(chn, 0x2C, 0x05);
       
    
    
    setAccelReg(chn, 0x2E, 0x40);//int enable
    setAccelReg(chn, 0x2F, 0x40);//int routed to INT2
    
    setAccelReg(chn, TAP_DURATION_REGISTER, TAP_DURATION);
    setAccelReg(chn, TAP_THRESHOLD_REGISTER, TAP_THRESHOLD);
    setAccelReg(chn, TAP_AXES_REGISTER, 0x07);
}

void initAccMasterSPI(SpiChannel chn)
{
    // opening flags of SPI are defined in spi_5xx_6xx_7xx.h starting at line 134
    // open the channel in master mode
    // enable the driving of the Slave Select (SS) output pin by the Master
    // pick the clock polarity as high
    // choose the data size as 8
    // choose the enhanced buffer mode
    SpiOpenFlags oFlags = (SPI_OPEN_MSTEN | SPI_OPEN_MSSEN | SPI_OPEN_MODE8 | SPI_OPEN_ENHBUF | SPI_OPEN_CKP_HIGH) ;
    SpiChnOpen(chn, oFlags, ACC_MASTER_SPI_CLK_DIVIDER);
}