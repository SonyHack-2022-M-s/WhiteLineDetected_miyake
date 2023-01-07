
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
uint16_t *bmp , *pMain;
uint16_t pix,pixR,pixG,pixB,pixAve = 0;
uint32_t pixSum1 = 0, *pixSum,pixAve32 = 0;
int i = 0,j = 0,k = 0;

unsigned short int count = 0, co[3];

#include <MP.h>
//#include <inttypes.h>
int     ret;
uint32_t  msg = 0;
int8_t   msgid = 0;

void setup() { 
  MP.begin();
}

void loop() {
  ret = MP.Recv(&msgid, &bmp);
 //   MPLog("Recv bmp %d\n",ret);
  ret = MP.Recv(&msgid, &pMain);
 //   MPLog("Recv pMain %d\n",ret);

  for (i = 0; i < pixsize ; i++){
    pixR = (*bmp & 0xF800)>>11;
    pixG = (*bmp & 0x7E0)>>6;
    pixB = (*bmp & 0x1F);
    *pMain = pixR>pixG ? (pixG>pixB ? pixB:pixG) : (pixR>pixB ? pixB:pixR);
    pixSum1 += (uint32_t)*pMain;
    pMain++;
    bmp++;
  }
  pMain -= pixsize;
  bmp -= pixsize;
  
  pixSum1 = pixSum1>>1;


 // MPLog("time1 %"PRIu64"\n",micros());
  ret = MP.Send(msgid, pixSum1);
 //   MPLog("Send pixSum1 %d\n",ret);
  ret = MP.Recv(&msgid, &pixAve32);
 //   MPLog("Recv pixSum2 %d\n",ret);

  pixAve = (uint16_t)pixAve32;
  
  //MPLog("pixSum1=%"PRIu32"\n", pixSum1);
  //MPLog("pixSum2=%"PRIu32"\n", pixSum2);
  //MPLog("pixAve=%lu\n", pixAve);

  for (i = 0; i < pixsize ; i++){
    *pMain = *pMain>pixAve ? 0x1F : 0;
    pMain++;
    bmp++;
  }
  pMain -= pixsize;
  bmp -= pixsize;

  //FILTERRRRRRRRRRRRRRRRR
  ret = MP.Send(msgid, msg);
  ret = MP.Recv(&msgid, &msg);
 //MPLog(" filterST %d\n",ret);

  co[3] = {0};
  if(*(pMain+320-320)==0){co[0]++;}
  if(*(pMain+320)==0){co[0]++;}
  if(*(pMain+320+320)==0){co[0]++;}
  
  if(*(pMain+321-320)==0){co[1]++;}
  if(*(pMain+321)==0){co[1]++;}
  if(*(pMain+321+320)==0){co[1]++;}

  if(*(pMain+322-320)==0){co[2]++;}
  if(*(pMain+322)==0){co[2]++;}
  if(*(pMain+322+320)==0){co[2]++;}

  for(i = 321; i < pixsize ; i++){
    *(pMain+i) = (co[0]+co[1]+co[2])>4 ? 0:monocolor;
    
    co[count] = 0;
    if(*(pMain+i+2-320)==0){co[count]++;}
    if(*(pMain+i+2)==0){co[count]++;}
    if(*(pMain+i+2+320)==0){co[count]++;}
    
    count = (count+1)%3;
    }

  co[3] = {0};
  if(*(pMain+320-320)==0){co[0]++;}
  if(*(pMain+321-320)==0){co[1]++;}
  if(*(pMain+322-320)==0){co[2]++;}
  if(*(pMain+320)==0){co[0]++;}
  if(*(pMain+321)==0){co[1]++;}
  if(*(pMain+322)==0){co[2]++;}
  if(*(pMain+320+320)==0){co[0]++;}
  if(*(pMain+321+320)==0){co[1]++;}
  if(*(pMain+322+320)==0){co[2]++;}

  for(i = 321; i < pixsize ; i++){
    *(pMain+i) = (co[0]+co[1]+co[2])>4 ? 0:monocolor;
    
    co[count] = 0;
    if(*(pMain+i+2-320)==0){co[count]++;}
    if(*(pMain+i+2)==0){co[count]++;}
    if(*(pMain+i+2+320)==0){co[count]++;}
    
    count = (count+1)%3;
    }

//  tft.drawRGBBitmap(0, 0, pMain, 320, 240);
//MPLog("time2 %"PRIu64"\n",micros());
  ret = MP.Send(msgid, 10);
  // put your main code here, to run repeatedly:
}
