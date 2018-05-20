/***************************************************
  This is our touchscreen painting example for the Adafruit ILI9341 Breakout
  ----> http://www.adafruit.com/products/1770

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include <Adafruit_GFX.h>    // Core graphics library
#include <gfxfont.h>
#include <TouchScreen_STM.h>
// if you get uint8_t error, go to TouchScreen_STM.cpp and comment #include "pins_arduino.h"
// also, you may try changing   #include <WProgram.h> to  #include <Arduino.h>
#include "Adafruit_ILI9341_8bit_STM.h"


/* --------- LCD-STM32 wiring:

8 bit parallel interface

Data pins
Port data |D7 |D6 |D5 |D4 |D3 |D2 |D1 |D0 |
Pin stm32 |PB15|PB14|PB13|PB12|PB11|PB10|PB9|PB8|

Control pins |RD |WR |RS |CS |RST|
Pin stm32    |PA0|PA1|PA2|PA3|PA8|

This lcd has 4-wire resisitive touch multiplexed with other tft pins:
YP is LCD_RS
XM is LCD_CS
YM is LCD_D0
XP is LCD_D1

 * Data lines D0...D7 are switched through U3, which is controlled by the LCD_RD signal.
   Make sure that this pin is set to "high" ("1") to use the touch pins.
 */

// These are the pins for the shield!
#define LCD_RD PA0

#define LCD_RS PA2
#define LCD_CS PA3
#define LCD_D0 PB8
#define LCD_D1 PB9

#define YP LCD_RS  // must be an analog pin
#define XM LCD_CS  // must be an analog pin
#define YM LCD_D0   // can be a digital pin
#define XP LCD_D1   // can be a digital pin

#define XMIN 500
#define XMINBLACK 320
#define XMAX 3740
#define YMIN 420
#define YMAX 3640

#define MINPRESSURE 10 //20
#define MAXPRESSURE 1000

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
Adafruit_ILI9341_8bit_STM tft = Adafruit_ILI9341_8bit_STM();


// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;

 int16_t touchX,touchY;

//rotations
#define PORTRAIT 0
#define LANDSCAPE 1

void setup(void) {
Serial.begin(115200);
  delay(5);
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(LANDSCAPE);
  delay(1000);
  // make the color selection boxes
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
  tft.fillRect(0, BOXSIZE, BOXSIZE, BOXSIZE, ILI9341_YELLOW);
  tft.fillRect(0, BOXSIZE*2, BOXSIZE, BOXSIZE, ILI9341_GREEN);
  tft.fillRect(0, BOXSIZE*3, BOXSIZE, BOXSIZE, ILI9341_CYAN);
  tft.fillRect(0, BOXSIZE*4, BOXSIZE, BOXSIZE, ILI9341_BLUE);
  tft.fillRect(0, BOXSIZE*5, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);

  // select the current color 'red'
  tft.drawRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
  currentcolor = ILI9341_RED;
}

uint16_t calculateX(uint16_t x)
{
  return 320-(320*(x-XMIN))/(XMAX-XMIN);
}
uint16_t calculateY(uint16_t y)
{
  return (240*(y-YMIN))/(YMAX-YMIN);
}



boolean readTouch(){
  // Retrieve a point
  pinMode(LCD_RD,OUTPUT);
  digitalWrite(LCD_RD, HIGH);
  TSPoint p = ts.getPoint();
  //set controlpins to output again
  TFT_CNTRL->regs->CRL = (TFT_CNTRL->regs->CRL & 0xFFFF0000) | 0x00003333;

  Serial.print("touchX = "); Serial.print(touchX);
  Serial.print("\ttouchY = "); Serial.print(touchY);
  Serial.print("\tPressure = "); Serial.println(p.z);

 if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
   touchX=calculateX(p.x);
   touchY=calculateY(p.y);
  //Serial.print("X = "); Serial.print(x);
   //Serial.print("\tY = "); Serial.print(y);
   //Serial.print("\tPressure = "); Serial.println(p.z);
   return true;
}
else return false;
}

void loop()
{
if(readTouch()) {}
else return;

  if (touchX< BOXSIZE) {
     oldcolor = currentcolor;

     if (touchY < BOXSIZE) {
       currentcolor = ILI9341_RED;
       tft.drawRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
     } else if (touchY < BOXSIZE*2) {
       currentcolor = ILI9341_YELLOW;
       tft.drawRect(0,BOXSIZE, BOXSIZE, BOXSIZE, ILI9341_WHITE);
     } else if (touchY < BOXSIZE*3) {
       currentcolor = ILI9341_GREEN;
       tft.drawRect(0,BOXSIZE*2, BOXSIZE, BOXSIZE, ILI9341_WHITE);
     } else if (touchY < BOXSIZE*4) {
       currentcolor = ILI9341_CYAN;
       tft.drawRect(0,BOXSIZE*3, BOXSIZE, BOXSIZE, ILI9341_WHITE);
     } else if (touchY < BOXSIZE*5) {
       currentcolor = ILI9341_BLUE;
       tft.drawRect(0,BOXSIZE*4, BOXSIZE, BOXSIZE, ILI9341_WHITE);
     } else if (touchY < BOXSIZE*6) {
       currentcolor = ILI9341_MAGENTA;
       tft.drawRect(0,BOXSIZE*5, BOXSIZE, BOXSIZE, ILI9341_WHITE);
     }

     if (oldcolor != currentcolor) {
        if (oldcolor == ILI9341_RED)
          tft.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
        if (oldcolor == ILI9341_YELLOW)
          tft.fillRect(0,BOXSIZE,BOXSIZE, BOXSIZE, ILI9341_YELLOW);
        if (oldcolor == ILI9341_GREEN)
          tft.fillRect(0,BOXSIZE*2, BOXSIZE, BOXSIZE, ILI9341_GREEN);
        if (oldcolor == ILI9341_CYAN)
          tft.fillRect(0,BOXSIZE*3, BOXSIZE, BOXSIZE, ILI9341_CYAN);
        if (oldcolor == ILI9341_BLUE)
          tft.fillRect(0,BOXSIZE*4, BOXSIZE, BOXSIZE, ILI9341_BLUE);
        if (oldcolor == ILI9341_MAGENTA)
          tft.fillRect(0,BOXSIZE*5, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);
     }
  }
  tft.fillCircle(touchX, touchY, PENRADIUS, currentcolor);

}
