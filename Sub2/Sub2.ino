
#include <SPI.h>
#include <inttypes.h>
//#include "Adafruit_GFX.h"
//#include "Adafruit_ILI9341.h"

//#define TFT_RST 8
//#define TFT_DC  9
//#define TFT_CS  10
#define monocolor 0x7E0
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS ,TFT_DC ,TFT_RST);

#define pixsize  38400 //76800/2
uint16_t bmpMain[pixsize] , *bmp , *pG , *pMain ,Medi9[9];
uint16_t pix,pixR,pixG,pixB,pixAve = 0,tmp=0,color;
uint32_t pixSum,pixSum2 = 0,pixAve32 = 0;
int i = 0,j = 0,k = 0,count = 0,h=0,ti=0,rightpoint=0,centerpoint=0,leftpoint=0;
float m=0,delta1=0,delta2=0;

#include <MP.h>
//#include <inttypes.h>
int8_t     MY_MSGID =   10;
int     ret;
int      subid;
int8_t   msgid;

void setup() { 
  MP.begin();
}
/*
void loop(){
  ret = MP.Recv(&msgid, &bmp);
    MPLog("Recv %d\n",ret);
  ret = MP.Recv(&msgid, &pMain);
    MPLog("Recv %d\n",ret);
    pMain+=pixsize;
    bmp+=pixsize;
  for (i = 0; i < pixsize ; i++){
    pixR = (*bmp & 0xF800)>>11;
    pixG = (*bmp & 0x7E0)>>6;
    pixB = (*bmp & 0x1F);
    *pMain = pixR>pixG ? (pixG>pixB ? pixB:pixG) : (pixR>pixB ? pixB:pixR);
    pixSum2 += (uint32_t)*pMain;
    pMain++;
    bmp++;
  }
  ret = MP.Send(MY_MSGID, 10);
  }
*/

void loop(){
  ret = MP.Recv(&msgid, &bmp);
 //   MPLog("Recv bmp %d\n",ret);
  ret = MP.Recv(&msgid, &pMain);
 //   MPLog("Recv pMain %d\n",ret);
  
    pMain+=pixsize;
    bmp+=pixsize;
  for (i = 0; i < pixsize ; i++){
    pixR = (*bmp & 0xF800)>>11;
    pixG = (*bmp & 0x7E0)>>6;
    pixB = (*bmp & 0x1F);
    *pMain = pixR>pixG ? (pixG>pixB ? pixB:pixG) : (pixR>pixB ? pixB:pixR);
    pixSum2 += (uint32_t)*pMain;
    pMain++;
    bmp++;
  }
  pMain -= pixsize;
  bmp -= pixsize;
  
  pixSum2 = pixSum2/2;


  //MPLog("time2 %"PRIu64"\n",micros());
  ret = MP.Send(MY_MSGID, pixSum2);
 // MPLog("Send pixSum2 %d\n",ret);
  ret = MP.Recv(&msgid, &pixAve32);
 // MPLog("Recv pixSum1 %d\n",ret);


  pixAve = (uint16_t)pixAve32;
  
  //MPLog("pixSum1=%"PRIu32"\n", pix2Sum1);
  //MPLog("pixSum2=%"PRIu32"\n", pix2Sum2);
  //MPLog("pixAve=%lu\n", pixAve);
  //MPLog("pixAve=%lu\n", pixSig);

  for (i = 0; i < pixsize ; i++){
    *pMain = *pMain>pixAve ? 0x1F : 0;
    pMain++;
    bmp++;
  }
  pMain -= pixsize;
  bmp -= pixsize;

//  tft.drawRGBBitmap(0, 0, pMain, 320, 240);
//MPLog("time2 %"PRIu64"\n",micros());
  ret = MP.Send(MY_MSGID, 10);
  // put your main code here, to run repeatedly:
}
