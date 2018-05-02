#include "app.h"
#include <stdio.h>
#include <stdlib.h>
#include "ST7735.h"
#include "i2c_master_noint.h"
#include <xc.h>
#include <math.h>
#include "IMU.h"


APP_DATA appData;
#define len 60

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

void LCD_drawCharacter(unsigned short x, unsigned short y, char ch, unsigned short color){
    int i,j;
    if ((x<_GRAMWIDTH-5)&&(y<_GRAMHEIGH-8)){
        for (i=0;i<5;i++){
            for(j=0;j<8;j++){
                if(((ASCII[ch-0x20][i])>>j)&(0x01))
                    LCD_drawPixel(x+i,y+j,color);
                else
                    LCD_drawPixel(x+i,y+j,BLACK); 
            }
        }
    }
}

void LCD_drawString(unsigned short x, unsigned short y, char ch[], unsigned short color){
    int i=0,k=0;
    while(1){
        if (ch[i] == '\0')
            break;
        LCD_drawCharacter(x+k,y,ch[i],color);
        i++;
        k+=6;
    }
}


void APP_Initialize ( void )
{
    __builtin_disable_interrupts();

    TRISAbits.TRISA4 = 0; // set LED an output pin
    LATAbits.LATA4 = 0; // turn LED off
    
    SPI1_init();
    LCD_init();
    LCD_clearScreen(BLACK);

    __builtin_enable_interrupts();
}

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;


            if (appInitialized)
            {

                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
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
            break;
        }


        /* The default state should never be executed. */
        default:
        {

            break;
        }
    }
}