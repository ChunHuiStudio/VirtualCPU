#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define WorkBit unsigned long long
#define CodeLong 0x8

using namespace std;

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
static WorkBit                CodeBaseAddress  = 0;
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