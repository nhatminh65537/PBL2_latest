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

using namespace ftxui;

void screenWelcome()
{
    // welcome screen
    HIDE_CURSOR;
    auto screen = ScreenInteractive::FixedSize(120, 30);
    
    ButtonOption myButtonOption;
    myButtonOption.transform = [](const EntryState& s) {
        auto element = text(s.label) | center | borderRounded;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    auto buttons = Container::Vertical({
        Button("Login", [&] {
            currentScreen = screenLogin;
            screen.Exit();
        }, myButtonOption) | size(WIDTH, EQUAL, 20),
        Button("Register", [&]{
            currentScreen = screenRegister;
            screen.Exit();
        }, myButtonOption) | size(WIDTH, EQUAL, 20),
        Button("Exit", [&] {
            currentScreen = nullptr;
            screen.Exit();
        }, myButtonOption) | size(WIDTH, EQUAL, 20),
    });
    auto welcome = Renderer(buttons, [&] {
        return vbox({
            text("Welcome") | hcenter,
            buttons->Render(),
        }) | center;
    });
    screen.Loop(welcome);
}

void screenLogin()
{
    // Error handle
    std::string errorUsername = "";
    std::string errorPassword = "";
    std::string errorRole     = "";
    auto errorHandle = [&] (int errorCode) {
        switch (errorCode)
        {
            case ERROR_CODE::LOGIN_USERNAME_EMPTY:
                errorUsername = "Username is empty";
                break;
            case ERROR_CODE::LOGIN_PASSWORD_EMPTY:
                errorPassword = "Password is empty";
                break;
            case ERROR_CODE::LOGIN_USER_AND_PASS_EMPTY:
                errorUsername = "Username is empty";
                errorPassword = "Password is empty";
                break;
            default:
                break;
        }
    };

    // Login screen
    auto screen = ScreenInteractive::FixedSize(120, 30);

    std::string username;
    std::string password;
    int role = 0;

    // Input
    Component inputUsername = Input(&username, "username");

    InputOption inputOptionPassword;
    inputOptionPassword.password = true;
    Component inputPassword = Input(&password, "password", inputOptionPassword);

    // Radiobox
    std::vector<std::string> roles = {
        "Stylist",
        "Customer",
        "Admin"
    };
    Component radioboxRole = Radiobox(&roles, &role);

    // Button
    ButtonOption buttonOptionAll;
    buttonOptionAll.transform = [](const EntryState& s) {
        auto element = text(s.label) | center | borderRounded;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    Component buttonLogin = Button("Login", [&] {
        errorUsername = "";
        errorPassword = "";
        errorRole     = "";
        try {
            callLogin(username, password, role);
            switch (role)
            {
                case 0:
                    currentScreen = screenStylist;
                    break;
                case 1:
                    currentScreen = screenCustomer;
                    break;
                case 2:
                    currentScreen = screenAdmin;
                    break;
            };
            screen.Exit();
        }
        catch (int errorCode) {
            errorHandle(errorCode);
        }
    }, buttonOptionAll) | size(WIDTH, EQUAL, 15);
    Component buttonBack  = Button("Back" , [&] {
        currentScreen = screenWelcome;
        screen.Exit();
    }, buttonOptionAll) | size(WIDTH, EQUAL, 15);
    int selectedButton = 1;
    Component containerButtons = Container::Horizontal({
        buttonBack,
        buttonLogin,
    }, &selectedButton);

    // Event
    inputUsername |= CatchEvent([&] (Event event) {
        bool check = event == Event::Return;
        if (check) {
            inputPassword->TakeFocus();
        }
        return check;
    });
    inputPassword |= CatchEvent([&] (Event event) {
    bool check = event == Event::Return;
    if (check) {
        radioboxRole->TakeFocus();
    }
    return check;
    });
    radioboxRole |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab;
        if (check) {
            containerButtons->TakeFocus();
        }
        return check;
    });

    // All
    Component containerAll = Container::Vertical({
        inputUsername,
        inputPassword,
        radioboxRole,
        containerButtons,
    });

    Component rendererAll = Renderer(containerAll, [&] {
        HIDE_CURSOR;
        return vbox({
            text("Enter username") | hcenter,
            inputUsername->Render() | borderRounded | size(WIDTH, EQUAL, 30),
            text(errorUsername) | align_right | size(HEIGHT, EQUAL, errorUsername.size() > 0 ? 1 : 0) | color(Color::Red),
            text("Enter password") | hcenter,
            inputPassword->Render() | borderRounded | size(WIDTH, EQUAL, 30),
            text(errorPassword) | align_right | size(HEIGHT, EQUAL, errorPassword.size() > 0 ? 1 : 0) | color(Color::Red),
            text("Select role") | hcenter,
            radioboxRole->Render(),
            containerButtons->Render(),
        }) | center;
    });

    screen.Loop(rendererAll);
}

void screenRegister()
{
    // register screen
    HIDE_CURSOR;
    auto screen = ScreenInteractive::FixedSize(120, 30);

    std::string firstname;
    std::string lastname;
    std::string username;
    std::string password;
    std::string confirmpassword;
    std::string phonenum;
    std::string age;
    int gender = 0;
    int role = 0;

    
    // Input components
    Component inputFirstname = Input(&firstname, "First Name");
    Component inputLastname = Input(&lastname, "Last Name");
    Component inputUsername = Input(&username, "Username");

    InputOption inputOptionPassword;
    inputOptionPassword.password = true;
    Component inputPassword = Input(&password, "Password", inputOptionPassword);
    Component inputConfirmpassword = Input(&confirmpassword, "Confirm Password", inputOptionPassword);

    Component inputPhonenumber = Input(&phonenum, "Phone Number");
    Component inputAge = Input(&age, "Age");

    ComponentDecorator eventNumberOnly = CatchEvent([&](Event event) {
        return event.is_character() && !std::isdigit(event.character()[0]);
    });
    inputPhonenumber |= eventNumberOnly;
    inputPhonenumber |=  CatchEvent([&](Event event) {
        return event.is_character() && phonenum.size() >= 11;
    });
    inputAge |= eventNumberOnly;
    inputAge |= CatchEvent([&](Event event) {
        return event.is_character() && age.size() >= 3;
    });


    int selectedInput = 0;
    Component containerInputs = Container::Vertical({
        inputFirstname,
        inputLastname,
        inputUsername,
        inputPassword,
        inputConfirmpassword,
        inputPhonenumber,
        inputAge,
    }, &selectedInput);
    containerInputs |= CatchEvent([&] (Event event) {
        bool check = event == Event::Return;
        if (check && selectedInput < containerInputs->ChildCount() - 1) {
            selectedInput++;
        }
        return check;
    });
    containerInputs |= CatchEvent([&] (Event event){
        bool check = event == Event::TabReverse;
        if (check && selectedInput > 0) {
            selectedInput--;
        }
        return check;
    });

    // Radiobox
    std::vector <std::string> genders = {"Male", "Female"};
    Component radioboxGender = Radiobox(&genders, &gender);

    std::vector <std::string> roles = {"Customer"};
    Component radioboxRole = Radiobox(&roles, &role);

    // Button
    ButtonOption buttonOptionAll;
    buttonOptionAll.transform = [](const EntryState& s) {
        auto element = text(s.label) | center | borderRounded;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    Component buttonRegister = Button("Register", [&] {}, buttonOptionAll) | size(WIDTH, EQUAL, 15);
    Component buttonBack  = Button("Back" , [&] {
        currentScreen = screenWelcome;
        screen.Exit();
    }, buttonOptionAll) | size(WIDTH, EQUAL, 15);
    int selectedButton = 1;
    Component containerButtons = Container::Horizontal({
        buttonBack,
        buttonRegister,
    }, &selectedButton);

    auto rendererAll = Renderer(
        Container::Vertical({
            containerInputs, 
            radioboxGender, 
            radioboxRole, 
            containerButtons,}),
        [&] {
            return 
                vbox({
                    vbox({
                        hbox({text("First Name      : "), inputFirstname->Render()}),
                        hbox({text("Last Name       : "), inputLastname->Render()}),
                        hbox({text("Username        : "), inputUsername->Render()}),
                        hbox({text("Password        : "), inputPassword->Render()}),
                        hbox({text("Confirm Password: "), inputConfirmpassword->Render()}),
                        hbox({text("Phone Number    : "), inputPhonenumber->Render()}),
                        hbox({text("Age             : "), inputAge->Render()}),
                        hbox({text("Gender          : "), radioboxGender->Render()}),
                        hbox({text("Role            : "), radioboxRole->Render()}),
                    }) | size(WIDTH, EQUAL, 50) | borderRounded,
                    containerButtons->Render() | center
                }) | center;
    });
    
    screen.Loop(rendererAll);
}

void screenCustomer()
{
    // test stuff
    std::string name = "John Doe";
    std::ofstream log;
    log.open("log.txt");

    // customer screen
    auto screen = ScreenInteractive::FixedSize(120, 30);
    int exit = 0;

    // Navigation tab buttons
    int selectedTab = 0;
    ButtonOption buttonOptionTab;
    buttonOptionTab.transform = [](const EntryState& s) {
        auto element = text(s.label) | center ;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    Component buttonHome = Button("Home", [&] {selectedTab = 0;}, buttonOptionTab);
    Component buttonAppointment = Button("Appointment", [&] {selectedTab = 1;}, buttonOptionTab);
    Component buttonHistory = Button("History", [&] {selectedTab = 2;}, buttonOptionTab);
    Component buttonProfile = Button("Profile", [&] {selectedTab = 3;}, buttonOptionTab);
    Component buttonLogout = Button("Logout", [&] {exit = 1;}, buttonOptionTab);
    Component containerButtons = Container::Vertical({
        buttonHome,
        buttonAppointment,
        buttonHistory,
        buttonProfile,
        buttonLogout,
    });
    Component rendererButtons = Renderer(containerButtons, [&] {
        return vbox({
            text("Welcome " + name) | hcenter,
            filler() | size(HEIGHT, EQUAL, 2),
            separator(),
            buttonHome->Render(),
            separator(),
            buttonAppointment->Render(),
            separator(),
            buttonHistory->Render(),
            separator(),
            buttonProfile->Render(),
            separator(),
            filler(),
            separator(),
            buttonLogout->Render()
        }) | borderRounded | size(WIDTH, EQUAL, 30);
    });

    // Home tab
    Component tabHome = Renderer([&] {
        return text("Home") | center ;
    });

    // Appointment tab
    int tabComfirm = 0;
    const int servicesCount = SERVICES_COUNT;
    std::array<bool, servicesCount> selectedServices;
    std::array<int, servicesCount> selectedStylist;
    std::vector<std::string> services = {
        "Cat toc",
        "Nhuom toc",
        "Tay toc",
        "Uon toc",
        "Phuc hoi toc",
        "Tao kieu",
        "Goi dau"
    };
    std::vector<std::string> stylists[servicesCount];
    int selectedCheckboxService = 0;
    Component containerServices = Container::Vertical({}, &selectedCheckboxService);
    for (int i = 0; i < servicesCount; ++i) {
        selectedServices[i] = false;
        stylists[i] = callGetStylist(i);
        selectedStylist[i] = 0;
        Component newBox = Container::Horizontal({
                Checkbox(services[i], &selectedServices[i]) | size(WIDTH, EQUAL, 20),
                Dropdown({
                    .radiobox = {.entries = &(stylists[i]), .selected = &(selectedStylist[i])},
                    .transform = [](bool open, Element checkbox, Element radiobox) {
                        if (open) {
                            const int maxHeight = 4;
                            return vbox({
                                checkbox | inverted,
                                radiobox | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, maxHeight),
                            });
                        }
                        return vbox({checkbox, filler()}) ;
                    },
                    }) | size(WIDTH, EQUAL, 20),
        });
        containerServices->Add(newBox);
    }

    int selectedDay = 0;
    int selectedMonth = 0;
    int selectedYear = 0;
    int selectedHour = 0;
    int selectedMinute = 0;

    std::vector<std::string> days = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    std::vector<std::string> months = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"};
    std::vector<std::string> years  = {"2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029", "2030"};
    std::vector<std::string> hours = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"};
    std::vector<std::string> minutes = {"00", "15", "30", "45"};


    MenuOption menuOptionAll;
    menuOptionAll.direction = Direction::Up;
    menuOptionAll.entries_option.transform = [](const EntryState& s) {
        auto element = text(s.label) | center;
        if (s.focused) {
            element = element | inverted;
        }
        return element;
    };

    Component menuDay = Menu(&days, &selectedDay, menuOptionAll);
    MenuOption menuOptionMonth = MenuOption(menuOptionAll);
    menuOptionMonth.on_change = ([&] () {
        int cur_year = 2021 + selectedYear;
        int cur_month = 1 + selectedMonth;
        int cur_day = 1 + selectedDay;
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
            selectedDay = max_day - 1;
        }
        days.clear();
        for (int i = 1; i <= max_day; ++i) {
            days.push_back(std::to_string(i).size() == 1 ? "0" + std::to_string(i) : std::to_string(i));
        }
    });

    Component menuMonth = Menu(&months, &selectedMonth, menuOptionMonth);
    Component menuYear = Menu(&years, &selectedYear, menuOptionAll);
    Component menuHour = Menu(&hours, &selectedHour, menuOptionAll);
    Component menuMinute = Menu(&minutes, &selectedMinute, menuOptionAll);

    Component containerDate = Container::Horizontal({
        menuDay,
        menuMonth,
        menuYear,
    });
    Component containerTime = Container::Horizontal({
        menuHour,
        menuMinute,
    });

    Component rendererDate = Renderer(containerDate, [&] {
        return hbox({
            text("Date: ") | size(WIDTH, EQUAL, 10),
            menuDay->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menuMonth->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menuYear->Render() | size(WIDTH, EQUAL, 4) | frame | size(HEIGHT, EQUAL, 1),
        });
    });
    Component rendererTime = Renderer(containerTime, [&] {
        return hbox({
            text("Time: ") | size(WIDTH, EQUAL, 10),
            menuHour->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text(":") | size(WIDTH, EQUAL, 1),
            menuMinute->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
        });
    });

    containerServices |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab;
        if (check && selectedCheckboxService < 6) {
            selectedCheckboxService++;
        } else if (check) {
            rendererDate->TakeFocus();
        }
        return check;
    });
    containerServices |= CatchEvent([&] (Event event) {
        bool check = event == Event::TabReverse;
        if (check && selectedCheckboxService > 0) {
            selectedCheckboxService--;
        }
        return check;
    });

    rendererDate |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab || event == Event::Return;
        if (check) {
            containerTime->TakeFocus();
        }
        return check;
    });
    rendererDate |= CatchEvent([&] (Event event) {
        bool check = event == Event::TabReverse;
        if (check) {
            containerServices->TakeFocus();
        }
        return check;
    });
    rendererTime |= CatchEvent([&] (Event event) {
        bool check = event == Event::TabReverse;
        if (check) {
            rendererDate->TakeFocus();
        }
        return check;
    });

    std::string requirement = "";
    InputOption inputOptionRequirement;
    inputOptionRequirement.transform = [](InputState state) {
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
    Component inputRequirement = Input(&requirement, inputOptionRequirement);

    rendererTime |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab || event == Event::Return;
        if (check) {
            inputRequirement->TakeFocus();
        }
        return check;
    });

    ButtonOption buttonOptionNext;
    buttonOptionNext.transform = [](const EntryState& s) {
        auto element = text(s.label) | center | borderRounded;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    Component buttonNext = Button("Next", [&] {
        try {
            callCreateAppointment(selectedServices, selectedStylist, selectedDay, selectedMonth, selectedYear, selectedHour, selectedMinute, requirement);
            tabComfirm = 1;
        }
        catch (int errorCode) {
            // error_handle(errorCode);
        }
    }, buttonOptionNext) | size(WIDTH, EQUAL, 8);
    buttonNext |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab;
        return check;
    });

    Component tabAppointmentInput = Renderer( 
            Container::Vertical({
                containerServices, 
                rendererDate,
                rendererTime,
                inputRequirement,
                buttonNext
            }) ,[&] {
        return 
            vbox({
                hbox({
                    text("Services") | size(WIDTH, EQUAL, 20),
                    text("Stylist") | size(WIDTH, EQUAL, 20),
                }),
                containerServices->Render(),
                text("Select date and time"),
                rendererDate->Render(),
                rendererTime->Render(),
                text("Reqirements"),
                inputRequirement->Render(),
                buttonNext->Render()| align_right,
            }) | size(WIDTH, EQUAL, 40);
    });

    auto resetAppointment = [&] {
        for (int i = 0; i < servicesCount; ++i) {
            selectedServices[i] = false;
            selectedStylist[i] = 0;
        }
        selectedCheckboxService = 0;
        selectedDay = 0;
        selectedMonth = 0;
        selectedYear = 0;
        selectedHour = 0;
        selectedMinute = 0;
        requirement = "";
    };
    int selectedButtonsAppointment = 1;

    ButtonOption buttonOptionConfirm(buttonOptionNext);
    Component buttonAppointmentConfirm = Button("Comfirm", [&] {
        tabComfirm = 0;
        resetAppointment();
    }, buttonOptionConfirm);
    Component buttonAppointmentCancel = Button("Cancel", [&] {
        tabComfirm = 0;
        selectedButtonsAppointment = 1;
    }, buttonOptionConfirm);
    Component tabAppointmentConfirm = Renderer(Container::Horizontal({
        buttonAppointmentCancel,
        buttonAppointmentConfirm
    }, &selectedButtonsAppointment) ,[&] {
        return vbox({
            text("Comfirm appointment") | center,
            filler() | size(HEIGHT, EQUAL, 20),
            hbox({
                buttonAppointmentCancel->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 5),
                buttonAppointmentConfirm->Render() | size(WIDTH, EQUAL, 10),
            })| vcenter,
        }) | center | borderRounded | size(WIDTH, EQUAL, 40);
    });
    
    Component tabAppointment = Renderer(
        Container::Tab({
            tabAppointmentInput,
            tabAppointmentConfirm
        }, &tabComfirm), [&] {
        auto document = hbox({
            filler() | size(WIDTH, EQUAL, 2),
            tabAppointmentInput->Render(),
        });
        if (tabComfirm == 1) {
            document = dbox({
                hbox({
                filler() | size(WIDTH, EQUAL, 2),
                tabAppointmentInput->Render(),
                }),
                tabAppointmentConfirm->Render() | clear_under | align_right,
            });
        }
        return document;
    });

    // History tab
    Component tabHistory = Renderer([&] {
        return text("History") | center ;
    });

    // Profile tab
    std::string firstname;
    std::string lastname;
    std::string username;
    std::string oldPassword;
    std::string newPassword;
    std::string confirmpassword;
    std::string phonenumber;
    std::string age;
    int gender = 0;

    /**
    Update value in profile
    Later
    (!)
    (!)
    (!)
    */
    auto resetName = [&] {
    };
    auto resetPassword = [&] {
    };
    auto resetPersonInfo = [&] {
    };
    auto resetProfile = [&] {
        resetName();
        resetPassword();
        resetPersonInfo();
    };

    const int PROFILE_ALIGN_WIDTH = 50;

    InputOption inputOptionProfile = InputOption::Default();
    inputOptionProfile.placeholder = "";
    inputOptionProfile.multiline = false;

    Component inputFirstname = Input(&firstname, "First Name", inputOptionProfile);
    Component inputLastname = Input(&lastname, "Last Name", inputOptionProfile);

    InputOption inputOptionPassword(inputOptionProfile);
    inputOptionPassword.password = true;

    Component inputOldpassword = Input(&oldPassword, "Old Password", inputOptionPassword);
    Component inputNewpassword = Input(&newPassword, "New Password", inputOptionPassword);
    Component inputConfirmpassword = Input(&confirmpassword, "Confirm Password", inputOptionPassword);

    Component inputPhonenumber = Input(&phonenumber, "Phone Number", inputOptionProfile);
    Component inputAge = Input(&age, "Age", inputOptionProfile);

    ComponentDecorator eventNumberOnly = CatchEvent([&](Event event) {
        return event.is_character() && !std::isdigit(event.character()[0]);
    });
    inputPhonenumber |= eventNumberOnly;
    inputPhonenumber |=  CatchEvent([&](Event event) {
        return event.is_character() && phonenumber.size() >= 11;
    });
    inputAge |= eventNumberOnly;
    inputAge |= CatchEvent([&](Event event) {
        return event.is_character() && age.size() >= 3;
    });

    std::vector <std::string> genders = {"Male", "Female"};
    Component radioboxGender = Radiobox(&genders, &gender);
;

    Component buttonNameUpdate = Button("Update", [&] {
        // name = firstname + " " + lastname;
    }, buttonOptionTab);
    Component buttonNameReset = Button("Reset", [&] {
            // firstname = "";
            // lastname = "";
    }, buttonOptionTab);

    Component buttonPasswordUpdate = Button("Update", [&] {
        // password = password;
    }, buttonOptionTab);
    Component buttonPasswordReset = Button("Reset", [&] {
            // password = ""
            // ssword = "";
    }, buttonOptionTab);

    Component buttonPersoninfoUpdate = Button("Update", [&] {
        // phonenumber = phonenumber;
        // age = age;
    }, buttonOptionTab);
    
    Component buttonPersoninfoReset = Button("Reset", [&] {
        // phonenumber = "";
        // age = "";
    }, buttonOptionTab);

    Component containerButtonsName = Container::Horizontal({
        buttonNameReset,
        buttonNameUpdate,
    });

    Component containerButtonsPassword = Container::Horizontal({
        buttonPasswordReset,
        buttonPasswordUpdate,
    });

    Component containerButtonsPersoninfo = Container::Horizontal({
        buttonPersoninfoReset,
        buttonPersoninfoUpdate,
    });

    radioboxGender |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab;
        if (check) {
            buttonPersoninfoReset->TakeFocus();
        }
        return check;
    });
    radioboxGender |= CatchEvent([&] (Event event) {
        bool check = event == Event::TabReverse;
        if (check) {
            inputAge->TakeFocus();
        }
        return check;
    });
    containerButtonsName |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab;
        if (check) {
            inputOldpassword->TakeFocus();
        }
        return check;
    });

    containerButtonsPassword |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab;
        if (check) {
            inputPhonenumber->TakeFocus();
        }
        return check;
    });

    Component containerUpdateName = Renderer(Container::Vertical({
        inputFirstname,
        inputLastname,
        containerButtonsName
    }), [&] {
        return vbox({
            hbox({text("First Name      : "), inputFirstname->Render()}),
            hbox({text("Last Name       : "), inputLastname->Render()}),
            separator(),
            hbox({
                buttonNameReset->Render() | size(WIDTH, EQUAL, 10),
                buttonNameUpdate->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter,
        }) | borderRounded | size(WIDTH, EQUAL, PROFILE_ALIGN_WIDTH);
    });

    Component containerUpdatePassword = Renderer(Container::Vertical({
        inputOldpassword,
        inputNewpassword,
        inputConfirmpassword,
        containerButtonsPassword,
    }), [&] {
        return vbox({
            hbox({text("Old Password    : "), inputOldpassword->Render()}),
            hbox({text("New Password    : "), inputNewpassword->Render()}),
            hbox({text("Confirm Password: "), inputConfirmpassword->Render()}),
            separator(),
            hbox({
                buttonPasswordReset->Render() | size(WIDTH, EQUAL, 10),
                buttonPasswordUpdate->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter,
        }) | borderRounded | size(WIDTH, EQUAL, PROFILE_ALIGN_WIDTH);
    });

    Component containerUpdatePersoninfo = Renderer(Container::Vertical({
        inputPhonenumber,
        inputAge,
        radioboxGender,
        containerButtonsPersoninfo,
    }), [&] {
        return vbox({
            hbox({text("Phone Number    : "), inputPhonenumber->Render()}),
            hbox({text("Age             : "), inputAge->Render()}),
            hbox({text("Gender          : "), radioboxGender->Render()}),
            separator(),
            hbox({
                buttonPersoninfoReset->Render() | size(WIDTH, EQUAL, 10),
                buttonPersoninfoUpdate->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter,
        }) | borderRounded | size(WIDTH, EQUAL, PROFILE_ALIGN_WIDTH);
    });

    Component tabProfile = Renderer(Container::Vertical({
        containerUpdateName,
        containerUpdatePassword,
        containerUpdatePersoninfo,
    }), [&] {
        return vbox({
            text("Name"),
            containerUpdateName->Render(),
            text("Password"),
            containerUpdatePassword->Render(),
            text("Personal Information"),
            containerUpdatePersoninfo->Render(),
        }) | size(WIDTH, EQUAL, PROFILE_ALIGN_WIDTH) | center;
    });
    
    // tab container
    Component containerTabs = Container::Tab({
        tabHome,
        tabAppointment,
        tabHistory,
        tabProfile,
    }, &selectedTab) | borderRounded | size(WIDTH, EQUAL, 90);

    // All
    Component containerAll = Container::Horizontal({
        rendererButtons,
        containerTabs,
    });

    // exit dialog
    Component buttonYes = Button("Yes", [&] {
        exit = 0;
        currentScreen = screenWelcome;
        screen.Exit();
    }, buttonOptionTab);
    Component buttonNo = Button("No", [&] {
        exit = 0; 
    }, buttonOptionTab);
    Component containerButtonsExit = Container::Horizontal({
        buttonNo,
        buttonYes,
    });
    Component DialogExit = Renderer([&] {
        return vbox({
            paragraphAlignCenter("Do you really want to logout?") | size(WIDTH, EQUAL, 21),
            separator(),
            hbox({
                buttonNo->Render() | size(WIDTH, EQUAL, 10),
                separator(),
                buttonYes->Render()| size(WIDTH, EQUAL, 10)
            }) 
        }) | borderRounded;
    });

    Component rendererAll = Renderer(Container::Tab({containerAll, containerButtonsExit}, &exit), [&] {
        auto document = containerAll->Render();
        if (exit == 1) {
            document = dbox({
                document,
                DialogExit->Render() | clear_under | center,
            });
        }
        return document;
    });
    screen.Loop(rendererAll);
}

void screenStylist() 
{
    // test stuff
    std::string name = "John Doe";
    std::ofstream log;
    log.open("log.txt");

    // customer screen
    auto screen = ScreenInteractive::FixedSize(120, 30);
    int exit = 0;

    // Navigation tab buttons
    int selectedTab = 0;
    ButtonOption buttonOptionTab;
    buttonOptionTab.transform = [](const EntryState& s) {
        auto element = text(s.label) | center ;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    Component buttonHome = Button("Home", [&] {
        selectedTab = 0;
    }, buttonOptionTab);
    Component buttonSchedule = Button("Schedule", [&] {
        selectedTab = 1;
    }, buttonOptionTab);
    Component buttonHistory = Button("History", [&] { // Look for feedbak
        selectedTab = 2;
    }, buttonOptionTab);
    Component buttonProfile = Button("Profile", [&] { // Only can change pass and person info
        selectedTab = 3;
    }, buttonOptionTab);
    Component buttonLogout = Button("Logout", [&] {
        exit = 1;
    }, buttonOptionTab);
    Component containerButtons = Container::Vertical({
        buttonHome,
        buttonSchedule,
        buttonHistory,
        buttonProfile,
        buttonLogout,
    });
    Component rendererButtons = Renderer(containerButtons, [&] {
        return vbox({
            text("Welcome " + name) | hcenter,
            filler() | size(HEIGHT, EQUAL, 2),
            separator(),
            buttonHome->Render(),
            separator(),
            buttonSchedule->Render(),
            separator(),
            buttonHistory->Render(),
            separator(),
            buttonProfile->Render(),
            separator(),
            filler(),
            separator(),
            buttonLogout->Render()
        }) | borderRounded | size(WIDTH, EQUAL, 30);
    });
    
    // Home tab
    Component tabHome = Renderer([&] {
        return text("Home") | center ;
    });

    // Schedule tab
    Component tabSchedule = Renderer([&] {
        return text("Schedule") | center;
    });

    // History tab
    Component tabHistory = Renderer([&] {
        return text("History") | center ;
    });

    // Profile tab
    Component tabProfile = Renderer([&] {
        return text("Profile") | center;
    });


    Component containerTabs = Container::Tab({
        tabHome,
        tabSchedule,
        tabHistory,
        tabProfile,
    }, &selectedTab) | borderRounded | size(WIDTH, EQUAL, 90);

    // All
    Component containerAll = Container::Horizontal({
        rendererButtons,
        containerTabs,
    });

    // exit dialog
    Component buttonYes = Button("Yes", [&] {
        exit = 0;
        currentScreen = screenWelcome;
        screen.Exit();
    }, buttonOptionTab);
    Component buttonNo = Button("No", [&] {
        exit = 0; 
    }, buttonOptionTab);
    Component containerButtonsExit = Container::Horizontal({
        buttonNo,
        buttonYes,
    });
    Component DialogExit = Renderer([&] {
        return vbox({
            paragraphAlignCenter("Do you really want to logout?") | size(WIDTH, EQUAL, 21),
            separator(),
            hbox({
                buttonNo->Render() | size(WIDTH, EQUAL, 10),
                separator(),
                buttonYes->Render()| size(WIDTH, EQUAL, 10)
            }) 
        }) | borderRounded;
    });

    Component rendererAll = Renderer(Container::Tab({containerAll, containerButtonsExit}, &exit), [&] {
        auto document = containerAll->Render();
        if (exit == 1) {
            document = dbox({
                document,
                DialogExit->Render() | clear_under | center,
            });
        }
        return document;
    });
    screen.Loop(rendererAll);
}

void screenAdmin()
{
    // test stuff
    std::string name = "John Doe";
    std::ofstream log;
    log.open("log.txt");

    // customer screen
    auto screen = ScreenInteractive::FixedSize(120, 30);
    int exit = 0;

    // Navigation tab buttons
    int selectedTab = 0;
    ButtonOption buttonOptionTab;
    buttonOptionTab.transform = [](const EntryState& s) {
        auto element = text(s.label) | center ;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    Component buttonHome = Button("Home", [&] {
        selectedTab = 0;
    }, buttonOptionTab);
    Component buttonStatistics = Button("Statistics", [&] {
        selectedTab = 1;
    }, buttonOptionTab);
    Component buttonStaff = Button("Staff", [&] { // Look for feedbak
        selectedTab = 2;
    }, buttonOptionTab);
    Component buttonProfile = Button("Profile", [&] { // Only can change pass and person info
        selectedTab = 3;
    }, buttonOptionTab);
    Component buttonLogout = Button("Logout", [&] {
        exit = 1;
    }, buttonOptionTab);
    Component containerButtons = Container::Vertical({
        buttonHome,
        buttonStatistics,
        buttonStaff,
        buttonProfile,
        buttonLogout,
    });
    Component rendererButtons = Renderer(containerButtons, [&] {
        return vbox({
            text("Welcome " + name) | hcenter,
            filler() | size(HEIGHT, EQUAL, 2),
            separator(),
            buttonHome->Render(),
            separator(),
            buttonStatistics->Render(),
            separator(),
            buttonStaff->Render(),
            separator(),
            buttonProfile->Render(),
            separator(),
            filler(),
            separator(),
            buttonLogout->Render()
        }) | borderRounded | size(WIDTH, EQUAL, 30);
    });
    
    // Home tab
    Component tabHome = Renderer([&] {
        return text("Home") | center ;
    });

    // Schedule tab
    Component tabStatistics = Renderer([&] {
        return text("Statistics") | center;
    });

    // History tab
    Component tabStaff = Renderer([&] {
        return text("Staff") | center ;
    });

    // Profile tab
    Component tabProfile = Renderer([&] {
        return text("Profile") | center;
    });


    Component containerTabs = Container::Tab({
        tabHome,
        tabStatistics,
        tabStaff,
        tabProfile,
    }, &selectedTab) | borderRounded | size(WIDTH, EQUAL, 90);

    // All
    Component containerAll = Container::Horizontal({
        rendererButtons,
        containerTabs,
    });

    // exit dialog
    Component buttonYes = Button("Yes", [&] {
        exit = 0;
        currentScreen = screenWelcome;
        screen.Exit();
    }, buttonOptionTab);
    Component buttonNo = Button("No", [&] {
        exit = 0; 
    }, buttonOptionTab);
    Component containerButtonsExit = Container::Horizontal({
        buttonNo,
        buttonYes,
    });
    Component DialogExit = Renderer([&] {
        return vbox({
            paragraphAlignCenter("Do you really want to logout?") | size(WIDTH, EQUAL, 21),
            separator(),
            hbox({
                buttonNo->Render() | size(WIDTH, EQUAL, 10),
                separator(),
                buttonYes->Render()| size(WIDTH, EQUAL, 10)
            }) 
        }) | borderRounded;
    });

    Component rendererAll = Renderer(Container::Tab({containerAll, containerButtonsExit}, &exit), [&] {
        auto document = containerAll->Render();
        if (exit == 1) {
            document = dbox({
                document,
                DialogExit->Render() | clear_under | center,
            });
        }
        return document;
    });
    screen.Loop(rendererAll);
}
