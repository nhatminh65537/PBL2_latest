#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <functional>
#include <exception>
#include <array>

#include "finish.h"
using namespace ftxui;

void example();

void stylist_screen();
void admin_screen();

std::function<void()> currentScreen = welcome_screen;

int main(void) {
    while(currentScreen != nullptr) {
        currentScreen();
    }

    return EXIT_SUCCESS;
}

void example()
{
    // Define the document
    Element document =
        hbox({
        text("left")   | border,
        text("middle") | border | flex,
        text("right")  | border,
        });

    auto screen = Screen::Create(
        Dimension::Full(),       // Width
        Dimension::Fit(document) // Height
    );
    Render(screen, document);
    screen.Print();
}

void customer_screen()
{
    // test stuff
    std::string name = "John Doe";
    std::ofstream log;
    log.open("log.txt");

    // customer screen
    auto screen = ScreenInteractive::FixedSize(120, 30);
    int exit = 0;

    // Navigation tab buttons
    int selected_tab = 0;
    ButtonOption myButtonOption;
    myButtonOption.transform = [](const EntryState& s) {
        auto element = text(s.label) | center ;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    Component button_home = Button("Home", [&] {
        selected_tab = 0;
    }, myButtonOption);
    Component button_appointment = Button("Appointment", [&] {
        selected_tab = 1;
    }, myButtonOption);
    Component button_history = Button("History", [&] {
        selected_tab = 2;
    }, myButtonOption);
    Component button_profile = Button("Profile", [&] {
        selected_tab = 3;
    }, myButtonOption);
    Component button_logout = Button("Logout", [&] {
        exit = 1;
    }, myButtonOption);
    Component tabs = Container::Vertical({
        button_home,
        button_appointment,
        button_history,
        button_profile,
        button_logout,
    });
    Component tab_renderer = Renderer(tabs, [&] {
        return vbox({
            text("Welcome " + name) | hcenter,
            filler() | size(HEIGHT, EQUAL, 2),
            separator(),
            button_home->Render(),
            separator(),
            button_appointment->Render(),
            separator(),
            button_history->Render(),
            separator(),
            button_profile->Render(),
            separator(),
            filler(),
            separator(),
            button_logout->Render()
        }) | borderRounded | size(WIDTH, EQUAL, 30);
    });

    // Home tab
    Component tab_home = Renderer([&] {
        return text("Home") | center ;
    });

    // Appointment tab
    std::array<bool, 7> states;
    std::array<int, 7> selected_stylist;
    std::vector<std::string> services = {
        "Cat toc",
        "Nhuom toc",
        "Tay toc",
        "Uon toc",
        "Phuc hoi toc",
        "Tao kieu",
        "Goi dau"
    };
    std::vector<std::string> stylists[7];
    int selected_service = 0;
    Component checkbox_services = Container::Vertical({}, &selected_service);
    for (int i = 0; i < 7; ++i) {
        states[i] = false;
        stylists[i] = get_stylist();
        selected_stylist[i] = 0;
        Component new_box = Container::Horizontal({
                Checkbox(services[i], &states[i]) | size(WIDTH, EQUAL, 20),
                Dropdown({
                    .radiobox = {.entries = &(stylists[i]), .selected = &(selected_stylist[i])},
                    .transform = [](bool open, Element checkbox, Element radiobox) {
                        if (open) {
                            const int max_height = 4;
                            return vbox({
                                checkbox | inverted,
                                radiobox | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, max_height),
                            });
                        }
                        return vbox({checkbox, filler()}) ;
                    },
                    }) | size(WIDTH, EQUAL, 20),
        });
        checkbox_services->Add(new_box);
    }

    int selected_day = 0;
    int selected_month = 0;
    int selected_year = 0;
    int selected_hour = 0;
    int selected_minute = 0;

    std::vector<std::string> days = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    std::vector<std::string> months = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"};
    std::vector<std::string> years  = {"2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029", "2030"};
    std::vector<std::string> hours = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"};
    std::vector<std::string> minutes = {"00", "15", "30", "45"};


    MenuOption myMenuOption;
    myMenuOption.entries_option.transform = [](const EntryState& s) {
        auto element = text(s.label) | center;
        if (s.focused) {
            element = element | inverted;
        }
        return element;
    };

    Component menu_day = Menu(&days, &selected_day, myMenuOption);
    MenuOption menu_option_month = MenuOption(myMenuOption);
    menu_option_month.on_change = ([&] () {
        int cur_year = 2021 + selected_year;
        int cur_month = 1 + selected_month;
        int cur_day = 1 + selected_day;
        bool is_leap_year = (cur_year % 4 == 0 && cur_year % 100 != 0) || (cur_year % 400 == 0);
        int max_day;  
        if (cur_month == 2) {
            max_day = is_leap_year ? 29 : 28;
        }
        else if (cur_month == 4 || cur_month == 6 || cur_month == 9 || cur_month == 11) {
            max_day = 30;
        }
        else {
            max_day = 31;
        }
        if (cur_day > max_day) {
            selected_day = max_day - 1;
        }
        days.clear();
        for (int i = 1; i <= max_day; ++i) {
            days.push_back(std::to_string(i).size() == 1 ? "0" + std::to_string(i) : std::to_string(i));
        }
    });

    Component menu_month = Menu(&months, &selected_month, menu_option_month);
    Component menu_year = Menu(&years, &selected_year, myMenuOption);
    Component menu_hour = Menu(&hours, &selected_hour, myMenuOption);
    Component menu_minute = Menu(&minutes, &selected_minute, myMenuOption);
    
    menu_day |= CatchEvent([&] (Event event) {
        if (event == Event::ArrowUp) {
            selected_day = (selected_day - 1 + days.size()) % days.size() + 1;
        }
        else if (event == Event::ArrowDown) {
            selected_day = (selected_day + 1) % days.size() - 1;
        }
        return false;
    });
    menu_month |= CatchEvent([&] (Event event) {
        if (event == Event::ArrowUp) {
            selected_month = (selected_month - 1 + months.size()) % months.size() + 1;
        }
        else if (event == Event::ArrowDown) {
            selected_month = (selected_month + 1) % months.size() - 1;
        }
        return false;
    });
    menu_year |= CatchEvent([&] (Event event) {
        if (event == Event::ArrowUp) {
            selected_year = (selected_year - 1 + years.size()) % years.size() + 1;
        }
        else if (event == Event::ArrowDown) {
            selected_year = (selected_year + 1) % years.size() - 1;
        }
        return false;
    });
    menu_hour |= CatchEvent([&] (Event event) {
        if (event == Event::ArrowUp) {
            selected_hour = (selected_hour - 1 + hours.size()) % hours.size() + 1;
        }
        else if (event == Event::ArrowDown) {
            selected_hour = (selected_hour + 1) % hours.size() - 1;
        }
        return false;
    });
    menu_minute |= CatchEvent([&] (Event event) {
        if (event == Event::ArrowUp) {
            selected_minute = (selected_minute - 1 + minutes.size()) % minutes.size() + 1;
        }
        else if (event == Event::ArrowDown) {
            selected_minute = (selected_minute + 1) % minutes.size() - 1;
        }
        return false;
    });

    Component container_date = Container::Horizontal({
        menu_day,
        menu_month,
        menu_year,
    });
    Component container_time = Container::Horizontal({
        menu_hour,
        menu_minute,
    });

    Component renderer_date = Renderer(container_date, [&] {
        return hbox({
            text("Date: ") | size(WIDTH, EQUAL, 10),
            menu_day->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menu_month->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menu_year->Render() | size(WIDTH, EQUAL, 4) | frame | size(HEIGHT, EQUAL, 1),
        });
    });
    Component renderer_time = Renderer(container_time, [&] {
        return hbox({
            text("Time: ") | size(WIDTH, EQUAL, 10),
            menu_hour->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text(":") | size(WIDTH, EQUAL, 1),
            menu_minute->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
        });
    });

    checkbox_services |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab;
        if (check && selected_service < 6) {
            selected_service++;
        } else if (check) {
            renderer_date->TakeFocus();
        }
        return check;
    });
    checkbox_services |= CatchEvent([&] (Event event) {
        bool check = event == Event::TabReverse;
        if (check && selected_service > 0) {
            selected_service--;
        }
        return check;
    });

    renderer_date |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab or event == Event::Return;
        if (check) {
            container_time->TakeFocus();
        }
        return check;
    });
    renderer_date |= CatchEvent([&] (Event event) {
        bool check = event == Event::TabReverse;
        if (check) {
            checkbox_services->TakeFocus();
        }
        return check;
    });
    renderer_time |= CatchEvent([&] (Event event) {
        bool check = event == Event::TabReverse;
        if (check) {
            renderer_date->TakeFocus();
        }
        return check;
    });

    std::string requirements = "";
    InputOption input_option_requirement;
    input_option_requirement.transform = [](InputState state) {
        state.element |= color(Color::White);

        if (state.is_placeholder) {
            state.element |= dim;
        }

        if (state.focused) {
            state.element |= inverted;
        } else if (state.hovered) {
        state.element |= bgcolor(Color::GrayDark);
    }
    state.element |= borderRounded;
    state.element |= size(WIDTH, EQUAL, 40);
    state.element |= size(HEIGHT, EQUAL, 8);
    return state.element;
    };
    Component input_requirements = Input(&requirements, input_option_requirement);

    renderer_time |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab or event == Event::Return;
        if (check) {
            input_requirements->TakeFocus();
        }
        return check;
    });

    ButtonOption button_option_next;
    button_option_next.transform = [](const EntryState& s) {
        auto element = text(s.label) | center | borderRounded;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    Component button_next = Button("Next", [&] {
    }, button_option_next) | size(WIDTH, EQUAL, 8);
    button_next |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab;
        return check;
    });

    Component tab_appointment = Renderer( 
            Container::Vertical({
                checkbox_services, 
                renderer_date,
                renderer_time,
                input_requirements,
                button_next
            }) ,[&] {
        return vbox({
            hbox({
                text("Services") | size(WIDTH, EQUAL, 20),
                text("Stylist") | size(WIDTH, EQUAL, 20),
            }),
            checkbox_services->Render(),
            text("Select date and time"),
            renderer_date->Render(),
            renderer_time->Render(),
            text("Reqirements"),
            input_requirements->Render(),
            button_next->Render()| align_right,
            }) | size(WIDTH, EQUAL, 40);
    });

    // History tab
    Component tab_history = Renderer([&] {
        return text("History") | center ;
    });

    // Profile tab
    Component tab_profile = Renderer([&] {
        return text("Profile") | center;
    });

    
    // tab container
    Component tab_container = Container::Tab({
        tab_home,
        tab_appointment,
        tab_history,
        tab_profile,
    }, &selected_tab) | borderRounded | size(WIDTH, EQUAL, 90);

    // All
    Component all_container = Container::Horizontal({
        tab_renderer,
        tab_container,
    });

    // exit dialog
    Component button_yes = Button("Yes", [&] {
        exit = 0;
        currentScreen = welcome_screen;
        screen.Exit();
    }, myButtonOption);
    Component button_no = Button("No", [&] {
        exit = 0; 
    }, myButtonOption);
    Component buttons_exit = Container::Horizontal({
        button_no,
        button_yes,
    });
    Component exit_dialog = Renderer([&] {
        return vbox({
            paragraphAlignCenter("Do you really want to logout?") | size(WIDTH, EQUAL, 21),
            separator(),
            hbox({
                button_no->Render() | size(WIDTH, EQUAL, 10),
                separator(),
                button_yes->Render()| size(WIDTH, EQUAL, 10)
            }) 
        }) | borderRounded;
    });

    Component all_renderer = Renderer(Container::Tab({all_container, buttons_exit}, &exit), [&] {
        auto document = all_container->Render();
        if (exit == 1) {
            document = dbox({
                document,
                exit_dialog->Render() | clear_under | center,
            });
        }
        return document;
    });
    screen.Loop(all_renderer);
}