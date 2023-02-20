
//#include <inttypes.h>
#define monocolor 0x7E0
#define monocolor2 0xF800

#define pixsize  38400 //76800/2
uint16_t *bmp, *pMainf, *pPixNum;
uint16_t pixR,pixG,pixB,pixAve = 0,scnt;
uint32_t pixSum1 = 0,Recv32 = 0,*pH;
int i = 0,j = 0,k = 0,strightDelta;

bool *pfil ,*pfil2 ,*pfil3 ,*pfil3f;

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
  ret = MP.Recv(&msgid, &pfil);//2 
 //   MPLog("Recv pMain %d\n",ret);
  ret = MP.Recv(&msgid, &pPixNum);//7
 //   MPLog("Recv pPixNum %d\n",ret);
  ret = MP.Recv(&msgid, &pH);//a1

 pfil2 = pfil + 76800;
 pfil3 = pfil2 + 76800;

//Ave
//  pixSum1 = 0;
  for (i = 0; i < pixsize ; i++){
    
    pixR = *bmp >> 11;
    pixG = (*bmp >> 6) & 0x1F;
    pixB = *bmp & 0x1F;
    /*
    pixR = (*bmp >> 5) & 0x7C0;
    pixG = *bmp & 0x7C0;
    pixB = (*bmp << 6) & 0x7C0;
    */
    *bmp = pixR>pixG ? (pixG>pixB ? pixB:pixG) : (pixR>pixB ? pixB:pixR);
//    pixSum1 += (uint32_t)*bmp;
    *(pH + *bmp) += 1;
    if(*bmp > 31){MPLog("over??????????????????????????????????????????????????");}
    if(*bmp < 0){MPLog("under??????????????????????????????????????????????????");}
    bmp++;
  }
  bmp -= pixsize;
//  pfil2 -= pixsize; 二値化は下からやる勢
//  bmp -= pixsize;二値化は下からやる勢

//  pixSum1 = pixSum1>>1;

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
    *pfil = *bmp > pixAve ? true : false;
    bmp++;
    pfil++;
  }
  bmp -= pixsize;
  pfil -= pixsize;
  
//FILTERRRRRRRRRRRRRRRRR
 //MPLog(" filterST %d\n",ret);
  MedianF(pfil , pfil2);
  for(i = 0 ; i < pixsize ; i++){
    *pfil3 = false;
    pfil3++;
    }
    pfil3 -= pixsize;
  miredu(pfil2 , pfil3);

  ret = MP.Send(MY_MSGID, 10);//5

//stright center st
  ret = MP.Recv(&msgid, &Recv32);//11
  while(ret != 3){
    if(ret == 1){
    scnt = (uint16_t)Recv32;
    //MPLog(" msgid= %d \n", scnt);
    strightSub1();
    }else{
      strightDelta = (int)Recv32-119;
      drawStrightSub1();
      }
    ret = MP.Send(MY_MSGID, 10);//12
    ret = MP.Recv(&msgid, &Recv32);//11
    }
//stright center ed

/*
  //輪郭
  tmp =0;
  for (i = 0; i < pixsize ; i++){
    if (*pfil2 == tmp){
      *pfil2 = 0;
    }else{
      tmp =*pfil2;
      *pfil2 = true;
      }
    pfil2++;
  }
  pfil2 -= pixsize;
*/


//MPLog("time2 %"PRIu64"\n",micros());
  //ret = MP.Send(msgid, 10);//6
  // put your main code here, to run repeatedly:
}

void strightSub1(){
//dx<0
  for(i = -119; i < 0 ; i++){
    fxy = 0;
    pfil3f = pfil3 + 38400 + scnt;
    for(k = 0 ; k <119 ; k++){
      fxy = fxy-i-i-120;
      if(fxy < 0){
        pfil3f = pfil3f-320;
        fxy = fxy + 120;
        if(*pfil3f != 0){(*pPixNum)++;}
        }else{
          pfil3f = pfil3f-321;
          fxy = fxy - 120;
          if(*pfil3f != 0){(*pPixNum)++;}
          }
      }
    pPixNum++;
    }
//dx=0
  pfil3f = pfil3 + 38400 + scnt;
  for(k = 0 ; k <119 ; k++){
    pfil3f -= 320;
    if(*pfil3f != 0){(*pPixNum)++;}
    }
  pPixNum++;
//dx>0
  for(i = 1; i < 120 ; i++){
    fxy = 0;
    pfil3f = pfil3 + 38400 + scnt;
    for(k = 0 ; k <119 ; k++){
      fxy = fxy+120-i-i;
      if(fxy < 0){
        pfil3f = pfil3f-319;
        fxy = fxy + 120;
        if(*pfil3f != 0){(*pPixNum)++;}
        }else{
          pfil3f = pfil3f-320;
          fxy = fxy - 120;
          if(*pfil3f != 0){(*pPixNum)++;}
          }
      }
    pPixNum++;
    }
  pPixNum -= 239;
  }

void drawStrightSub1(){
  pMainf = bmp + 38400 + scnt;
  fxy = 0;
  if(strightDelta < 0){
    for(k = 0 ; k <119 ; k++){
      fxy = fxy-strightDelta-strightDelta-120;
      if(fxy < 0){
        pMainf = pMainf-320;
        fxy = fxy + 120;
        *pMainf = monocolor2;
        }else{
          pMainf = pMainf-321;
          fxy = fxy - 120;
          *pMainf = monocolor2;
          }
      }
    }else if(strightDelta == 0){
      for(k = 0 ; k <119 ; k++){
        pMainf -= 320;
        *pMainf = monocolor2;
        }
      }else{
        for(k = 0 ; k <119 ; k++){
          fxy = fxy+120-strightDelta-strightDelta;
          if(fxy < 0){
            pMainf = pMainf-319;
            fxy = fxy + 120;
            *pMainf = monocolor2;
            }else{
              pMainf = pMainf-320;
              fxy = fxy - 120;
              *pMainf = monocolor2;
              }
          }
        }
  }

void  MedianF(bool *pfn2 , bool *pfn3){
  co[0] = 0;
  co[1] = 0;
  co[2] = 0;
  count = 0;
  if(*(pfn2+320-320)==0){co[0]++;}
  if(*(pfn2+321-320)==0){co[1]++;}
  if(*(pfn2+322-320)==0){co[2]++;}
  if(*(pfn2+320)==0){co[0]++;}
  if(*(pfn2+321)==0){co[1]++;}
  if(*(pfn2+322)==0){co[2]++;}
  if(*(pfn2+320+320)==0){co[0]++;}
  if(*(pfn2+321+320)==0){co[1]++;}
  if(*(pfn2+322+320)==0){co[2]++;}

  for(i = 321; i < pixsize ; i++){
    *(pfn3+i) = (co[0]+co[1]+co[2])>4 ? false : true;
    
    co[count] = 0;
    if(*(pfn2+i+2-320)==0){co[count]++;}
    if(*(pfn2+i+2)==0){co[count]++;}
    if(*(pfn2+i+2+320)==0){co[count]++;}
    count++;
    if(count == 3){count = 0;}
    }
  }

void miredu(bool *pfn2 , bool *pfn3){
  bool jug1 = false;
  for(i = 2 ; i < pixsize-2 ; i++){
    if(*(pfn2 + i) == true){
      if(jug1 == false){
        jug1 = true;
        j = i;
        }
      }else{
        if(jug1 == true){
          jug1 = false;
          if(i-j > 5){
            k = (i + j) / 2;
            *(pfn3 + k-2) = true;
            *(pfn3 + k-1) = true;
            *(pfn3 + k) = true;
            *(pfn3 + k+1) = true;
            *(pfn3 + k+2) = true;
            }
        }
        }
    }
  }
