// #include <iostream>

#include "Database.h"
#include "serviceDone.h"
#include "Statistics.h"
#include "Member.h"
#include "Salon.h"
#include "ui.h"
#include "test.h"
#include <functional>

std::ofstream flog("log.txt");
std::function<void()> currentScreen = screenWelcome;

int main() {
    try {
        while (currentScreen)
        {
            HIDE_CURSOR;
            currentScreen();
            SHOW_CURSOR;
        }
        
    } catch (const std::exception& e) {
        flog << e.what() << '\n';
    } catch (...) {
        flog << "Unknown error\n";
    }
    return 0;
}