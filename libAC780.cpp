/*
 * libAC780.cpp
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#include "mbed.h"
#include "libAC780.h"
#include "libAC780_UDC.inc"

#define AC780_CMD_CLEAR             (1 << 0)
#define AC780_CMD_HOME              (1 << 1)
#define AC780_CMD_ENTRY_MODE_SET    (1 << 2)
#define AC780_CMD_DISPLAY           (1 << 3)
#define AC780_CMD_CDS               (1 << 3)
#define AC780_CMD_FS                (1 << 5)
#define AC780_CMD_CGRAM_ADDR        (1 << 6)
#define AC780_CMD_DDRAM_ADDR        (1 << 7)

#define AC780_CTRL_BYTE             (1 << 6)

#define AC780_CONF_ROWS             (4)
#define AC780_CONF_COLS             (20)

AC780::AC780(I2C *i2c, char deviceAddress, PinName backlight, int contrastAddress) {
  _slaveAddress = deviceAddress & 0xfe;

  _i2c = i2c;

  if (backlight != NC) {
      _backlight = new DigitalOut(backlight);
      _backlight->write(0);
  } else {
      _backlight = NULL;
  }

  if (contrastAddress >= 0) {
      _contrastAddress = contrastAddress & 0xfe;
      setContrast(50);
  }

  _init();
}

AC780::~AC780() {
   if (_backlight != NULL) {delete _backlight;}  // BL pin
}

void AC780::_init() {

    wait_ms(100);

    _writeCommand(AC780_CMD_FS | (0 << 4) | (1 << 3) | (0 << 2)); // Function set: DL=8bit, F=5x8 dots
    // _writeCommand(AC780_CMD_FS | (1 << 4) | (1 << 2)); // Function set: DL=8bit, F=5x11 dots
    wait_us(100);
    setDisplayMode(true);
    wait_us(100);
    _writeCommand(AC780_CMD_CLEAR);
    wait_ms(2);
    _writeCommand(AC780_CMD_ENTRY_MODE_SET | (1 << 1) | (0 << 0));
    wait_us(100);
}

void AC780::_writeByte(int value) {
    char data[] = {_controlbyte, (char) value};

    _i2c->write(_slaveAddress, data, 2);
}

void AC780::clear() {
    _writeCommand(AC780_CMD_CLEAR);
    wait_ms(2);
    _writeCommand(AC780_CMD_HOME);
    wait_us(50);
}

void AC780::setCursor(bool cursor, bool blink) {
    _cursor = cursor;
    _blink = blink;

    _setDisplay(_mode, _cursor, _blink);
}

void AC780::setDisplayMode(bool on) {
    _mode = on;

    _setDisplay(_mode, _cursor, _blink);
}

void AC780::_setDisplay(bool on, bool cursor, bool blink) {
    _writeCommand(AC780_CMD_DISPLAY | ((char) on << 2) | ((char) cursor << 1) | ((char) blink << 0));
    wait_us(50);
}

void AC780::setUDC(unsigned char c, const char *udc_data) {
    c &= 0x07;
    _writeCommand(AC780_CMD_CGRAM_ADDR | ((c << 3) & 0x3f));

    for (int i=0; i<8; i++) {
        _writeData(*udc_data++);
    }

    // Restore DDRAM address
    int addr = getAddress(_row, _column);
    _writeCommand(AC780_CMD_DDRAM_ADDR | addr);  
}

void AC780::locate(int row, int column) {
    setAddress(row, column);
}

int AC780::getAddress(int row, int column) {
    switch (row) {
        case 0:
            return 0x00 + column;
            break;
        case 1:
            return 0x40 + column;
            break;
        case 2:
            return 0x14 + column;
            break;
        case 3:
            return 0x54 + column;
            break;
        default:
            return 0x00;
            break;
    }
}

void AC780::setAddress(int row, int column) {
    _row = (row < 0) ? 0 : (row >= AC780_CONF_ROWS ? AC780_CONF_ROWS : row);
    _column = (column < 0) ? 0 : (column >= AC780_CONF_COLS ? AC780_CONF_COLS : column);

    int addr = getAddress(_row, _column);
    _writeCommand(AC780_CMD_DDRAM_ADDR | addr);
}

int AC780::_putc(int value) {
    if (value == '\n') {
        _column = 0;
        _row = (_row >= AC780_CONF_ROWS) ? 0 : _row+1;
        return 0;
    }

    _writeData(value);

    _column++;
    if (_column >= AC780_CONF_COLS) {
        _row++;
        _column = 0;
        _row = (_row >= AC780_CONF_ROWS) ? 0 : _row;
    }
    _writeCommand(AC780_CMD_DDRAM_ADDR | getAddress(_row, _column));
    return value;
}

int AC780::_getc() {
    return -1;
}

void AC780::_writeCommand(int command) {
    _controlbyte = 0x00;
    this->_writeByte(command);
    wait_us(50);
}

// Write a data byte to the LCD controller
void AC780::_writeData(int data) {
    _controlbyte = AC780_CTRL_BYTE;
    this->_writeByte(data);
    wait_us(50);
}

int AC780::putc(int c) {
    return _putc(c);
}

int AC780::printf(const char* text, ...) {
    while (*text !=0) {
        _putc(*text);
        text++;
    }

    return 0;
}

void AC780::setBacklight(bool on) {
    if (_backlight != NULL)
        _backlight->write((int) on);
}

void AC780::setContrast(char contrast) {
    _contrast = contrast & 0x7f;
    char data[2] = {0x00, (char) _contrast};
    _i2c->write(_contrastAddress, data, 2);
}
