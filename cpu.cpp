#include "cpu.hpp"

using namespace std;

void CpuStatusFlag::operator[](const char po[]) {
	char d1 = po[0];
	char d2 = po[1];
	if (d1=='1'&&d2=='6') {
		HelpFunc(Bit16Mode);
	} else if (d1=='3'&&d2=='2') {
		HelpFunc(Bit32Mode);
	} else if (d1=='6'&&d2=='4') {
		HelpFunc(Bit64Mode);
	} else if (d1=='r'&&d2=='m') {
		HelpFunc(RealMode);
	} else if (d1=='s'&&d2=='m') {
		HelpFunc(SafeMode);
	} else if (d1=='p'&&d2=='m') {
		HelpFunc(PageMode);
	}
}
bool CpuStatusFlag::Get(const char po[]) {//length=2(yy)
	char d1 = po[0];
	char d2 = po[1];
	if (d1=='1'&&d2=='6') {
		return HelpGetFunc(Bit16Mode);
	} else if (d1=='3'&&d2=='2') {
		return HelpGetFunc(Bit32Mode);
	} else if (d1=='6'&&d2=='4') {
		return HelpGetFunc(Bit64Mode);
	} else if (d1=='r'&&d2=='m') {
		return HelpGetFunc(RealMode);
	} else if (d1=='s'&&d2=='m') {
		return HelpGetFunc(SafeMode);
	} else if (d1=='p'&&d2=='m') {
		return HelpGetFunc(PageMode);
	} else {
		return false;
	}
}
void CpuStatusFlag::HelpFunc(bit08jcq s) {
	Data = Data >> s;
	if (Data & 1ull == 1) {
		--Data;
	} else {
		++Data;
	}
	Data = Data << s;
}
bool CpuStatusFlag::HelpGetFunc(bit08jcq s) {
	return (Data >> s) & 1ull == 1;
}

bit64jcq* CPUJCQs::GetBit64JCQ(const string name) {
	const array<string,16> JCQs = {"rax","rbx","rcx","rdx","rex","rfx","r7","r8","r9","r10","r11","r12","r13","r14","r15","r16"};
	const array<bit64jcq*,16> JCQRefs = {&rax,&rbx,&rcx,&rdx,&rex,&rfx,&r7,&r8,&r9,&r10,&r11,&r12,&r13,&r14,&r15,&r16};
	for (int i = 0;i < JCQs.size();++i) {
		if (JCQs[i] == name) {
			return JCQRefs[i];
		}
	}
	return nullptr;
}

bit32jcq* CPUJCQs::GetBit32JCQ(const string name) {
	const array<string,16> JCQs = {"eax","ebx","ecx","edx","eex","efx","e7","e8","e9","e10","e11","e12","e13","e14","e15","e16"};
	const array<bit32jcq*,16> JCQRefs = {&eax,&ebx,&ecx,&edx,&eex,&efx,&e7,&e8,&e9,&e10,&e11,&e12,&e13,&e14,&e15,&e16};
	for (int i = 0;i < JCQs.size();++i) {
		if (JCQs[i] == name) {
			return JCQRefs[i];
		}
	}
	return nullptr;
}

bit16jcq* CPUJCQs::GetBit16JCQ(const string name) {
	const array<string,16> JCQs = {"ax","bx","cx","dx","ex","fx","x7","x8","x9","x10","x11","x12","x13","x14","x15","x16"};
	const array<bit16jcq*,16> JCQRefs = {&ax,&bx,&cx,&dx,&ex,&fx,&x7,&x8,&x9,&x10,&x11,&x12,&x13,&x14,&x15,&x16};
	for (int i = 0;i < JCQs.size();++i) {
		if (JCQs[i] == name) {
			return JCQRefs[i];
		}
	}
	return nullptr;
}

bit64jcq* CPUJCQs::GetTCJCQ(const string name) {
	const char tmp[3] = "16";
	if (!flag.Get(tmp)) {
		return nullptr;
	}
	const array<string,3> JCQs = {"tr","ts","ca"};
	const array<bit64jcq*,3> JCQRefs = {&tr,&ts,&ca};
	for (int i = 0;i < JCQs.size();++i) {
		if (JCQs[i] == name) {
			return JCQRefs[i];
		}
	}
	return nullptr;
}

Memory::Memory() {
	MemoryStart = malloc(MemoryLong);
	MemoryEnd = MemoryStart + MemoryLong;
	if (MemoryStart == NULL) {
		MemoryStart = nullptr;
		MemoryEnd = nullptr;
	}
}
Memory::~Memory() {
	free(MemoryStart);
}

bit08jcq* Memory::GetMemory(bit08addr pi) {
	bit08addr* get = (bit08addr*)MemoryStart + pi;
	if (get >= (bit08addr*)MemoryEnd) {
		return NULL;
	} else {
		return get;
	}
}

bit16jcq* Memory::GetMemory(bit16addr pi) {
	bit16addr* get = (bit16addr*)MemoryStart + pi;
	if (get >= (bit16addr*)MemoryEnd) {
		return NULL;
	} else {
		return get;
	}
}

bit32jcq* Memory::GetMemory(bit32addr pi) {
	bit32addr* get = (bit32addr*)MemoryStart + pi;
	if (get >= (bit32addr*)MemoryEnd) {
		return NULL;
	} else {
		return get;
	}
}

bit64jcq* Memory::GetMemory(bit64addr pi) {
	bit64addr* get = (bit64addr*)MemoryStart + pi;
	if (get >= (bit64addr*)MemoryEnd) {
		return NULL;
	} else {
		return get;
	}
}

Memory::operator unsigned long long() {
	return MemoryLong;
}

void Thread::Work() {
	if (JCQ.flag.Get("16")) {
		WorkForBit<bit16jcq,bit16addr>();
	} else if (JCQ.flag.Get("32")) {
		WorkForBit<bit32jcq,bit32addr>();
	} else if (JCQ.flag.Get("64")) {
		WorkForBit<bit64jcq,bit64addr>();
	} else {
		channel.Throw = true;
		channel.ThrowTo = -1;
		channel.ThrowID = 0;
	}
}

template <>
void Thread::WorkForBit<bit16jcq,bit16addr>() {
	using T     = bit16jcq;
	using TAddr = bit16addr;
	auto GetJCQ    = [&](const string name){return JCQ.GetBit16JCQ(name);};
	auto GetMemory = [&](TAddr addr){return MemoryPoint->GetMemory(addr);};

	Help<T,TAddr>(GetMemory,GetJCQ,("0x%0" + to_string(sizeof(T)*8/16) + "x").data());
}

template <>
void Thread::WorkForBit<bit32jcq,bit32addr>() {
	using T     = bit32jcq;
	using TAddr = bit32addr;
	auto GetMemory = [&](TAddr addr){return MemoryPoint->GetMemory(addr);};
	auto GetJCQ    = [&](const string name){return JCQ.GetBit32JCQ(name);};

	Help<T,TAddr>(GetMemory,GetJCQ,("0x%0" + to_string(sizeof(T)*8/16) + "x").data());
}

template <>
void Thread::WorkForBit<bit64jcq,bit64addr>() {
	using T     = bit64jcq;
	using TAddr = bit64addr;
	auto GetMemory = [&](TAddr addr){return MemoryPoint->GetMemory(addr);};
	auto GetJCQ    = [&](const string name){return JCQ.GetBit64JCQ(name);};

	Help<T,TAddr>(GetMemory,GetJCQ,("0x%0" + to_string(sizeof(T)*8/16) + "x").data());
}

template <typename T,typename TAddr>
void Thread::Help(function<T* (TAddr)> GetMemory,function<T* (const string)> GetJCQ,const char* DisplayFlag) {
	Status = CPUStatus::Working;
	while (true) {
		if (channel.HadData) {
			if (channel.ID==1) {
			} else {
				cout << endl << "At CPU:" << 0 << ";Int ID:" << channel.ID << endl;
				break;
			}
		}
		string code = GetCode(codelong);
		//cout << "code:" << code << endl;
		if (starts_with(code,"shut")) {
			break;
		} else if (starts_with(code,"mov")) {
			auto args = GetArgs<T,TAddr>(code,3,2,GetMemory,GetJCQ);
			*args[1] = *args[0];
		} else if (starts_with(code,"disl")) {
			auto args = GetArgs<T,TAddr>(code,4,1,GetMemory,GetJCQ);
			printf(DisplayFlag, *args[0]);
			cout << endl;
		} else if (starts_with(code,"discl")) {
			auto args = GetArgs<T,TAddr>(code,5,1,GetMemory,GetJCQ);
			cout << ((char)*args[0]) << ((char)(*args[0] >> 8)) << endl;
		} else if (starts_with(code,"disnl")) {
			cout << endl;
		} else if (starts_with(code,"disc")) {
			auto args = GetArgs<T,TAddr>(code,4,1,GetMemory,GetJCQ);
			cout << (char)*args[0] << (char)(*args[0] >> 8);
		} else if (starts_with(code,"dis")) {
			auto args = GetArgs<T,TAddr>(code,3,1,GetMemory,GetJCQ);
			printf(DisplayFlag, *args[0]);
		} else if (starts_with(code,"add")) {
			auto args = GetArgs<T,TAddr>(code,3,2,GetMemory,GetJCQ);
			*args[1] += *args[0];
		} else if (starts_with(code,"sub")) {
			auto args = GetArgs<T,TAddr>(code,3,2,GetMemory,GetJCQ);
			*args[1] -= *args[0];
		} else if (starts_with(code,"hlt")) {
			Status = CPUStatus::HLT;
			while (true) {
				if (channel.HadData && channel.ID == 1) {
					break;
				}
				this_thread::sleep_for(1000ms);
			}
			Status = CPUStatus::Working;
		} else if (starts_with(code,"int")) {
			auto args = GetArgs<T,TAddr>(code,3,1,GetMemory,GetJCQ);
			channel.ThrowID = *args[0];
			channel.ThrowTo = -1;
			channel.ThrowData = 0;
			channel.Throw = true;
		} else if (starts_with(code,"in3")) {
			auto args = GetArgs<T,TAddr>(code,3,3,GetMemory,GetJCQ);
			channel.ThrowID = *args[0];
			channel.ThrowTo = *args[1];
			channel.ThrowData = *args[2];
			channel.Throw = true;
		} else if (starts_with(code,"nop")) {
			;
		} else if (starts_with(code,"jmp")) {
			auto args = GetArgs<T,TAddr>(code,3,1,GetMemory,GetJCQ);
			*JCQ.GetTCJCQ("ca") = *args[0];
		} else if (starts_with(code,"lmb")) {
			auto args = GetArgs<T,TAddr>(code,3,2,GetMemory,GetJCQ);
			*args[1] << *args[0];
		} else if (starts_with(code,"rmb")) {
			auto args = GetArgs<T,TAddr>(code,3,2,GetMemory,GetJCQ);
			*args[1] >> *args[0];
		} else if (starts_with(code,"push")) {
			auto args = GetArgs<T,TAddr>(code,4,1,GetMemory,GetJCQ);
			*GetMemory(*GetJCQ("tr") + *GetJCQ("ts")) = *args[0];
			*GetJCQ("ts") += 1;
		} else if (starts_with(code,"pop")) {
			auto args = GetArgs<T,TAddr>(code,3,1,GetMemory,GetJCQ);
			*args[0] = *GetMemory(*GetJCQ("tr") + *GetJCQ("ts"));
			*GetJCQ("ts") -= 1;
		} else if (starts_with(code,"je")) {
			auto args = GetArgs<T,TAddr>(code,2,3,GetMemory,GetJCQ);
			if (*args[0] == *args[1]) {
				*JCQ.GetTCJCQ("ca") = *args[2];
			}
		} else if (starts_with(code,"jne")) {
			auto args = GetArgs<T,TAddr>(code,3,3,GetMemory,GetJCQ);
			if (*args[0] != *args[1]) {
				*JCQ.GetTCJCQ("ca") = *args[2];
			}
		} else if (starts_with(code,"jg")) {
			auto args = GetArgs<T,TAddr>(code,2,3,GetMemory,GetJCQ);
			if (*args[0] > *args[1]) {
				*JCQ.GetTCJCQ("ca") = *args[2];
			}
		} else if (starts_with(code,"jl")) {
			auto args = GetArgs<T,TAddr>(code,2,3,GetMemory,GetJCQ);
			if (*args[0] < *args[1]) {
				*JCQ.GetTCJCQ("ca") = *args[2];
			}
		} else if (starts_with(code,"mod")) {
			auto args = GetArgs<T,TAddr>(code,3,2,GetMemory,GetJCQ);
			*args[1] %= *args[0];
		} else if (starts_with(code,"div")) {
			auto args = GetArgs<T,TAddr>(code,3,2,GetMemory,GetJCQ);
			*args[1] /= *args[0];
		} else if (starts_with(code,"mul")) {
			auto args = GetArgs<T,TAddr>(code,3,2,GetMemory,GetJCQ);
			*args[1] *= *args[0];
		} else if (starts_with(code,"cpuid")) {
			char info[6][64] = {
				"LiZongEn(r) Bandysol(r) Galaxy(c) CPU G9-1900F               \n",
				"Core1 Thread2 1.10GHz                                        \n",
				"Instruction Set Version:Galaxy  1.0.0.0_0                    \n",
				"Baseband Version       :NewStar 1.0.0.0_0                    \n",
				"Made in China & Made in PRC                                  \n",
				"2023-08-26                                                   \n"
			};

			auto args = GetArgs<T,TAddr>(code,5,1,GetMemory,GetJCQ);
			for (int i = 0;i < 6;++i) {
				testInsr(MemoryPoint,*args[0]+i,info[i]);
			}

		}
		HadLets = 0;
	}
	Status = CPUStatus::Shutdown;
}

string Thread::GetCode(int length) {
	string ret = "";
	for (int i = 0;i < 0x0010;++i) {
		bit64jcq tttmp = *(MemoryPoint->GetMemory(*JCQ.GetTCJCQ("ca")));
		for (int j = 0;j < 4;++j) {
			ret += (char)(tttmp >> (j*8));
		}
		++(*JCQ.GetTCJCQ("ca"));
	}
	return ret;
}

template <typename WorkBitJCQT,typename TAddr>
vector<WorkBitJCQT*> Thread::GetArgs(string con,int conlen,int args,function<WorkBitJCQT* (TAddr)> GetMemory,function<WorkBitJCQT* (const string)> GetJCQ,vector<char> mid,vector<char> end) {
	con.erase(0,conlen+1);
	vector<WorkBitJCQT*> ret;

	for (int i = 0;i < args;++i) {
		vector<char> scanVector = i==args-1 ? end : mid;
		string thisstr = "";
		while (true) {
			if (scanVector[0] == con[0]) {
				con.erase(0,1);
				break;
			}
			thisstr += con[0];
			con.erase(0,1);
		}
		WorkBitJCQT* thisptr = nullptr;
		if (starts_with(thisstr,"%")) {
			thisstr.erase(0,1);
			thisptr = GetJCQ(thisstr);
			if (thisptr==nullptr) {
				thisptr = GetMemory(stoi(thisstr,nullptr,0));
			} else {
				thisptr = GetMemory(*thisptr);
			}
		} else {
			thisptr = GetJCQ(thisstr);
			if (thisptr==nullptr && !thisstr.empty()) {
				*(WorkBitJCQT*)(LetsNumberBuffer + HadLets*2) = stoull(thisstr,nullptr,0);
				thisptr = (WorkBitJCQT*)(LetsNumberBuffer + HadLets*2);
				++HadLets;
			}
		}
		ret.push_back(thisptr);
	}
	return ret;
}

template <typename WorkBitJCQT,typename TAddr>
vector<WorkBitJCQT> Thread::GetDataArgs(string con,int conlen,int args, function<WorkBitJCQT* (TAddr)> GetMemory,function<WorkBitJCQT* (const string)> GetJCQ,vector<char> mid,vector<char> end) {
	auto pois = GetArgs(con,conlen,args,GetMemory,GetJCQ);
	vector<WorkBitJCQT> ret;
	for (int i = 0;i < pois.size();++i) {
		ret.push_back(pois[i]);
	}
	return ret;
}

Thread::~Thread() {
	free(LetsNumberBuffer);
}

void Thread::Init(bit16addr code,bit16addr sr) {
	LetsNumberBuffer = malloc(LetsNumberBufferLength);

	*JCQ.GetTCJCQ("ca") = code;
	*JCQ.GetTCJCQ("tr") = sr;
}

Core::operator int() {
	return 2;
}

Thread& Core::operator[](int ID) {
	return Data[ID];
}

template<int Cores>
void CPU<Cores>::Init() {
	for (int i = 0;i < Cores;++i) {
		Data[i][0].MemoryPoint = memoryp;
		Data[i][1].MemoryPoint = memoryp;
	}
	threadc.Threads = &(Data[0][0]);
	threadc.memory = memoryp;
	threadc.ThreadNumber = Cores*2;


}

template<int Cores>
CPU<Cores>::operator int() {
	return Cores*2;
}

template<int Cores>
Thread& CPU<Cores>::operator[](int ID) {
	int CoreID = ID%2==0 ? ID/2 : (ID-ID%2)/2;
	int ThreadId = ID%2;
	return Data[CoreID][ThreadId];
}

void PC::Powar() {
	cpu.memoryp = &memory;
	cpu.Init();
	cpu[0].Authority = CPUAuthority::System;
	cpu[0].Init(0x0000,0x1000);
}

int main() {
	auto str = clock();
	char Oldcommand[100][65] = {
		"sub 1,x16                                                       ",
		"mov 0xe0,bx                                                     ",

		//"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345678900",

		"add 0x1,x16                                                     ",
		"mov %x16,x14                                                    ",
		"disc x14                                                        ",
		"sub 1,bx                                                        ",
		"mov bx,cx                                                       ",
		"mod 0x10,cx                                                     ",
		"je cx,0,0xc0                                                    ",
		"jne bx,0,0x20                                                   ",
		"je bx,0,0xd0                                                    ",

		"disnl                                                           ",
		"jmp 0x90                                                        ",

		"shut                                                            "
	};
	char command[100][65] = {
		"mov 0x60,x16                                                    ",
		"cpuid 0x900                                                     ",

		"mov 0x900,ax                                                    ",
		"disc %ax                                                        ",
		"add 1,ax                                                        ",
		"sub 1,x16                                                       ",
		"je x16,0,0x90                                                   ",
		"jmp 0x30                                                        ",

		"shut                                                            "
	};
	PC pc;
	pc.Powar();
	for (int i = 0;i < 100;++i) {
		testInsr(&pc,0x0010*i,command[i]);
	}
	for (bit64addr i = 0;i < 0x100;++i) {
		printf("%08x ", i);
		printf("%08x ", *pc.memory.GetMemory(i) );
		auto tmp = *pc.memory.GetMemory(i);
		for (int j = 0;j < 4;++j) {
			cout << ((char)(tmp >> (j*8)));
		}
		cout << endl;
	}
	pc.cpu[0].JCQ.flag["16"];
	pc.cpu[0].JCQ.flag["64"];
	pc.cpu[0].Work();
	auto end = clock();
	cout << endl << (end-str) << "ms" << endl;
}

void testInsr(PC* pc,bit64addr add,char d[64]) {
	testInsr(&(pc->memory),add,d);
}

void testInsr(Memory* mem,bit64addr add,char d[64]) {
	bit64jcq* tmp = mem->GetMemory(add);
	for (int i = 0;i < 64;i+=4) {
		bit08jcq* low = (bit08jcq*)(tmp+(i/4));
		for (int j = 0;j < 4;++j) {
			*(low+j) = d[i+j];
		}
	}
}