
#include <Camera.h>
//#define monocolor 0x7E0

#define pixsize  76800
uint16_t bmpMain[pixsize] , *bmp ,*pMain ;
uint16_t stPixNum[239] ={0} , *pPixNum ,test;
uint32_t pixSum1 = 0,pixSum2 = 0, pixAve = 0;
int i = 0,j = 0,k = 0,m = 0,n = 0,h = 0, strightDelta;
bool col = false , st = false;

unsigned short int count = 0;

int subcre1 =1 ,subcre2 =2;

#include <MP.h>
int8_t     MY_MSGID =   10;
int     ret;
uint32_t msg = 0;
int8_t   msgid;
uint64_t myTime1,myTime2;

//#include <inttypes.h>

void CamCB(CamImage img){
  if (!img.isAvailable()) {return;}
  bmp = (uint16_t*)img.getImgBuff();  //bmpはポインタ
  myTime1 = micros();

  //// put your main code here, to run repeatedly:
  ret = MP.Send(MY_MSGID, bmp, subcre1);//1-1
 //   MPLog("Send1 bmp %d\n",ret);
  ret = MP.Send(MY_MSGID, bmp, subcre2);//2-1
 //   MPLog("Send2 bmp %d\n",ret);
  ret = MP.Send(MY_MSGID, pMain, subcre1);//1-2
 //   MPLog("Send1 pMain %d\n",ret);
  ret = MP.Send(MY_MSGID, pMain,subcre2);//2-2
 //   MPLog("Send2 pMain %d\n",ret);
  ret = MP.Send(MY_MSGID, pPixNum, subcre1);//1-7
 //   MPLog("Send1 pMain %d\n",ret);
  ret = MP.Send(MY_MSGID, pPixNum,subcre2);//2-7
 //   MPLog("Send2 pMain %d\n",ret);pPixNum


//平均値計算
  ret = MP.Recv(&msgid, &pixSum1, subcre1);//1-3
  ret = MP.Recv(&msgid, &pixSum2, subcre2);//2-3
  pixAve = (pixSum1+pixSum2)/38400;
  ret = MP.Send(MY_MSGID, pixAve, subcre1);//1-4
  ret = MP.Send(MY_MSGID, pixAve, subcre2);//2-4

//Filter完了
  ret = MP.Recv(&msgid, &pixSum1, subcre1);//1-5
  ret = MP.Recv(&msgid, &pixSum2, subcre2);//2-5

//直線かいし
  i=0;
  j=0;
  count = 0;
  col = false;
  pMain += 38400;
  
  for (i = 0; i < 320 ; i++){
    if(*pMain == 0){
      if(col == true){
        col = false;
        j = (j + i)/2;
//        MPLog(" i= %d \n", i);
        MPLog(" j= %d \n", j);
//        for(m = 0 ; m < 239 ; m++){stPixNum[m] = 0;}//初期化 218234885
        ret = MP.Send(1, (uint32_t)j, subcre1);//1-11
        ret = MP.Send(1, (uint32_t)j, subcre2);//2-11
        ret = MP.Recv(&msgid, &msg, subcre1);//1-12
        ret = MP.Recv(&msgid, &msg, subcre2);//2-12
        stright3();
        count++;
        if(count == 4){
          break;
          }
        }
      }else{
      if(col == false){
        col = true;
        j = i-1;
//        MPLog(" i-1= %d \n", j);
        }
      }
    pMain++;
    }
    pMain = pMain-i;
    ret = MP.Send(2, (uint32_t)j, subcre1);    //1-11
    ret = MP.Send(2, (uint32_t)j, subcre2);    //2-11
    //MP.Send(100, &j, subcre1);
    
  pMain -= 38400;
//直線ここまで

//SubCore完了
  ret = MP.Recv(&msgid, &msg, subcre1);//1-6
  ret = MP.Recv(&msgid, &msg, subcre2);//2-6

  myTime2 = micros();
//  MPLog(",%"PRIu64",%"PRIu64"\n", myTime1, myTime2);

//LCD Core3
  MP.RecvTimeout(MP_RECV_POLLING);
  ret = MP.Recv(&msgid, &msg,3);
  MP.RecvTimeout(MP_RECV_BLOCKING);
  if(ret == 10){
    ret = MP.Send(MY_MSGID,pMain,3);
  }
}

void stright3(){
  st = false;
  for(n = 0; n < 239 ; n++){
    if(*pPixNum < 120){//本来は120
      if(st == true){
        st = false;
        strightDelta = (strightDelta + n )/2;
        MPLog("strightDelta = %d\n", strightDelta);
        break;
        }
      }else{
      if(st == false){
        st = true;
        strightDelta = n-1;
        }
      }
    *pPixNum = 0;
    pPixNum++;
    }
  for(n = n; n < 239 ; n++){
    *pPixNum = 0;
    pPixNum++;
    }
  pPixNum -= 239;
  }

void setup() {
  pMain = bmpMain;
  pPixNum = stPixNum;
  Serial.begin(115200);
  Serial.println("start");

  MP.begin(1);
  MP.begin(2);
  MP.begin(3);

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
