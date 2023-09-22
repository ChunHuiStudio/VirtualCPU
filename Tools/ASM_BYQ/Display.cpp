#include "Display.hpp"

#ifdef _WIN32
	namespace Windows {
		#include <windows.h>
	}

	void SetColorAndBackground(int ForgC,int BackC) {
		Windows::WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);
		Windows::SetConsoleTextAttribute(Windows::GetStdHandle((Windows::DWORD)-11), wColor);
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