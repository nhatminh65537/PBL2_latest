#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <functional>
#include <array>
#include "ui.h"
#include "test.h"

using namespace ftxui;

std::function<void()> currentScreen = screenWelcome;

int main(){
  freopen("log.txt","w",stderr);
  while(currentScreen) currentScreen();
  return 0;
}