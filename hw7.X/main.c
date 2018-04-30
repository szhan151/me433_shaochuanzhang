#include <xc.h>
#include <sys/attribs.h>  // __ISR macro
#include <stdio.h>
#include "i2c_master_noint.h"
#include "ST7735.h"
#include "LSM6DS333.h"
#include <stdlib.h>
#include <math.h>

#define len 60

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll, this uses external crystal*
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode*
#pragma config OSCIOFNC = OFF // free up secondary osc pins
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock,then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = ON // allow multiple reconfigurations
#pragma config IOL1WAY = ON // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

void acc_display(float Gx, float Gy){
    int i,j;
    char xlength = abs(Gx * len);
    char ylength = abs(Gy * len);
    if (Gx>0){
        for (i=0;i<xlength;i++){
            for(j=0;j<2;j++){
                LCD_drawPixel(64+i,64+j,GREEN);
            }
        }
        if (xlength < len){
            for (i=xlength;i<len;i++){
                for(j=0;j<2;j++){
                    LCD_drawPixel(64+i,64+j,BLUE);
                }
            }
        }
        for (i=1;i<len;i++){
            for(j=0;j<2;j++){
                LCD_drawPixel(64-i,64+j,BLUE);
            }
        }    
    }
    else{
        for (i=0;i<xlength;i++){
            for(j=0;j<2;j++){
                LCD_drawPixel(64-i,64+j,GREEN);
            }
        }
        if (xlength < len){
            for (i=xlength;i<len;i++){
                for(j=0;j<2;j++){
                    LCD_drawPixel(64-i,64+j,BLUE);
                }
            }
        }
        for (i=1;i<len;i++){
            for(j=0;j<2;j++){
                LCD_drawPixel(64+i,64+j,BLUE);
            }
        } 
    }
    if (Gy>0){
        for (i=0;i<ylength;i++){
            for(j=0;j<2;j++){
                LCD_drawPixel(64+j,64+i,GREEN);
            }
        }
        if (ylength < len){
            for (i=ylength;i<len;i++){
                for(j=0;j<2;j++){
                    LCD_drawPixel(64+j,64+i,BLUE);
                }
            }
        }
        for (i=1;i<len;i++){
            for(j=0;j<2;j++){
                LCD_drawPixel(64+j,64-i,BLUE);
            }
        }
    }
    else{
        for (i=0;i<ylength;i++){
            for(j=0;j<2;j++){
                LCD_drawPixel(64+j,64-i,GREEN);
            }
        }
        if (ylength < len){
            for (i=ylength;i<len;i++){
                for(j=0;j<2;j++){
                    LCD_drawPixel(64+j,64-i,BLUE);
                }
            }
        }
        for (i=1;i<len;i++){
            for(j=0;j<2;j++){
                LCD_drawPixel(64+j,64+i,BLUE);
            }
        }
    }
    
}

int main() {
    __builtin_disable_interrupts();

    TRISAbits.TRISA4 = 0; // set LED an output pin
    LATAbits.LATA4 = 0; // turn LED off
   
    LCD_init();
    LCD_clearScreen(BLACK);

__builtin_enable_interrupts();

unsigned char data[100];
float Gx,Gy;
char str1[100],str2[100];

while(1) {
        _CP0_SET_COUNT(0);
        I2C_read_multiple(0b1101010, 0x20, data, 14);
        Gx = read_x(data);
        Gy = read_y(data);
        acc_display(Gx,Gy);
        sprintf(str1,"x_raw: %d",(int)(Gx*16000));
        LCD_drawString(10,130,str1,BLUE); //String
        sprintf(str2,"y_raw: %d",(int)(Gy*16000));
        LCD_drawString(10,140,str2,BLUE); //String       
        while (_CP0_GET_COUNT()<1200000) {;}
        LATAbits.LATA4=!LATAbits.LATA4;
    }

}
