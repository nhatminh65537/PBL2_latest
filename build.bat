@REM git clone https://github.com/nhatminh65537/PBL2_latest.git PBL2_latest
@REM cd PBL2_latest
mkdir build
cd build
cmake ../ -G "MinGW Makefiles"
cmake --build .
echo "Open folder build and run Salon_Management.exe (screen size 120*40)"
pause