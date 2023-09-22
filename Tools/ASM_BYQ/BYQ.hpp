#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <array>

#define WorkBit unsigned long long
#define CodeLong 0x8
#define CodeWith (64 - 2)

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

class LinkTableClass {
public:
	string LinkName;
	string Replace;

	LinkTableClass() : LinkTableClass("","") {}
	LinkTableClass(string n,string r) {
		LinkName = n;
		Replace = r;
	}
};

static string                 EndFile = "a.cw";
static string                 FileData = "";
static vector<LinkTableClass> LinkTable;
static unsigned long long     CodeNumber = 0;
static WorkBit                StackBaseAddress = 0x1000;
static WorkBit                CodeBaseAddress  = 0x8;
static bool                   DontWork = false;

class Command final {
public:
	class FirstWork final {
	private:
		string FileName;
	public:
		FirstWork() : FirstWork("") {}
		FirstWork(string filename) {
			FileName = filename;
		}

		void operator()();
	};
	class LastWork  final {
	public:
		void operator()();
	};
};