#pragma once
#include <string>
#include <iostream>

using namespace std;

#if _WIN32
	void SetColorAndBackground(int ForgC = 0x7, int BackC = 0x0);
#endif

void DisplayAtColor(string str,int color);

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