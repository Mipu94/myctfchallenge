
#include <windows.h>
#include "dll.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define N 256   // 2^8
#define keyLen 36



/*
P1: 04 08 03 06 09 08 04 05 04 05 03 06 05 05 05 04 0c 0e 05 03 06 0d 06 04 06 03 03 03 04 03 05 04 04 06 07 0e
P2: 0f 04 07 0d 0f 07 0f 05 00 0f 05 06 07 03 06 07 03 07 04 0c 06 06 0d 0e 04 03 06 06 0b 00 04 06 05 0f 05 05
 
C1: 5e 0a 68 86 1d f1 85 91 db b0 4c ac d9 b9 8e f6 aa b4 40 74 9b 25 39 75 62 25 8a 5e d2 07 27 9e 85 eb 4c c8
C2: 8f d2 e8 af 9d 1a 76 72 8a da 98 4b 27 3f 43 61 fb ea c6 34 eb ff c9 e2 a6 de cd 3b de 4a 23 53 9d a5 9b 5e
*/

void init(InternalInfo *Iinfo)
{
    Child_Got = (MyCuteGoT*) malloc(sizeof(MyCuteGoT));
    memset(Child_Got, 0, sizeof(MyCuteGoT));
	Child_Data = (InternalData*) malloc(sizeof(InternalData));
    memcpy(Child_Data, Iinfo->RemoteArgsSegment+sizeof(InternalInfo), sizeof(InternalData));
	Child_Iinfo = (InternalInfo *) malloc(sizeof(InternalInfo));
    memset(Child_Iinfo, 0, sizeof(InternalInfo));
	Iinfo->P1 = (char*)&Child_Data->P1;
    Iinfo->P2 = (char*)&Child_Data->P2;
    Iinfo->Key = (char*)&Child_Data->Key;
    Iinfo->C1 = (char*)&Child_Data->C1;
    Iinfo->C2 = (char*)&Child_Data->C2;
    Iinfo->S = (unsigned char*)malloc(sizeof(unsigned char)*N);
    memcpy(Child_Iinfo, Iinfo, sizeof(InternalInfo));
}

void Fswap(unsigned char *a, unsigned char *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

/*
int KSA(int pid, unsigned char *S) {

    Func_void_pchar_pchar* pswap =(Func_void_pchar_pchar*)0x0404040404040404;
    Func_int_pchar* pstrlen = (Func_int_pchar*)0x0606060606060606;
    Func_pchar_int* pFsprintf = (Func_pchar_int*)0x0c0c0c0c0c0c0c0c;
    char *key = pFsprintf(pid);
    int len = pstrlen(key);
    int j = 0;
    int i;
    for(i = 0; i < N; i++)
        S[i] = i;

    for(i = 0; i < N; i++) {
        j = (j + S[i] + key[i % len]) % N;

        pswap(&S[i], &S[j]);
    }
    return 0;
}


int PRGA(unsigned char *S, char *plaintext, unsigned char *ciphertext) {

    int i = 0;
    int j = 0;
    size_t n;
    Func_void_pchar_pchar* pswap =(Func_void_pchar_pchar*)0x0404040404040404;
    int len = keyLen;
    for( n = 0; n < len; n++) {
        i = (i + 1) % N;
        j = (j + S[i]) % N;

        pswap(&S[i], &S[j]);
        int rnd = S[(S[i] + S[j]) % N];
        ciphertext[n] = rnd ^ plaintext[n];

    }
    return 0;
}

int RC4(char *key, char *plaintext, unsigned char *ciphertext) {

    unsigned char S[N];
    //KSA(key, S);
    PRGA(S, plaintext, ciphertext);
    return 0;
}
*/


LPVOID FVirtualAllocEx(void *arg)
{
        InternalInfo *p = (InternalInfo*)arg;
        return VirtualAllocEx(p->hProcess, 0, 0x100, MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);
}

void FWriteProcessMemory(void *arg)
{
        InternalInfo *p = (InternalInfo*)arg;
		    LPVOID data = (LPVOID)malloc(0x10);
        WriteProcessMemory( p->hProcess, p->RemoteCodeSegment, data, 0x10, (SIZE_T*)&p->BytesWrited);
}



HANDLE FHandleProcess(void *arg)
{
        int *pid = (int*)arg;
        return OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, *pid);
}

int FCurrentPId()
{
    return GetCurrentProcessId();
}

void SetChildID(InternalInfo *Iinfo)
{
  Func_int_void* pFCurrentPId  = (Func_int_void*)0x0707070707070707;
  Iinfo->Cid = pFCurrentPId();
}

int Fstrlen(char *arg)
{
    return strlen(arg);
}

char *Fmalloc(int size)
{
    char *pmalloc = (char*) malloc(size);
    if(!pmalloc)
        return 0;
    return pmalloc;
}

char *Fsprintf(int pid)
{
    char *buff = (char*)malloc(0x10);
    sprintf(buff,"%d",pid);
    return buff;
}

void* FBinOffset()
{
    char *x = "<3Mipu";
    return x+9; 

}


BYTE getHigherbits(BYTE b )
{
    return  (b & 0xf0) >> 4;
}

BYTE getLowerbits(BYTE b )
{
    return (b & 0xf);
}

BYTE Fxor(BYTE x, BYTE y)
{
    return x^y;
}


char* getPOS()
{
    return gPOS;
}


void genP(InternalInfo * Iinfo) {
  Func_pchar_void * pgetPOS = (Func_pchar_void * ) 0xDEADC0D3;
  Func_BYTE_BYTE * pgetHigherbits = (Func_BYTE_BYTE * ) 0x0202020202020202;
  Func_BYTE_BYTE * pgetLowerbits = (Func_BYTE_BYTE * ) 0x0101010101010101;

  char * POS = (char*)0xC0FFEE;
  char * p1 = (char*)Iinfo -> C1;
  char * p2 = (char*)Iinfo -> C2;
  char * key = (char*)Iinfo->RemoteDllSegment;
  int i;
  int k;
  BYTE tmp;
  BYTE tmp___0;
  BYTE tmp___1;
  BYTE tmp___2;
  unsigned long _1_genP_next;

  {
    {
      _1_genP_next = 11;
    }
    while (1) {
      switch (_1_genP_next) {
      case 4:
        tmp = getLowerbits((BYTE) * (key + k)); * (p1 + i % 36) = (char) tmp; {
          _1_genP_next = 3;
        }
        break;
      case 12:
        tmp___0 = getHigherbits((BYTE) * (key + k)); * (p1 + i % 36) = (char) tmp___0; {
          _1_genP_next = 3;
        }
        break;
      case 8:
        k = (int) POS[i] / 2; {
          _1_genP_next = 0;
        }
        break;
      case 1:
        ;
        if (i < 72) {
          {
            _1_genP_next = 8;
          }
        } else {
          {
            _1_genP_next = 9;
          }
        }
        break;
      case 3:
        i++; {
          _1_genP_next = 1;
        }
        break;
      case 11:
        i = 0; {
          _1_genP_next = 1;
        }
        break;
      case 9:
        ;
        return;
        break;
      case 6:
        ;
        if ((int) POS[i] % 2 == 0) {
          {
            _1_genP_next = 7;
          }
        } else {
          {
            _1_genP_next = 10;
          }
        }
        break;
      case 10:
        tmp___2 = getHigherbits((BYTE) * (key + k)); * (p2 + i % 36) = (char) tmp___2; {
          _1_genP_next = 3;
        }
        break;
      case 0:
        ;
        if (i < 36) {
          {
            _1_genP_next = 2;
          }
        } else {
          {
            _1_genP_next = 6;
          }
        }
        break;
      case 7:
        tmp___1 = getLowerbits((BYTE) * (key + k)); * (p2 + i % 36) = (char) tmp___1; {
          _1_genP_next = 3;
        }
        break;
      case 2:
        ;
        if ((int) POS[i] % 2 == 0) {
          {
            _1_genP_next = 4;
          }
        } else {
          {
            _1_genP_next = 12;
          }
        }
        break;
      }
    }
  }
}


/*void genP(InternalInfo *Iinfo)
{
    char *p1 = Iinfo->P1;
    char *p2 = Iinfo->P1;
    char *key = Iinfo->Key;

    for(int i = 0; i < 72; i++ )
    {
        int k = POS[i]/2;
        if(i < 36)
        {
            if( POS[i] % 2 == 0 )  p1[i%36] = getLowerbits((BYTE)key[k]);
            else p1[i%36] = getHigherbits((BYTE)key[k]);
        }
        else
        {
            if( POS[i] % 2 == 0 )  p2[i%36] = getLowerbits((BYTE)key[k]);
            else p2[i%36] = getHigherbits((BYTE)key[k]);
        }   
    }
}
*/


void Send()
{
    memcpy(Child_Iinfo->RemoteArgsSegment, (LPVOID)Child_Got, sizeof(MyCuteGoT));
    memcpy(((char*)Child_Iinfo->RemoteArgsSegment) + sizeof(MyCuteGoT), Child_Iinfo, sizeof(InternalInfo));
    memcpy(((char*)Child_Iinfo->RemoteArgsSegment) + sizeof(MyCuteGoT) + sizeof(InternalInfo), (char*)Child_Data, sizeof(InternalData));
}

void Reciveve(InternalInfo * Iinfo)
{
    memcpy((char*)Child_Got, (char*) Iinfo->RemoteArgsSegment, sizeof(MyCuteGoT));
    memcpy((char*)Child_Iinfo , ((char*)Iinfo->RemoteArgsSegment) + sizeof(MyCuteGoT), sizeof(InternalInfo));
    memcpy((char*)Child_Data , ((char*)Iinfo->RemoteArgsSegment) + sizeof(MyCuteGoT) + sizeof(InternalInfo), sizeof(InternalData));
}

void GenGot(MyCuteGoT *pGot)
{   
    pGot->FgetLowerbits = (LPVOID)&getLowerbits;
    pGot->FgetHigherbits = (LPVOID)&getHigherbits;
    pGot->Fxor = (LPVOID)&Fxor;
    pGot->Fswap = (LPVOID)&Fswap;
    pGot->Fmalloc = (LPVOID)&Fmalloc;
    pGot->Fstrlen = (LPVOID)&Fstrlen;
    pGot->FCurrentPId = (LPVOID)&FCurrentPId;
    pGot->FVirtualAllocEx = (LPVOID)&FVirtualAllocEx;
    pGot->FWriteProcessMemory = (LPVOID)&FWriteProcessMemory;
   // pGot->FCreateRemoteThread = (LPVOID)&FCreateRemoteThread;
    pGot->FHandleProcess = (LPVOID)&FHandleProcess;
    pGot->Fsprintf = (LPVOID)&Fsprintf;
    pGot->FgetPOS = (LPVOID)&getPOS;
    pGot->Fsend = (LPVOID)&Send;
    pGot->FReciveve = (LPVOID)&Reciveve;
    pGot->Binoffset = FBinOffset();
}



void GOINT(InternalInfo * Iinfo){
    init(Iinfo);
    GenGot(Child_Got);
    Send();
    return;
}
 
