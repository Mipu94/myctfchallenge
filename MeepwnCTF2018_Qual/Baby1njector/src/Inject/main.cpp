//#include <iostream>
#include <stdio.h>
#include <windows.h>
#include "mydll.h"
#include "inject.h"


/*
bool is_regedit_created(const char strKeyName[]){
	HKEY hKey = NULL;
	long sts = RegOpenKeyEx(HKEY_CURRENT_USER, strKeyName, 0, KEY_ALL_ACCESS, &hKey);
	if (ERROR_NO_MATCH == sts || ERROR_FILE_NOT_FOUND == sts || ERROR_SUCCESS != sts)
		return 0;
	return 1;
}

bool new_regeditkey(const char strKeyName[],LPCTSTR name, LPCTSTR value) 
{	
	HKEY hKey = NULL;
	DWORD rtime;

	//Step 1: Open the key
	long sts = RegOpenKeyEx(HKEY_CURRENT_USER, strKeyName, 0, KEY_ALL_ACCESS, &hKey);
	RegCreateKeyEx(HKEY_CURRENT_USER, strKeyName, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL );
	LONG j = RegSetValueEx(hKey, name, 0, REG_BINARY, (LPBYTE)value, 36);
	//Step 2: If failed, create the key
	if (ERROR_NO_MATCH == sts || ERROR_FILE_NOT_FOUND == sts)
	{
		RegCreateKeyEx(HKEY_CURRENT_USER, strKeyName, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL );
		LONG j = RegSetValueEx(hKey, name, 0, REG_BINARY, (LPBYTE)value, 36);
		if (ERROR_SUCCESS != j)
			rtime = 0;
		else
			rtime = 1;
	}
	else if (ERROR_SUCCESS != sts)
	{
		
		rtime = 0;
	}
	RegCloseKey(hKey);
	return rtime;
}

char* read_regeditkey(char strKeyName[],LPCTSTR name){

	char *buff=(char*)malloc(0x100);
	DWORD length = 200;
	DWORD type = REG_SZ;
	HKEY hKey = NULL;
	RegOpenKeyEx(HKEY_CURRENT_USER, strKeyName, 0, KEY_ALL_ACCESS, &hKey);
	RegQueryValueEx(hKey, name, 0 , (LPDWORD)&type, (LPBYTE)buff, &length);
	return buff;
}

*/



void Send()
{
    WriteProcessMemory(pi.hProcess, Parent_Iinfo->RemoteArgsSegment, (LPVOID)Child_Got, sizeof(MyCuteGoT), NULL);
    WriteProcessMemory(pi.hProcess, ((char*)Parent_Iinfo->RemoteArgsSegment) + sizeof(MyCuteGoT), Parent_Iinfo, sizeof(InternalInfo), NULL);
    WriteProcessMemory(pi.hProcess, ((char*)Parent_Iinfo->RemoteArgsSegment) + sizeof(MyCuteGoT) + sizeof(InternalInfo), (char*)Parent_Data, sizeof(InternalData), NULL);
}

void Reciveve(InternalInfo * Parent_Iinfo)
{
	size_t size;
    ReadProcessMemory(pi.hProcess, (char*) Parent_Iinfo->RemoteArgsSegment, (char*)Child_Got, sizeof(MyCuteGoT), &size);
    ReadProcessMemory(pi.hProcess, ((char*)Parent_Iinfo->RemoteArgsSegment) + sizeof(MyCuteGoT), (char*)Parent_Iinfo, sizeof(InternalInfo), &size);
    ReadProcessMemory(pi.hProcess, ((char*)Parent_Iinfo->RemoteArgsSegment) + sizeof(MyCuteGoT) + sizeof(InternalInfo), (char*)Parent_Data, sizeof(InternalData), &size);
}

PROCESS_INFORMATION get_handle(char * pathname){
	CreateProcess(NULL,pathname,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&si,&pi);
	return pi;
}

char *xorencode(char * message, size_t messagelen, char * key) {
    int keylen = strlen(key);
    char * encrypted = (char*)malloc(messagelen+1);
    int i;
    for(i = 0; i < messagelen; i++) {
        encrypted[i] = message[i] ^ key[i % keylen];
    }
    encrypted[messagelen] = '\0';
    return encrypted;
}

void ReplaceAddress(char *src, char* patten, void* value ,size_t sizec)
{
	char *psrc = src;
	int i;
	for (i=0; i<sizec; i++)
	{
		if(!memcmp((void*)&psrc[i],(void*)patten,8))
		{
			memcpy((void*)(&psrc[i]), value, 8 );
			i += 8;	
		}	
		
	}
}

void FixCallFunMyCuteGot(char *shellcode, size_t len)
{
	int i;
	char buff[0x10];
	void *pGot = (void*)Child_Got;

	for(i = 0; i<= 0x11; i++)
	{
		memset(buff,0,sizeof(buff));
		memcpy(buff,(void*)&offset_GOT[i*8],8);
		ReplaceAddress(shellcode,buff,pGot+i*8,len);
	}	

}

void* gen_shellcodeA(InternalInfo *Parent_Iinfo)
{
	void* shell = malloc(0x100);
	char *opcode0 = "\x48\xB9\x01\x02\x03\x04\x05\x06\x07\x08\x55\x48\x89\xE5\x48\x83\xEC\x30\x48\x89\x4D\x10\x48\xC7\x45\xF8\x00\x00\x00\x00\x48\x8B\x4D\x10\x48\xB8\x01\x02\x03\x04\x05\x06\x07\x08\xFF\xD0\x48\x89\x45\xF8\x48\x8B\x45\xF8\x48\xba\x01\x02\x03\x04\x05\x06\x07\x08\x48\x89\xC1\x48\xb8\x01\x02\x03\x04\x05\x06\x07\x08\xFF\xD0\x48\xB9\x01\x02\x03\x04\x05\x06\x07\x08\x48\x89\x45\xF0\x48\x8B\x45\xF0\xFF\xD0\x90\x48\x83\xC4\x30\x5D\xC3\x55\x57\x53\x48\x81\xEC\x80\x03\x00\x00\x48";
	memcpy(shell,opcode0,0x100);
	*(int long long*)(shell+2) = (int long long)Parent_Iinfo->RemoteArgsSegment;
	*(int long long*)(shell+36) = (int long long)Parent_Iinfo->EntryFuncPointer;	
	*(int long long*)(shell+36+8+12) = (int long long)(Parent_Iinfo->EntryFuncName);	
	*(int long long*)(shell+36+8+12+8+5) = (int long long)Parent_Iinfo->EntryFuncPointer+0x5cc;
	*(int long long*)(shell+36+8+12+8+5+8+4) = (int long long)Parent_Iinfo->RemoteArgsSegment;
	return shell;
}

void* ShellcodeRound1()
{
	char *shell = xorencode(shellcode1,lenshellcode1,"\xde\xad\xc0\xd3");
	*(int long long*)(shell+2) = (int long long)Parent_Iinfo->RemoteArgsSegment+sizeof(MyCuteGoT);
	ReplaceAddress(shell,"\xda\xda\xda\xda\xda\xda\xda\xda",&Child_Got->Binoffset,lenshellcode1);
	FixCallFunMyCuteGot(shell, lenshellcode1);
	return shell;
}
 
void* ShellcodeRound2() //gen S1
{
	char *shell = xorencode(shellcode2,lenshellcode2,"\xde\xad\xc0\xd3");
	*(int long long*)(shell+2) = (int long long)Parent_Iinfo->Pid;
	*(int long long*)(shell+12) = (int long long)Parent_Iinfo->S;
	FixCallFunMyCuteGot(shell, lenshellcode2);
	return shell;
}

void *ShellcodeRound3()// take C1
{
	char *shell = xorencode(shellcode3,lenshellcode3,"\xde\xad\xc0\xd3");
	*(int long long*)(shell+2) = (int long long)Parent_Iinfo->S;
	*(int long long*)(shell+12) = (int long long)Parent_Iinfo->P1;
	*(int long long*)(shell+22) = (int long long)Parent_Iinfo->C1;
	FixCallFunMyCuteGot(shell, lenshellcode3);
	return shell;
}


void *ShellcodeRound4()//get Cid
{
	char *shell = xorencode(shellcode4,lenshellcode4,"\xde\xad\xc0\xd3");
	*(int long long*)(shell+2) = (int long long)Parent_Iinfo->RemoteArgsSegment+sizeof(MyCuteGoT);
	FixCallFunMyCuteGot(shell, lenshellcode4);
	return shell;
}

void* ShellcodeRound5()//gen S
{
	char *shell = xorencode(shellcode2,lenshellcode2,"\xde\xad\xc0\xd3");
	*(int long long*)(shell+2) = (int long long)Parent_Iinfo->Cid;
	*(int long long*)(shell+12) = (int long long)Parent_Iinfo->S;
	FixCallFunMyCuteGot(shell, lenshellcode2);
	return shell;
}

void *ShellcodeRound6()//take C2
{
	char *shell = xorencode(shellcode3,lenshellcode3,"\xde\xad\xc0\xd3");
	*(int long long*)(shell+2) = (int long long)Parent_Iinfo->S;
	*(int long long*)(shell+12) = (int long long)Parent_Iinfo->P2;
	*(int long long*)(shell+22) = (int long long)Parent_Iinfo->C2;
	FixCallFunMyCuteGot(shell, lenshellcode3);
	return shell;
}



void write2file(char*filename, char* buff, int size)
{
	FILE *fp;
	fp = fopen(filename, "wb");
	if(fp == 0)
	   return;
	fwrite(buff, 1, size, fp);
	fclose(fp);
}

char* readfile(char*filename, int size)
{
	FILE *fp;
	fp = fopen(filename,"rb");
	char *buff = (char*)malloc(0x50);
	fread(buff,size,1,fp);
	fclose(fp);
	return buff;
}

/*
void print_hex(char *str, int size)
{
	 for(int i=0; i < size ;i++)
    {
    	unsigned int x = (unsigned int)str[i]&0xff;
    	printf("\\x%02hhx",x);
    }
    printf("\n");
}

int ae_load_file_to_memory(const char *filename, char **result) 
{ 
	int size = 0;
	FILE *f = fopen(filename, "rb");
	if (f == NULL) 
	{ 
		*result = NULL;
		return -1; // -1 means file opening fail 
	} 
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	*result = (char *)malloc(size+1);
	if (size != fread(*result, sizeof(char), size, f)) 
	{ 
		free(*result);
		return -2; // -2 means file reading fail 
	} 
	fclose(f);
	(*result)[size] = 0;
	return size;
}
*/
void InitInject()
{
	Parent_Iinfo = (InternalInfo *)malloc(sizeof(InternalInfo));
    Parent_Data = (InternalData*)malloc(sizeof(InternalData));
    Child_Got = (MyCuteGoT*)malloc(sizeof(MyCuteGoT));
    memset(Parent_Iinfo,1,sizeof(InternalInfo));
    memset(Parent_Data,2,sizeof(InternalData));
    memset(Child_Got,3,sizeof(MyCuteGoT));
    pi = get_handle("C:\\Windows\\System32\\svchost.exe");
	if (pi.hProcess == NULL) {
		exit(1);
	}

	TCHAR szTempPathBuffer[MAX_PATH];
	GetTempPath(MAX_PATH, szTempPathBuffer);
	GetTempFileName(szTempPathBuffer, 0, 0, TempFile);
	char * data = xorencode(MeeLoader, MeeLoaderSize, "GABOGOBO");
	write2file(TempFile,data,MeeLoaderSize);
	free(data);
}



int GetInjectInfo(char *pre_key){
	//char* pre_key= "MeePwnCTF{Th3_C4ll_Of_Th3_M0unt4in5}";	
	InitInject();    
    

	//load MeeLoader -> injected process
	LPVOID BUFF_RMT = VirtualAllocEx(pi.hProcess, NULL, 0x1000, MEM_COMMIT , PAGE_READWRITE);
	char *Loader_Path = TempFile;
	SIZE_T bytes_write = 0;
	WriteProcessMemory( pi.hProcess, BUFF_RMT, Loader_Path,strlen(Loader_Path)+1, &bytes_write );
	HMODULE hDll = LoadLibrary("Kernel32.dll");
  	LPVOID loadLibAddr = (LPVOID)GetProcAddress(hDll, "LoadLibraryA");
	HANDLE rThread = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)(loadLibAddr), BUFF_RMT, 0, NULL);
	WaitForSingleObject( rThread, INFINITE );
	CloseHandle( rThread );
	//end load
	
	
	char *myDLL = xorencode(BSNL2,BSNL2Size,"GABOGOBO");
	SIZE_T dllFileLength = BSNL2Size;
	
	LPVOID RemoteDllSegment = VirtualAllocEx(pi.hProcess, 0,dllFileLength+1,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	WriteProcessMemory( pi.hProcess, RemoteDllSegment, myDLL , dllFileLength+1, &bytes_write );
	

	/**********************/
	
	bytes_write = 0;
	hDll = LoadLibraryA(Loader_Path);
  	loadLibAddr = (LPVOID)GetProcAddress(hDll, "LoadML");

	Parent_Iinfo->RemoteDllSegment = RemoteDllSegment;
	Parent_Iinfo->LenghtRemoteDllSegment = dllFileLength;
	Parent_Iinfo->RemoteArgsSegment = BUFF_RMT;
	Parent_Iinfo->LenghtRemoteArgsSegment = 0x1000;
	Parent_Iinfo->EntryFuncPointer = loadLibAddr;
	

	Parent_Iinfo->RemoteCodeSegment = VirtualAllocEx(pi.hProcess, 0,0x1000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	Parent_Iinfo->LenghtRemoteCodeSegment = 0x1000;

	char *RMT_DATA = (char*)Parent_Iinfo->RemoteArgsSegment+sizeof(InternalInfo);
	Parent_Iinfo->EntryFuncName = &((InternalData*)RMT_DATA)->EntryFuncName;
	Parent_Iinfo->Key = &((InternalData*)RMT_DATA)->Key;

	memcpy(&((InternalData*)Parent_Data)->EntryFuncName,"_Z5GOINTP14s_InternalInfo",strlen("_Z5GOINTP14s_InternalInfo")+1);
	memcpy(&((InternalData*)Parent_Data)->Key,pre_key,strlen(pre_key)+1);
	void *sc = gen_shellcodeA((InternalInfo*)Parent_Iinfo);

	WriteProcessMemory( pi.hProcess, Parent_Iinfo->RemoteArgsSegment, Parent_Iinfo,sizeof(InternalInfo), NULL );
	WriteProcessMemory( pi.hProcess, RMT_DATA, Parent_Data,sizeof(InternalData), NULL );
	WriteProcessMemory( pi.hProcess, Parent_Iinfo->RemoteCodeSegment, sc, 0x100, NULL );

	rThread = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)(Parent_Iinfo->RemoteCodeSegment), 0, 0, NULL);
	WaitForSingleObject( rThread, INFINITE );
	//CloseHandle( rThread );
	Reciveve(Parent_Iinfo);
	free(sc);
	remove(TempFile);
	return 0;
}

void run()
{
	Send();
	long long int  child_info = (long long int)Parent_Iinfo->RemoteArgsSegment + sizeof(MyCuteGoT);
	long long int  pRemoteCode = (long long int)Parent_Iinfo->RemoteCodeSegment+0x100;

	char *runcode = (char*)malloc(SizeRunCode+1);
	memcpy(runcode, RunCode, SizeRunCode);
	ReplaceAddress(runcode, "\xc0\xc0\xc0\xc0\xc0\xc0\xc0\xc0", (void*)&pRemoteCode, SizeRunCode);
	ReplaceAddress(runcode, "\x01\x02\x03\x04\x05\x06\x07\x08", (void*)&child_info, SizeRunCode);

	FixCallFunMyCuteGot(runcode,SizeRunCode);

	WriteProcessMemory( pi.hProcess, Parent_Iinfo->RemoteCodeSegment, runcode, SizeRunCode, NULL );
	free(runcode);
	WriteProcessMemory( pi.hProcess, Parent_Iinfo->RemoteCodeSegment+0x100, InjectCode, SizeInjectCode, NULL );
	free(InjectCode);
	HANDLE rThread = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)(Parent_Iinfo->RemoteCodeSegment), 0, 0, NULL);
	WaitForSingleObject( rThread, INFINITE );
	CloseHandle( rThread );
	Reciveve(Parent_Iinfo);

}

void InjectRound1()
{
	InjectCode = (char*)ShellcodeRound1();
	SizeInjectCode = lenshellcode1;
	run();	
}

void InjectRound2()
{
	InjectCode = (char*)ShellcodeRound2();
	SizeInjectCode = lenshellcode2;
	run();
}

void InjectRound3()
{
	InjectCode = (char*)ShellcodeRound3();
	SizeInjectCode = lenshellcode3;
	run();
}

void InjectRound4()
{
	InjectCode = (char*)ShellcodeRound4();
	SizeInjectCode = lenshellcode4;
	run();
}

void InjectRound5()
{
	InjectCode = (char*)ShellcodeRound5();
	SizeInjectCode = lenshellcode2;
	run();
}

void InjectRound6()
{
	InjectCode = (char*)ShellcodeRound6();
	SizeInjectCode = lenshellcode3;
	run();
}




BYTE getHigherbits(BYTE b )
{
    return  (b & 0xf0) >> 4;
}

BYTE getLowerbits(BYTE b )
{
    return (b & 0xf);
}


/*
void genP()
{
    char *p1 = (char*)malloc(0x100);
    char *p2 = (char*)malloc(0x100);
    char *key = "MeePwnCTF{Th3_C4ll_Of_Th3_M0unt4in5}";
	char POS[80] = {53,46,55,59,64,22,30,4,20,15,31,41,51,68,27,14,34,66,21,48,33,70,3,17,35,25,12,49,44,63,56,60,1,11,19,10,50,62,57,52,36,8,42,2,6,26,45,40,71,69,47,61,24,9,39,32,65,5,0,58,13,28,23,67,18,54,29,16,43,38,7,37};
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

    print_hex(p1,36);
    print_hex(p2,36);
}
*/


int main(int argc, char** argv) {

	char *KEY;
	KEY = readfile("flag",36);
	//KEY = "MeePwnCTF{Th3_C4ll_Of_Th3_M0unt4in5}";
	//printf("%s",KEY);
	//print_hex(KEY,36);
	
	GetInjectInfo(KEY);
  	InjectRound1();
	
	Parent_Iinfo->Cid = pi.dwProcessId;
	Parent_Iinfo->Pid = GetCurrentProcessId();
	//genP();

  	InjectRound2();
  	
  	InjectRound3();
  	
  	InjectRound4();
	
	InjectRound5();	
	
	InjectRound6();	
	
	char * buff = (char*) malloc(0x60);
	memcpy(buff,(char*)&Parent_Data->C1,36);
	memcpy(buff+36,(char*)&Parent_Data->C2,36);
	//printf("%d",Parent_Iinfo->Cid);
	write2file("flag",buff,72);
	//print_hex(buff,72);
	free(buff);
  	return EXIT_SUCCESS;  	
}
