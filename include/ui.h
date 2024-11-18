#ifndef UI_H
#define UI_H

#include <functional>
#include "call.h"
#define HIDE_CURSOR (std::cout << "\033[?25l")
#define SHOW_CURSOR (std::cout << "\033[?25h")

extern std::function<void()> currentScreen;
void screenWelcome();
void screenLogin();
void screenRegister();
void screenCustomer();
void screenStylist();
void screenAdmin();


#endif // UI_H