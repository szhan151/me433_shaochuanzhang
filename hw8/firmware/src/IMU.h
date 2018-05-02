#ifndef IMU_H__
#define IMU_H__


void init(void);
void I2C_read_multiple(unsigned char, unsigned char, unsigned char *, int);
float read_x(unsigned char * );
float read_y(unsigned char * );

#endif