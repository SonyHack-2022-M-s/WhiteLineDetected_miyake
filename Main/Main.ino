
#include <Camera.h>
//#define monocolor 0x7E0

#define pixsize  76800
uint16_t *bmp;
uint16_t stPixNum[239] ={0} , *pPixNum ,maxPixNum;
uint32_t pixSum1 = 0,pixSum2 = 0, pixAve = 0 ,*pH1 , H1[32] = {0};
uint32_t *pH2 , H2[32] = {0} , S1 , S2 ,omega1 ,omega2 , otMax, ot ,otsu, avedif;
int i = 0,j = 0,k = 0,m = 0,n = 0,h = 0, strightDelta, strightDelta2;
bool col = false , st = false , jug1 = false ,jug2 = false;
bool filpix[230400], *pfil ,*pfil2 ,*pfil3;


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
  //MPLog("CamCB　start");
  if (!img.isAvailable()) {return;}
  bmp = (uint16_t*)img.getImgBuff();  //bmpはポインタ
  myTime1 = micros();

  //// put your main code here, to run repeatedly:
  ret = MP.Send(MY_MSGID, bmp, subcre1);//1-1
 //   MPLog("Send1 bmp %d\n",ret);
  ret = MP.Send(MY_MSGID, bmp, subcre2);//2-1
 //   MPLog("Send2 bmp %d\n",ret);
  ret = MP.Send(MY_MSGID, pfil, subcre1);//1-2
 //   MPLog("Send1 pMain %d\n",ret);
  ret = MP.Send(MY_MSGID, pfil,subcre2);//2-2
 //   MPLog("Send2 pMain %d\n",ret);
  ret = MP.Send(MY_MSGID, pPixNum, subcre1);//1-7
 //   MPLog("Send1 pMain %d\n",ret);
  ret = MP.Send(MY_MSGID, pPixNum,subcre2);//2-7
 //   MPLog("Send2 pMain %d\n",ret);pPixNum

  ret = MP.Send(MY_MSGID, pH1, subcre1);//1-a1
  ret = MP.Send(MY_MSGID, pH2, subcre2);//2-a1
//MPLog("ポインタSend");
//平均値計算
  ret = MP.Recv(&msgid, &pixSum1, subcre1);//1-3
  ret = MP.Recv(&msgid, &pixSum2, subcre2);//2-3


//ヒストグラム描写
  MPLog("\n\n");
  for(i = 0 ; i < 32 ; i++){
    MPLog("[%d],%d,\n" ,i, *pH1 + *pH2);
    pH1 ++;
    pH2 ++;
    }
    pH1 -= 32;
    pH2 -= 32;

//大津の二値化
    S1 = *pH1 + *pH2;
    S2 = pixsize - (*pH1 + *pH2);
    *pH1 = 0;
    *pH2 = 0;
    pH1 ++;
    pH2 ++;
    //MPLog("[S1S2],%d,%d,\n" ,S1,S2);
    omega1 = 1;
    omega2 = 31;
    avedif = S1/omega1>S2/omega2 ? (S1/omega1 - S2/omega2)>>5 :(S2/omega2 - S1/omega1)>>5;
    //MPLog("[avedif],%d,\n" ,avedif);
    otMax = 0;//omega1*omega2*avedif*avedif
    MPLog("[ot],%u,\n" ,otMax);
    otsu = 0;
    for(i = 1 ;i < 31 ; i++){
      S1 += *pH1 + *pH2;
      S2 -= *pH1 + *pH2;
      *pH1 = 0;
      *pH2 = 0;
      pH1 ++;
      pH2 ++;
      omega1 += 1;
      omega2 -= 1;
      avedif = S1/omega1>S2/omega2 ? (S1/omega1 - S2/omega2)>>5 :(S2/omega2 - S1/omega1)>>5;
      //MPLog("[avedif],%u,\n" ,avedif);
      //MPLog("[S1S2],%d,%d,\n" ,S1,S2);
      ot = omega1*omega2*avedif*avedif;
      MPLog("[ot],%u,\n" ,ot);
      if(ot>otMax){
        otMax = ot;
        otsu = i;
        }
      }
    pH1 -= 31;//間の数
    pH2 -= 31;//間の数
    MPLog("[otsu],%u,\n" ,otsu);

//平均値計算
  pixAve = (pixSum1+pixSum2)/38400;
  ret = MP.Send(MY_MSGID, otsu, subcre1);//1-4
  ret = MP.Send(MY_MSGID, otsu, subcre2);//2-4
  //MPLog("平均値計算");

//Filter完了
  ret = MP.Recv(&msgid, &pixSum1, subcre1);//1-5
  ret = MP.Recv(&msgid, &pixSum2, subcre2);//2-5
  //MPLog("Filter完了");
  BooltoBmp(pfil2,0x7FF);
  BooltoBmp(pfil3,0x1F); ///BOOL確認用関数
//直線かいし
  i=0;
  j=0;
  count = 0;
  col = false;
  pfil3 += 38400;
  
  for (i = 0; i < 320 ; i++){
    if(*pfil3 == 0){
      if(col == true){
        col = false;
        j = (j + i)/2;
//        MPLog(" i= %d \n", i);
//        MPLog(" j= %d \n", j);
//        for(m = 0 ; m < 239 ; m++){stPixNum[m] = 0;}//初期化 218234885
        ret = MP.Send(1, (uint32_t)j, subcre1);//1-11
        ret = MP.Send(1, (uint32_t)j, subcre2);//2-11
        ret = MP.Recv(&msgid, &msg, subcre1);//1-12
        ret = MP.Recv(&msgid, &msg, subcre2);//2-12
        stright4(); //又は stright3()
        if(jug1 == true){
          ret = MP.Send(2, (uint32_t)strightDelta, subcre1);//1-11
          ret = MP.Send(2, (uint32_t)strightDelta, subcre2);//2-11
          ret = MP.Recv(&msgid, &msg, subcre1);//1-12
          ret = MP.Recv(&msgid, &msg, subcre2);//2-12
          }
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
    pfil3++;
    }
    pfil3 = pfil3-i;
    ret = MP.Send(3, (uint32_t)j, subcre1);    //1-11
    ret = MP.Send(3, (uint32_t)j, subcre2);    //2-11
    //MP.Send(100, &j, subcre1);
    
  pfil3 -= 38400;
//直線ここまで

//SubCore完了
  //ret = MP.Recv(&msgid, &msg, subcre1);//1-6
  //ret = MP.Recv(&msgid, &msg, subcre2);//2-6

  myTime2 = micros();

//LCD Core3
  MP.RecvTimeout(MP_RECV_POLLING);
  ret = MP.Recv(&msgid, &msg,3);
  MP.RecvTimeout(MP_RECV_BLOCKING);
  if(ret == 10){
    ret = MP.Send(MY_MSGID,bmp,3);
  }
  //MPLog(",%"PRIu64",%"PRIu64"\n", myTime1, myTime2);
}

void stright3(){
  st = false;
  jug1 = false;
  for(n = 0; n < 239 ; n++){
    if(*pPixNum < 100){//本来は120
      if(st == true){
        st = false;
        strightDelta = (strightDelta + n )/2;
        //MPLog("strightDelta = %d\n", strightDelta);
        jug1 = true;
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

void stright4(){
  jug1 = false;
  jug2 = false;
  maxPixNum = 0;
  strightDelta = 0;
  for(n = 0; n < 239 ; n++){
    if(*pPixNum > maxPixNum && *pPixNum > 100){//本来は120
      strightDelta = n;
      maxPixNum = *pPixNum;
      jug1 = true;
      jug2 = false;
      }else if(*pPixNum == maxPixNum){
        strightDelta2 = n;
        jug2 = true;
        }
    *pPixNum = 0;
    pPixNum++;
    }
  if(jug2 == true){strightDelta = (strightDelta + strightDelta2)/2;}
  pPixNum -= 239;
  }

void setup() {
  pfil = filpix;
  pfil2 = pfil + 76800;
  pfil3 = pfil2 + 76800;
  pPixNum = stPixNum;
  pH1 = H1;
  pH2 = H2;
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

///BOOL確認用関数

void BooltoBmp(bool *pfn , uint16_t color){
  for(i = 0 ; i < pixsize ; i++){
    if(*pfn == true){
      *bmp = color;
      }
    pfn++;
    bmp++;
    }
  pfn -= pixsize;
  bmp -= pixsize;
  }


void loop() {
  // put your main code here, to run repeatedly:
}
