#include <Database.h>
#include <fstream>
#include <iostream>
#include <functional>
#include <csignal> 
#include "ui.h"
#include "Salon.h"

std::function<void()> currentScreen = screenWelcome;

void signal_handler(int signal_num) 
{ 
    currentScreen = nullptr;
    exit(0);
} 

int main(){
    freopen("log.txt","w", stderr);
    signal(SIGBREAK, signal_handler);

    try {
        while(currentScreen) currentScreen();
    } catch (int code) {
        std::cerr << "Exit main with code " << code << std::endl;
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
