/*
 Just another lcd library for arduino.
 
 This sketch is intented for reduced learning purposes and reducing
 memory consumption so may be usable on a tiny45/tiny85 uC
 Instead using classes and library use a procedures and preprocessor directives
  
 Typical LCD pinout (15 and 16 may vary):
 
  1 VSS
  2 VCC
  3 VEE
  4 RS
  5 R/W
  6 E
  7 DB0
  8 DB1
  9 DB2
 10 DB3
 11 DB4
 12 DB5
 13 DB6
 14 DB7
 15 LED+
 16 LED-
 
 TODO:
   - Add support to drive LCD using 595 shift register (reduces pinout consumption, cheap) on both 8 bits and 4 bits mode
   - Add support to drive LCD using MCP23017/MCP23008 (reduces pinout consumption, more expensive but allow reading inputs)
     on both 8 bits and 4 bits mode
   - Add support for character generation
 
 Copyright (c) 2013, Cristo Saulo Bolaños Trujillo <cbolanos@gmail.com>
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

void HD44780_write_instruction(int data, bool one_read=false);
void HD44780_write(int rs, int data, bool one_write=false);

#define LCD_INCREMENT 1
#define LCD_DECREMENT 0
#define LCD_SHIFT 1
#define LCD_NO_SHIFT 0

// Enable 8 bits or 4 bits operating mode
//#define LCD_8BITS
#define LCD_4BITS

#ifdef LCD_8BITS
  #define RS 2
  #define E 3
  #define DB0 4
  #define DB1 5
  #define DB2 6
  #define DB3 7
  #define DB4 8
  #define DB5 9
  #define DB6 10
  #define DB7 11
#endif

#ifdef LCD_4BITS
  #define RS 2
  #define E 3
  #define DB4 8
  #define DB5 9
  #define DB6 10
  #define DB7 11
#endif
  
void HD44780_init(int lines) {
  delay(25);
  
  pinMode(RS, OUTPUT);
  pinMode(E, OUTPUT);
  
  #ifdef LCD_8BITS
    pinMode(DB0, OUTPUT);
    pinMode(DB1, OUTPUT);
    pinMode(DB2, OUTPUT);
    pinMode(DB3, OUTPUT);
  #endif
  pinMode(DB4, OUTPUT);
  pinMode(DB5, OUTPUT);
  pinMode(DB6, OUTPUT);
  pinMode(DB7, OUTPUT);

  HD44780_function_set(2, 8);
  HD44780_display_control(true, false, false);
  HD44780_entry_mode(LCD_INCREMENT, LCD_NO_SHIFT);
  HD44780_cursor_display_shift(0, 0);
  
}

void HD44780_function_set(int lines, int char_font) {
  int data = 0x20; // DL = 0, N = 0, F = 0
  
  // Set DL value (8 bits = 1, 4 bits = 0)
  #ifdef LCD_8BITS
    data |= 0x10;
  #endif
  
  // Set N value (2 lines = 1, 1 line = 0)
  if (lines==2)
    data |= 0x08;
  
  // Set char font
  if (char_font==8)
    data |= 0x04;

  #ifdef LCD_8BITS
    HD44780_write_instruction(data);  
  #endif
  
  #ifdef LCD_4BITS
    // According to HD44780U datasheet, when operating in 4 bits mode,
    // must write only one instruction on MSB with 0010 bits (4 bits mode)
    // and after, write full data again
    HD44780_write_instruction(0x20, true);
    delay(1);
    HD44780_write_instruction(data);  
  #endif
}

void HD44780_display_control(bool disp, bool curs, bool blinking) {
  
  int data = 0x08; // D = 0, C = 0, B = 0
  
  // Set D value (display on = 1, display off = 0)
  if (disp)
    data |= 0x04;
  
  // Set C value (cursor on = 1, cursor off = 0)
  if (curs)
    data |= 0x02;
    
  // Set B value (blinking on = 1, blinking off = 0)
  if (blinking)
    data |= 0x01;
  
  HD44780_write_instruction(data);
}

void HD44780_clear_display() {
  HD44780_write_instruction(0x01); 
}

void HD44780_entry_mode(int dir, bool shift) {
  // left, not shift
  int data = 0x04;
  
  // Set cursor direction (left == 0, right == 1)
  if (dir==1)
    data |= 0x02;
    
  // Set display shift
  if (shift)
    data |= 0x01;
    
  HD44780_write_instruction(data);     
}

void HD44780_cursor_display_shift(int dc, int dir) {
  // cursor move, shift left
  int data = 0x0;
  
  if (dc)
    data |= 0x08;
    
  if (dir)
    data |= 0x04;
    
  HD44780_write_instruction(data); 
}

void HD44780_return_home() {
  HD44780_write_instruction(0x02);  
}

void HD44780_set_ddram_address(int addr) {
  HD44780_write_instruction(0x80|addr);  
}

void HD44780_write_instruction(int data, bool one_write) {
  HD44780_write(LOW, data, one_write);
}

void HD44780_write_char(char c) {
  HD44780_write_data(c);
}

void HD44780_write_string(char *s, int length) {
  for (int i=0; i<length; i++)
    HD44780_write_char(s[i]);
}

void HD44780_set_cursor(int line, int col) {
  int dest=0;

  if (line == 2)
    dest = 0x40;
    
  dest += (col-1);
  
  HD44780_set_ddram_address(dest);
}

void HD44780_write_data(int data) {
  HD44780_write(HIGH, data); 
}

void HD44780_write(int rs, int data, bool one_write) {
  digitalWrite(RS, rs);
  
  #ifdef LCD_8BITS
    digitalWrite(DB0, data&0x1 ? HIGH: LOW);
    digitalWrite(DB1, data&0x2 ? HIGH: LOW);
    digitalWrite(DB2, data&0x4 ? HIGH: LOW);
    digitalWrite(DB3, data&0x8 ? HIGH: LOW);
    digitalWrite(DB4, data&0x10 ? HIGH: LOW);
    digitalWrite(DB5, data&0x20 ? HIGH: LOW);
    digitalWrite(DB6, data&0x40 ? HIGH: LOW);
    digitalWrite(DB7, data&0x80 ? HIGH: LOW);
  
    // Clock pulse of E signal HIGH->LOW
    __asm__("nop\n\t"); 
    digitalWrite(E, HIGH);
    __asm__("nop\n\t"); 
    digitalWrite(E, LOW);  
  #endif
  
  #ifdef LCD_4BITS
  
    // On 4 bits operation mode, must load first 4 MSB and after 4 LSB
    digitalWrite(DB4, data&0x10 ? HIGH: LOW);
    digitalWrite(DB5, data&0x20 ? HIGH: LOW);
    digitalWrite(DB6, data&0x40 ? HIGH: LOW);
    digitalWrite(DB7, data&0x80 ? HIGH: LOW);

    // Clock pulse of E signal HIGH->LOW
    __asm__("nop\n\t"); 
    digitalWrite(E, HIGH);
    __asm__("nop\n\t"); 
    digitalWrite(E, LOW);
    delay(1);  
    
    // Return if only write one instruction (required for function set on 4 bits operation mode)
    if (one_write)
      return;

    digitalWrite(DB4, data&0x1 ? HIGH: LOW);
    digitalWrite(DB5, data&0x2 ? HIGH: LOW);
    digitalWrite(DB6, data&0x4 ? HIGH: LOW);
    digitalWrite(DB7, data&0x8 ? HIGH: LOW);
    
    // Clock pulse of E signal HIGH->LOW
    __asm__("nop\n\t"); 
    digitalWrite(E, HIGH);
    __asm__("nop\n\t"); 
    digitalWrite(E, LOW);  
  #endif   
}

void setup() {
  // Init lcd using 2 lines
  HD44780_init(2);
}

void loop() {  
  HD44780_write_string("Hello world", 11);
  delay(1000);
  HD44780_set_cursor(2,1);
  HD44780_write_string("See you!", 8);
  delay(1000);
  HD44780_clear_display();
  delay(1000);
}


