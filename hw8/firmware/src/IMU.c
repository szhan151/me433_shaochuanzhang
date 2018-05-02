#include<xc.h>
#include "IMU.h"
#include "i2c_master_noint.h"

#define imuadd 0b1101010  // SDO pin is connected to ground

//initialize I/O expander
void init(){
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;
    i2c_master_setup();
    
    // CTRL1_XL  0b10000010
    i2c_master_start();
    i2c_master_send(imuadd<<1);
    i2c_master_send(0x10);
    i2c_master_send(0x82); 
    i2c_master_stop();
    
    // CTRL2_G     0b10001000 
    i2c_master_start();
    i2c_master_send(imuadd<<1);
    i2c_master_send(0x11);
    i2c_master_send(0x88);
    i2c_master_stop();
    
    // CTRL3_C     0b00000100
    i2c_master_start();
    i2c_master_send(imuadd<<1);   // R/W = 0 = write
    i2c_master_send(0x12);  // 0x12 = CTRL3_C
    i2c_master_send(0x04);  // default = 0x04, IF_INC = 1(default)
    i2c_master_stop();
}

void I2C_read_multiple(unsigned char addr, unsigned char regist, unsigned char * data, int length){
    char i;
    i2c_master_start();
    i2c_master_send(addr<<1);   
    i2c_master_send(regist);  // 0x20
    i2c_master_restart(); 
    i2c_master_send((addr<<1)|1); // R/W = 1 = read
    for(i=0;i<length-1;i++){
        data[i] = i2c_master_recv(); // receive a byte from GP7
        i2c_master_ack(0);
    }
    data[length-1]=i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
}

float read_x(unsigned char * data) { 
    float value;
    short int x = data[9]<<8 | data[8];
    value = ((float)x/(float)16000);
    return value;
}

float read_y(unsigned char * data) { 
    float value;
    short int y = data[11]<<8 | data[10];
    value = ((float)y/(float)16000);
    return value;
}

