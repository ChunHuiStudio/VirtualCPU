#pragma once

#define bit64jcq unsigned long long
#define bit32jcq unsigned int
#define bit16jcq unsigned short
#define bit08jcq unsigned char

#define bit64doujcq double
#define bit32doujcq float

#define bit64addr unsigned long long
#define bit32addr unsigned int
#define bit16addr unsigned short
#define bit08addr unsigned char

#define codelong (sizeof(char) * 64)

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <thread>
#include <cstdio>
#include <functional>

using namespace std;

constexpr chrono::milliseconds operator ""ms(unsigned long long ms) {
	return chrono::milliseconds(ms);
}

struct CpuStatusFlag {
	private:
		static const bit08jcq Bit16Mode = 0;
		static const bit08jcq Bit32Mode = 1;
		static const bit08jcq Bit64Mode = 2;
		static const bit08jcq RealMode  = 3;
		static const bit08jcq SafeMode  = 4;
		static const bit08jcq PageMode  = 5;
	public:
		bit64jcq Data = 0b1001;
	public:
		bool Get(const char po[]);
		void operator[](const char po[]);
	private:
		void HelpFunc(bit08jcq s);
		bool HelpGetFunc(bit08jcq s);
};

struct CPUJCQs {
	private://64b
	bit64jcq rax = 0;
	bit64jcq rbx = 0;
	bit64jcq rcx = 0;
	bit64jcq rdx = 0;
	bit64jcq rex = 0;
	bit64jcq rfx = 0;
	bit64jcq r7 = 0;
	bit64jcq r8 = 0;
	bit64jcq r9 = 0;
	bit64jcq r10 = 0;
	bit64jcq r11 = 0;
	bit64jcq r12 = 0;
	bit64jcq r13 = 0;
	bit64jcq r14 = 0;
	bit64jcq r15 = 0;
	bit64jcq r16 = 0;

	//32b
	bit32jcq eax = 0;
	bit32jcq ebx = 0;
	bit32jcq ecx = 0;
	bit32jcq edx = 0;
	bit32jcq eex = 0;
	bit32jcq efx = 0;
	bit32jcq e7 = 0;
	bit32jcq e8 = 0;
	bit32jcq e9 = 0;
	bit32jcq e10 = 0;
	bit32jcq e11 = 0;
	bit32jcq e12 = 0;
	bit32jcq e13 = 0;
	bit32jcq e14 = 0;
	bit32jcq e15 = 0;
	bit32jcq e16 = 0;

	//16b
	bit16jcq ax = 0;
	bit16jcq bx = 0;
	bit16jcq cx = 0;
	bit16jcq dx = 0;
	bit16jcq ex = 0;
	bit16jcq fx = 0;
	bit16jcq x7 = 0;
	bit16jcq x8 = 0;
	bit16jcq x9 = 0;
	bit16jcq x10 = 0;
	bit16jcq x11 = 0;
	bit16jcq x12 = 0;
	bit16jcq x13 = 0;
	bit16jcq x14 = 0;
	bit16jcq x15 = 0;
	bit16jcq x16 = 0;

	//ts&ro
	bit64jcq tr = 0;//stack root address

	bit64jcq ts = 0;//stack shift address

	bit64jcq ca = 0;//code address

	public:
	CpuStatusFlag flag;

	bit64jcq* GetBit64JCQ(string name);
	bit32jcq* GetBit32JCQ(string name);
	bit16jcq* GetBit16JCQ(string name);
	bit64jcq* GetTCJCQ(string name);
};

struct Memory {
	void* MemoryStart = nullptr;
	void* MemoryEnd = nullptr;
	unsigned long long MemoryLong = 1024 * 1024 * 64;//B 64MB

	Memory();
	~Memory();

	bit08jcq* GetMemory(bit08addr pi);
	bit16jcq* GetMemory(bit16addr pi);
	bit32jcq* GetMemory(bit32addr pi);
	bit64jcq* GetMemory(bit64addr pi);

	operator unsigned long long();
};

enum CPUStatus {
	Working,//Working
	Wait,//
	IO,//IO Disk NET...
	HLT,//HLT Command
	Shutdown,//No Start
	Ukdown
};

enum CPUAuthority {
	System,
	Service,
	Admin,
	User
};

struct ChannelAtThread {
	bool HadData = false;
	int ID = 0;
	bit64jcq Data = 0;

	bool Throw = false;
	int ThrowID = 0;
	int ThrowTo = 0;
	bit64jcq ThrowData = 0;
};

struct Thread {
	CPUJCQs JCQ;
	CPUStatus Status = CPUStatus::Shutdown;
	CPUAuthority Authority = CPUAuthority::User;
	Memory* MemoryPoint = nullptr;
	ChannelAtThread channel;

	void* LetsNumberBuffer = nullptr;
	int LetsNumberBufferLength = 512;
	int HadLets = 0;

	~Thread();

	void Work();

	template <typename TJCQ,typename Taddr>
	void WorkForBit();

	template <typename T,typename TAddr>
	void Help(function<T* (TAddr)> GetMemory,function<T* (const string)> GetJCQ,const char* DisplayFlag);

	string GetCode(int length=16);

	template <typename WorkBitJCQT,typename TAddr>
	vector<WorkBitJCQT*> GetArgs(string con,int conlen,int args, function<WorkBitJCQT* (TAddr)> GetMemory,function<WorkBitJCQT* (const string)> GetJCQ,vector<char> mid = {','},vector<char> end = {' '});

	template <typename WorkBitJCQT,typename TAddr>
	vector<WorkBitJCQT> GetDataArgs(string con,int conlen,int args, function<WorkBitJCQT* (TAddr)> GetMemory,function<WorkBitJCQT* (const string)> GetJCQ,vector<char> mid = {','},vector<char> end = {' '});

	void Init(bit16addr code,bit16addr sr);
};

struct Core {
	array<Thread,2> Data;

	operator int();
	Thread& operator[](int ID);
};

struct ThreadChannel {
	thread ListenThread = thread([=]{
		while (true) {
			bool exit = true;
			for (int i = 0;i < ThreadNumber;++i) {
				if ((Threads+i)->channel.Throw) {
					if ((Threads+i)->channel.ThrowTo == -1) {
						(Threads+i)->channel.HadData = true;
						(Threads+i)->channel.ID = (Threads+i)->channel.ThrowID;
						(Threads+i)->channel.Data = (Threads+i)->channel.ThrowData;
						(Threads+i)->channel.Throw = false;
					} else {
						int tt = (Threads+i)->channel.ThrowTo;
						(Threads+tt)->channel.HadData = true;
						(Threads+tt)->channel.ID = (Threads+i)->channel.ThrowID;
						(Threads+tt)->channel.Data = (Threads+i)->channel.ThrowData;
						(Threads+i)->channel.Throw = false;
					}
				}
				exit = exit && ((Threads+i)->Status == CPUStatus::Shutdown);
			}
			if (exit) {
				break;
			}
			this_thread::sleep_for(13ms);
		}
	});
	Thread* Threads = nullptr;
	Memory* memory = nullptr;
	int ThreadNumber = 0;

	~ThreadChannel() {ListenThread.join();}
};

template <int Cores>
struct CPU {
	array<Core,Cores> Data;
	Memory* memoryp = nullptr;
	ThreadChannel threadc;

	void Init();

	operator int();
	Thread& operator[](int ID);
};

struct PC {
	CPU<1> cpu;
	Memory memory = Memory();

	void Powar();
};

void testInsr(PC* pc,bit64addr add,char d[]);
void testInsr(Memory* mem,bit64addr add,char d[]);

bool starts_with(string d,string ser) {
	for (int i = 0;i < ser.size();++i) {
		if (d[i] != ser[i]) {
			return false;
		}
	}
	return true;
}



template <>
void Thread::WorkForBit<bit16jcq,bit16addr>();

template <>
void Thread::WorkForBit<bit32jcq,bit32addr>();

template <>
void Thread::WorkForBit<bit64jcq,bit64addr>();