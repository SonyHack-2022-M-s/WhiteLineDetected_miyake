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
//#include "Adafruit_GFX.h"
//#include "Adafruit_ILI9341.h"

//#define TFT_RST 8
//#define TFT_DC  9
//#define TFT_CS  10
#define monocolor 0x7E0

//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS ,TFT_DC ,TFT_RST);

#define pixsize  76800
uint16_t bmpMain[pixsize] , *bmp , *pG ,*pMain ,Medi9[9];
uint16_t pix,pixR,pixG,pixB,pixMax = 0,pixMini = 0x1F,tmp=0,color;
uint32_t pixSum1 = 0,pixSum2 = 0, pixAve = 0;
int i = 0,j = 0,k = 0,count = 0,h=0,ti=0,rightpoint=0,centerpoint=0,leftpoint=0;
float m=0,delta1=0,delta2=0;

int subcre1 =1 ,subcre2 =2;

#include <MP.h>
int8_t     MY_MSGID =   10;
int     ret;
uint32_t msg = 0;
uint64_t myTime1,myTime2;
int      subid;
int8_t   msgid;

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
    if(*(pMain+i+1)==0){count++;
    if(*(pMain+i+319)==0){count++;}
    if(*(pMain+i+320)==0){count++;}
    if(*(pMain+i+321)==0){count++;}
    *(pMain+i) = count>4 ? 0:monocolor;
    }
  }
*/

#include <inttypes.h>

void CamCB(CamImage img){
  if (!img.isAvailable()) {return;}
  bmp = (uint16_t*)img.getImgBuff();  //bmpはポインタ
  myTime1 = micros();

  //// put your main code here, to run repeatedly:
  ret = MP.Send(MY_MSGID, bmp, subcre1);
 //   MPLog("Send1 bmp %d\n",ret);
  ret = MP.Send(MY_MSGID, bmp, subcre2);
 //   MPLog("Send2 bmp %d\n",ret);
  ret = MP.Send(MY_MSGID, pMain, subcre1);
 //   MPLog("Send1 pMain %d\n",ret);
  ret = MP.Send(MY_MSGID, pMain,subcre2);
 //   MPLog("Send2 pMain %d\n",ret);
 /*
  ret = MP.Send(MY_MSGID, ppixSum, subcre1);
  ret = MP.Send(MY_MSGID, ppixSum,subcre2);
*/

//平均値計算
  ret = MP.Recv(&msgid, &pixSum1, subcre1);
  ret = MP.Recv(&msgid, &pixSum2, subcre2);
  pixAve = (pixSum1+pixSum2)/38400;
  ret = MP.Send(MY_MSGID, pixAve, subcre1);
  ret = MP.Send(MY_MSGID, pixAve, subcre2);

//Filter完了
  ret = MP.Recv(&msgid, &pixSum1, subcre1);
  ret = MP.Recv(&msgid, &pixSum2, subcre2);

//SubCore完了
  ret = MP.Recv(&msgid, &msg, subcre1);
  ret = MP.Recv(&msgid, &msg, subcre2);

  myTime2 = micros();
//  MPLog(",%"PRIu64",%"PRIu64"\n", myTime1, myTime2);

  MP.RecvTimeout(MP_RECV_POLLING);
  ret = MP.Recv(&msgid, &msg,3);
  MP.RecvTimeout(MP_RECV_BLOCKING);
  if(ret != 10){
    ret = MP.Send(MY_MSGID,pMain,3);
  }

//  tft.drawRGBBitmap(0, 0, pMain, 320, 240);
}

void setup() {
  pMain = bmpMain;
  Serial.begin(115200);
  Serial.println("start");

  MP.begin(1);
  MP.begin(2);
  MP.begin(3);

//  tft.begin();
//  tft.setRotation(3);

  theCamera.begin(
    1,
    CAM_VIDEO_FPS_5,
    CAM_IMGSIZE_QVGA_H,
    CAM_IMGSIZE_QVGA_V,
    CAM_IMAGE_PIX_FMT_RGB565
    );
  theCamera.startStreaming(true, CamCB);
}

void loop() {
  // put your main code here, to run repeatedly:
}
