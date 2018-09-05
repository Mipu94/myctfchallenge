#ifndef _DLL_H_
#define _DLL_H_

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

char *P1;
char *P2;
unsigned char *C1;
unsigned char *C2;
char gPOS[80] = {53,46,55,59,64,22,30,4,20,15,31,41,51,68,27,14,34,66,21,48,33,70,3,17,35,25,12,49,44,63,56,60,1,11,19,10,50,62,57,52,36,8,42,2,6,26,45,40,71,69,47,61,24,9,39,32,65,5,0,58,13,28,23,67,18,54,29,16,43,38,7,37};

typedef char* Func_pchar_void (void);
typedef BYTE Func_BYTE_BYTE (BYTE);
typedef void Func_void_pchar_pchar (unsigned char*,unsigned char*);
typedef int Func_int_pchar (char*);
typedef char* Func_pchar_int (int);
typedef int Func_int_void (void);




typedef struct s_MyCuteGoT
{
	LPVOID FgetLowerbits;
	LPVOID FgetHigherbits;
	LPVOID Fxor;
    LPVOID Fswap;
	LPVOID Fmalloc;
	LPVOID Fstrlen;
	LPVOID FCurrentPId;
	LPVOID FVirtualAllocEx;
	LPVOID FWriteProcessMemory;
	LPVOID FCreateRemoteThread;
	LPVOID FHandleProcess;
	LPVOID Fsprintf;
    LPVOID FgetPOS;
    LPVOID Fsend;
    LPVOID FReciveve;
	LPVOID Binoffset;
} MyCuteGoT;

typedef struct s_InternalInfo
{
        int Pid;
        int Cid;
        HANDLE hProcess;
        LPVOID RemoteDllSegment;
        SIZE_T LenghtRemoteDllSegment;
        LPVOID RemoteArgsSegment;
        SIZE_T LenghtRemoteArgsSegment;
        LPVOID RemoteCodeSegment;
        SIZE_T LenghtRemoteCodeSegment;
        LPVOID EntryFuncName;
        SIZE_T BytesWrited;
        LPVOID EntryFuncPointer;
        LPVOID InternalData;
        LPVOID Key;
        LPVOID C1;
        LPVOID C2;
        LPVOID P1;
        LPVOID P2;
        unsigned char* S;

}InternalInfo;

typedef struct s_Data
{
    char RemoteArgsSegment[0x100];
    char EntryFuncName[0x50];
    char Key[0x50];
    char C1[0x50];
    char C2[0x50];
    char P1[0x50];
    char P2[0x50];
}InternalData;

MyCuteGoT *Child_Got;
InternalInfo *Child_Iinfo;
InternalData *Child_Data;
HANDLE hPid;

DLLIMPORT void GOINT(InternalInfo * Iinfo);
#endif

