@REM git clone https://github.com/nhatminh65537/PBL2_latest.git PBL2_latest
@REM cd PBL2_latest
mkdir build
cd build
cmake ../ -G "MinGW Makefiles"
cmake --build .
echo "Enter PBL2_Clion.exe in terminal to run (screen size 120*35)"
pause