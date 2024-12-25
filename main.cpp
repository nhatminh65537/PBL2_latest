#include <Database.h>
#include <fstream>
#include <iostream>
#include <functional>
#include "ui.h"
#include "Salon.h"
#include <windows.h>

std::function<void()> currentScreen = screenWelcome;

BOOL WINAPI consoleHandler(DWORD signal) {
    if (signal == CTRL_CLOSE_EVENT || signal == CTRL_LOGOFF_EVENT || signal == CTRL_SHUTDOWN_EVENT) {
        std::cerr << "\nConsole is being closed. Exiting safely...\n";
        std::exit(0);
        return TRUE;
    }
    return FALSE;
}

int main(){
    freopen("log.txt","w", stderr);
    std::cerr << Hash("pass") << '\n';

    SetConsoleCtrlHandler(consoleHandler, TRUE);

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
