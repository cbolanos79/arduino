/*
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

// Enable 8 bits or 4 bits operating mode
//#define LCD_8BITS
#define LCD_4BITS

// Enable latched mode using 595 shift register
#define LATCHED

#define LCD_INCREMENT 1
#define LCD_DECREMENT 0
#define LCD_SHIFT 1
#define LCD_NO_SHIFT 0
#define LCD_FONT_5x8 8
#define LCD_FONT_5x10 10

#ifdef LATCHED
  #define LATCH_LATCH 16
  #define LATCH_CLOCK 14
  #define LATCH_DATA 15
#endif

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

// Mask for display command
#define DISPLAY_BITS(x) (0x8|(0x7&(x)))

struct lcd_status {
  // Instead using individual var for each control bit, store them into a 
  // byte array to reduce memory consumption.
  // Individual vars would require 32 bytes of SRAM: 8 bytes * 4 vars = 32 bytes
  // Using just a byte, can fit 8 control bits (a bit more complex to drive)
  byte control_bits;
  /*
     Bits order (LSB first):
     
     Blink cursor: 1 blink, 0 not blink
     Cursor on/off: 1 on, 0 off
     Display on/off: 1 on, 0 off
     Cursor move direction: 1 increment, 0 decrement
  */
                     
} lcd;

void HD44780_init(int lines, int font, struct lcd_status *lcd) {
  delay(25);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  
  #ifdef LATCHED
    pinMode(LATCH_LATCH, OUTPUT);
    pinMode(LATCH_CLOCK, OUTPUT);
    pinMode(LATCH_DATA, OUTPUT);
  #endif
  
  #ifndef LATCHED
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
  #endif

  lcd->control_bits = 0x0;

  HD44780_function_set(lines, font);
  HD44780_display_on(lcd);
  HD44780_entry_mode(LCD_INCREMENT, LCD_NO_SHIFT);
  HD44780_cursor_display_shift(0, 0);
  HD44780_clear_display();

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

void HD44780_display_on(struct lcd_status *lcd) {
  // Set display bit to 1 (0000 X1XX)
  lcd->control_bits |= 0x04;
  
  // Mask control bits so get only 3 LSB bits (00000111): blink, cursor and display
  HD44780_write_instruction(DISPLAY_BITS(lcd->control_bits));
  
}

void HD44780_display_off(struct lcd_status *lcd) {
  // Set display bit to 0 (0000 X0XX)
  lcd->control_bits &= 0x0B;
  
  // Mask control bits so get only 3 LSB bits (00000111): blink, cursor and display
  HD44780_write_instruction(DISPLAY_BITS(lcd->control_bits));
}

void HD44780_cursor_on(struct lcd_status *lcd) {
  // Set cursor bit to 1 (0000 XX1X)
  lcd->control_bits |= 0x02;
  
  // Mask control bits so get only 3 LSB bits (00000111): blink, cursor and display
  HD44780_write_instruction(DISPLAY_BITS(lcd->control_bits));  
}

void HD44780_cursor_off(struct lcd_status *lcd) {
  // Set cursor bit to 0 (0000 XX0X)
  lcd->control_bits &= 0x0D;
  
  // Mask control bits so get only 3 LSB bits (00000111): blink, cursor and display
  HD44780_write_instruction(DISPLAY_BITS(lcd->control_bits));  
}

void HD44780_blink_on(struct lcd_status *lcd) {
  // Set cursor bit to 1 (0000 XXX1)
  lcd->control_bits |= 0x01;
  
  // Mask control bits so get only 3 LSB bits (00000111): blink, cursor and display
  HD44780_write_instruction(DISPLAY_BITS(lcd->control_bits));  
}

void HD44780_blink_off(struct lcd_status *lcd) {
  // Set cursor bit to 0 (0000 XXX0)
  lcd->control_bits &= 0x0E;
  
  // Mask control bits so get only 3 LSB bits (00000111): blink, cursor and display
  HD44780_write_instruction(DISPLAY_BITS(lcd->control_bits));  
}

void HD44780_clear_display() {
  HD44780_write_instruction(0x01);
  delay(2);
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
  #ifndef LATCHED
    digitalWrite(RS, rs);
  #endif
  
  #ifdef LCD_8BITS
  
    #ifdef LATCHED
      digitalWrite(LATCH_LATCH, LOW);

      /*
        Using 595 shift register on 8 bits mode, requires 2 IC's chained, so
        must send data on two steps: 
        RS | ENABLE | 000000
        DATA0 ... DATA7 
        
        DATA needs 8 bits so it takes a full IC
        
        ENABLE is set to HIGH
      */
      
      shiftOut(LATCH_DATA, LATCH_CLOCK, MSBFIRST, rs | 0x2);  
      shiftOut(LATCH_DATA, LATCH_CLOCK, MSBFIRST, data);  
      digitalWrite(LATCH_LATCH, HIGH);
      __asm__("nop\n\t");
      digitalWrite(LATCH_LATCH, LOW);

      /*
        Set ENABLE signal to LOW, so send only RS signal data to shift register:
        0 RS 0 ...0
      */
      shiftOut(LATCH_DATA, LATCH_CLOCK, MSBFIRST, rs);  
      shiftOut(LATCH_DATA, LATCH_CLOCK, MSBFIRST, 0);  
      digitalWrite(LATCH_LATCH, HIGH);
      __asm__("nop\n\t");

    #endif
    
    #ifndef LATCHED
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
    
  #endif
  
  #ifdef LCD_4BITS
  
    #ifdef LATCHED
      digitalWrite(LATCH_LATCH, LOW);
      //digitalWrite(2, rs);
      __asm__("nop\n\t");
      __asm__("nop\n\t");
      int rs_bit;
      
      rs_bit = rs == HIGH?1:0;
      
      /*
        Using 595 shift register on 4 bits mode, just need one shift register but must send data four times:
        
        RS | ENABLE HIGH | MSB data 4 bits (DATA4 ... DATA7)
        RS | ENABLE LOW  | 000000 (doesn't matter)
        RS | ENABLE HIGH | LSB data 4 bits (DATA0 ... DATA3)
        RS | ENABLE LOW  | 000000 (doesn't matter)
      */
 
      // RS | ENABLE HIGH | MSB data (DATA4 ... DATA7)
      shiftOut(LATCH_DATA, LATCH_CLOCK, MSBFIRST, rs_bit | 0x2 | ((data & 0xF0)>>2)); 
      
      digitalWrite(LATCH_LATCH, HIGH);
      __asm__("nop\n\t");
      digitalWrite(LATCH_LATCH, LOW);
      __asm__("nop\n\t");
      delay(10);
      digitalWrite(3, HIGH);
      __asm__("nop\n\t");
      digitalWrite(3, LOW);

      // RS | ENABLE LOW | 000000
      
      shiftOut(LATCH_DATA, LATCH_CLOCK, MSBFIRST, rs_bit);  
      digitalWrite(LATCH_LATCH, HIGH);
      __asm__("nop\n\t");
      digitalWrite(LATCH_LATCH, LOW);
      __asm__("nop\n\t");
      
      
      // Return if only write one instruction (required for function set on 4 bits operation mode)
      if (one_write)
        return;

      // RS | ENABLE HIGH | LSB data (DATA0 ... DATA3)
      shiftOut(LATCH_DATA, LATCH_CLOCK, MSBFIRST, rs | 0x2 | ((data & 0x0F)<<2));  
      //shiftOut(LATCH_DATA, LATCH_CLOCK, MSBFIRST, (data & 0x0F)<<2);  
      
      digitalWrite(LATCH_LATCH, HIGH);
      __asm__("nop\n\t");
      digitalWrite(LATCH_LATCH, LOW);
      __asm__("nop\n\t");
      delay(10);
      digitalWrite(3, HIGH);
      __asm__("nop\n\t");
      digitalWrite(3, LOW);


      // RS | ENABLE LOW | 000000
      shiftOut(LATCH_DATA, LATCH_CLOCK, MSBFIRST, rs);  
      digitalWrite(LATCH_LATCH, HIGH);
      __asm__("nop\n\t");
      digitalWrite(LATCH_LATCH, LOW);
      __asm__("nop\n\t");


    #endif
    
    #ifndef LATCHED
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
  #endif
}

void setup() {
  // Init lcd using 2 lines
  HD44780_init(2, LCD_FONT_5x8, &lcd);

  HD44780_set_cursor(1,1);
  HD44780_write_string("Hello world", 11);
  HD44780_set_cursor(2,1);
  HD44780_write_string("See you!", 8);
}

void loop() {
  HD44780_blink_on(&lcd);
  delay(1000);
  HD44780_blink_off(&lcd);
  delay(1000);
  HD44780_cursor_on(&lcd);
  delay(1000);
  HD44780_cursor_off(&lcd);
  delay(1000);
  HD44780_display_off(&lcd);
  delay(1000);
  HD44780_display_on(&lcd);
  delay(1000);
  
}


