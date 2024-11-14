#ifndef FINISH_H
#define FINISH_H

#include <functional>
#include "call.h"
#define HIDE_CURSOR (std::cout << "\033[?25l")
#define SHOW_CURSOR (std::cout << "\033[?25h")

extern std::function<void()> currentScreen;
void welcome_screen();
void login_screen();
void register_screen();
void customer_screen();

#endif