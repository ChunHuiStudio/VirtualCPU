#pragma once

#define bit64data unsigned long long
#define bit32data unsigned int
#define bit16data unsigned short
#define bit08data unsigned char

#define GetJCQ(addr) JCQ.GetNativeJCQ(addr)

#define codelong (sizeof(char) * 64)

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <thread>
#include <cstdio>
#include <functional>

using namespace std;

#if __GNUC__ && __cplusplus<201402L
	constexpr chrono::milliseconds operator ""ms(unsigned long long ms) {
		return chrono::milliseconds(ms);
	}
#endif

struct CpuStatusFlag {
	private:
		static const bit08data Bit16Mode = 0;
		static const bit08data Bit32Mode = 1;
		static const bit08data Bit64Mode = 2;
		static const bit08data RealMode  = 3;
		static const bit08data SafeMode  = 4;
		static const bit08data PageMode  = 5;
	public:
		bit64data Data = 0b1001;
	public:
		bool Get(const char po[]);
		void operator[](const char po[]);
	private:
		void HelpFunc(bit08data s);
		bool HelpGetFunc(bit08data s);
};

struct CPUJCQs {
	private://64b
	bit64data rax = 0;
	bit64data rbx = 0;
	bit64data rcx = 0;
	bit64data rdx = 0;
	bit64data rex = 0;
	bit64data rfx = 0;
	bit64data r7 = 0;
	bit64data r8 = 0;
	bit64data r9 = 0;
	bit64data r10 = 0;
	bit64data r11 = 0;
	bit64data r12 = 0;
	bit64data r13 = 0;
	bit64data r14 = 0;
	bit64data r15 = 0;
	bit64data r16 = 0;

	//ts&ro
	bit64data tr = 0;//stack root address

	bit64data ts = 0;//stack shift address

	bit64data ca = 0;//code address

	public:
	CpuStatusFlag flag;

	bit64data* GetNativeJCQ(string name);
};

struct Memory {
	bit64data* MemoryStart = nullptr;
	bit64data* MemoryEnd = nullptr;
	unsigned long long MemoryLong = 1024 * 1024 * 64;//B 64MB

	Memory();
	~Memory();

	bit64data* GetMemory(bit64data pi);

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
	bit64data Data = 0;

	bool Throw = false;
	int ThrowID = 0;
	int ThrowTo = 0;
	bit64data ThrowData = 0;
};

struct Thread {
	CPUJCQs JCQ;
	CPUStatus Status = CPUStatus::Shutdown;
	CPUAuthority Authority = CPUAuthority::User;
	Memory* MemoryPoint = nullptr;
	ChannelAtThread channel;

	bit64data* LetsNumberBuffer = nullptr;
	int LetsNumberBufferLength = 512;
	int HadLets = 0;

	Thread();
	Thread(bit64data code,bit64data sr);
	~Thread();

	void Work();

	void Help(function<bit64data* (bit64data)> GetMemory,const char* DisplayFlag);

	string GetCode(int length=16);

	vector<bit64data*> GetArgs(string con,int conlen,int args, function<bit64data* (bit64data)> GetMemory,vector<char> mid = {','},vector<char> end = {' '});

	void Init(bit64data code,bit64data sr);
};

struct Core {
	array<Thread,2> Data;

	operator int();
	Thread& operator[](int ID);
};

struct ThreadChannel {
	#if __GNUC__ && __cplusplus>201703L
		#define threadClass jthread
	#else
		#define threadClass thread
	#endif
	threadClass ListenThread = threadClass([this]{
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

	~ThreadChannel() {
		#if threadClass==thread
			ListenThread.join();
		#endif
	}
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

void testInsr(PC* pc,bit64data add,char d[]);
void testInsr(Memory* mem,bit64data add,char d[]);

bool starts_with(string d,string ser) {
	#if __GNUC__ && __cplusplus<=201703L
		for (int i = 0;i < ser.size();++i) {
			if (d[i] != ser[i]) {
				return false;
			}
		}
		return true;
	#else
		return d.starts_with(ser);
	#endif
}