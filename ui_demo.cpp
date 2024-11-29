#include <fstream>
#include <functional>
#include "ui.h"
#include "test.h"

std::function<void()> currentScreen = screenWelcome;
std::ofstream flog("log.txt");

int main(){
    freopen("log.txt","w",stderr);
    while(currentScreen) currentScreen();
    return 0;
}
