g++ -std=c++23 cpu.cpp -O3 -DDiskStart                -o Binarys\DiskStart.exe
g++ -std=c++23 cpu.cpp -O3 -DDLLVersion -shared -fPIC -o Binarys\Impact.dll

g++ -std=c++11 cpu.cpp -O3 -DDiskStart                -o Binarys\DiskStart_Compatible.exe
g++ -std=c++11 cpu.cpp -O3 -DDLLVersion -shared -fPIC -o Binarys\Impact_Compatible.dll