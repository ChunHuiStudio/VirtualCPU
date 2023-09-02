g++ -std=c++23 cpu.cpp -O3 -DDisplay -o DisplayCode.exe
g++ -std=c++23 cpu.cpp -O3 -DCPUID   -o CPUID.exe
g++ -std=c++23 cpu.cpp -O3 -DDLLVersion -shared -fPIC -o Impact.dll

g++ -std=c++11 cpu.cpp -O3 -DDisplay -o DisplayCode_Compatible.exe
g++ -std=c++11 cpu.cpp -O3 -DCPUID   -o CPUID_Compatible.exe
g++ -std=c++11 cpu.cpp -O3 -DDLLVersion -shared -fPIC -o Impact_Compatible.dll