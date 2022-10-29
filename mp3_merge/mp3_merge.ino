/*
//カメラのプレビュー画像から白線認識を行い，LCDに検出された白線を表示します．
//車道左端から離れすぎると赤，
//逆走している(右に白線がいる)と青，
//正常な位置で紫，
//の直線がLCDにマークされます．
//"Adafruit_GFX.h"，"Adafruit_ILI9341.h"を使用しています．
*/
#include <Camera.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <SDHCI.h>
#include <Audio.h>
#include <String.h>
SDClass theSD;
AudioClass *theAudio = AudioClass::getInstance();
String filename1 = "Slipping.mp3";
String filename2 = "Reversing.mp3";



#define TFT_RST 8
#define TFT_DC  9
#define TFT_CS  10
#define monocolor 0x7E0

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS ,TFT_DC ,TFT_RST);

#define pixsize  76800
uint16_t bmpMain[pixsize] , *bmp , *pG , *pMain ,Medi9[9];
uint16_t pix,pixR,pixG,pixB,pixMax = 0,pixMini = 0x1F,tmp=0,color;
int i = 0,j = 0,k = 0,count = 0,h=0,ti=0,rightpoint=0,centerpoint=0,leftpoint=0, Flag = 0;
float m=0,delta1=0,delta2=0;

/*
void straight(){
  for (i = 0; i <320; i++){
    for (m = -100; m < 1; m++){
      count = 0;
      delta1 = 0.00147*m;
      delta2 = 0;
      if((int)(i/(-delta1)) < 240){continue;}
      for (h = 0; h < 240 ; h++){
        if(*((pMain+i)+(uint32_t)delta2) == 0xF800){count++;}
        delta2 = delta2+320+delta1;
        }
      if(count > 80){
        delta2 = 0;
        for (h = 0; h < 240 ; h++){
          *((pMain+i)+(uint32_t)delta2) = 0x7E0;
          delta2 = delta2+320+delta1;
          }
        }
      }
    }
  }
*/

void straight2(){
    centerpoint = 0;
    rightpoint = 0;
    leftpoint = 0;

  j = 100;
  k = 0;
  for (m = -300; m < 200; m++){
  count = 0;
  delta1 = 0.00147*m;
  delta2 = 0;
  for (h = 120; h < 240 ; h++){
    if(*((pMain+38400+i)-2+(uint32_t)delta2) == 0x7E0){k++;}
    if(*((pMain+38400+i)-1+(uint32_t)delta2) == 0x7E0){k++;}
    if(*((pMain+38400+i)+(uint32_t)delta2) == 0x7E0){k++;}
    if(*((pMain+38400+i)+1+(uint32_t)delta2) == 0x7E0){k++;}
    if(*((pMain+38400+i)+2+(uint32_t)delta2) == 0x7E0){k++;}
    if(k){
      count++;
      k = 0;
      }
    delta2 = delta2+320+delta1;
    }
  delta2 = 0;
  for (h = 120; h > 0 ; h--){
    if(*((pMain+38400+i)-2-(uint32_t)delta2) == 0x7E0){k++;}
    if(*((pMain+38400+i)-1-(uint32_t)delta2) == 0x7E0){k++;}
    if(*((pMain+38400+i)-(uint32_t)delta2) == 0x7E0){k++;}
    if(*((pMain+38400+i)+1-(uint32_t)delta2) == 0x7E0){k++;}
    if(*((pMain+38400+i)+2-(uint32_t)delta2) == 0x7E0){k++;}
    if(k){
      count++;
      k = 0;
      }
    delta2 = delta2+320+delta1;
    }
    if (count >= j){
      j = count;
      continue;
      }
    if(count > 100){
      if(m < -200){
        leftpoint++;
        color = 0xF800;//左から遠くなっている場合
        }else if(i > 160){
        rightpoint++;
        color = 0x1F;//右側を逆走している場合
        }else{
        centerpoint++;
        color = 0xF81F;//正常
          }
      delta2 = 0;
      for (h = 120; h < 240 ; h++){
        *((pMain+38400+i)+(uint32_t)delta2) = color;
        delta2 = delta2+320+delta1;
        }
      delta2 = 0;
      for (h = 120; h > 0 ; h--){
        *((pMain+38400+i)-(uint32_t)delta2) = color;
        delta2 = delta2+320+delta1;
        }
      break;
      }
    }
  }

void mifilter(){
  for (i = 1 ; i < 319 ; i++){
    if(*(pMain+38400+i) == monocolor){
      count=0;
      if(*(pMain+38400+i-321)==0){count++;}
      if(*(pMain+38400+i-320)==0){count++;}
      if(*(pMain+38400+i-319)==0){count++;}
      if(count == 0){
        continue;
        }
        count = 0;
      if(*(pMain+38400+i+319)==0){count++;}
      if(*(pMain+38400+i+320)==0){count++;}
      if(*(pMain+38400+i+321)==0){count++;}
      if(count == 0){
        continue;
        }
      straight2();
      if (centerpoint != 0){
        //警告しない
        Flag = 0;
      }else if (rightpoint !=0 && leftpoint == 0){//逆走
        Flag = 1;


      }else if (rightpoint == 0 && leftpoint != 0){ //白線から離れている
        Flag = 2;


      }

      }
    }
  }

/*
void MedianF(){
  //MedianF;
  for (i = 320; i < pixsize-320 ; i++){
    //if (i % 320 == 0 | i % 320 == 319) {continue;}
    Medi9[0] = *(pMain+i-321);
    Medi9[1] = *(pMain+i-320);
    Medi9[2] = *(pMain+i-319);
    Medi9[3] = *(pMain+i-1);
    Medi9[4] = *(pMain+i);
    Medi9[5] = *(pMain+i+1);
    Medi9[6] = *(pMain+i+319);
    Medi9[7] = *(pMain+i+320);
    Medi9[8] = *(pMain+i+321);
    for (j = 0; j <5  ; j++){
      for (k = 0; k <9  ; k++){
        if (Medi9[j]<Medi9[k]){
          tmp = Medi9[k];
          Medi9[k] = Medi9[j];
          Medi9[j] = tmp;
          }
        }
      }
    *(pMain+i) = Medi9[4];
    }
  }
*/

void MedianUpToDown(){
  //MedianF;
  for (i = 321; i < pixsize-320 ; i++){
    count=0;
    if(*(pMain+i-321)==0){count++;}
    if(*(pMain+i-320)==0){count++;}
    if(*(pMain+i-319)==0){count++;}
    if(*(pMain+i-1)==0){count++;}
    if(*(pMain+i)==0){count++;}
    if(*(pMain+i+1)==0){count++;}
    if(*(pMain+i+319)==0){count++;}
    if(*(pMain+i+320)==0){count++;}
    if(*(pMain+i+321)==0){count++;}
    *(pMain+i) = count>4 ? 0:monocolor;
    }
  }
void MedianDownToUp(){
  //MedianF;
  for (i = pixsize-321; i > 320 ; i--){
    count=0;
    if(*(pMain+i-321)==0){count++;}
    if(*(pMain+i-320)==0){count++;}
    if(*(pMain+i-319)==0){count++;}
    if(*(pMain+i-1)==0){count++;}
    if(*(pMain+i)==0){count++;}
    if(*(pMain+i+1)==0){count++;}
    if(*(pMain+i+319)==0){count++;}
    if(*(pMain+i+320)==0){count++;}
    if(*(pMain+i+321)==0){count++;}
    *(pMain+i) = count>4 ? 0:monocolor;
    }
  }

void CamCB(CamImage img){
  if (!img.isAvailable()) {return;}
  bmp = (uint16_t*)img.getImgBuff();  //bmpはポインタ
  //モノクロ～
  for (i = 0; i < pixsize ; i++){
    pixR = (*bmp & 0xF800)>>11;
    pixG = (*bmp & 0x7E0)>>6;
    pixB = (*bmp & 0x1F);
    *pMain = pixR>pixG ? (pixG>pixB ? pixB:pixG) : (pixR>pixB ? pixB:pixR);
    if (*pMain>pixMax){pixMax = *pMain;}
    if (pixMini>*pMain){pixMini = *pMain;}
    pMain++;
    bmp++;
  }
  pMain -= pixsize;
  bmp -= pixsize;

  //MedianF();

  //正規化～
  for (i = 0; i < pixsize ; i++){
    *pMain = 0x1F*(*pMain-pixMini)/(pixMax-pixMini);
    *pMain = 20>*pMain ? 0 : monocolor;
    pMain++;
    bmp++;
  }
  pMain -= pixsize;
  bmp -= pixsize;

  //メディアンフィルタ
  //MedianUpToDown();
  //MedianDownToUp();

  //輪郭
  /*
  tmp =0;
  for (i = 0; i < pixsize ; i++){
    if (*pMain == tmp){
      *pMain = 0;
    }else{
      tmp =*pMain;
      *pMain = monocolor;
      }
    pMain++;
  }
  pMain -= pixsize;
  */

//おえかき
/*
  i = 160;
  delta2 = 0;
  delta1 = 0.00147*300;
for (h = 0; h < 240 ; h++){
  *(pMain+i+(uint32_t)delta2) = 0x7E0;
  delta2 = delta2+320+delta1;
  }
*/
//おえかき

  mifilter();
  
  //Display～
  //tft.drawRGBBitmap(0, 0, bmp, 320, 240);
}

/**
*Audio function
**/
void player_start(String filename, File &myFile) {
  Serial.println("Start play");
  myFile = theSD.open(filename);
  if (!myFile) {
    Serial.println("File open error\n");
    while(1);
  }
  int err = theAudio->writeFrames(AudioClass::Player0, myFile);
  if ((err != AUDIOLIB_ECODE_OK) && (err != AUDIOLIB_ECODE_FILEEND)) {
    Serial.println("File Read Error! =" + String(err));
  }
  theAudio->setVolume(-160);
  theAudio->startPlayer(AudioClass::Player0);

}

void player_continue(File *myFile){
  while (true){
  int err = theAudio->writeFrames(AudioClass::Player0, *myFile);
  usleep(40000);
  if (err == AUDIOLIB_ECODE_FILEEND ){  // || 白線との位置関係が正常になった場合
    break;
  }
}
}

void player_end(File *myFile){
  theAudio->stopPlayer(AudioClass::Player0);
  myFile->close();
  theAudio->setReadyMode();
  theAudio->end();
  theAudio->begin();
  theAudio->setRenderingClockMode(AS_CLKMODE_NORMAL);
  theAudio->setPlayerMode(AS_SETPLAYER_OUTPUTDEVICE_SPHP ,AS_SP_DRV_MODE_LINEOUT);

  int err = theAudio->initPlayer(AudioClass::Player0 ,AS_CODECTYPE_MP3 ,"/mnt/sd0/BIN"
                            ,AS_SAMPLINGRATE_AUTO ,AS_CHANNEL_STEREO);

}

void setup() {
  pMain = bmpMain;
  Serial.begin(115200);
  Serial.println("start");

  tft.begin();
  tft.setRotation(3);

  theCamera.begin(
    1,
    CAM_VIDEO_FPS_5,
    CAM_IMGSIZE_QVGA_H,
    CAM_IMGSIZE_QVGA_V,
    CAM_IMAGE_PIX_FMT_RGB565
    );
  theAudio->begin();
  theAudio->setRenderingClockMode(AS_CLKMODE_NORMAL);
  theAudio->setPlayerMode(AS_SETPLAYER_OUTPUTDEVICE_SPHP ,AS_SP_DRV_MODE_LINEOUT);

  int err = theAudio->initPlayer(AudioClass::Player0 ,AS_CODECTYPE_MP3 ,"/mnt/sd0/BIN"
                            ,AS_SAMPLINGRATE_AUTO ,AS_CHANNEL_STEREO);
  


  
  theCamera.startStreaming(true, CamCB);
}
int rcount = 0, lcount = 0;
File *myFile2;
void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("test");
  if (Flag == 1){// right -> 逆走
    rcount ++;
    Serial.println(Flag);
  }else if (Flag == 2){
    lcount ++;
    Serial.println(Flag);
  }else if (Flag == 0){
    rcount = 0;
    lcount = 0;
    Serial.println(Flag);
    player_end(myFile2);
  }
  
  if(rcount >= 5){
    if (rcount == 5){
      File myFile;
    player_start(filename2, myFile);
    myFile2 = &myFile;
    }
    player_continue(myFile2);
  }
  if(lcount >= 5){
    if (lcount == 5){
    File myFile;
    player_start(filename1, myFile);
    myFile2 = &myFile;
    }
    player_continue(myFile2);
  }
  

}
