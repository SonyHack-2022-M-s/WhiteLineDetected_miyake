
//#include <inttypes.h>
#define monocolor 0x7E0

#define pixsize  38400 //76800/2
uint16_t *bmp , *pMain, *pMainf, *pPixNum;
uint16_t pixR,pixG,pixB,pixAve = 0,scnt;
uint32_t pixSum1 = 0,Recv32 = 0;
int i = 0,j = 0,k = 0;

int fxy;

unsigned short int count = 0, co[3];

#include <MP.h>
//#include <inttypes.h>
int     ret;
uint32_t  msg = 0;
int8_t     MY_MSGID =   10;
int8_t   msgid = 0;

void setup() { 
  MP.begin();
}

void loop() {
  ret = MP.Recv(&msgid, &bmp);//1
 //   MPLog("Recv bmp %d\n",ret);
  ret = MP.Recv(&msgid, &pMain);//2
 //   MPLog("Recv pMain %d\n",ret);
  ret = MP.Recv(&msgid, &pPixNum);//7
 //   MPLog("Recv pPixNum %d\n",ret);

//Ave
  for (i = 0; i < pixsize ; i++){
    pixR = (*bmp & 0xF800)>>11;
    pixG = (*bmp & 0x7E0)>>6;
    pixB = (*bmp & 0x1F);
    *pMain = pixR>pixG ? (pixG>pixB ? pixB:pixG) : (pixR>pixB ? pixB:pixR);
    pixSum1 += (uint32_t)*pMain;
    pMain++;
    bmp++;
  }
//  pMain -= pixsize; 二値化は下からやる勢
//  bmp -= pixsize;
  
  pixSum1 = pixSum1>>1;


 // MPLog("time1 %"PRIu64"\n",micros());
  ret = MP.Send(MY_MSGID, pixSum1);//3
 //   MPLog("Send pixSum1 %d\n",ret);
  ret = MP.Recv(&msgid, &Recv32);//4
 //   MPLog("Recv pixSum2 %d\n",ret);

  pixAve = (uint16_t)Recv32;
  
  //MPLog("pixSum1=%"PRIu32"\n", pixSum1);
  //MPLog("pixSum2=%"PRIu32"\n", pixSum2);
  //MPLog("pixAve=%lu\n", pixAve);

//二値化
  for (i = 0; i < pixsize ; i++){
    *pMain = *pMain>pixAve ? monocolor : 0;
    pMain--;
    bmp--;
  }

//FILTERRRRRRRRRRRRRRRRR
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
    count++;
    if(count == 3){count = 0;}
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
    count++;
    if(count == 3){count = 0;}
    }
  ret = MP.Send(MY_MSGID, 10);//5

//stright center st
  ret = MP.Recv(&msgid, &Recv32);//11
  while(ret == 1){
    scnt = (uint16_t)Recv32;
    MPLog(" msgid= %d \n", scnt);
    strightSub1();
    ret = MP.Send(MY_MSGID, 10);//12
    ret = MP.Recv(&msgid, &Recv32);//11
    }
//stright center ed

/*
  //輪郭
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


//MPLog("time2 %"PRIu64"\n",micros());
  ret = MP.Send(msgid, 10);//6
  // put your main code here, to run repeatedly:
}

void strightSub1(){
//dx<0
  for(i = -119; i < 0 ; i++){
    fxy = 0;
    pMainf = pMain + 38400 + scnt;
    for(k = 0 ; k <119 ; k++){
      fxy = fxy-i-i-120;
      if(fxy < 0){
        pMainf = pMainf-320;
        fxy = fxy + 120;
        if(*pMainf != 0){(*pPixNum)++;}
        }else{
          pMainf = pMainf-321;
          fxy = fxy - 120;
          if(*pMainf != 0){(*pPixNum)++;}
          }
      }
    pPixNum++;
    }
//dx=0
  pMainf = pMain + 38400 + scnt;
  for(k = 0 ; k <119 ; k++){
    pMainf -= 320;
    if(*pMainf != 0){(*pPixNum)++;}
    }
  pPixNum++;
//dx>0
  for(i = 1; i < 120 ; i++){
    fxy = 0;
    pMainf = pMain + 38400 + scnt;
    for(k = 0 ; k <119 ; k++){
      fxy = fxy+120-i-i;
      if(fxy < 0){
        pMainf = pMainf-319;
        fxy = fxy + 120;
        if(*pMainf != 0){(*pPixNum)++;}
        }else{
          pMainf = pMainf-320;
          fxy = fxy - 120;
          if(*pMainf != 0){(*pPixNum)++;}
          }
      }
    pPixNum++;
    }
  pPixNum -= 239;
  }
