/*************************************************** 
  This is a example sketch demonstrating bitmap drawing
  capabilities of the SSD1331 library  for the 0.96" 
  16-bit Color OLED with SSD1331 driver chip

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/684

  These displays use SPI to communicate, 4 or 5 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution

  The Adafruit GFX Graphics core library is also required
  https://github.com/adafruit/Adafruit-GFX-Library
  Be sure to install it!
 ****************************************************/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SD.h>
#include <SPI.h>


// If we are using the hardware SPI interface, these are the pins (for future ref)

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

// GND (G) - Gnd (Black Wire)
// VCC (+) - 5v (Red Wire)
// SDCS (SC) - Digital #53 (Gray Wire)
// OCS (OC) - Digital #47 (Orange Wire)
// RST (R) - Digital #49 (Green Wire)
// D/C (DC) - Digital #48 (Brown Wire)
// SCK (CK) - Digital #52 (White Wire)
// MOSI (SI) - Digital #51 (Blue Wire)
// MISO (SO) - Digital #50 (Yellow Wire)
// CD (CD) - skip

#define miso 50
#define mosi 51
#define sclk 52
#define cs   47
#define dc   48
#define rst  49
#else  // defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define sclk 13
#define mosi 11
#define cs   10
#define rst  9
#define dc   8
#endif

// Color definitions
#define  BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

// to draw images from the SD card, we will share the hardware SPI interface
Adafruit_SSD1331 tft = Adafruit_SSD1331(cs, dc, rst);  

// For Arduino Uno/Duemilanove, etc
//  connect the SD card with MOSI going to pin 11, MISO going to pin 12 and SCK going to pin 13 (standard)
//  Then pin 4 goes to CS (or whatever you have set up)
#define SD_CS 53    // Set the chip select line to whatever you use (4 doesnt conflict with the library)

// the file itself
File bmpFile;

// information we extract about the bitmap file
int bmpWidth, bmpHeight;
uint8_t bmpDepth, bmpImageoffset;

void testdrawtext(char *text, uint16_t color) {
  tft.setTextSize(1);
  tft.setTextColor(color);
  tft.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    tft.write(i);
    if ((i > 0) && (i % 21 == 0))
      tft.println();
  }    
}

void setup(void) {
  Serial.begin(9600);

  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
     
  // initialize the OLED
  tft.begin();

  Serial.println("init");
  
  tft.fillScreen(BLUE);
  testdrawtext("Hello", YELLOW);
  
  delay(500);
  Serial.print("Initializing SD card...");

  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
    return;
  }
  Serial.println("SD OK!");

}

void loop() {
  b2dPlayMovie("Leia.bd2");
  b2dPlayMovie("Plans.bd2");
  b2dPlayMovie("R2.bd2");
}

boolean bd2DrawFrame(File fd)
{
  uint16_t sdbuffer[96];
  int frameType = (char)fd.read();
  if (frameType == -1)
  {
    /* Keyframe */
    for (int y = 0; y < 64; y++)
    {
      int num = fd.read(sdbuffer, sizeof(sdbuffer));
      if (num != sizeof(sdbuffer))
      {
        Serial.println(F("Error reading key frame"));
        Serial.print(F("Incomplete scanline: "));
        Serial.println(y);
        return false;
      }
      tft.setAddrWindow(0, y, 96, 1);
      tft.writePixels(sdbuffer, 96);
      tft.endWrite();
    }
  }
  else if (frameType == 1)
  {
    /* Incremental */
    int x = fd.read();
    int y = fd.read();
    int w = fd.read();
    int h = fd.read();
    while (h-- > 0)
    {
      int num2 = fd.read(sdbuffer, w*sizeof(uint16_t));
      if (num2 != w*sizeof(uint16_t))
      {
        Serial.println(F("Error reading frame"));
        Serial.print(F("Incomplete scanline: "));
        Serial.println(y);
        return false;
      }
      tft.setAddrWindow(x,y,w,1);
      tft.writePixels(sdbuffer, w);
      tft.endWrite();
      y++;
    }
  }
  return true;
}

void b2dPlayMovie(const char* filename)
{
  Serial.print("Play: ");
  Serial.println(filename);
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File not found");
    return;
  }
  uint32_t timeStamp = 0;
  uint32_t frameCount = read32(bmpFile);
  uint32_t frameCountRemaining = frameCount;
  uint32_t fps  = read32(bmpFile);
  uint32_t timeStart = millis();
  while (frameCountRemaining > 0)
  {
    // Frame rate is 10fps which is about the max the Arduino can push
    if (millis() - timeStamp >= 100)
    {
      timeStamp = millis();
      if (!bd2DrawFrame(bmpFile))
        break;
      frameCountRemaining--;
    }
  }
  uint32_t timeElapsed = timeStamp-timeStart;
  Serial.print("Time elapsed: ");
  Serial.print(timeElapsed);
  Serial.print("ms ");
  Serial.print((float)frameCount / ((timeElapsed)/1000.0));
  Serial.println(" fps");
  bmpFile.close();
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
