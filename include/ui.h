#ifndef UI_H
#define UI_H

#include <functional>
#include "call.h"

#define HIDE_CURSOR (std::cout << "\033[?25l")
#define SHOW_CURSOR (std::cout << "\033[?25h")
#define SCREEN_WIDTH 120
#define STAR_COUNT 6
#define SCREEN_HEIGHT 30

extern std::function<void()> currentScreen;
void screenWelcome();
void screenLogin();
void screenRegister();
void screenCustomer();
void screenStylist();
void screenAdmin();

inline const std::string blackStar = " ☆ ";
inline const std::string whiteStar = " ★ ";

#endif // UI_H