IF NOT EXIST "build" (
    mkdir build
)

cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
cd ..
.\build\remote_led_functions --gtest_color=yes
