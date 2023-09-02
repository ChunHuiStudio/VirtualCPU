#include "cpu.hpp"

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
void CpuStatusFlag::HelpFunc(bit08data s) {
	Data = Data >> s;
	if (Data & 1ull == 1) {
		--Data;
	} else {
		++Data;
	}
	Data = Data << s;
}
bool CpuStatusFlag::HelpGetFunc(bit08data s) {
	return (Data >> s) & 1ull == 1;
}

workBit* CPUJCQs::GetNativeJCQ(const string name) {
	const array<string,19> JCQs = {"tr","ts","ca","rax","rbx","rcx","rdx","rex","rfx","r7","r8","r9","r10","r11","r12","r13","r14","r15","r16"};
	const array<workBit*,19> JCQRefs = {&tr,&ts,&ca,&rax,&rbx,&rcx,&rdx,&rex,&rfx,&r7,&r8,&r9,&r10,&r11,&r12,&r13,&r14,&r15,&r16};
	for (int i = 0;i < JCQs.size();++i) {
		if (JCQs[i] == name) {
			return JCQRefs[i];
		}
	}
	return nullptr;
}

Memory::Memory() {
	MemoryStart = new workBit[MemoryLong];
	MemoryEnd = MemoryStart + MemoryLong - 1;
}
Memory::~Memory() {
	delete[] MemoryStart;
}

workBit* Memory::GetMemory(workBit pi) {
	workBit* get = (workBit*)MemoryStart + pi;
	if (get >= (workBit*)MemoryEnd) {
		return NULL;
	} else {
		return get;
	}
}

Memory::operator unsigned long long() {
	return MemoryLong;
}

Thread::Thread() {
	Thread(0,0x1000);
}

Thread::Thread(workBit code,workBit sr) {
	Init(code,sr);
}

void Thread::Work() {
	auto GetMemory = [&](workBit d){return MemoryPoint->GetMemory(d);};
	Help(GetMemory,"0x%04x");
}

void Thread::Help(function<workBit* (workBit)> GetMemory,const char* DisplayFlag) {
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
			auto args = GetArgs(code,3,2,GetMemory);
			*args[1] = *args[0];
		} else if (starts_with(code,"disl")) {
			auto args = GetArgs(code,4,1,GetMemory);
			printf(DisplayFlag, *args[0]);
			cout << endl;
		} else if (starts_with(code,"discl")) {
			auto args = GetArgs(code,5,1,GetMemory);
			for (int i = 0;i < OnceBitChars;++i) {
				cout << (char)(*args[0] >> 8*i);
			}
			cout << endl;
		} else if (starts_with(code,"disnl")) {
			cout << endl;
		} else if (starts_with(code,"disc")) {
			auto args = GetArgs(code,4,1,GetMemory);
			for (int i = 0;i < OnceBitChars;++i) {
				cout << (char)(*args[0] >> 8*i);
			}
		} else if (starts_with(code,"dis")) {
			auto args = GetArgs(code,3,1,GetMemory);
			printf(DisplayFlag, *args[0]);
		} else if (starts_with(code,"add")) {
			auto args = GetArgs(code,3,2,GetMemory);
			*args[1] += *args[0];
		} else if (starts_with(code,"sub")) {
			auto args = GetArgs(code,3,2,GetMemory);
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
			auto args = GetArgs(code,3,1,GetMemory);
			channel.ThrowID = *args[0];
			channel.ThrowTo = -1;
			channel.ThrowData = 0;
			channel.Throw = true;
		} else if (starts_with(code,"in3")) {
			auto args = GetArgs(code,3,3,GetMemory);
			channel.ThrowID = *args[0];
			channel.ThrowTo = *args[1];
			channel.ThrowData = *args[2];
			channel.Throw = true;
		} else if (starts_with(code,"nop")) {
			;
		} else if (starts_with(code,"jmp")) {
			auto args = GetArgs(code,3,1,GetMemory);
			*GetJCQ("ca") = *args[0];
		} else if (starts_with(code,"lmb")) {
			auto args = GetArgs(code,3,2,GetMemory);
			*args[1] << *args[0];
		} else if (starts_with(code,"rmb")) {
			auto args = GetArgs(code,3,2,GetMemory);
			*args[1] >> *args[0];
		} else if (starts_with(code,"push")) {
			auto args = GetArgs(code,4,1,GetMemory);
			*GetMemory(*GetJCQ("tr") + *GetJCQ("ts")) = *args[0];
			*GetJCQ("ts") += 1;
		} else if (starts_with(code,"pop")) {
			auto args = GetArgs(code,3,1,GetMemory);
			*args[0] = *GetMemory(*GetJCQ("tr") + *GetJCQ("ts"));
			*GetJCQ("ts") -= 1;
		} else if (starts_with(code,"je")) {
			auto args = GetArgs(code,2,3,GetMemory);
			if (*args[0] == *args[1]) {
				*GetJCQ("ca") = *args[2];
			}
		} else if (starts_with(code,"jne")) {
			auto args = GetArgs(code,3,3,GetMemory);
			if (*args[0] != *args[1]) {
				*GetJCQ("ca") = *args[2];
			}
		} else if (starts_with(code,"jg")) {
			auto args = GetArgs(code,2,3,GetMemory);
			if (*args[0] > *args[1]) {
				*GetJCQ("ca") = *args[2];
			}
		} else if (starts_with(code,"jl")) {
			auto args = GetArgs(code,2,3,GetMemory);
			if (*args[0] < *args[1]) {
				*GetJCQ("ca") = *args[2];
			}
		} else if (starts_with(code,"mod")) {
			auto args = GetArgs(code,3,2,GetMemory);
			*args[1] %= *args[0];
		} else if (starts_with(code,"div")) {
			auto args = GetArgs(code,3,2,GetMemory);
			*args[1] /= *args[0];
		} else if (starts_with(code,"mul")) {
			auto args = GetArgs(code,3,2,GetMemory);
			*args[1] *= *args[0];
		} else if (starts_with(code,"cpuid")) {
			char info[7][65] = {
				"Copyright(c) 2023~2023 ChunHuiStudio(r)                        \n",
				"WinterSun(r) CPU G9-1900F                                      \n",
				"1*Core 2*Thread 1.10GHz                                        \n",
				"Instruction Set Version:ChillyWinter 1.0.0.0_1                 \n",
				"Architecture Version   :ChunHui      1.0.2.1_3                 \n",
				"Made in China/PRC                                              \n",
				"2023-09-02                                                     \n"
			};

			auto args = GetArgs(code,5,1,GetMemory);
			for (int i = 0;i < 6;++i) {
				testInsr(MemoryPoint,*args[0]+i*0x0008,info[i]);
			}

		}
		HadLets = 0;
	}
	Status = CPUStatus::Shutdown;
}

string Thread::GetCode(int length) {
	string ret = ""; 
	for (int i = 0;i < 0x0008;++i) {
		workBit tttmp = *(MemoryPoint->GetMemory(*GetJCQ("ca")));
		for (int j = 0;j < OnceBitChars;++j) {
			ret += (char)(tttmp >> (j*8));
		}
		++(*GetJCQ("ca"));
	}
	return ret;
}

vector<workBit*> Thread::GetArgs(string con,int conlen,int args,function<workBit* (workBit)> GetMemory,vector<char> mid,vector<char> end) {
	con.erase(0,conlen+1);
	vector<workBit*> ret;

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
		workBit* thisptr = nullptr;
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
				*(workBit*)(LetsNumberBuffer + HadLets*2) = stoull(thisstr,nullptr,0);
				thisptr = (workBit*)(LetsNumberBuffer + HadLets*2);
				++HadLets; 
			}
		}
		ret.push_back(thisptr);
	}
	return ret;
}

Thread::~Thread() {
	delete[] LetsNumberBuffer;
}

void Thread::Init(workBit code,workBit sr) {
	LetsNumberBuffer = new workBit[8];

	*GetJCQ("ca") = code;
	*GetJCQ("tr") = sr;
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

#if DLLVersion
#else
int main() {
	auto str = clock();
#if Display
	char command[100][65] = {
		"sub 1,r16                                                       ",
		"mov 0x78,rbx                                                    ",

		"add 0x1,r16                                                     ",
		"mov %r16,r14                                                    ",
		"disc r14                                                        ",
		"sub 1,rbx                                                       ",
		"mov rbx,rcx                                                     ",
		"mod 0x08,rcx                                                    ",
		"je rcx,0,0x58                                                   ",
		"jne rbx,0,0x10                                                  ",
		"je rbx,0,0x68                                                   ",

		"disnl                                                           ",
		"jmp 0x48                                                        ",

		"shut                                                            "
	};
#endif
#if CPUID
	char command[100][65] = {
		"mov 0x38,r16                                                    ",
		"cpuid 0x900                                                     ",

		"mov 0x900,rax                                                   ",
		"disc %rax                                                       ",
		"add 1,rax                                                       ",
		"sub 1,r16                                                       ",
		"je r16,0,0x40                                                   ",
		"jmp 0x18                                                        ",

		"shut                                                            "
	};
#endif
	PC pc;
	pc.Powar();
	for (workBit i = 0;i < 100;++i) {
		testInsr(&pc,0x0008*i,command[i]);
	}
#if DEBUG
	for (workBit i = 0;i < 0x80;++i) {
		printf("%08x ", i);
		printf("%08x ", *pc.memory.GetMemory(i) );
		auto tmp = *pc.memory.GetMemory(i);
		for (int j = 0;j < OnceBitChars;++j) {
			cout << ((char)(tmp >> (j*8)));
		}
		cout << endl;
	}
#endif
	pc.cpu[0].Work();
	auto end = clock();
	cout << endl << (end-str) << "ms" << endl;
}
#endif

void testInsr(PC* pc,workBit add,char d[64]) {
	testInsr(&(pc->memory),add,d);
}

void testInsr(Memory* mem,workBit add,char d[64]) {
	workBit* tmp = mem->GetMemory(add);
	for (int i = 0;i < 64;i+=OnceBitChars) {
		bit08data* low = (bit08data*)(tmp+(i/OnceBitChars));
		for (int j = 0;j < OnceBitChars;++j) {
			*(low+j) = d[i+j];
		}
	}
}