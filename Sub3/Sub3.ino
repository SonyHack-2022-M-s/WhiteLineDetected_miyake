#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define TFT_RST 8
#define TFT_DC  9
#define TFT_CS  10

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS ,TFT_DC ,TFT_RST);

uint16_t *pMain;

#include <MP.h>
int8_t     MY_MSGID =   10;
int     ret;
uint32_t msg = 0;
uint64_t myTime1,myTime2;
int      subid;
int8_t   msgid;


#include <inttypes.h>

void setup() {
  MP.begin();
  ret = MP.Send(MY_MSGID, msg);
  tft.begin();
  tft.setRotation(3);
}

void loop() {
  ret = MP.Recv(&msgid, &pMain);
  // put your main code here, to run repeatedly:
  tft.drawRGBBitmap(0, 0, pMain, 320, 240);
  ret = MP.Send(MY_MSGID, msg);
}
