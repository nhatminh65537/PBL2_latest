#include <Database.h>
#include <fstream>
#include <iostream>
#include <functional>
#include "ui.h"
#include "Salon.h"

std::function<void()> currentScreen = screenWelcome;

int main(){
    freopen("log.txt","w", stderr);
    std::cerr << Hash("pass");
    try {
        while(currentScreen) currentScreen();
    } catch (int code) {
        return code;
    } catch (const char* msg) {
        std::cerr << msg << std::endl;
        return 1;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
