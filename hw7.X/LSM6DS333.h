#ifndef LSM6DS333_H__
#define LSM6DS333_H__

#define IMU_ADDR 0b1101010 

void init(void);
void I2C_read_multiple(unsigned char, unsigned char, unsigned char *, int);
float read_x(unsigned char * );
float read_y(unsigned char * );

#endif