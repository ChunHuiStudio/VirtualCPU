g++ -std=c++23 cpu.cpp -O3 -DDisplay            -fPIC -o DisplayCode.exe
g++ -std=c++23 cpu.cpp -O3 -DCPUID              -fPIC -o CPUID.exe
g++ -std=c++23 cpu.cpp -O3 -DDLLVersion -shared -fPIC -o Impact.dll

g++ -std=c++11 cpu.cpp -O3 -DDisplay            -fPIC -o DisplayCode_Compatible.exe
g++ -std=c++11 cpu.cpp -O3 -DCPUID              -fPIC -o CPUID_Compatible.exe
g++ -std=c++11 cpu.cpp -O3 -DDLLVersion -shared -fPIC -o Impact_Compatible.dll