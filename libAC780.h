/*
 * libAC780.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef LIBAC780_H
#define LIBAC780_H

#include "mbed.h"
#include "libAC780_UDC.h"


class AC780 : public Stream {    
public:
    /** Create an AC780 interface using a native I2C interface
     *
     * @param i2c             I2C Bus
     * @param deviceAddress   I2C slave address (default = ST7032_SA = 0x7C)  
     * @param backlight       Backlight control line (optional, default = NC)       
     * @param contrastAddress Contrast controller address (default = -1)                     
     */
    AC780(I2C *i2c, char deviceAddress, PinName backlight = NC, int contrastAddress = -1);

    virtual ~AC780(void);

    int putc(int c);

    int printf(const char* text, ...);

    void locate(int row, int column);

    int getAddress(int row, int column);     

    void setAddress(int row, int column);        

    void clear();

    void setCursor(bool cursor, bool blink);     

    void setDisplayMode(bool on);     

    void setUDC(unsigned char c, const char *udc_data);

    void setBacklight(bool on); 

    void setContrast(char contrast);

    void setFont(char font=0);

protected:

    // Stream implementation functions
    virtual int _putc(int value);
    virtual int _getc();

    void _init();     

    void _setDisplay(bool display, bool cursor, bool blink);       

    // Display mode
    bool _mode;

    int _font;          // ASCII character fonttable

    // Cursor
    bool _cursor;
    bool _blink;

    int _column;
    int _row;

    int _contrast;          

private:

    virtual void _writeCommand(int command);

    virtual void _writeData(int data);

    virtual void _writeByte(int value);

    //I2C bus
    I2C *_i2c;
    char _slaveAddress;
    char _contrastAddress;

    // controlbyte to select between data and command. Internal shadow value for serial bus only
    char _controlbyte;   

    //Backlight
    DigitalOut *_backlight;       

};

#endif /* !LIBAC780_H */
