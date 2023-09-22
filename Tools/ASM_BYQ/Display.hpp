#pragma once
#include <string>
#include <iostream>

using namespace std;

#if _WIN32
	__declspec(dllexport) void SetColorAndBackground(int ForgC = 0x7, int BackC = 0x0);
#endif

__declspec(dllexport) void DisplayAtColor(string str,int color);

__declspec(dllexport) enum DisplayColor {
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