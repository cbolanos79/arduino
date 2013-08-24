Just another LCD HD44780 based sketch for arduino.
=======================================
 
This sketch is intented for reduced learning purposes and reducing
memory consumption so may be usable on a tiny45/tiny85 uC
Instead using classes and library use a procedures and preprocessor directives
  
##Typical LCD pinout:
*    1 VSS
*    2 VCC
*    3 VEE
*    4 RS
*    5 R/W // Connect to ground
*    6 E
*    7 DB0
*    8 DB1
*    9 DB2
*   10 DB3 // DB0 - DB3 are not used on 4 bits mode
*   11 DB4
*   12 DB5 
*   13 DB6
*   14 DB7
*   15 LED+ // May vary
*   16 LED- // May vary
 
###8 bits mode
Just place this line on the code

    #define LCD_8BITS

**Remember: there must be only one mode defined**

###4 bits mode
Just place this line on the code

    #define LCD_4BITS

**Remember: there must be only one mode defined**

Latched mode
--------------------
  There are two modes available: 8 bits and 4 bits
  You should use always 4 bits mode unless you have any good reason (8 bits mode require two shift registers).
###8 bits
  *Must use two chained shift registers: first one for data bus and second one for Enable and RS signals*
  
Place this line on the code to enable the latched mode:

    #define LATCHED

Shift registers pinout

*    First shift register
    *    DS -> Arduino LATCH_DATA
    *    SHCP -> Arduino LATCH_CLOCK
    *    STCP -> Arduino LATCH_LATCH
    *    VCC -> +5V
    *    GND -> GND
    *    MR -> +5V
    *    OE -> GND
    *    Q0 -> D0
    *    Q1 -> D1
    *    Q2 -> D2
    *    Q3 -> D3
    *    Q4 -> D4
    *    Q5 -> D5
    *    Q6 -> D6
    *    Q7 -> D7
    *    Q7S -> second shift register DS input
       
*    Second shift register
    *    DS -> First shift register Q7S output
    *    SHCP -> Arduino LATCH_CLOCK
    *    STCP -> Arduino LATCH_LATCH
    *    VCC -> +5V
    *    GND -> GND
    *    MR -> +5V
    *    OE -> GND     
    *    Q0 -> RS
    *    Q1 -> Enable

###4 bits
  *Must use only one latch
  
Place this line on the code to enable the latched mode:

    #define LATCHED

*    Shift register
    *    DS -> Arduino LATCH_DATA
    *    SHCP -> Arduino LATCH_CLOCK
    *    STCP -> Arduino LATCH_LATCH
    *    VCC -> +5V
    *    GND -> GND
    *    MR -> +5V
    *    OE -> GND
    *    Q0 -> D4
    *    Q1 -> D5
    *    Q2 -> D6
    *    Q3 -> D7
    *    Q4 -> RS
    *    Q5 -> ENABLE
    *    Q6 -> Not used
    *    Q7 -> Not used
    *    Q7S -> Not used
       
#TODO
   - Add support to drive LCD using MCP23017/MCP23008 (reduces pinout consumption, more expensive but allow reading inputs) on both 8 bits and 4 bits mode 
   - Write a library instead a sketch
 
#CHANGELOG
*    v0.1 Initial release supporting basic 8 bits mode and major instruction set except for custom character generator
*    v0.2 Support for 4 bits mode
*    v0.3 Added functions to display on/off, cursor on/off, blink cursor on/off. 
    *    Fixed error using clear_display()
    *    Clear display on init()
*    v0.4 Support for 8 bits mode latched (595 shift register)
*    v0.41 Support for 4 bits mode latched (595 shift register)

#Licence
 
_Copyright (c) 2013, Cristo Saulo Bolaños Trujillo <cbolanos@gmail.com>
All rights reserved._

_Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:_

*    _Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer._
*    _Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution._
*    _Neither the name of the <organization> nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission._

_THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE._
