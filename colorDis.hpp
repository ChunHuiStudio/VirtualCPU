#include <iostream>

using namespace std;

#if _WIN32
	#include <windows.h>

	void SetColorAndBackground(int ForgC = 0x7, int BackC = 0x0) {
		WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wColor);
	}
#endif

void DisplayAtColor(string str,int color) {
	if (color < 0x0 || color > 0xf) {
		cout << str;
	} else {
		#if _WIN32
			SetColorAndBackground(color);
			cout << str;
			SetColorAndBackground();
		#elif __linux__
    		cout << "\033[3"+to_string(color)+";1m" + str + "\033[0m";
		#else
			cout << str;
		#endif
	}
}

enum DisplayColor {
	#if _WIN32
		black,
		blue,
		green,
		null1,
		red,
		purple,
		yellow,
		white
	#elif __linux__
		black,
		red,
		green,
		yellow,
		blue,
		purple,
		null1,
		white
	#endif
};


std::string CPUInfo() {
	PC pc;
	auto tmp = pc.cpu.CPUInfo;
	std::string ret = "";
	for (int i = 0;i < 7;++i) {
		std::string push = tmp[i];
		ret += push;
	}
	return ret;
}