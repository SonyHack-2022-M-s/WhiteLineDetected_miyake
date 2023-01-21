
#include <Camera.h>
//#define monocolor 0x7E0

#define pixsize  76800
uint16_t bmpMain[pixsize] , *bmp ,*pMain;
uint32_t pixSum1 = 0,pixSum2 = 0, pixAve = 0;
int i = 0,j = 0,k = 0,m = 0,n = 0,h = 0;

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
  ret = MP.Send(MY_MSGID, bmp, subcre1);
 //   MPLog("Send1 bmp %d\n",ret);
  ret = MP.Send(MY_MSGID, bmp, subcre2);
 //   MPLog("Send2 bmp %d\n",ret);
  ret = MP.Send(MY_MSGID, pMain, subcre1);
 //   MPLog("Send1 pMain %d\n",ret);
  ret = MP.Send(MY_MSGID, pMain,subcre2);
 //   MPLog("Send2 pMain %d\n",ret);


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
  if(ret == 10){
    ret = MP.Send(MY_MSGID,pMain,3);
  }
}

void setup() {
  pMain = bmpMain;
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
