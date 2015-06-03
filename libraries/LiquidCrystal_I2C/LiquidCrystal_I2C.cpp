//www.DFRobot.com
//last updated on 21/12/2011
//Tim Starling Fix the reset bug (Thanks Tim)
//wiki doc http://www.dfrobot.com/wiki/index.php?title=I2C/TWI_LCD1602_Module_(SKU:_DFR0063)
//Support Forum: http://www.dfrobot.com/forum/
//Compatible with the Arduino IDE 1.0
//Library version:1.1


#include "LiquidCrystal_I2C.h"
#include <inttypes.h>
#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#define printIIC(args)	Wire.write(args)
inline size_t LiquidCrystal_I2C::write(uint8_t value) {
	send(value, Rs);
	return 0;
}

#else
#include "WProgram.h"

#define printIIC(args)	Wire.send(args)
inline void LiquidCrystal_I2C::write(uint8_t value) {
	send(value, Rs);
}

#endif
#include "Wire.h"



// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

LiquidCrystal_I2C::LiquidCrystal_I2C(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows)
{
  _Addr = lcd_Addr;
  _cols = lcd_cols;
  _rows = lcd_rows;
  _backlightval = LCD_NOBACKLIGHT;
}

uint8_t LiquidCrystal_I2C::init(){
	return init_priv();
}

uint8_t LiquidCrystal_I2C::init_priv()
{
	uint8_t ret;
	
	Wire.begin();
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	ret = begin(_cols, _rows);
	if (ret != 0) return (ret);
	
	// create custom chars
    uint8_t bell[8]     = {0x4, 0xe, 0xe, 0xe, 0x1f,0x0, 0x4};
    uint8_t note[8]     = {0x2, 0x3, 0x2, 0xe, 0x1e,0xc, 0x0};
    uint8_t clock[8]    = {0x0, 0xe, 0x15,0x17,0x11,0xe, 0x0};
    uint8_t smiley[8]   = {0x0, 0x11,0x0, 0x0, 0x11,0xe, 0x0};	
    uint8_t heart[8]    = {0x0, 0xa, 0x1f,0x1f,0xe, 0x4, 0x0};
    uint8_t duck[8]     = {0x0, 0xc, 0x1d,0xf, 0xf, 0x6, 0x0};
    uint8_t check[8]    = {0x0, 0x1, 0x3, 0x16,0x1c,0x8, 0x0};
    uint8_t cross[8]    = {0x0, 0x1b,0xe, 0x4, 0xe, 0x1b,0x0};
    uint8_t retarrow[8] = {0x1, 0x1, 0x5, 0x9, 0x1f,0x8, 0x4};
    uint8_t celcius[8]  = {0x10,0x6, 0x9, 0x8, 0x8, 0x9, 0x6};
    uint8_t pipe[8]     = {0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4};
    
    createChar(lcd_bell,bell);
    createChar(lcd_note,note);
    createChar(lcd_clock,clock);
    createChar(lcd_smiley,smiley);
    createChar(lcd_duck,duck);
    createChar(lcd_celcius,celcius);
    createChar(lcd_pipe,pipe);
    
    home();  
}

uint8_t LiquidCrystal_I2C::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
    uint8_t ret;
    
	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	delay(50); 
  
	// Now we pull both RS and R/W low to begin commands
	ret = expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
    if (ret != 0) return (ret);	
	delay(1000);

  	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46
	
    // we start in 8bit mode, try to set 4 bit mode
    ret = write4bits(0x03 << 4);
    if (ret != 0) return (ret);
    delayMicroseconds(4500); // wait min 4.1ms
   
    // second try
    ret = write4bits(0x03 << 4);
    if (ret != 0) return (ret);
    delayMicroseconds(4500); // wait min 4.1ms
   
    // third go!
    ret = write4bits(0x03 << 4); 
    if (ret != 0) return (ret);
    delayMicroseconds(150);
   
    // finally, set to 4-bit interface
    ret = write4bits(0x02 << 4); 
    if (ret != 0) return (ret);

	// set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);  
	
	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();
	
	// clear it off
	clear();
	
	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	
	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);
	
	home();
  
}

/********** high level commands, for the user! */
void LiquidCrystal_I2C::clear(){
	command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal_I2C::home(){
	command(LCD_RETURNHOME);  // set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal_I2C::setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > _numlines ) {
		row = _numlines-1;    // we count rows starting w/0
	}
	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidCrystal_I2C::noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal_I2C::display() {
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LiquidCrystal_I2C::noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal_I2C::cursor() {
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LiquidCrystal_I2C::noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal_I2C::blink() {
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal_I2C::scrollDisplayLeft(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidCrystal_I2C::scrollDisplayRight(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystal_I2C::leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LiquidCrystal_I2C::rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal_I2C::autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal_I2C::noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LiquidCrystal_I2C::createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		write(charmap[i]);
	}
}

// Turn the (optional) backlight off/on
void LiquidCrystal_I2C::noBacklight(void) {
	_backlightval=LCD_NOBACKLIGHT;
	expanderWrite(0);
}

void LiquidCrystal_I2C::backlight(void) {
	_backlightval=LCD_BACKLIGHT;
	expanderWrite(0);
}



/*********** mid level commands, for sending data/cmds */

inline void LiquidCrystal_I2C::command(uint8_t value) {
	send(value, 0);
}


/************ low level data pushing commands **********/

// write either command or data
uint8_t LiquidCrystal_I2C::send(uint8_t value, uint8_t mode) {
    uint8_t ret;
    
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
    ret = write4bits((highnib)|mode);
    if (ret != 0) return (ret);
    	    
	return write4bits((lownib)|mode); 
}

uint8_t LiquidCrystal_I2C::write4bits(uint8_t value) {
	uint8_t ret;
	
	ret = expanderWrite(value);
	if (ret != 0) return (ret);
	    
	return pulseEnable(value);
}

uint8_t LiquidCrystal_I2C::expanderWrite(uint8_t _data){                                        
	Wire.beginTransmission(_Addr);
	printIIC((int)(_data) | _backlightval);
	return Wire.endTransmission(); 
}

uint8_t LiquidCrystal_I2C::pulseEnable(uint8_t _data){
	uint8_t ret;
	
	ret = expanderWrite(_data | En);	// En high
	if (ret != 0) return (ret);
	    
	delayMicroseconds(1);		// enable pulse must be >450ns
	
	ret = expanderWrite(_data & ~En);	// En low
	delayMicroseconds(50);		// commands need > 37us to settle
	
	return ret;
} 


// Alias functions

void LiquidCrystal_I2C::cursor_on(){
	cursor();
}

void LiquidCrystal_I2C::cursor_off(){
	noCursor();
}

void LiquidCrystal_I2C::blink_on(){
	blink();
}

void LiquidCrystal_I2C::blink_off(){
	noBlink();
}

void LiquidCrystal_I2C::load_custom_character(uint8_t char_num, uint8_t *rows){
		createChar(char_num, rows);
}

void LiquidCrystal_I2C::setBacklight(uint8_t new_val){
	if(new_val){
		backlight();		// turn backlight on
	}else{
		noBacklight();		// turn backlight off
	}
}

void LiquidCrystal_I2C::printstr(const char c[]){
	//This function is not identical to the function used for "real" I2C displays
	//it's here so the user sketch doesn't have to be changed 
	print(c);
}


// unsupported API functions
void LiquidCrystal_I2C::off(){}
void LiquidCrystal_I2C::on(){}
void LiquidCrystal_I2C::setDelay (int cmdDelay,int charDelay) {}
uint8_t LiquidCrystal_I2C::status(){return 0;}
uint8_t LiquidCrystal_I2C::keypad (){return 0;}
uint8_t LiquidCrystal_I2C::init_bargraph(uint8_t graphtype){return 0;}
void LiquidCrystal_I2C::draw_horizontal_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end){}
void LiquidCrystal_I2C::draw_vertical_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_row_end){}
void LiquidCrystal_I2C::setContrast(uint8_t new_val){}
