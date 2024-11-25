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

std::function<void()> currentScreen = screenWelcome;

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
    std::string errorUsername  = "";
    std::string errorPassword  = "";
    std::string errorIncorrect = "";
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
            case ERROR_CODE::LOGIN_INCORRECT_INPUT:
                errorIncorrect = "Incorrect user, pass of role";
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
    inputUsername |= CatchEvent([&](Event event) {
        return event.is_character() && !std::isalnum(event.character()[0]);
    });
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
        errorUsername  = "";
        errorPassword  = "";
        errorIncorrect = "";
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
        auto textError = [&](std::string error) {
            if (error.size() == 0) {
                return text("") | size(HEIGHT, EQUAL, 0);
            }
            return paragraph(error) | align_right | color(Color::Red);
        };

        return vbox({
            text("Enter username") | hcenter,
            inputUsername->Render() | borderRounded | size(WIDTH, EQUAL, 30),
            textError(errorUsername),
            text("Enter password") | hcenter,
            inputPassword->Render() | borderRounded | size(WIDTH, EQUAL, 30),
            textError(errorPassword ),
            text("Select role") | hcenter,
            radioboxRole->Render(),
            textError(errorIncorrect),
            containerButtons->Render(),
        }) | center;
    });

    screen.Loop(rendererAll);
}

void screenRegister()
{   
    // Error handle
    std::string errorSomeFieldEmpty = "";
    std::string errorPasswordNotMatch = "";
    std::string errorUsernameExist = "";
    auto errorHandle = [&] (int errorCode) {
        switch (errorCode)
        {
            case ERROR_CODE::REGISTER_SOME_FIELD_EMPTY:
                errorSomeFieldEmpty = "Please fill all fields";
                break;
            case ERROR_CODE::REGISTER_PASSWORD_NOT_MATCH:
                errorPasswordNotMatch = "Password not match";
                break;
            case ERROR_CODE::REGISTER_USERNAME_EXIST:
                errorUsernameExist = "Username already exist";
                break;
            default:
                break;
        }
    };

    // success message
    std::string massageSuccess = "";

    // register screen
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
    ComponentDecorator eventAlphabetOnly = CatchEvent([&](Event event) {
        return event.is_character() && 
            !std::isalpha(event.character()[0]) && 
            !std::isspace(event.character()[0]);
    });
    inputPhonenumber |= eventNumberOnly;
    inputPhonenumber |=  CatchEvent([&](Event event) {
        return event.is_character() && phonenum.size() >= 11;
    });
    inputAge |= eventNumberOnly;
    inputAge |= CatchEvent([&](Event event) {
        return event.is_character() && age.size() >= 3;
    });
    inputUsername |= CatchEvent([&](Event event) {
        return event.is_character() && !std::isalnum(event.character()[0]);
    });
    inputFirstname|= eventAlphabetOnly;
    inputLastname |= eventAlphabetOnly;

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
    Component buttonRegister = Button("Register", [&] {
        errorSomeFieldEmpty = "";
        errorPasswordNotMatch = "";
        errorUsernameExist = "";
        massageSuccess = "";
        try {
            callRegister(firstname, lastname, username, password, confirmpassword, phonenum, age, gender, role);
            massageSuccess = "Register success";
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
        buttonRegister,
    }, &selectedButton);

    auto rendererAll = Renderer(
        Container::Vertical({
            containerInputs, 
            radioboxGender, 
            radioboxRole, 
            containerButtons,}),
        [&] {
            auto textError = [&](std::string error) {
                return text(error) | align_right | size(HEIGHT, EQUAL, error.size() > 0 ? 1 : 0) | color(Color::Red);
            };

            return 
                vbox({
                    vbox({
                        hbox({text("First Name      : "), inputFirstname->Render()}),
                        hbox({text("Last Name       : "), inputLastname->Render()}),
                        hbox({text("Username        : "), inputUsername->Render()}),
                        hbox({text("Password        : "), inputPassword->Render()}),
                        hbox({text("Confirm Password: "), inputConfirmpassword->Render()}),
                        textError(errorPasswordNotMatch),
                        hbox({text("Phone Number    : "), inputPhonenumber->Render()}),
                        hbox({text("Age             : "), inputAge->Render()}),
                        hbox({text("Gender          : "), radioboxGender->Render()}),
                        hbox({text("Role            : "), radioboxRole->Render()}),
                        textError(errorSomeFieldEmpty),
                        textError(errorUsernameExist),
                        text(massageSuccess) | color(Color::Green) | size(HEIGHT, EQUAL, massageSuccess.size() > 0 ? 1 : 0) | align_right,
                    }) | size(WIDTH, EQUAL, 50) | borderRounded,
                    containerButtons->Render() | center
                }) | center;
    });
    
    screen.Loop(rendererAll);
}

void screenCustomer()
{
    // Begin
    #pragma region
    // test stuff
    std::string name = callGetCurrentUserName();
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
    #pragma endregion

    // Home Tab
    #pragma region
    Component tabHome = Renderer([&] {
        return text("Home") | center ;
    });
    #pragma endregion

    // Appointment Tab
    #pragma region
    std::string errorDateTime = "";
    std::string errorServicesEmpty = "";
    auto errorHandleAppointment = [&] (int errorCode) {
        switch (errorCode)
        {
            case ERROR_CODE::CREATE_APPOINTMENT_BUSY_TIME:
                errorDateTime = "The time you selected is already taken. Please choose another time.";
                break;
            case ERROR_CODE::CREATE_APPOINTMENT_CLOSED_TIME:
                errorDateTime = "The time you selected is outside of our working hours. Please choose another time.";
                break;
            case ERROR_CODE::CREATE_APPOINTMENT_SERVICES_EMPTY:
                errorServicesEmpty = "Please select at least one service.";
                break;
            default:
                break;
        }
    };

    int tabComfirm = 0;
    
    // Services
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
    std::vector<std::string> appointmentStylists[servicesCount];
    int selectedCheckboxService = 0;
    Component containerServices = Container::Vertical({}, &selectedCheckboxService);
    for (int i = 0; i < servicesCount; ++i) {
        selectedServices[i] = false;
        appointmentStylists[i] = callGetStylist(i);
        selectedStylist[i] = 0;
        Component newBox = Container::Horizontal({
                Checkbox(services[i], &selectedServices[i]) | size(WIDTH, EQUAL, 20),
                Dropdown({
                    .radiobox = {.entries = &(appointmentStylists[i]), .selected = &(selectedStylist[i])},
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

    // Date and time
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
        int curYear = 2021 + selectedYear;
        int curMonth = 1 + selectedMonth;
        int curDay = 1 + selectedDay;
        bool isLeapYear = (curYear % 4 == 0 && curYear % 100 != 0) || (curYear % 400 == 0);
        int maxDay;  
        if (curMonth == 2) {
            maxDay = isLeapYear ? 29 : 28;
        }
        else if (curMonth == 4 || curMonth == 6 || curMonth == 9 || curMonth == 11) {
            maxDay = 30;
        }
        else {
            maxDay = 31;
        }
        if (curDay > maxDay) {
            selectedDay = maxDay - 1;
        }
        days.clear();
        for (int i = 1; i <= maxDay; ++i) {
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

    // Catch event
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

    // Requirement
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

    // event
    rendererTime |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab || event == Event::Return;
        if (check) {
            inputRequirement->TakeFocus();
        }
        return check;
    });

    // Button next
    ButtonOption buttonOptionNext;
    buttonOptionNext.transform = [](const EntryState& s) {
        auto element = text(s.label) | center | borderRounded;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    
    std::string newAppointmentId;
    Component buttonNext = Button("Next", [&] {
        errorDateTime = "";
        errorServicesEmpty = "";
        try {
            callCreateNewAppointment(selectedServices, selectedStylist, selectedDay, selectedMonth, selectedYear, selectedHour, selectedMinute, requirement);
            newAppointmentId = callGetNewAppointmentId();
            tabComfirm = 1;
        }
        catch (int errorCode) {
            errorHandleAppointment(errorCode);
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
            auto textError = [&](std::string error) {
                if (error.size() == 0) {
                    return text("") | size(HEIGHT, EQUAL, 0);
                }
                return paragraph(error) | align_right | color(Color::Red);
            };

            return vbox({
                hbox({
                    text("Services") | size(WIDTH, EQUAL, 20),
                    text("Stylist") | size(WIDTH, EQUAL, 20),
                }),
                containerServices->Render(),
                textError(errorServicesEmpty),
                text("Select date and time"),
                rendererDate->Render(),
                rendererTime->Render(),
                textError(errorDateTime),
                text("Reqirements"),
                inputRequirement->Render(),
                buttonNext->Render()| align_right,
            }) | size(WIDTH, EQUAL, 40);
    });

    // Comfirm appointment
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
        newAppointmentId = "";
    };
    int selectedButtonsAppointment = 1;

    ButtonOption buttonOptionConfirm(buttonOptionNext);
    Component buttonAppointmentConfirm = Button("Comfirm", [&] {
        tabComfirm = 0;
        resetAppointment();
        callAddNewAppointment();
    }, buttonOptionConfirm);
    Component buttonAppointmentCancel = Button("Cancel", [&] {
        tabComfirm = 0;
        selectedButtonsAppointment = 1;
    }, buttonOptionConfirm);
    Component tabAppointmentConfirm = Renderer(Container::Horizontal({
        buttonAppointmentCancel,
        buttonAppointmentConfirm
    }, &selectedButtonsAppointment) ,[&] {
        auto textService = [&](int i) {
            if (!selectedServices[i]) {
                return text("") | size(HEIGHT, EQUAL, 0);
            }
            return hbox({
                text(services[i]) | size(WIDTH, EQUAL, 20),
                text(appointmentStylists[i][selectedStylist[i]]) | size(WIDTH, EQUAL, 20),
            });
        };
        Elements textServiceList;
        for (int i = 0; i < servicesCount; ++i) {
            textServiceList.push_back(textService(i));
        }

        return vbox({
            text("Comfirm appointment") | center,
            separator(),
            text("Appointment ID") | bold,
            text(newAppointmentId),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Customer name") | bold,
            text(name),
            filler() | size(HEIGHT, EQUAL, 1),
            hbox({
                text("Services") | size(WIDTH, EQUAL, 20),
                text("Stylist") | size(WIDTH, EQUAL, 20),
            }) | bold,
            vbox(textServiceList),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Date and time") | size(WIDTH, EQUAL, 40) | bold,
            text("Date: " + days[selectedDay] + "/" + months[selectedMonth] + "/" + years[selectedYear]) | size(WIDTH, EQUAL, 40),
            text("Time: " + hours[selectedHour] + ":" + minutes[selectedMinute]) | size(WIDTH, EQUAL, 40),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Requirement: ") | size(WIDTH, EQUAL, 40) | bold,
            paragraph(requirement) | size(WIDTH, EQUAL, 40),
            filler(),
            separator(),
            hbox({
                buttonAppointmentCancel->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 5),
                buttonAppointmentConfirm->Render() | size(WIDTH, EQUAL, 10),
            })| hcenter,
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
    #pragma endregion

    // History Tab
    #pragma region
    // History list
    std::vector<std::string> listHistory; // format: "ID: ... Status: ... Date: ... Time: ... Services: ..."
    // filter container
    int filterDay;
    int filterMonth;
    int filterYear;
    std::array<bool, SERVICES_COUNT> filterServices;
    std::vector<std::string> filterDays = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    std::vector<std::string> filterMonths = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"};
    std::vector<std::string> filterYears = {"----", "2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029", "2030"};
    int status = 0; // 0: all, 1: done, 2: pending, 3: cancel

    auto resetFilter = [&] {
        status = 0;
        filterDay = 0;
        filterMonth = 0;
        filterYear = 0;
        for (int i = 0; i < SERVICES_COUNT; ++i) {
            filterServices[i] = true;
        }
    };
    listHistory = callGetCurrentUserHistoryList(filterDay, filterMonth, filterYear, filterServices, status);
    
    MenuOption menuOptionFilterMonth = MenuOption(menuOptionAll);
    menuOptionFilterMonth.on_change = ([&] () {
        int curYear = 2020 + filterYear;
        int curMonth = filterMonth;
        int curDay = filterDay;
        bool isLeapYear = (curYear % 4 == 0 && curYear % 100 != 0) || (curYear % 400 == 0);
        int maxDay; 
        if (filterYear == 0) {
            isLeapYear = true;
        }
        if (curMonth == 2) {
            maxDay = isLeapYear ? 29 : 28;
        }
        else if (curMonth == 4 || curMonth == 6 || curMonth == 9 || curMonth == 11) {
            maxDay = 30;
        }
        else {
            maxDay = 31;
        }
        if (curDay > maxDay) {
            selectedDay = maxDay;
        }
        filterDays.clear();
        filterDays.push_back("--");
        for (int i = 1; i <= maxDay; ++i) {
            filterDays.push_back(std::to_string(i).size() == 1 ? "0" + std::to_string(i) : std::to_string(i));
        }
    });
    Component menuFilterDay = Menu(&filterDays, &filterDay, menuOptionAll);
    Component menuFilterMonth = Menu(&filterMonths, &filterMonth, menuOptionFilterMonth);
    Component menuFilterYear = Menu(&filterYears, &filterYear, menuOptionAll);
    Component rendererFilterDate = Renderer(
        Container::Horizontal({
            menuFilterDay,
            menuFilterMonth,
            menuFilterYear,
        }), [&] {
        return hbox({
            menuFilterDay->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menuFilterMonth->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menuFilterYear->Render() | size(WIDTH, EQUAL, 4) | frame | size(HEIGHT, EQUAL, 1),
        });
    });
    int selectedFilterService = 0;
    Component containerFilterServices = Container::Vertical({}, &selectedFilterService);
    for (int i = 0; i < SERVICES_COUNT; ++i) {
        filterServices[i] = true;
        containerFilterServices->Add(Checkbox(services[i], &filterServices[i]) | size(WIDTH, EQUAL, 20));
    }
    std::vector<std::string> filterStatus = {"All", "Done", "Pending", "Cancel"};
    Component radioboxStatus = Radiobox(filterStatus, &status);
    Component buttonFilter = Button("Filter", [&] {
        listHistory = callGetCurrentUserHistoryList(filterDay, filterMonth, filterYear, filterServices, status);
    }, buttonOptionNext);
    Component buttonResetFilter = Button("Reset", [&] {
        resetFilter();
    }, buttonOptionNext);

    rendererFilterDate |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab;
        if (check) {
            containerFilterServices->TakeFocus();
        }
        return check;
    });
    containerFilterServices |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab && selectedFilterService == selectedServices.size() - 1;
        if (check) {
            radioboxStatus->TakeFocus();
        }
        return check;
    });
    radioboxStatus |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab;
        if (check) {
            buttonFilter->TakeFocus();
        }
        return check;
    });

    Component rendererFilter = Renderer(Container::Vertical({
        rendererFilterDate,
        containerFilterServices,
        radioboxStatus,
        Container::Horizontal({
            buttonFilter,
            buttonResetFilter,
        }),
    }), [&] {
        return vbox({
            text("Filter") | center,
            separator(),
            hbox({
                text("Date") | size(WIDTH, EQUAL, 10),
                rendererFilterDate->Render() | size(WIDTH, EQUAL, 10),
            }),
            text("Services"),
            containerFilterServices->Render(),
            text("Status"),
            radioboxStatus->Render(),
            hbox({
                buttonFilter->Render() | size(WIDTH, EQUAL, 10),
                buttonResetFilter->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter
        });
    });

    // History list
    int tabDetail = 0;
    int selectedButtonDetail = 0;

    // Detail appointment
    std::string detailAppointmentID;
    std::string detailAppointmentCustomerName;
    std::string detailAppointmentStatus;
    std::string detailAppointmentDate;
    std::string detailAppointmentTime;
    std::vector<std::string> detailAppointmentServices;
    std::vector<std::string> detailAppointmentStylists;
    std::string detailAppointmentRequirement;

    Components componentHistoryList = {};
    for (int i = 0; i < listHistory.size(); ++i) {
        Component buttonDetail = Button("Detail", [&] {
            tabDetail = 1;
            callGetAppointmentDetailByID(detailAppointmentID, detailAppointmentCustomerName, detailAppointmentStatus, detailAppointmentDate, detailAppointmentTime, detailAppointmentServices, detailAppointmentStylists, detailAppointmentRequirement);
        }, buttonOptionTab);
        componentHistoryList.push_back(buttonDetail);
    }

    Component containerHistoryList = Container::Vertical({});
    for (int i = 0; i < listHistory.size(); ++i) {
        Component c = componentHistoryList[i];
        
        std::string s = listHistory[i];
        std::string id = s.substr(0, s.find(" "));
        s = s.substr(s.find(" ") + 1);
        std::string status = s.substr(0, s.find(" "));
        s = s.substr(s.find(" ") + 1);
        std::string date = s.substr(0, s.find(" "));
        s = s.substr(s.find(" ") + 1);
        std::string time = s.substr(0, s.find(" "));
        s = s.substr(s.find(" ") + 1);
        std::string services = s;
        containerHistoryList->Add(Renderer(
            c, [&, c, id, status, date, time, services] {
            return hbox({
                text(id) | size(WIDTH, EQUAL, 10),
                text(status) | size(WIDTH, EQUAL, 10),
                text(date) | size(WIDTH, EQUAL, 13),
                text(time) | size(WIDTH, EQUAL, 7),
                text(services) | size(WIDTH, EQUAL, 15),
                separator(),
                c->Render() | center | size(WIDTH, EQUAL, 8),
            });
        }));
    }
    Component rendererHistoryList = Renderer(containerHistoryList ,[&] {
        return vbox({
            hbox({
                text("ID") | size(WIDTH, EQUAL, 10),
                text("Status") | size(WIDTH, EQUAL, 10),
                text("Date") | size(WIDTH, EQUAL, 13),
                text("Time") | size(WIDTH, EQUAL, 7),
                text("Services") | size(WIDTH, EQUAL, 15),
            }) | bold,
            containerHistoryList->Render() | vscroll_indicator | frame 
        });
    });

    std::string t = listHistory[selectedButtonDetail];
    detailAppointmentID = t.substr(0, t.find(" "));

    callGetAppointmentDetailByID(detailAppointmentID, detailAppointmentCustomerName, detailAppointmentStatus, detailAppointmentDate, detailAppointmentTime, detailAppointmentServices, detailAppointmentStylists, detailAppointmentRequirement);

    int selectedButtonsDetail = 0;
    Component buttonDetailCancel = Button("Cancel", [&] {
        callCancelAppointment(detailAppointmentID);
        callGetAppointmentDetailByID(detailAppointmentID, detailAppointmentCustomerName,detailAppointmentStatus, detailAppointmentDate, detailAppointmentTime, detailAppointmentServices, detailAppointmentStylists, detailAppointmentRequirement);
    }, buttonOptionNext);
    Component buttonDetailBack = Button("Back", [&] {
        tabDetail = 0;
    }, buttonOptionNext);
    Component rendererTabDetail = Renderer(Container::Horizontal({
        buttonDetailBack,
        buttonDetailCancel
    }, &selectedButtonsAppointment) ,[&] {
        auto textService = [&](int i) {
            return hbox({
                text(detailAppointmentServices[i]) | size(WIDTH, EQUAL, 20),
                text(detailAppointmentStylists[i]) | size(WIDTH, EQUAL, 20),
            });
        };
        Elements textServiceList;
        for (int i = 0; i < detailAppointmentServices.size(); ++i) {
            textServiceList.push_back(textService(i));
        }
        return vbox({
            text("Detail appointment") | center,
            separator(),
            text("Appointment ID") | bold,
            text(detailAppointmentID),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Customer name") | bold,
            text(detailAppointmentCustomerName),
            filler() | size(HEIGHT, EQUAL, 1),
            hbox({
                text("Services") | size(WIDTH, EQUAL, 20),
                text("Stylist") | size(WIDTH, EQUAL, 20),
            }) | bold,
            vbox(textServiceList),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Date and time") | size(WIDTH, EQUAL, 40) | bold,
            text("Date: " + detailAppointmentDate) | size(WIDTH, EQUAL, 40),
            text("Time: " + detailAppointmentTime) | size(WIDTH, EQUAL, 40),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Requirement: ") | size(WIDTH, EQUAL, 40) | bold,
            paragraph(requirement) | size(WIDTH, EQUAL, 40),
            filler(),
            separator(),
            hbox({
                buttonDetailBack->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 5),
                buttonDetailCancel->Render() | size(WIDTH, EQUAL, 10),
            })| hcenter,
        }) | center | borderRounded | size(WIDTH, EQUAL, 40);
    });

    Component containerTabHistory = Container::Tab({
        rendererHistoryList,
        rendererTabDetail
    }, &tabDetail);

    Component rendererTabHistory = Renderer(
        containerTabHistory,
        [&] {
        Element document = rendererHistoryList->Render();
        if (tabDetail == 1) {
            document = dbox({
                rendererHistoryList->Render(),
                rendererTabDetail->Render() | clear_under | center,
            });
        }
        return document;
    });

    Component tabHistory = Renderer(
        Container::Horizontal({
            rendererFilter,
            rendererTabHistory,
        }), [&] {
        return hbox({
            rendererFilter->Render(),
            separator(),
            rendererTabHistory->Render(),
        });
    });    

    #pragma endregion

    // Profile Tab
    #pragma region
    std::string firstname;
    std::string lastname;
    std::string username;
    std::string oldPassword;
    std::string newPassword;
    std::string confirmpassword;
    std::string phonenumber;
    std::string age;
    int gender = 0;

    auto resetName = [&] {
        callGetCurrentUserName(firstname, lastname);
    };
    auto resetPassword = [&] {
        oldPassword = "";
        newPassword = "";
        confirmpassword = "";
    };
    auto resetPersonInfo = [&] {
        callGetCurrentUserPersonInfo(phonenumber, age, gender);
    };
    auto resetProfile = [&] {
        resetName();
        resetPassword();
        resetPersonInfo();
    };
    username = callGetCurrentUserUsername();
    resetProfile();

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
    ComponentDecorator eventAlphabetOnly = CatchEvent([&](Event event) {
        return event.is_character() && 
            !std::isalpha(event.character()[0]) && 
            !std::isspace(event.character()[0]);
    });
    inputFirstname |= eventAlphabetOnly;
    inputLastname  |= eventAlphabetOnly;

    std::vector <std::string> genders = {"Male", "Female"};
    Component radioboxGender = Radiobox(&genders, &gender);

    std::string errorUpdateProfileFirstnameEmpty = "";
    std::string errorUpdateProfileLastnameEmpty  = "";
    Component buttonNameUpdate = Button("Update", [&] {
        errorUpdateProfileFirstnameEmpty = "";
        errorUpdateProfileLastnameEmpty = "";
        try {
            callUpdateCurrentUserName(firstname, lastname);
        }
        catch (int errorCode) {
            switch (errorCode)
            {
                case ERROR_CODE::UPDATE_PROFILE_NAME_EMPTY:
                    errorUpdateProfileFirstnameEmpty = "First name is empty";
                    errorUpdateProfileLastnameEmpty = "Last name is empty";
                    break;
                case ERROR_CODE::UPDATE_PROFILE_FIRSTNAME_EMPTY:
                    errorUpdateProfileFirstnameEmpty = "First name is empty";
                    break;
                case ERROR_CODE::UPDATE_PROFILE_LASTNAME_EMPTY:
                    errorUpdateProfileLastnameEmpty = "Last name is empty";
                    break;
                default:
                    break;
            }
        }
    }, buttonOptionTab);
    Component buttonNameReset = Button("Reset", [&] {
        errorUpdateProfileFirstnameEmpty = "";
        errorUpdateProfileLastnameEmpty = "";
        resetName();
    }, buttonOptionTab);

    std::string errorUpdateProfilePassword = "";
    Component buttonPasswordUpdate = Button("Update", [&] {
        errorUpdateProfilePassword = "";
        try {
            callUpdateCurrentPassword(oldPassword, newPassword, confirmpassword);
        }
        catch (int errorCode) {
            switch (errorCode)
            {
                case ERROR_CODE::UPDATE_PROFILE_INVALID_PASSWORD:
                    errorUpdateProfilePassword = "Invalid old password";
                    break;
                case ERROR_CODE::UPDATE_PROFILE_PASSWORD_NOT_MATCH:
                    errorUpdateProfilePassword = "Password not match";
                    break;
                default:
                    break;
            }
        }
    }, buttonOptionTab);
    Component buttonPasswordReset = Button("Reset", [&] {
        errorUpdateProfilePassword = "";
        resetPassword();
    }, buttonOptionTab);

    std::string errorUpdateProfilePersoninfoEmpty = "";
    Component buttonPersoninfoUpdate = Button("Update", [&] {
        errorUpdateProfilePersoninfoEmpty = "";
        try {
            callUpdateCurrentPersonInfo(phonenumber, age, gender);
        }
        catch (int errorCode) {
            switch (errorCode)
            {
                case ERROR_CODE::UPDATE_PROFILE_PERSONINFO_EMPTY:
                    errorUpdateProfilePersoninfoEmpty = "Phone number or age is empty";
                    break;
                default:
                    break;
            }
        }
    }, buttonOptionTab);
    
    Component buttonPersoninfoReset = Button("Reset", [&] {
        errorUpdateProfilePersoninfoEmpty = "";
        resetPersonInfo();
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

    auto textError = [&](std::string error) {
        if (error.size() == 0) {
            return text("") | size(HEIGHT, EQUAL, 0);
        }
        return paragraph(error) | align_right | color(Color::Red);
    };
    Component containerUpdateName = Renderer(Container::Vertical({
        inputFirstname,
        inputLastname,
        containerButtonsName
    }), [&] {

        return vbox({
            hbox({text("First Name      : "), inputFirstname->Render()}),
            textError(errorUpdateProfileFirstnameEmpty),
            hbox({text("Last Name       : "), inputLastname->Render()}),
            textError(errorUpdateProfileLastnameEmpty),
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
            textError(errorUpdateProfilePassword),
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
            textError(errorUpdateProfilePersoninfoEmpty),
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
            text("Username") | bold,
            text(username),
            text("Name") | bold,
            containerUpdateName->Render(),
            text("Password") | bold,
            containerUpdatePassword->Render(),
            text("Personal Information") | bold,
            containerUpdatePersoninfo->Render(),
        }) | size(WIDTH, EQUAL, PROFILE_ALIGN_WIDTH) | center;
    });
    #pragma endregion   

    // All
    #pragma region 
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
    #pragma endregion
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

    #pragma region
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
    Component buttonAppointment = Button("Appointment", [&] {
        selectedTab = 2;
    }, buttonOptionTab);
    Component buttonCustomer = Button("Customer", [&] {
        selectedTab = 3;
    }, buttonOptionTab);
    Component buttonStylist = Button("Stylist", [&] {
        selectedTab = 4;
    }, buttonOptionTab);

    Component buttonProfile = Button("Profile", [&] {
        selectedTab = 5;
    }, buttonOptionTab);
    Component buttonLogout = Button("Logout", [&] {
        exit = 1;
    }, buttonOptionTab);
    Component containerButtons = Container::Vertical({
        buttonHome,
        buttonStatistics,
        buttonAppointment,
        buttonCustomer,
        buttonStylist,
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
            buttonAppointment->Render(),
            separator(),
            buttonCustomer->Render(),
            separator(),
            buttonStylist->Render(),
            separator(),
            buttonProfile->Render(),
            separator(),
            filler(),
            separator(),
            buttonLogout->Render()
        }) | borderRounded | size(WIDTH, EQUAL, 30);
    });
    #pragma endregion

    // Home tab
    Component tabHome = Renderer([&] {
        return text("Home") | center ;
    });

    // Statistics tab
    Component tabStatistics = Renderer([&] {
        return text("Statistics") | center;
    });

    // Appointment tab
    #pragma region

    std::vector<std::string> listAppointmentHistory;
    int countAppointmentHistory = 0;
    // Filter
    int filterDay;
    int filterMonth;
    int filterYear;
    int filterHour;
    int filterMinute;
    std::array<bool, SERVICES_COUNT> chosenFilterServices;
    int status;
    std::string customerID;
    std::vector<std::string> filterDays = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    std::vector<std::string> filterMonths = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"};
    std::vector<std::string> filterYears = {"----", "2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029", "2030"};
    std::vector<std::string> filterHours = {"--", "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"};
    std::vector<std::string> filterMinutes = {"--", "00", "15", "30", "45"};
    std::vector<std::string> filterStatus = {"All", "Done", "Pending", "Cancel"};
    std::vector<std::string> listServices = {
        "Cat toc",
        "Nhuom toc",
        "Tay toc",
        "Uon toc",
        "Phuc hoi toc",
        "Tao kieu",
        "Goi dau"
    };
    auto resetFilter = [&] {
        filterDay = 0;
        filterMonth = 0;
        filterYear = 0;
        filterHour = 0;
        filterMinute = 0;
        status = 0;
        customerID = "";
        std::fill(chosenFilterServices.begin(), chosenFilterServices.end(), true);
    };

    resetFilter();
    MenuOption menuOptionAll;
    menuOptionAll.direction = Direction::Up;
    menuOptionAll.entries_option.transform = [](const EntryState& s) {
        auto element = text(s.label) | center;
        if (s.focused) {
            element = element | inverted;
        }
        return element;
    };

    Component menuFilterDay = Menu(&filterDays, &filterDay, menuOptionAll);
    MenuOption menuOptionMonth = MenuOption(menuOptionAll);
    menuOptionMonth.on_change = ([&] () {
        int curYear = 2020 + filterYear;
        int curMonth = filterMonth;
        int curDay = filterDay;
        bool isLeapYear = (curYear % 4 == 0 && curYear % 100 != 0) || (curYear % 400 == 0);
        int maxDay;
        if (filterYear == 0) {
            isLeapYear = true;
        }
        if (curMonth == 2) {
            maxDay = isLeapYear ? 29 : 28;
        }
        else if (curMonth == 4 || curMonth == 6 || curMonth == 9 || curMonth == 11) {
            maxDay = 30;
        }
        else {
            maxDay = 31;
        }
        if (curDay > maxDay) {
            filterDay = maxDay;
        }
        filterDays.clear();
        filterDays.push_back("--");
        for (int i = 1; i <= maxDay; ++i) {
            filterDays.push_back(std::to_string(i).size() == 1 ? "0" + std::to_string(i) : std::to_string(i));
        }
    });

    Component menuFilterMonth = Menu(&filterMonths, &filterMonth, menuOptionMonth);
    Component menuFilterYear = Menu(&filterYears, &filterYear, menuOptionAll);
    Component menuFilterHour = Menu(&filterHours, &filterHour, menuOptionAll);
    Component menuFilterMinute = Menu(&filterMinutes, &filterMinute, menuOptionAll);

    Component rendererFilterDate = Renderer(Container::Horizontal({
        menuFilterDay,
        menuFilterMonth,
        menuFilterYear,
    }), [&] {
        return hbox({
            menuFilterDay->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menuFilterMonth->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menuFilterYear->Render() | size(WIDTH, EQUAL, 4) | frame | size(HEIGHT, EQUAL, 1),
        });
    });

    Component rendererFilterTime = Renderer(
        Container::Horizontal({
            menuFilterHour,
            menuFilterMinute,
        }), [&] {
        return hbox({
            menuFilterHour->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text(":") | size(WIDTH, EQUAL, 1),
            menuFilterMinute->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
        });
    });

    int selectedFilterService = 0;
    Component containerFilterServices = Container::Vertical({}, &selectedFilterService);
    for (int i = 0; i < SERVICES_COUNT; ++i) {
        chosenFilterServices[i] = true;
        containerFilterServices->Add(Checkbox(listServices[i], &chosenFilterServices[i]) | size(WIDTH, EQUAL, 20));
    }

    Component radioboxStatus = Radiobox(filterStatus, &status);
    InputOption inputOptionStatus = InputOption::Default();
    inputOptionStatus.multiline = false;
    Component inputCustomerID = Input(&customerID, "Customer ID", inputOptionStatus);

    ButtonOption buttonOptionAll;
    buttonOptionAll.transform = [](const EntryState& s) {
        auto element = text(s.label) | center | borderRounded;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    Component buttonFilter = Button("Filter", [&] {
        listAppointmentHistory = callGetApointmentHistoryList(filterDay, filterMonth, filterYear, filterHour, filterMinute, chosenFilterServices, status, customerID, countAppointmentHistory);
    }, buttonOptionAll);
    Component buttonResetFilter = Button("Reset", [&] {
        resetFilter();
    }, buttonOptionAll);

    int childAppointmentFilter = 0;
    Component containerAppointmentFilter = Container::Vertical({
        rendererFilterDate,
        rendererFilterTime,
        containerFilterServices,
        radioboxStatus,
        inputCustomerID,
        Container::Horizontal({
            buttonFilter,
            buttonResetFilter,
        }),
    }, &childAppointmentFilter);

    containerAppointmentFilter |= CatchEvent([&] (Event event) {
        bool check = (event == Event::Tab) && (childAppointmentFilter < 5); // 6 is the number of small component in containerAppointmentFilter
        if (check) {
            childAppointmentFilter++;
        }
        return check;
    });
    containerAppointmentFilter |= CatchEvent([&] (Event event) {
        bool check = (event == Event::TabReverse) && (childAppointmentFilter > 0);
        if (check) {
            childAppointmentFilter--;
        }
        return check;
    });

    Component rendererAppointmentFilter = Renderer(containerAppointmentFilter, [&] {
        return vbox({
            text("Filter") | center,
            separator(),
            hbox({
                text("Date") | size(WIDTH, EQUAL, 10),
                rendererFilterDate->Render() | size(WIDTH, EQUAL, 10),
            }),
            hbox({
                text("Time") | size(WIDTH, EQUAL, 10),
                rendererFilterTime->Render() | size(WIDTH, EQUAL, 10),
            }),
            text("Services"),
            containerFilterServices->Render(),
            text("Status"),
            radioboxStatus->Render(),
            hbox({
                text("Customer ID") | size(WIDTH, EQUAL, 12),
                inputCustomerID->Render() | size(WIDTH, EQUAL, 12),
            }),
            filler(),
            hbox({
                buttonFilter->Render() | size(WIDTH, EQUAL, 10),
                buttonResetFilter->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter
        });
    });

    // HistoryList
    std::string detailAppointmentID;
    std::string detailAppointmentCustomerName;
    std::string detailAppointmentStatus;
    std::string detailAppointmentDate;
    std::string detailAppointmentTime;
    std::vector<std::string> detailAppointmentServices;
    std::vector<std::string> detailAppointmentStylists;
    std::string detailAppointmentRequirement;
    int tabDetail = 0;

    listAppointmentHistory = callGetApointmentHistoryList(filterDay, filterMonth, filterYear, filterHour, filterMinute, chosenFilterServices, status, customerID, countAppointmentHistory);

    Component containerAppointmentHistoryList = Container::Vertical({});
    for (int i = 0; i < listAppointmentHistory.size(); ++i) {
        std::string sid = listAppointmentHistory[i].substr(0, listAppointmentHistory[i].find(" "));
        Component c = Button("Detail", [&, sid] {
            tabDetail = 1;
            detailAppointmentID = sid;
            callGetAppointmentDetailByID(detailAppointmentID, detailAppointmentCustomerName, detailAppointmentStatus, detailAppointmentDate, detailAppointmentTime, detailAppointmentServices, detailAppointmentStylists, detailAppointmentRequirement);
        }, buttonOptionTab);

        std::string s = listAppointmentHistory[i];
        std::string id = s.substr(0, s.find(" "));
        s = s.substr(s.find(" ") + 1);
        std::string status = s.substr(0, s.find(" "));
        s = s.substr(s.find(" ") + 1);
        std::string date = s.substr(0, s.find(" "));
        s = s.substr(s.find(" ") + 1);
        std::string time = s.substr(0, s.find(" "));
        s = s.substr(s.find(" ") + 1);
        std::string services = s;
        containerAppointmentHistoryList->Add(Renderer(
            c, [&, c, id, status, date, time, services] {
            return hbox({
                text(id) | size(WIDTH, EQUAL, 10),
                text(status) | size(WIDTH, EQUAL, 10),
                text(date) | size(WIDTH, EQUAL, 13),
                text(time) | size(WIDTH, EQUAL, 7),
                text(services) | size(WIDTH, EQUAL, 15),
                separator(),
                c->Render() | center | size(WIDTH, EQUAL, 8),
            });
        }));
    }

    Component rendererAppointmentHistoryList = Renderer(containerAppointmentHistoryList ,[&] {
        return vbox({
            hbox({
                text("ID") | size(WIDTH, EQUAL, 10),
                text("Status") | size(WIDTH, EQUAL, 10),
                text("Date") | size(WIDTH, EQUAL, 13),
                text("Time") | size(WIDTH, EQUAL, 7),
                text("Services") | size(WIDTH, EQUAL, 15),
            }) | bold,
            separator(),
            containerAppointmentHistoryList->Render() | vscroll_indicator | frame,
            separator(),
            filler(),
            separator(),
            text("Total: " + std::to_string(countAppointmentHistory)) | size(WIDTH, EQUAL, 20) | align_right,
        });
    });

    // Detail appointment
    Component buttonAppointmentDetailCancel = Button("Cancel", [&] {
        callCancelAppointment(detailAppointmentID);
        callGetAppointmentDetailByID(detailAppointmentID, detailAppointmentCustomerName,detailAppointmentStatus, detailAppointmentDate, detailAppointmentTime, detailAppointmentServices, detailAppointmentStylists, detailAppointmentRequirement);
    }, buttonOptionAll);
    Component buttonAppointmentDetailBack = Button("Back", [&] {
        tabDetail = 0;
    }, buttonOptionAll);
    int selectedButtonsAppointment = 0;
    Component rendererTabDetail = Renderer(Container::Horizontal({
        buttonAppointmentDetailBack,
        buttonAppointmentDetailCancel
    }, &selectedButtonsAppointment) ,[&] {
        auto textService = [&](int i) {
            return hbox({
                text(detailAppointmentServices[i]) | size(WIDTH, EQUAL, 20),
                text(detailAppointmentStylists[i]) | size(WIDTH, EQUAL, 20),
            });
        };
        Elements textServiceList;
        for (int i = 0; i < detailAppointmentServices.size(); ++i) {
            textServiceList.push_back(textService(i));
        }
        return vbox({
            text("Detail appointment") | center,
            separator(),
            text("Appointment ID") | bold,
            text(detailAppointmentID),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Customer name") | bold,
            text(detailAppointmentCustomerName),
            filler() | size(HEIGHT, EQUAL, 1),
            hbox({
                text("Services") | size(WIDTH, EQUAL, 20),
                text("Stylist") | size(WIDTH, EQUAL, 20),
            }) | bold,
            vbox(textServiceList),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Date and time") | size(WIDTH, EQUAL, 40) | bold,
            text("Date: " + detailAppointmentDate) | size(WIDTH, EQUAL, 40),
            text("Time: " + detailAppointmentTime) | size(WIDTH, EQUAL, 40),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Requirement: ") | size(WIDTH, EQUAL, 40) | bold,
            paragraph(detailAppointmentRequirement) | size(WIDTH, EQUAL, 40),
            filler(),
            separator(),
            hbox({
                buttonAppointmentDetailBack->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 5),
                buttonAppointmentDetailCancel->Render() | size(WIDTH, EQUAL, 10),
            })| hcenter,
        }) | center | borderRounded | size(WIDTH, EQUAL, 40);
    });

    Component rendererAppointmentHistoryAll = Renderer(Container::Tab({
        rendererAppointmentHistoryList,
        rendererTabDetail
    }, &tabDetail),
    [&] {
        auto document = rendererAppointmentHistoryList->Render();
        if (tabDetail == 1) {
            document = dbox({
                rendererAppointmentHistoryList->Render(),
                rendererTabDetail->Render() | clear_under | center,
            });
        }
        return document;
    });

    Component tabAppointment = Renderer(
        Container::Horizontal({
            rendererAppointmentFilter,
            rendererAppointmentHistoryAll,
        }), [&] {
        return hbox({
            rendererAppointmentFilter->Render(),
            separator(),
            rendererAppointmentHistoryAll->Render(),
        });
    });

    #pragma endregion

    // Customer tab
    #pragma region Customer
    Component tabCustomer = Renderer([&] {
        return text("Customer") | center;
    });
    #pragma endregion

    // Stylist tab
    #pragma region Stylist

    std::vector<std::string> stylistList;
    int countStylistList = 0;
    // Stylist filter
    std::array<bool, SERVICES_COUNT> stylistFilterServices;
    int stylistFilterGender = 0; // 0: Male, 1: Female
    std::string stylistFilterName;
    std::string stylistFilterAge;
    std::vector<std::string> listGenders = {"Male", "Female"};

    auto resetStylistFilter = [&] {
        std::fill(stylistFilterServices.begin(), stylistFilterServices.end(), true);
        stylistFilterGender = 0;
        stylistFilterName = "";
        stylistFilterAge = "";
    };
    resetStylistFilter();

    Component containerStylistFilterServices = Container::Vertical({});
    for (int i = 0; i < SERVICES_COUNT; ++i) {
        containerStylistFilterServices->Add(Checkbox(listServices[i], &stylistFilterServices[i]) | size(WIDTH, EQUAL, 20));
    }
    Component radioboxStylistGender = Radiobox(listGenders, &stylistFilterGender);

    InputOption inputOptionStylistAll = InputOption::Default();
    inputOptionStylistAll.multiline = false;
    Component inputStylistAge = Input(&stylistFilterAge, "Age", inputOptionStylistAll);
    Component inputStylistName = Input(&stylistFilterName, "Name", inputOptionStylistAll);

    Component buttonStylistFilter = Button("Filter", [&] {
        stylistList = callGetStylistList(stylistFilterServices, stylistFilterGender, stylistFilterName, stylistFilterAge, countStylistList);
    }, buttonOptionAll);
    Component buttonStylistResetFilter = Button("Reset", [&] {
        resetStylistFilter();
    }, buttonOptionAll);

    int childStylistFilter = 0;
    Component containerStylistFilter = Container::Vertical({
        containerStylistFilterServices,
        radioboxStylistGender,
        inputStylistAge,
        inputStylistName,
        Container::Horizontal({
            buttonStylistFilter,
            buttonStylistResetFilter,
        }),
    }, &childStylistFilter);

    containerStylistFilter |= CatchEvent([&] (Event event) {
        bool check = (event == Event::Tab) && (childStylistFilter < 4); // 5 is the number of small component in containerStylistFilter
        if (check) {
            childStylistFilter++;
        }
        return check;
    });
    containerStylistFilter |= CatchEvent([&] (Event event) {
        bool check = (event == Event::TabReverse) && (childStylistFilter > 0);
        if (check) {
            childStylistFilter--;
        }
        return check;
    });

    Component rendererStylistFilter = Renderer(containerStylistFilter, [&] {
        return vbox({
            text("Filter") | center,
            separator(),
            text("Services"),
            containerStylistFilterServices->Render(),
            text("Gender"),
            radioboxStylistGender->Render(),
            hbox({
                text("Age : "),
                inputStylistAge->Render(),
            }),
            hbox({
                text("Name: "),
                inputStylistName->Render(),
            }),
            filler(),
            hbox({
                buttonStylistFilter->Render() | size(WIDTH, EQUAL, 10),
                buttonStylistResetFilter->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter
        }) | size(WIDTH, EQUAL, 20);
    });

    // tab in Stylist list
    int selectedTabStylist = 0; // 0: Stylist list, 1: Stylist detail 2: Update stylist 3: Add stylist
    // Stylist list
    std::string detailStylistID;
    std::string detailStylistName;
    std::string detailStylistGender;
    std::string detailStylistAge;
    std::string detailStylistPhonenumber;
    std::vector<std::string> detailStylistServices;
    std::string detailStylistUsername;
    std::string detailStylistPassword;

    stylistList = callGetStylistList(stylistFilterServices, stylistFilterGender, stylistFilterName, stylistFilterAge, countStylistList);
    Component containerStylistList = Container::Vertical({});
    for (int i = 0; i < stylistList.size(); ++i) {
        std::string sid = stylistList[i].substr(0, stylistList[i].find(","));
        Component c = Button("Detail", [&, sid] {
            selectedTabStylist = 1;
            detailStylistID = sid;
            callGetStylistDetailByID(detailStylistID, detailStylistName, detailStylistGender, detailStylistAge, detailStylistPhonenumber, detailStylistServices, detailStylistUsername, detailStylistPassword);
        }, buttonOptionTab);

        std::string s = stylistList[i];
        std::string id = s.substr(0, s.find(","));
        s = s.substr(s.find(",") + 1);
        std::string name = s.substr(0, s.find(","));
        s = s.substr(s.find(",") + 1);
        std::string gender = s.substr(0, s.find(","));
        s = s.substr(s.find(",") + 1);
        std::string age = s.substr(0, s.find(","));
        s = s.substr(s.find(",") + 1);
        std::string phonenumber = s;
        containerStylistList->Add(Renderer(
            c, [&, c, id, name, gender, age, phonenumber] {
            return hbox({
                text(id) | size(WIDTH, EQUAL, 7),
                text(name) | size(WIDTH, EQUAL, 25),
                text(gender) | size(WIDTH, EQUAL, 8),
                text(age) | size(WIDTH, EQUAL, 5),
                text(phonenumber) | size(WIDTH, EQUAL, 12),
                separator(),
                c->Render() | center | size(WIDTH, EQUAL, 8),
            });
        }));
    }

    Component buttonAddStylist = Button("Add Stylist", [&] {
        selectedTabStylist = 3;
    }, buttonOptionAll);

    containerStylistList |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab;
        if (check) {
            buttonAddStylist->TakeFocus();
        }
        return check;
    });

    Component rendererStylistList = Renderer(
        Container::Vertical({
            containerStylistList,
            buttonAddStylist,
        }), [&] {
        return vbox({
            hbox({
                text("ID") | size(WIDTH, EQUAL, 7),
                text("Name") | size(WIDTH, EQUAL, 25),
                text("Gender") | size(WIDTH, EQUAL, 8),
                text("Age") | size(WIDTH, EQUAL, 5),
                text("Phonenumber") | size(WIDTH, EQUAL, 12),
            }) | bold,
            separator(),
            containerStylistList->Render() | vscroll_indicator | frame,
            separator(),
            filler(),
            separator(),
            hbox({
                text("Total: " + std::to_string(countStylistList)),
                filler(),
                buttonAddStylist->Render() | size(WIDTH, EQUAL, 15),
            })
        });
    });

    // Detail Stylist
    std::string stylistUpdateFirstName;
    std::string stylistUpdateLastName;
    int stylistUpdateGender;
    std::string stylistUpdateAge;
    std::string stylistUpdatePhonenumber;
    std::string stylistUpdateUsername;
    std::string stylistUpdatePassword;
    std::array<bool, SERVICES_COUNT> stylistUpdateServices;

    auto resetUpdateStylist = [&] {
        breakName(detailStylistName, stylistUpdateFirstName, stylistUpdateLastName);
        stylistUpdateGender = detailStylistGender == "Male"? 0 : 1;
        stylistUpdateAge = detailStylistAge;
        stylistUpdatePhonenumber = detailStylistPhonenumber;
        stylistUpdateUsername = detailStylistUsername;
        stylistUpdatePassword = detailStylistPassword;
        for (int i = 0; i < SERVICES_COUNT; ++i) {
            if (std::find(detailStylistServices.begin(), detailStylistServices.end(), listServices[i]) != detailStylistServices.end()) {
                stylistUpdateServices[i] = true;
            }
            else
                stylistUpdateServices[i] = false;
        }
    };

    Component buttonStylistDetailBack = Button("Back", [&] {
        selectedTabStylist = 0;
    }, buttonOptionAll);
    Component buttonStylistDetailModify = Button("Update", [&] {
        resetUpdateStylist();
        selectedTabStylist = 2;
    }, buttonOptionAll);
    Component buttonStylistDetailDelete = Button("Delete", [&] {
        callDeleteStylist(detailStylistID);
        stylistList = callGetStylistList(stylistFilterServices, stylistFilterGender, stylistFilterName, stylistFilterAge, countStylistList);
        selectedTabStylist = 0;
    }, buttonOptionAll);

    int selectedButtonsStylistDetail = 0;
    Component rendererTabStylistDetail = Renderer(Container::Horizontal({
        buttonStylistDetailBack,
        buttonStylistDetailModify,
        buttonStylistDetailDelete,
    }, &selectedButtonsStylistDetail) ,[&] {
        auto textService = [&](int i) {
            return vbox({
                text("    " + detailStylistServices[i]) | size(WIDTH, EQUAL, 20),
            });
        };
        Elements textServiceList;
        for (int i = 0; i < detailStylistServices.size(); ++i) {
            textServiceList.push_back(textService(i));
        }
        return vbox({
            text("Detail stylist") | center,
            separator(),
            text("Stylist ID") | bold,
            text("    " + detailStylistID),
            text("Name") | bold,
            text("    " + detailStylistName),
            text("Services") | bold,
            vbox(textServiceList),
            text("Gender") | bold,
            text("    " + detailStylistGender),
            text("Age") | bold,
            text("    " + detailStylistAge),
            text("Phone Number") | bold,
            text("    " + detailStylistPhonenumber),
            text("Username") | bold,
            text("    " + detailStylistUsername),
            text("Password") | bold,
            text("    " + detailStylistPassword),
            filler(),
            separator(),
            hbox({
                buttonStylistDetailBack->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 4),
                buttonStylistDetailModify->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 4),
                buttonStylistDetailDelete->Render() | size(WIDTH, EQUAL, 10),
            })| hcenter,
        }) | center | borderRounded | size(WIDTH, EQUAL, 40);
    });

    // Update Stylist

    Component inputUpdateStylistFirstName = Input(&stylistUpdateFirstName, "First Name", inputOptionStylistAll);
    Component inputUpdateStylistLastName = Input(&stylistUpdateLastName, "Last Name", inputOptionStylistAll);
    Component radioboxUpdateStylistGender = Radiobox(listGenders, &stylistUpdateGender);
    Component inputUpdateStylistAge = Input(&stylistUpdateAge, "Age", inputOptionStylistAll);
    Component inputUpdateStylistPhonenumber = Input(&stylistUpdatePhonenumber, "Phone Number", inputOptionStylistAll);
    Component inputUpdateStylistUsername = Input(&stylistUpdateUsername, "Username", inputOptionStylistAll);
    Component inputUpdateStylistPassword = Input(&stylistUpdatePassword, "Password", inputOptionStylistAll);

    Component containerUpdateStylistServices = Container::Vertical({});
    for (int i = 0; i < SERVICES_COUNT; ++i) {
        containerUpdateStylistServices->Add(Checkbox(listServices[i], &stylistUpdateServices[i]) | size(WIDTH, EQUAL, 20));
    }

    int selectedButtonsUpdateStylist = 0;
    Component buttonUpdateStylistBack = Button("Back", [&] {
        selectedTabStylist = 1;
    }, buttonOptionAll);
    Component buttonUpdateStylistReset = Button("Reset", [&] {
        resetUpdateStylist();
    }, buttonOptionAll);
    Component buttonUpdateStylist = Button("Update", [&] {
        callUpdateStylist(detailStylistID, stylistUpdateFirstName, stylistUpdateLastName, stylistUpdateGender, stylistUpdateAge, stylistUpdatePhonenumber, stylistUpdateUsername, stylistUpdatePassword, stylistUpdateServices);
        callGetStylistDetailByID(detailStylistID, detailStylistName, detailStylistGender, detailStylistAge, detailStylistPhonenumber, detailStylistServices, detailStylistUsername, detailStylistPassword);
        selectedTabStylist = 1;
    }, buttonOptionAll);

    int selectedUpdateStylist = 0;
    Component containerUpdateStylist = Container::Vertical({
        inputUpdateStylistFirstName,
        inputUpdateStylistLastName,
        containerUpdateStylistServices,
        radioboxUpdateStylistGender,
        inputUpdateStylistAge,
        inputUpdateStylistPhonenumber,
        inputUpdateStylistUsername,
        inputUpdateStylistPassword,
        Container::Horizontal({
            buttonUpdateStylist,
            buttonUpdateStylistReset,
            buttonUpdateStylistBack,
        }),
    }, &selectedUpdateStylist);

    containerUpdateStylist |= CatchEvent([&] (Event event) {
        bool check = (event == Event::Tab) && (selectedUpdateStylist < 8); // 9 is the number of small component in containerUpdateStylist
        if (check) {
            selectedUpdateStylist++;
        }
        return check;
    });

    containerUpdateStylist |= CatchEvent([&] (Event event) {
        bool check = (event == Event::TabReverse) && (selectedUpdateStylist > 0);
        if (check) {
            selectedUpdateStylist--;
        }
        return check;
    });

    Component rendererUpdateStylist = Renderer(containerUpdateStylist, [&] {
        return vbox({
            text("Update stylist") | center,
            separator(),
            hbox({
                text("First Name: "),
                inputUpdateStylistFirstName->Render() | size(WIDTH, EQUAL, 20),
            }),
            hbox({
                text("Last Name: "),
                inputUpdateStylistLastName->Render() | size(WIDTH, EQUAL, 20),
            }),
            text("Services"),
            containerUpdateStylistServices->Render(),
            text("Gender"),
            radioboxUpdateStylistGender->Render(),
            hbox({
                text("Age: "),
                inputUpdateStylistAge->Render(),
            }),
            hbox({
                text("Phone Number: "),
                inputUpdateStylistPhonenumber->Render(),
            }),
            hbox({
                text("Username: "),
                inputUpdateStylistUsername->Render(),
            }),
            hbox({
                text("Password: "),
                inputUpdateStylistPassword->Render(),
            }),
            filler(),
            hbox({
                buttonUpdateStylist->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 5),
                buttonUpdateStylistReset->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 5),
                buttonUpdateStylistBack->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter,
        }) | size(WIDTH, EQUAL, 40) | borderRounded;
    });

    // Add Stylist
    std::string stylistAddFirstName;
    std::string stylistAddLastName;
    int stylistAddGender;
    std::string stylistAddAge;
    std::string stylistAddPhonenumber;
    std::string stylistAddUsername;
    std::string stylistAddPassword;
    std::array<bool, SERVICES_COUNT> stylistAddServices;

    auto resetAddStylist = [&] {
        stylistAddFirstName = "";
        stylistAddLastName = "";
        stylistAddGender = 0;
        stylistAddAge = "";
        stylistAddPhonenumber = "";
        stylistAddUsername = "";
        stylistAddPassword = "";
        std::fill(stylistAddServices.begin(), stylistAddServices.end(), false);
    };
    resetAddStylist();

    Component inputAddStylistFirstName = Input(&stylistAddFirstName, "First Name", inputOptionStylistAll);
    Component inputAddStylistLastName = Input(&stylistAddLastName, "Last Name", inputOptionStylistAll);
    Component radioboxAddStylistGender = Radiobox(listGenders, &stylistAddGender);
    Component inputAddStylistAge = Input(&stylistAddAge, "Age", inputOptionStylistAll);
    Component inputAddStylistPhonenumber = Input(&stylistAddPhonenumber, "Phone Number", inputOptionStylistAll);
    Component inputAddStylistUsername = Input(&stylistAddUsername, "Username", inputOptionStylistAll);
    Component inputAddStylistPassword = Input(&stylistAddPassword, "Password", inputOptionStylistAll);

    Component containerAddStylistServices = Container::Vertical({});
    for (int i = 0; i < SERVICES_COUNT; ++i) {
        containerAddStylistServices->Add(Checkbox(listServices[i], &stylistAddServices[i]));
    }

    Component buttonAddStylistBack = Button("Back", [&] {
        selectedTabStylist = 0;
    }, buttonOptionAll);
    Component buttonAddStylistReset = Button("Reset", [&] {
        resetAddStylist();
    }, buttonOptionAll);
    Component buttonAddStylistAdd = Button("Add", [&] {
        callAddStylist(stylistAddFirstName, stylistAddLastName, stylistAddGender, stylistAddAge, stylistAddPhonenumber, stylistAddUsername, stylistAddPassword, stylistAddServices);
        stylistList = callGetStylistList(stylistFilterServices, stylistFilterGender, stylistFilterName, stylistFilterAge, countStylistList);
        selectedTabStylist = 0;
    }, buttonOptionAll);

    int selectedAddStylist = 0;
    Component containerAddStylist = Container::Vertical({
        inputAddStylistFirstName,
        inputAddStylistLastName,
        containerAddStylistServices,
        radioboxAddStylistGender,
        inputAddStylistAge,
        inputAddStylistPhonenumber,
        inputAddStylistUsername,
        inputAddStylistPassword,
        Container::Horizontal({
            buttonAddStylistBack,
            buttonAddStylistReset,
            buttonAddStylistAdd,
        }),
    }, &selectedAddStylist);

    containerAddStylist |= CatchEvent([&] (Event event) {
        bool check = (event == Event::Tab) && (selectedAddStylist < 8); // 9 is the number of small component in containerAddStylist
        if (check) {
            selectedAddStylist++;
        }
        return check;
    });

    containerAddStylist |= CatchEvent([&] (Event event) {
        bool check = (event == Event::TabReverse) && (selectedAddStylist > 0);
        if (check) {
            selectedAddStylist--;
        }
        return check;
    });

    Component rendererAddStylist = Renderer(containerAddStylist, [&] {
        return vbox({
            text("Add stylist") | center,
            separator(),
            hbox({
                text("First Name: "),
                inputAddStylistFirstName->Render(),
            }),
            hbox({
                text("Last Name: "),
                inputAddStylistLastName->Render(),
            }),
            text("Services"),
            containerAddStylistServices->Render(),
            text("Gender"),
            radioboxAddStylistGender->Render(),
            hbox({
                text("Age: "),
                inputAddStylistAge->Render(),
            }),
            hbox({
                text("Phone Number: "),
                inputAddStylistPhonenumber->Render(),
            }),
            hbox({
                text("Username: "),
                inputAddStylistUsername->Render(),
            }),
            hbox({
                text("Password: "),
                inputAddStylistPassword->Render(),
            }),
            filler(),
            hbox({
                buttonAddStylistAdd->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 5),
                buttonAddStylistReset->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 5),
                buttonAddStylistBack->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter,
        }) | size(WIDTH, EQUAL, 40) | borderRounded;
    });

    Component rendererTabStylist = Renderer(Container::Tab({
        rendererStylistList,
        rendererTabStylistDetail,
        rendererUpdateStylist,
        rendererAddStylist,
    }, &selectedTabStylist), [&] {
        auto document = rendererStylistList->Render();
        if (selectedTabStylist == 1) {
            document = dbox({
                rendererStylistList->Render(),
                rendererTabStylistDetail->Render() | clear_under | center,
            });
        }
        else if (selectedTabStylist == 2) {
            document = dbox({
                rendererStylistList->Render(),
                rendererUpdateStylist->Render() | clear_under | center,
            });
        }
        else if (selectedTabStylist == 3) {
            document = dbox({
                rendererStylistList->Render(),
                rendererAddStylist->Render() | clear_under | center,
            });
        }
        return document;
    });

    Component tabStylist = Renderer(
        Container::Horizontal({
            rendererStylistFilter,
            rendererTabStylist,
        }), [&] {
        return hbox({
            rendererStylistFilter->Render(),
            separator(),
            rendererTabStylist->Render(),
        });
    });
    #pragma endregion

    // Profile tab
    Component tabProfile = Renderer([&] {
        return text("Profile") | center;
    });


    Component containerTabs = Container::Tab({
        tabHome,
        tabStatistics,
        tabAppointment,
        tabCustomer,
        tabStylist,
        tabProfile,
    }, &selectedTab) | borderRounded | size(WIDTH, EQUAL, 90);

    // All
    Component containerAll = Container::Horizontal({
        rendererButtons,
        containerTabs,
    });

    #pragma region
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
    #pragma endregion

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
