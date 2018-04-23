#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include"i2c_master_noint.h" // I2C library

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

#define DELAYTIME 4000000 // 40000 yields 0.001 s delay time when using Core Timer
#define CS LATAbits.LATA0  // chip select pin
#define SLAVE_ADDR 0x20 // I2C hardware address of MCP23008

void initExpander() {
    i2c_master_setup(); // setup I2C2 at 400 kHz

    // init GP0-3 as outputs
    i2c_master_start(); // send the start bit
    i2c_master_send(SLAVE_ADDR << 1);  // hardware address; RW (lsb) = 0, indicates write
    i2c_master_send(0x00);  // specify address to write to: 0x00 = IODIR
    i2c_master_send(0xF0);  // send value byte to address specified above
                        // a value of 0xF0 makes GP0-3 outputs and 4-7 inputs.
    i2c_master_stop();  // STOP bit
}

void setExpander(char pin, char level) { // write to expander
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR << 1); // hardware address and write bit
    i2c_master_send(0x0A); // LAT register = 0x0A
    i2c_master_send(level << pin); // write "level" (hi/lo) to "pin"
    i2c_master_stop();
}

char getExpander() {
    char level;
    i2c_master_start();
    i2c_master_send((SLAVE_ADDR << 1)); // hardware address and write bit
    i2c_master_send(0x09);  // PORT register = 0x09
    i2c_master_restart(); // this line is REALLY important!
    i2c_master_send((SLAVE_ADDR << 1) | 1); // hardware address and read bit
    level = i2c_master_recv(); // receive a byte from the slave
    i2c_master_ack(1); // send NACK to slave
    i2c_master_stop();
    return level;
}

int main() {
    __builtin_disable_interrupts();

    // Turn off AN2 and AN3 analog inputs (make B2 and B3 available for I2C)
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;

    initExpander();
    setExpander(0,1); // turn on LED connected to GP0

    __builtin_enable_interrupts();

    _CP0_SET_COUNT(0);
    while(_CP0_GET_COUNT() < 48000000) {} // wait 1 second
    while(1) {
        if((getExpander()>>7)==0x00){// button is on GP7, when the push bottom is 1, 
            setExpander(0,0);} 
        else{setExpander(0,1); }// turn on LED connected to GP0// turn off LED connected on 
    }
  
    
    return 0;
}