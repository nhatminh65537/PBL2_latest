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
#include "call.h"

using namespace ftxui;

int main() {
    while(currentScreen != nullptr) {
        currentScreen();
    }
    return 0;
}