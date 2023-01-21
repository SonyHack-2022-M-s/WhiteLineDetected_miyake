
//#include <inttypes.h>
#define monocolor 0x7E0

#define pixsize  38400 //76800/2
uint16_t *bmp , *pMain;
uint16_t pixR,pixG,pixB,pixAve = 0;
uint32_t pixSum2 = 0,Recv32 = 0;
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

void loop(){
  ret = MP.Recv(&msgid, &bmp);
 //   MPLog("Recv bmp %d\n",ret);
  ret = MP.Recv(&msgid, &pMain);
 //   MPLog("Recv pMain %d\n",ret);
  
  pMain+=76800;//下担当
  bmp+=76800;//下担当
//Ave
  for (i = 0; i < pixsize ; i++){
    pixR = (*bmp & 0xF800)>>11;
    pixG = (*bmp & 0x7E0)>>6;
    pixB = (*bmp & 0x1F);
    *pMain = pixR>pixG ? (pixG>pixB ? pixB:pixG) : (pixR>pixB ? pixB:pixR);
    pixSum2 += (uint32_t)*pMain;
    pMain--;
    bmp--;
  }
  
  pixSum2 = pixSum2>>1;


  //MPLog("time2 %"PRIu64"\n",micros());
  ret = MP.Send(msgid, pixSum2);
 // MPLog("Send pixSum2 %d\n",ret);
  ret = MP.Recv(&msgid, &Recv32);
 // MPLog("Recv pixSum1 %d\n",ret);

  pixAve = (uint16_t)Recv32;
  
  //MPLog("pixSum1=%"PRIu32"\n", pix2Sum1);
  //MPLog("pixSum2=%"PRIu32"\n", pix2Sum2);
  //MPLog("pixAve=%lu\n", pixAve);
  //MPLog("pixAve=%lu\n", pixSig);

//二値化
  for (i = 0; i < pixsize ; i++){
    *pMain = *pMain>pixAve ? monocolor : 0;
    pMain++;
    bmp++;
  }
  pMain -= pixsize;
  bmp -= pixsize;

//FILTERRRRRRRRRRRRRRRRR
 //MPLog(" filterST %d\n",ret);

  co[3] = {0};
  if(*(pMain-1-320)==0){co[0]++;}
  if(*(pMain-1)==0){co[0]++;}
  if(*(pMain-1+320)==0){co[0]++;}
  
  if(*(pMain-320)==0){co[1]++;}
  if(*(pMain)==0){co[1]++;}
  if(*(pMain+320)==0){co[1]++;}

  if(*(pMain+1-320)==0){co[2]++;}
  if(*(pMain+1)==0){co[2]++;}
  if(*(pMain+1+320)==0){co[2]++;}

  for(i = 0; i < pixsize-321 ; i++){
    *(pMain+i) = (co[0]+co[1]+co[2])>4 ? 0:monocolor;
    co[count] = 0;
    if(*(pMain+i+2-320)==0){co[count]++;}
    if(*(pMain+i+2)==0){co[count]++;}
    if(*(pMain+i+2+320)==0){co[count]++;}
    count++;
    if(count == 3){count = 0;}
    }

  co[3] = {0};
  if(*(pMain-1-320)==0){co[0]++;}
  if(*(pMain-320)==0){co[1]++;}
  if(*(pMain+1-320)==0){co[2]++;}
  if(*(pMain-1)==0){co[0]++;}
  if(*(pMain)==0){co[1]++;}
  if(*(pMain+1)==0){co[2]++;}
  if(*(pMain-1+320)==0){co[0]++;}
  if(*(pMain+320)==0){co[1]++;}
  if(*(pMain+1+320)==0){co[2]++;}

  for(i = 0; i < pixsize-321 ; i++){
    *(pMain+i) = (co[0]+co[1]+co[2])>4 ? 0:monocolor;
    co[count] = 0;
    if(*(pMain+i+2-320)==0){co[count]++;}
    if(*(pMain+i+2)==0){co[count]++;}
    if(*(pMain+i+2+320)==0){co[count]++;}
    count++;
    if(count == 3){count = 0;}
    }
  ret = MP.Send(msgid, 10);

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
  ret = MP.Send(msgid, 10);
  // put your main code here, to run repeatedly:
}
