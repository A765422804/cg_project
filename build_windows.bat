rd /s /q build 
mkdir build 
cd build 
cmake .. -G "MinGW Makefiles" 
mingw32-make -j4 
.\cg_project.exe