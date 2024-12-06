#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <functional>
#include <cmath>
#include <Datetime.h>
#include <ftxui/dom/table.hpp>
#include "ui.h"

using namespace ftxui;

std::string trash;
ComponentDecorator getComponentDecorator(std::string label, std::string& value = trash)
{
    if (label == "phonenumber") {
        return CatchEvent([&](Event event) {
            bool check = event.is_character() &&
                        (!std::isdigit(event.character()[0]) ||
                        (value.size() > 10));
            return check;
        });
    }
    if (label == "username") {
        return CatchEvent([&](Event event) {
            bool check = event.is_character() &&
                        !std::isalnum(event.character()[0]);
            return check;
        });
    }
    if (label == "password") {
        return CatchEvent([&](Event event) {
            bool check = std::isspace(event.character()[0]);
            return check;
        });
    }
    if (label == "name") {
        return CatchEvent([&](Event event) {
            bool check = event.is_character() &&
                        !std::isalpha(event.character()[0]) &&
                        !std::isspace(event.character()[0]);
            return check;
        });
    }
    return CatchEvent([](Event event) {
        return false;
    });
}

void screenWelcome()
{
    // welcome screen
    auto screen = ScreenInteractive::FixedSize(SCREEN_WIDTH, SCREEN_HEIGHT);

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
                errorIncorrect = "Incorrect user and pass";
                break;
            default:
                break;
        }
    };

    // Login screen
    auto screen = ScreenInteractive::FixedSize(SCREEN_WIDTH, SCREEN_HEIGHT);

    std::string username;
    std::string password;

    // Input
    Component inputUsername = Input(&username, "username");
    inputUsername |= CatchEvent([&](Event event) {
        return event.is_character() && !std::isalnum(event.character()[0]);
    });
    InputOption inputOptionPassword;
    inputOptionPassword.password = true;
    Component inputPassword = Input(&password, "password", inputOptionPassword);

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
            int res = callLogin(username, password);
            switch (res)
            {
                case 1:
                    currentScreen = screenCustomer;
                    break;
                case 2:
                    currentScreen = screenStylist;
                    break;
                case 3:
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
            buttonLogin->TakeFocus();
        }
        return check;
    });

    // All
    Component containerAll = Container::Vertical({
        inputUsername,
        inputPassword,
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
    auto screen = ScreenInteractive::FixedSize(SCREEN_WIDTH, SCREEN_HEIGHT);

    std::string firstname;
    std::string lastname;
    std::string username;
    std::string password;
    std::string confirmpassword;
    std::string phonenum;
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

    inputPhonenumber |= getComponentDecorator("phonenumber", phonenum);
    inputUsername |= getComponentDecorator("username");
    inputFirstname|= getComponentDecorator("name");
    inputLastname |= getComponentDecorator("name");
    inputPassword |= getComponentDecorator("password");
    inputConfirmpassword |= getComponentDecorator("password");

    int selectedInput = 0;
    Component containerInputs = Container::Vertical({
        inputFirstname,
        inputLastname,
        inputUsername,
        inputPassword,
        inputConfirmpassword,
        inputPhonenumber,
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
            callRegister(firstname, lastname, username, password, confirmpassword, phonenum, gender, role);
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
    // Function variables
    #pragma region
    std::string currentUserID = callGetCurrentUserID();
    std::vector<std::string> services;
    int serviceCount = callGetServiceList(services);
    std::string nameCurrentUser = callGetCurrentUserName();

    ButtonOption buttonOptionAll;
    buttonOptionAll.transform = [](const EntryState& s) {
        auto element = text(s.label) | center | borderRounded;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    ButtonOption buttonOptionTab;
    buttonOptionTab.transform = [](const EntryState& s) {
        auto element = text(s.label) | center ;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    MenuOption menuOptionAll;
    menuOptionAll.direction = Direction::Up;
    menuOptionAll.entries_option.transform = [](const EntryState& s) {
        auto element = text(s.label) | center;
        if (s.focused) {
            element = element | inverted;
        }
        return element;
    };
    InputOption inputOptionAll = InputOption::Default();
    inputOptionAll.multiline = false;
    #pragma endregion

    // Home Tab
    #pragma region
    Component tabHome = Renderer([&] {
        return text("Home") | center ;
    });
    #pragma endregion

    // Appointment Tab
    #pragma region
    std::string errorServicesEmpty = "";
    std::string errorApointmentTime = "";

    std::string newAppointmentId;
    int tabComfirm = 0;

    // Services
    bool* selectedServices = new bool[serviceCount];
    int selectedCheckboxService = 0;
    CheckboxOption checkboxOptionAll;
    checkboxOptionAll.transform = [](const EntryState& s) {
        auto prefix = text(s.state ? "▣ " : "☐ ");
        auto t = text(s.label);
        if (s.focused) {
        t |= inverted;
        }
        return hbox({prefix, t});
    };
    Component containerServices = Container::Vertical({}, &selectedCheckboxService);
    for (int i = 0; i < serviceCount; ++i) {
        selectedServices[i] = false;
        containerServices->Add(Checkbox(services[i], selectedServices + i, checkboxOptionAll) | size(WIDTH, EQUAL, 20));
    }

    // stylist and time check busy
    int selectedDay = 0;
    int selectedMonth = 0;
    int selectedYear = 0;
    int selectedHour = 0;
    int selectedMinute = 0;
    int selectedStylist = 0;
    std::string selectedStylistID = "";
    std::vector<std::string> appointmentStylistIDs = callGetStylistIDList();
    auto toStylistID = [&] (int i) {
        if (i == 0) {
            return std::string("null");
        }
        return appointmentStylistIDs.at(i - 1);
    };

    auto checkBusy = [&] () {
        errorApointmentTime = callCheckStylistBusy(toStylistID(selectedStylist), selectedDay, selectedMonth, selectedYear, selectedHour, selectedMinute);
    };
    checkBusy();

    // stylist
    std::vector<std::string> appointmentStylists;
    appointmentStylists.push_back("null");
    for (int i = 0; i < appointmentStylistIDs.size(); ++i) {
        appointmentStylists.push_back(callGetMemberNameByID(appointmentStylistIDs[i]) + " (" + appointmentStylistIDs[i] + ")");
    }

    int focusedStylist = 0;
    Component dropdownStylists = Dropdown({
        .radiobox = {
            .entries = &appointmentStylists,
            .selected = &selectedStylist,
            .on_change = checkBusy,
            .focused_entry = &focusedStylist,
        },
        .transform = [](bool open, Element checkbox, Element radiobox) {
            if (open) {
                const int maxHeight = 5;
                return vbox({
                    checkbox | inverted,
                    radiobox | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, maxHeight),
                });
            }
            return vbox({checkbox, filler()});
        },
    }) | size(WIDTH, EQUAL, 40);

    // Date and time
    std::vector<std::string> days = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    std::vector<std::string> months = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"};
    std::vector<std::string> years  = {"2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029", "2030"};
    std::vector<std::string> hours = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"};
    std::vector<std::string> minutes = {"00", "30"};

    MenuOption menuOptionAppointment(menuOptionAll);
    menuOptionAppointment.on_change = checkBusy;

    MenuOption menuOptionDay = MenuOption(menuOptionAppointment);
    int focusedDay = 0;
    menuOptionDay.focused_entry = &focusedDay;
    Component menuDay = Menu(&days, &selectedDay, menuOptionAppointment);

    MenuOption menuOptionMonth = MenuOption(menuOptionAppointment);
    menuOptionMonth.on_change = ([&] () {
        Datetime dt(0, 0, 1 + selectedDay, 1 + selectedMonth, 2021 + selectedYear);
        if (selectedDay > dt.MonthDays() - 1) {
            selectedDay = dt.MonthDays() - 1;
        }
        days.clear();
        for (int i = 1; i <= dt.MonthDays(); ++i) {
            days.push_back(std::to_string(i).size() == 1 ? "0" + std::to_string(i) : std::to_string(i));
        }
        checkBusy();
    });
    int focusedMonth = 0;
    menuOptionMonth.focused_entry = &focusedMonth;
    Component menuMonth = Menu(&months, &selectedMonth, menuOptionMonth);

    MenuOption menuOptionYear = MenuOption(menuOptionAppointment);
    int focusedYear = 0;
    menuOptionYear.focused_entry = &focusedYear;
    Component menuYear = Menu(&years, &selectedYear, menuOptionAppointment);

    MenuOption menuOptionHour = MenuOption(menuOptionAppointment);
    int focusedHour = 0;
    Component menuHour = Menu(&hours, &selectedHour, menuOptionAppointment);

    MenuOption menuOptionMinute = MenuOption(menuOptionAppointment);
    int focusedMinute = 0;
    Component menuMinute = Menu(&minutes, &selectedMinute, menuOptionAppointment);

    int selectedDate = 0;
    Component containerDate = Container::Horizontal({
        menuDay,
        menuMonth,
        menuYear,
    }, &selectedDate);
    int selectedTime = 0;
    Component containerTime = Container::Horizontal({
        menuHour,
        menuMinute,
    }, &selectedTime);

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

    Component buttonNext = Button("Next", [&] {
        if (!errorApointmentTime.empty()) {
            return;
        }
        errorServicesEmpty = "";
        try {
            callCreateNewAppointment(selectedServices, toStylistID(selectedStylist), selectedDay, selectedMonth, selectedYear, selectedHour, selectedMinute);
            newAppointmentId = callGetNewAppointmentId();
            tabComfirm = 1;
        }
        catch (int errorCode) {
            switch (errorCode)
            {
                case ERROR_CODE::CREATE_APPOINTMENT_SERVICES_EMPTY:
                    errorServicesEmpty = "Please select at least one service.";
                    break;
                default:
                    break;
            }
        }
    }, buttonOptionAll) | size(WIDTH, EQUAL, 8);
    buttonNext |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab;
        return check;
    });

    // Catch event
    containerServices |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab;
        if (check && selectedCheckboxService < 6) {
            selectedCheckboxService++;
        } else if (check) {
            dropdownStylists->TakeFocus();
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
            dropdownStylists->TakeFocus();
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
    rendererTime |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab || event == Event::Return;
        if (check) {
            buttonNext->TakeFocus();
        }
        return check;
    });

    Component tabAppointmentInput = Renderer(
        Container::Vertical({
                containerServices,
                dropdownStylists,
                rendererDate,
                rendererTime,
                buttonNext
        }) ,[&] {
            auto textError = [&](std::string error) {
                if (error.empty()) {
                    return text("") | size(HEIGHT, EQUAL, 0);
                }
                return paragraph(error) | align_right | color(Color::Red);
            };

            Element textBusy;
            if (!errorApointmentTime.empty()) {
                textBusy = paragraph(errorApointmentTime) | color(Color::Red);
            } else {
                textBusy = text("Can choose now    ") | color(Color::Green);
            }

            return vbox({
                text("Make an appointment") | center,
                separator(),
                text("Services") | size(WIDTH, EQUAL, 20) | bold,
                hbox({
                    filler() | size(WIDTH, EQUAL, 2),
                    containerServices->Render(),
                }),
                textError(errorServicesEmpty),
                text("Select stylist") | bold,
                hbox({
                    filler() | size(WIDTH, EQUAL, 2),
                    dropdownStylists->Render(),
                }),
                text("Select date and time") | bold,
                hbox({
                    filler() | size(WIDTH, EQUAL, 2),
                    vbox({
                        rendererDate->Render(),
                        rendererTime->Render(),
                    }),
                }),
                textBusy | align_right,
                filler() | size(HEIGHT, EQUAL, 1),
                buttonNext->Render()| align_right,
            }) | size(WIDTH, EQUAL, 40) | borderRounded;
    });

    // Comfirm appointment
    auto resetAppointment = [&] {
        for (int i = 0; i < serviceCount; ++i) {
            selectedServices[i] = false;
        }
        focusedStylist = 0;
        focusedDay = 0;
        focusedMonth = 0;
        focusedYear = 0;
        focusedHour = 0;
        focusedMinute = 0;

        selectedStylist = 0;
        selectedDate = 0;
        selectedTime = 0;
        selectedDay = 0;
        selectedMonth = 0;
        selectedYear = 0;
        selectedHour = 0;
        selectedMinute = 0;
        selectedCheckboxService = 0;

        containerServices->TakeFocus();
        checkBusy();
        newAppointmentId = "";
    };

    int selectedButtonsAppointment = 0;
    ButtonOption buttonOptionConfirm(buttonOptionAll);
    Component buttonAppointmentConfirm = Button("Confirm", [&] {
        tabComfirm = 0;
        resetAppointment();
        selectedButtonsAppointment = 0;
        callAddNewAppointment();
    }, buttonOptionConfirm);
    Component buttonAppointmentCancel = Button("Back", [&] {
        tabComfirm = 0;
        selectedButtonsAppointment = 0;
    }, buttonOptionConfirm);
    Component tabAppointmentConfirm = Renderer(Container::Horizontal({
        buttonAppointmentCancel,
        buttonAppointmentConfirm
    }, &selectedButtonsAppointment) ,[&] {
        auto textService = [&](int i) {
            if (!selectedServices[i]) {
                return text("") | size(HEIGHT, EQUAL, 0);
            }
            return text("  " + services[i]) | size(WIDTH, EQUAL, 20);
        };
        Elements textServiceList;
        for (int i = 0; i < serviceCount; ++i) {
            textServiceList.push_back(textService(i));
        }
        return vbox({
            text("Comfirm appointment") | center,
            separator(),
            text("Appointment ID") | bold,
            text("  " + newAppointmentId),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Customer name") | bold,
            text("  " + nameCurrentUser),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Services") | bold,
            vbox(textServiceList),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Stylist") | bold,
            text("  " + appointmentStylists[selectedStylist]),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Date and time") | bold,
            text("  Date: " + days[selectedDay] + "/" + months[selectedMonth] + "/" + years[selectedYear]),
            text("  Time: " + hours[selectedHour] + ":" + minutes[selectedMinute]),
            filler() | size(HEIGHT, EQUAL, 1),
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

    // Service done Tab
    #pragma region ServiceDone

    std::vector<std::string> serviceDoneIDList;
    int countServiceDone = 0;

    // ServiceDone filter
    int filterServiceDoneDay;
    int filterServiceDoneMonth;
    int filterServiceDoneYear;
    std::string filterServiceDoneStylistID;
    bool filterServiceDoneRating[STAR_COUNT] = {true, true, true, true, true}; // 1-5 stars
    bool filterServiceDoneStatus[2] = {true, true}; // 0: False, 1: True
    bool *filterServiceDoneServices = new bool[serviceCount];

    std::vector<std::string> filterSericeDoneDays = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    std::vector<std::string> filterSericeDoneMonths = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"};
    std::vector<std::string> filterSericeDoneYears = {"----", "2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029", "2030"};

    auto resetServiceDoneFilter = [&] {
        filterServiceDoneDay = 0;
        filterServiceDoneMonth = 0;
        filterServiceDoneYear = 0;
        filterServiceDoneStylistID = "";
        std::fill(filterServiceDoneRating, filterServiceDoneRating + STAR_COUNT, true);
        std::fill(filterServiceDoneStatus, filterServiceDoneStatus + 2, true);
        std::fill(filterServiceDoneServices, filterServiceDoneServices + serviceCount, true);
    };
    int detailTabServiceDone = 0;
    std::string detailServiceDoneID;
    std::string detailServiceDoneStylistID;
    std::string detailServiceDoneRating;
    std::string detailServiceDoneDate;
    std::string detailServiceDoneStatus;
    std::string detailServiceDoneService;

    auto setDetailServiceDone = [&] (std::string id) {
        detailServiceDoneID = id;
        detailServiceDoneStylistID = callGetServiceDoneStylistIDByID(id);
        detailServiceDoneRating = callGetServiceDoneRatingByID(id);
        detailServiceDoneDate = callGetServiceDoneDateByID(id);
        detailServiceDoneStatus = callGetServiceDoneStatusByID(id);
        detailServiceDoneService = callGetServiceDoneServiceByID(id);
    };


    Component containerServiceDoneList = Container::Vertical({});
    auto reloadServiceDoneList = [&] () {
        serviceDoneIDList = callGetServiceDoneIDList(filterServiceDoneDay, filterServiceDoneMonth, filterServiceDoneYear, currentUserID, filterServiceDoneStylistID, filterServiceDoneRating, filterServiceDoneStatus, filterServiceDoneServices, countServiceDone);
        containerServiceDoneList->DetachAllChildren();
            for (int i = 0; i < serviceDoneIDList.size(); ++i) {
            std::string id = serviceDoneIDList[i];
            Component c = Button("Detail", [&, id] {
                detailTabServiceDone = 1;
                setDetailServiceDone(id);
            }, buttonOptionTab);

            std::string rating = callGetServiceDoneRatingByID(id);
            std::string date = callGetServiceDoneDateByID(id);
            std::string status = callGetServiceDoneStatusByID(id);
            std::string service = callGetServiceDoneServiceByID(id);

            containerServiceDoneList->Add(Renderer(
                c, [&, c, id, rating, date, status, service] {
                return hbox({
                    text(id) | size(WIDTH, EQUAL, 12),
                    text(rating + whiteStar) | size(WIDTH, EQUAL, 8),
                    text(date) | size(WIDTH, EQUAL, 12),
                    text(status) | size(WIDTH, EQUAL, 8),
                    text(service) | size(WIDTH, EQUAL, 13),
                    separator(),
                    c->Render() | center | size(WIDTH, EQUAL, 8),

                });
            }));
        }
    };
    resetServiceDoneFilter();

    MenuOption menuOptionServiceDoneMonth = MenuOption(menuOptionAll);
    menuOptionServiceDoneMonth.on_change = ([&] () {
        int maxDay;
        if (filterServiceDoneMonth == 0) {
            maxDay = 31;
        } else {
            Datetime dt(0, 0, filterServiceDoneDay, filterServiceDoneMonth, 2020 + filterServiceDoneYear);
            maxDay = dt.MonthDays();
            if (filterServiceDoneDay > maxDay) {
                filterServiceDoneDay = maxDay;
            }
        }
        filterSericeDoneDays.clear();
        filterSericeDoneDays.push_back("--");
        for (int i = 1; i <= maxDay; ++i) {
            filterSericeDoneDays.push_back(std::to_string(i).size() == 1 ? "0" + std::to_string(i) : std::to_string(i));
        }
    });

    Component menuServiceDoneFilterDay = Menu(&filterSericeDoneDays, &filterServiceDoneDay, menuOptionAll);
    Component menuServiceDoneFilterMonth = Menu(&filterSericeDoneMonths, &filterServiceDoneMonth, menuOptionServiceDoneMonth);
    Component menuServiceDoneFilterYear = Menu(&filterSericeDoneYears, &filterServiceDoneYear, menuOptionAll);

    Component rendererServiceDoneFilterDate = Renderer(Container::Horizontal({
        menuServiceDoneFilterDay,
        menuServiceDoneFilterMonth,
        menuServiceDoneFilterYear,
    }), [&] {
        return hbox({
            menuServiceDoneFilterDay->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menuServiceDoneFilterMonth->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menuServiceDoneFilterYear->Render() | size(WIDTH, EQUAL, 4) | frame | size(HEIGHT, EQUAL, 1),
        });
    });

    Component inputServiceDoneStylistID = Input(&filterServiceDoneStylistID, "Stylist ID", inputOptionAll);

    Component containerServiceDoneRating = Container::Vertical({});
    for (int i = 0; i < STAR_COUNT; ++i) {
        containerServiceDoneRating->Add(Checkbox(std::to_string(i) + whiteStar, &filterServiceDoneRating[i]));
    }

    Component containerServiceDoneStatus = Container::Vertical({
        Checkbox("True", &filterServiceDoneStatus[1]),
        Checkbox("False", &filterServiceDoneStatus[0]),
    });

    Component containerServiceDoneServices = Container::Vertical({});
    for (int i = 0; i < serviceCount; ++i) {
        containerServiceDoneServices->Add(Checkbox(services[i], &filterServiceDoneServices[i]) | size(WIDTH, EQUAL, 20));
    }

    Component buttonServiceDoneFilter = Button("Filter", [&] {
        reloadServiceDoneList();
    }, buttonOptionAll);
    Component buttonServiceDoneResetFilter = Button("Reset", [&] {
        resetServiceDoneFilter();
    }, buttonOptionAll);

    int childServiceDoneFilter = 0;
    Component containerServiceDoneFilter = Container::Vertical({
        rendererServiceDoneFilterDate,
        inputServiceDoneStylistID,
        containerServiceDoneRating,
        containerServiceDoneStatus,
        containerServiceDoneServices,
        Container::Horizontal({
            buttonServiceDoneFilter,
            buttonServiceDoneResetFilter,
        }),
    }, &childServiceDoneFilter);

    containerServiceDoneFilter |= CatchEvent([&] (Event event) {
        bool check = (event == Event::Tab) && (childServiceDoneFilter < 4); // 5 is the number of small component in containerServiceDoneFilter
        if (check) {
            childServiceDoneFilter++;
        }
        return check;
    });
    containerServiceDoneFilter |= CatchEvent([&] (Event event) {
        bool check = (event == Event::TabReverse) && (childServiceDoneFilter > 0);
        if (check) {
            childServiceDoneFilter--;
        }
        return check;
    });

    Component rendererServiceDoneFilter = Renderer(containerServiceDoneFilter, [&] {
        return vbox({
            text("Filter") | center,
            separator(),
            hbox({
                text("Date") | size(WIDTH, EQUAL, 12),
                rendererServiceDoneFilterDate->Render() | size(WIDTH, EQUAL, 10),
            }),
            hbox({
                text("Stylist ID") | size(WIDTH, EQUAL, 12),
                inputServiceDoneStylistID->Render() | size(WIDTH, EQUAL, 12),
            }),
            text("Rating"),
            containerServiceDoneRating->Render(),
            text("isBook"),
            containerServiceDoneStatus->Render(),
            text("Services"),
            containerServiceDoneServices->Render(),
            hbox({
                buttonServiceDoneFilter->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 2),
                buttonServiceDoneResetFilter->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter
        });
    });

    // ServiceDone list
    Component rendererServiceDoneList = Renderer(containerServiceDoneList ,[&] {
        return vbox({
            hbox({
                text("ID") | size(WIDTH, EQUAL, 12),
                text("Rating") | size(WIDTH, EQUAL, 8),
                text("Date") | size(WIDTH, EQUAL, 12),
                text("isBook") | size(WIDTH, EQUAL, 8),
                text("Service") | size(WIDTH, EQUAL, 13),
            }) | bold,
            separator(),
            containerServiceDoneList->Render() | vscroll_indicator | frame,
            separator(),
            filler(),
            separator(),
            text("Total: " + std::to_string(countServiceDone)) | size(WIDTH, EQUAL, 20) | align_right,
        });
    });

    // ServiceDone detail
    Component buttonServiceDoneDetailBack = Button("Back", [&] {
        reloadServiceDoneList();
        detailTabServiceDone = 0;
    }, buttonOptionAll);

    std::vector<std::string> starRating = {blackStar, blackStar, blackStar, blackStar, blackStar};
    int hoverButtonStar = 0;
    int starCount = 0;
    bool rating = false;
    Component buttonServiceDoneRating = Button("Rating", [&] {
        std::fill(starRating.begin(), starRating.end(), blackStar);
        hoverButtonStar = 0;
        starCount = 0;
        rating = false;
        detailTabServiceDone = 2;
    }, buttonOptionAll);

    Component rendererServiceDoneDetail = Renderer(Container::Horizontal({
        buttonServiceDoneDetailBack,
        buttonServiceDoneRating,
    }), [&] {
        return vbox({
            text("Service Done Detail") | center,
            separator(),
            text("ID") | bold,
            text("  " + detailServiceDoneID),
            text("Stylist") | bold,
            text("  " + callGetMemberNameByID(detailServiceDoneStylistID) + " (" + detailServiceDoneStylistID + ")"),
            text("Rating") | bold,
            text("  " + detailServiceDoneRating + whiteStar),
            text("Date") | bold,
            text("  " + detailServiceDoneDate),
            text("isBook") | bold,
            text("  " + detailServiceDoneStatus),
            text("Service") | bold,
            text("  " + detailServiceDoneService),
            separator(),
            hbox({
                buttonServiceDoneDetailBack->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 5),
                buttonServiceDoneRating->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter,
        }) | borderRounded | size(WIDTH, EQUAL, 40);
    });

    // Rating

    Component rendererServiceDoneRating = Container::Horizontal({}, &hoverButtonStar);

    for (int i = 1; i < STAR_COUNT; ++i) {
        ButtonOption buttonOptionStar;
        buttonOptionStar.transform = [&, i](const EntryState& s) {
            auto element = text(starRating[i - 1]) | center | borderRounded;
            if (s.focused) {
                element |= inverted;
            }
            return element;
        };
        Component b = Button("", [&, i] {
            for (int j = 1; j < STAR_COUNT; ++j) {
                starRating[j - 1] = (j <= i ? whiteStar : blackStar);
            }
            starCount = i;
            rating = true;
        }, buttonOptionStar);
        rendererServiceDoneRating->Add(b);
    }

    int selectedButtonsTabServiceDoneRating = 0;
    Component buttonServiceDoneRatingBack = Button("Back", [&] {
        detailTabServiceDone = 1;
        selectedButtonsTabServiceDoneRating = 0;
    }, buttonOptionTab);
    Component buttonServiceDoneRatingDone = Button("Done", [&] {
        if (rating) {
            callRateServiceDone(detailServiceDoneID, starCount);
            detailServiceDoneRating = std::to_string(starCount);
            // setDetailServiceDone(detailServiceDoneID);
        }
        detailTabServiceDone = 1;
        selectedButtonsTabServiceDoneRating = 0;
    }, buttonOptionTab);

    Component buttonsServiceDoneRating = Renderer(Container::Horizontal({
        buttonServiceDoneRatingBack,
        buttonServiceDoneRatingDone,
    }), [&] {
        return hbox({
            buttonServiceDoneRatingBack->Render() | size(WIDTH, EQUAL, 8),
            filler() | size(WIDTH, EQUAL, 4),
            buttonServiceDoneRatingDone->Render() | size(WIDTH, EQUAL, 8),
        }) | hcenter;
    });

    Component tabServiceDoneRating = Renderer(Container::Vertical({
        rendererServiceDoneRating,
        buttonsServiceDoneRating,
    }, &selectedButtonsTabServiceDoneRating), [&] {
        return vbox({
            text("Rating") | center,
            separator(),
            rendererServiceDoneRating->Render() | center,
            separator(),
            buttonsServiceDoneRating->Render() | center,
        }) | borderRounded;
    });

    Component containerTabServiceDone = Renderer(Container::Tab({
        rendererServiceDoneList,
        rendererServiceDoneDetail,
        tabServiceDoneRating,
    }, &detailTabServiceDone), [&] {
        auto document = rendererServiceDoneList->Render();
        if (detailTabServiceDone == 1) {
            document = dbox({
                rendererServiceDoneList->Render(),
                rendererServiceDoneDetail->Render() | clear_under | center,
            });
        } else if (detailTabServiceDone == 2) {
            document = dbox({
                rendererServiceDoneList->Render(),
                rendererServiceDoneDetail->Render() | clear_under | center | dim,
                tabServiceDoneRating->Render() | clear_under | center,
            });
        }
        return document;
    });

    Component tabServiceDone = Renderer(
        Container::Horizontal({
            rendererServiceDoneFilter,
            containerTabServiceDone,
        }), [&] {
        return hbox({
            rendererServiceDoneFilter->Render(),
            separator(),
            containerTabServiceDone->Render(),
        });
    });
    #pragma endregion

    // History Tab
    #pragma region history
    std::vector<std::string> listAppointmentID;

    // History list
    int filterHistoryDay;
    int filterHistoryMonth;
    int filterHistoryYear;
    bool *filterHistoryServices = new bool[serviceCount];
    int filterHistoryStatus = 0; // 0: all, 1: done, 2: Waiting, 3: cancel

    int tabDetail = 0;
    int selectedButtonDetail = 0;

    std::string detailAppointmentID;
    std::string detailAppointmentCustomerName;
    std::string detailAppointmentStatus;
    std::string detailAppointmentDate;
    std::string detailAppointmentTime;
    std::vector<std::string> detailAppointmentServices;
    std::string detailAppointmentStylist;

    auto setDetailAppointment = [&] (std::string id) {
        detailAppointmentCustomerName = callGetAppointmentCustomerNameByID(id);
        detailAppointmentStatus = callGetAppointmentStatusByID(id);
        detailAppointmentDate = callGetAppointmentDateByID(id);
        detailAppointmentTime = callGetAppointmentTimeByID(id);
        detailAppointmentServices = callGetAppointmentServicesByID(id);
        detailAppointmentStylist = callGetAppointmentStylistByID(id);
        if (detailAppointmentStylist != "null") {
            detailAppointmentStylist += " (ID: " + callGetAppointmentStylistIDByID(id) + ")";
        }
    };

    Component containerHistoryList = Container::Vertical({});

    auto reloadHistoryList = [&] () {
        listAppointmentID = callGetCurrentUserAppointmentIDList(filterHistoryDay, filterHistoryMonth, filterHistoryYear, filterHistoryServices, filterHistoryStatus);
        containerHistoryList->DetachAllChildren();
        for (int i = 0; i < listAppointmentID.size(); ++i) {
            std::string id = listAppointmentID[i];
            Component c = Button("Detail", [&, id] {
                tabDetail = 1;
                detailAppointmentID = id;
                setDetailAppointment(detailAppointmentID);
            }, buttonOptionTab);

            std::string status = callGetAppointmentStatusByID(id);
            std::string date = callGetAppointmentDateByID(id);
            std::string time = callGetAppointmentTimeByID(id);
            std::vector<std::string> servicesList = callGetAppointmentServicesByID(id);
            std::string services = "";
            for (int j = 0; j < servicesList.size(); ++j) {
                services += servicesList[j];
                if (j < servicesList.size() - 1) {
                    services += ", ";
                }
            }

            containerHistoryList->Add(Renderer(
                c, [&, c, id, status, date, time, services] {
                return hbox({
                    text(id) | size(WIDTH, EQUAL, 12),
                    text(status) | size(WIDTH, EQUAL, 8),
                    text(date) | size(WIDTH, EQUAL, 12),
                    text(time) | size(WIDTH, EQUAL, 7),
                    text(services) | size(WIDTH, EQUAL, 14),
                    separator(),
                    c->Render() | center | size(WIDTH, EQUAL, 8),
                });
            }));
        }
    };
    reloadHistoryList();

    Component rendererHistoryList = Renderer(containerHistoryList ,[&] {
        return vbox({
            hbox({
                text("ID") | size(WIDTH, EQUAL, 12),
                text("Status") | size(WIDTH, EQUAL, 8),
                text("Date") | size(WIDTH, EQUAL, 12),
                text("Time") | size(WIDTH, EQUAL, 7),
                text("Services") | size(WIDTH, EQUAL, 14),
            }) | bold,
            separator(),
            containerHistoryList->Render() | vscroll_indicator | frame ,
            separator(),
        });
    });

    // Detail appointment
    int selectedButtonsDetail = 0;
    std::string errorCancel = "";
    Component buttonDetailCancel = Button("Cancel", [&] {
        try {
            callCancelAppointment(detailAppointmentID);
            detailAppointmentStatus = callGetAppointmentStatusByID(detailAppointmentID);
            errorCancel = "";
        }
        catch (int errorCode) {
            switch (errorCode) {
                case ERROR_CODE::APPOINTMENT_IS_DONE:
                    errorCancel = "Appointment is done";
                    break;
            }
        }
    }, buttonOptionAll);
    Component buttonDetailBack = Button("Back", [&] {
        reloadHistoryList();
        tabDetail = 0;
    }, buttonOptionAll);
    Component rendererTabDetail = Renderer(Container::Horizontal({
        buttonDetailBack,
        buttonDetailCancel
    }, &selectedButtonsAppointment) ,[&] {
        auto textService = [&](int i) {
            return hbox({
                text("  " + detailAppointmentServices[i]) | size(WIDTH, EQUAL, 20),
            });
        };
        Elements textServiceList;
        for (int i = 0; i < detailAppointmentServices.size(); ++i) {
            textServiceList.push_back(textService(i));
        }
        auto textError = [&](std::string error) {
            if (error.size() == 0) {
                return text("") | size(HEIGHT, EQUAL, 0);
            }
            return paragraph(error) | align_right | color(Color::Red);
        };
        return vbox({
            text("Detail appointment") | center,
            separator(),
            text("Appointment ID") | bold,
            text("  " + detailAppointmentID),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Status") | bold,
            text("  " + detailAppointmentStatus),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Customer name") | bold,
            text("  " + detailAppointmentCustomerName),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Services") | bold,
            vbox(textServiceList),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Stylist") | bold,
            text("  " + detailAppointmentStylist),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Date and time") | bold,
            text("  Date: " + detailAppointmentDate),
            text("  Time: " + detailAppointmentTime),
            filler(),
            separator(),
            hbox({
                buttonDetailBack->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 5),
                buttonDetailCancel->Render() | size(WIDTH, EQUAL, 10),
            })| hcenter,
            textError(errorCancel),
        }) | center | borderRounded | size(WIDTH, EQUAL, 40);
    });

    // filter History
    std::vector<std::string> historyDays = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    std::vector<std::string> historyMonths = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"};
    std::vector<std::string> historyYears = {"----", "2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029", "2030"};

    auto resetFilter = [&] {
        filterHistoryStatus = 0;
        filterHistoryDay = 0;
        filterHistoryMonth = 0;
        filterHistoryYear = 0;
        std::fill(filterHistoryServices, filterHistoryServices + serviceCount, true);
    };

    MenuOption menuOptionFilterMonth = MenuOption(menuOptionAll);
    menuOptionFilterMonth.on_change = ([&] () {
        int maxDay;
        if (filterHistoryMonth == 0) {
            maxDay = 31;
        } else {
            Datetime dt(0, 0, filterHistoryDay, filterHistoryMonth, 2020 + filterHistoryYear);
            maxDay = dt.MonthDays();
            if (filterHistoryDay > maxDay) {
                filterHistoryDay = maxDay;
            }
        }
        historyDays.clear();
        historyDays.push_back("--");
        for (int i = 1; i <= maxDay; ++i) {
            historyDays.push_back(std::to_string(i).size() == 1 ? "0" + std::to_string(i) : std::to_string(i));
        }
    });

    Component menuFilterDay = Menu(&historyDays, &filterHistoryDay, menuOptionAll);
    Component menuFilterMonth = Menu(&historyMonths, &filterHistoryMonth, menuOptionFilterMonth);
    Component menuFilterYear = Menu(&historyYears, &filterHistoryYear, menuOptionAll);
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
    for (int i = 0; i < serviceCount; ++i) {
        filterHistoryServices[i] = true;
        containerFilterServices->Add(Checkbox(services[i], &filterHistoryServices[i]) | size(WIDTH, EQUAL, 20));
    }

    std::vector<std::string> filterStatus = {"All", "Done", "Waiting", "Cancel"};
    Component radioboxStatus = Radiobox(filterStatus, &filterHistoryStatus);

    Component buttonFilter = Button("Filter", [&] {
        reloadHistoryList();
    }, buttonOptionAll);

    Component buttonResetFilter = Button("Reset", [&] {
        resetFilter();
    }, buttonOptionAll);

    // Catch event
    rendererFilterDate |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab;
        if (check) {
            containerFilterServices->TakeFocus();
        }
        return check;
    });
    containerFilterServices |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab && selectedFilterService == serviceCount - 1;
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
                text("Date") | size(WIDTH, EQUAL, 12),
                rendererFilterDate->Render() | size(WIDTH, EQUAL, 10),
            }),
            text("Services"),
            containerFilterServices->Render(),
            text("Status"),
            radioboxStatus->Render(),
            hbox({
                buttonFilter->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 2),
                buttonResetFilter->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter | size(WIDTH, EQUAL, 24),
        });
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
    #pragma region profile
    std::string firstname;
    std::string lastname;
    std::string username;
    std::string oldPassword;
    std::string newPassword;
    std::string confirmpassword;
    std::string phonenumber;
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
        callGetCurrentUserPersonInfo(phonenumber, gender);
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

    inputFirstname |= getComponentDecorator("name");
    inputLastname |= getComponentDecorator("name");
    inputOldpassword |= getComponentDecorator("password");
    inputNewpassword |= getComponentDecorator("password");
    inputConfirmpassword |= getComponentDecorator("password");
    inputPhonenumber |= getComponentDecorator("phonenumber", phonenumber);

    std::vector <std::string> genders = {"Male", "Female"};
    Component radioboxGender = Radiobox(&genders, &gender);

    std::string errorUpdateProfileFirstnameEmpty = "";
    std::string errorUpdateProfileLastnameEmpty  = "";
    Component buttonNameUpdate = Button("Update", [&] {
        errorUpdateProfileFirstnameEmpty = "";
        errorUpdateProfileLastnameEmpty  = "";
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
            resetPassword();
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
            callUpdateCurrentPersonInfo(phonenumber, gender);
        }
        catch (int errorCode) {
            switch (errorCode)
            {
                case ERROR_CODE::UPDATE_PROFILE_PERSONINFO_EMPTY:
                    errorUpdateProfilePersoninfoEmpty = "Phone number is empty";
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
            inputPhonenumber->TakeFocus();
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
        radioboxGender,
        containerButtonsPersoninfo,
    }), [&] {
        return vbox({
            hbox({text("Phone Number    : "), inputPhonenumber->Render()}),
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
    auto screen = ScreenInteractive::FixedSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    int  exit = 0;

    // Navigation tab buttons

    int selectedTab = 0;

    Component buttonHome = Button("Home", [&] {selectedTab = 0;}, buttonOptionTab);
    Component buttonAppointment = Button("Appointment", [&] {selectedTab = 1;}, buttonOptionTab);
    Component buttonServiceDone = Button("Service Done", [&] {
            reloadServiceDoneList();
            selectedTab = 2;
    }, buttonOptionTab);
    Component buttonHistory = Button("History", [&] {
        reloadHistoryList();
        selectedTab = 3;
    }, buttonOptionTab);
    Component buttonProfile = Button("Profile", [&] {selectedTab = 4;}, buttonOptionTab);
    Component buttonLogout = Button("Logout", [&] {exit = 1;}, buttonOptionTab);
    Component containerButtons = Container::Vertical({
        buttonHome,
        buttonAppointment,
        buttonServiceDone,
        buttonHistory,
        buttonProfile,
        buttonLogout,
    });
    Component rendererButtons = Renderer(containerButtons, [&] {
        return vbox({
            text("Welcome " + nameCurrentUser) | hcenter,
            filler() | size(HEIGHT, EQUAL, 2),
            separator(),
            buttonHome->Render(),
            separator(),
            buttonAppointment->Render(),
            separator(),
            buttonServiceDone->Render(),
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

    Component containerTabs = Container::Tab({
        tabHome,
        tabAppointment,
        tabServiceDone,
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

    // delete
    delete[] selectedServices;
    delete[] filterServiceDoneServices;
    delete[] filterHistoryServices;
}

void screenStylist()
{
    // Function variables
    #pragma region
    std::string currentUserID = callGetCurrentUserID();
    std::vector<std::string> services;
    int serviceCount = callGetServiceList(services);
    std::string nameCurrentUser = callGetCurrentUserName();

    ButtonOption buttonOptionAll;
    buttonOptionAll.transform = [](const EntryState& s) {
        auto element = text(s.label) | center | borderRounded;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };

    MenuOption menuOptionAll;
    menuOptionAll.direction = Direction::Up;
    menuOptionAll.entries_option.transform = [](const EntryState& s) {
        auto element = text(s.label) | center;
        if (s.focused) {
            element = element | inverted;
        }
        return element;
    };

    ButtonOption buttonOptionTab;
    buttonOptionTab.transform = [](const EntryState& s) {
        auto element = text(s.label) | center ;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    #pragma endregion

    // Home tab
    Component tabHome = Renderer([&] {
        return text("Home") | center ;
    });

    // Schedule tab
    #pragma region Schedule

    std::vector<std::string> listScheduleID;
    int countSchedule = 0;

    // Filter
    int filterScheduleDay;
    int filterScheduleMonth;
    int filterScheduleYear;
    bool *filterScheduleServices = new bool[serviceCount];
    int filterScheduleStatus;
    std::string filterScheduleCustomerID;
    std::vector<std::string> scheduleDays = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    std::vector<std::string> scheduleMonths = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"};
    std::vector<std::string> scheduleYears = {"----", "2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029", "2030"};
    std::vector<std::string> scheduleStatus = {"All", "Done", "Waiting", "Cancel"};

    auto resetScheduleFilter = [&] {
        filterScheduleDay = 0;
        filterScheduleMonth = 0;
        filterScheduleYear = 0;
        filterScheduleStatus = 0;
        filterScheduleCustomerID = "";
        std::fill(filterScheduleServices, filterScheduleServices + serviceCount, true);
    };

    std::string detailScheduleID;
    std::string detailScheduleCustomerName;
    std::string detailScheduleStatus;
    std::string detailScheduleDate;
    std::string detailScheduleTime;
    std::vector<std::string> detailScheduleServices;
    std::string detailScheduleStylist;
    int tabDetail = 0;

    auto setDetailSchedule = [&] (std::string id) {
        detailScheduleCustomerName = callGetAppointmentCustomerNameByID(id) + " (ID: " + callGetAppointmentCustomerIDByID(id) + ")";
        detailScheduleStatus = callGetAppointmentStatusByID(id);
        detailScheduleDate = callGetAppointmentDateByID(id);
        detailScheduleTime = callGetAppointmentTimeByID(id);
        detailScheduleServices = callGetAppointmentServicesByID(id);
        detailScheduleStylist = callGetAppointmentStylistByID(id);
        if (detailScheduleStylist != "null") {
            detailScheduleStylist += " (ID: " + callGetAppointmentStylistIDByID(id) + ")";
        }
    };

    Component containerScheduleHistoryList = Container::Vertical({});
    listScheduleID = callGetApointmentIDList(filterScheduleDay, filterScheduleMonth, filterScheduleYear, 0, 0, filterScheduleServices, filterScheduleStatus, filterScheduleCustomerID, currentUserID, countSchedule);

    int selectedButtonScheduleDetail = 0;
    auto reloadScheduleHistoryList = [&] () {
        containerScheduleHistoryList->DetachAllChildren();
        for (int i = 0; i < listScheduleID.size(); ++i) {
            std::string id = listScheduleID[i];
            Component c = Button("Detail", [&, id] {
                tabDetail = 1;
                setDetailSchedule(id);
                detailScheduleID = id;
                selectedButtonScheduleDetail = 0;
            }, buttonOptionTab);

            std::string status = callGetAppointmentStatusByID(id);
            std::string date = callGetAppointmentDateByID(id);
            std::string time = callGetAppointmentTimeByID(id);
            std::vector<std::string> servicesList = callGetAppointmentServicesByID(id);
            std::string services = "";
            for (int j = 0; j < servicesList.size(); ++j) {
                services += servicesList[j];
                if (j < servicesList.size() - 1) {
                    services += ", ";
                }
            }

            containerScheduleHistoryList->Add(Renderer(
                c, [&, c, id, status, date, time, services] {
                return hbox({
                    text(id) | size(WIDTH, EQUAL, 12),
                    text(status) | size(WIDTH, EQUAL, 10),
                    text(date) | size(WIDTH, EQUAL, 12),
                    text(time) | size(WIDTH, EQUAL, 7),
                    text(services) | size(WIDTH, EQUAL, 12),
                    separator(),
                    c->Render() | center | size(WIDTH, EQUAL, 8),
                });
            }));
        }
    };
    resetScheduleFilter();

    Component menuScheduleFilterDay = Menu(&scheduleDays, &filterScheduleDay, menuOptionAll);
    MenuOption menuOptionScheduleMonth = MenuOption(menuOptionAll);
    menuOptionScheduleMonth.on_change = ([&] () {
        int maxDay;
        if (filterScheduleMonth == 0) {
            maxDay = 31;
        } else {
            Datetime dt(0, 0, filterScheduleDay, filterScheduleMonth, 2020 + filterScheduleYear);
            maxDay = dt.MonthDays();
            if (filterScheduleDay > maxDay) {
                filterScheduleDay = maxDay;
            }
        }
        scheduleDays.clear();
        scheduleDays.push_back("--");
        for (int i = 1; i <= maxDay; ++i) {
            scheduleDays.push_back(std::to_string(i).size() == 1 ? "0" + std::to_string(i) : std::to_string(i));
        }
    });

    Component menuScheduleFilterMonth = Menu(&scheduleMonths, &filterScheduleMonth, menuOptionScheduleMonth);
    Component menuScheduleFilterYear = Menu(&scheduleYears, &filterScheduleYear, menuOptionAll);

    Component rendererScheduleFilterDate = Renderer(Container::Horizontal({
        menuScheduleFilterDay,
        menuScheduleFilterMonth,
        menuScheduleFilterYear,
    }), [&] {
        return hbox({
            menuScheduleFilterDay->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menuScheduleFilterMonth->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menuScheduleFilterYear->Render() | size(WIDTH, EQUAL, 4) | frame | size(HEIGHT, EQUAL, 1),
        });
    });

    int selectedScheduleFilterService = 0;
    Component containerFilterServices = Container::Vertical({}, &selectedScheduleFilterService);
    for (int i = 0; i < serviceCount; ++i) {
        filterScheduleServices[i] = true;
        containerFilterServices->Add(Checkbox(services[i], &filterScheduleServices[i]) | size(WIDTH, EQUAL, 20));
    }

    Component radioboxScheduleStatus = Radiobox(scheduleStatus, &filterScheduleStatus);
    InputOption inputOptionAll = InputOption::Default();
    inputOptionAll.multiline = false;
    Component inputCustomerID = Input(&filterScheduleCustomerID, "Customer ID", inputOptionAll);

    Component buttonScheduleFilter = Button("Filter", [&] {
        listScheduleID = callGetApointmentIDList(filterScheduleDay, filterScheduleMonth, filterScheduleYear, 0, 0, filterScheduleServices, filterScheduleStatus, filterScheduleCustomerID, currentUserID, countSchedule);
        reloadScheduleHistoryList();
    }, buttonOptionAll);
    Component buttonScheduleResetFilter = Button("Reset", [&] {
        resetScheduleFilter();
    }, buttonOptionAll);

    Component buttonScheduleToday = Button("Today", [&] {
        listScheduleID = callGetApointmentIDList(-1, -1, -1, 0, 0, filterScheduleServices, filterScheduleStatus, filterScheduleCustomerID, currentUserID, countSchedule);
        reloadScheduleHistoryList();
    }, buttonOptionAll);
    Component buttonScheduleNow   = Button("Now", [&] {
        listScheduleID = callGetApointmentIDList(-1, -1, -1, -1, -1, filterScheduleServices, filterScheduleStatus, filterScheduleCustomerID, currentUserID, countSchedule);
        reloadScheduleHistoryList();
    }, buttonOptionAll);

    int childScheduleFilter = 0;
    Component containerScheduleFilter = Container::Vertical({
        rendererScheduleFilterDate,
        containerFilterServices,
        radioboxScheduleStatus,
        inputCustomerID,
        Container::Horizontal({
            buttonScheduleFilter,
            buttonScheduleResetFilter,
        }),
        buttonScheduleToday,
        buttonScheduleNow,
    }, &childScheduleFilter);

    containerScheduleFilter |= CatchEvent([&] (Event event) {
        bool check = (event == Event::Tab) && (childScheduleFilter < 5); // 6 is the number of small component in containerScheduleFilter
        if (check) {
            childScheduleFilter++;
        }
        return check;
    });
    containerScheduleFilter |= CatchEvent([&] (Event event) {
        bool check = (event == Event::TabReverse) && (childScheduleFilter > 0);
        if (check) {
            childScheduleFilter--;
        }
        return check;
    });

    Component rendererScheduleFilter = Renderer(containerScheduleFilter, [&] {
        return vbox({
            text("Filter") | center,
            separator(),
            hbox({
                text("Date") | size(WIDTH, EQUAL, 12),
                rendererScheduleFilterDate->Render(),
            }),
            hbox({
                text("CustomervID") | size(WIDTH, EQUAL, 12),
                inputCustomerID->Render() | size(WIDTH, EQUAL, 12),
            }),
            text("Services"),
            containerFilterServices->Render(),
            text("Status"),
            radioboxScheduleStatus->Render(),
            filler(),
            hbox({
                buttonScheduleFilter->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 2),
                buttonScheduleResetFilter->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter,
            hbox({
                buttonScheduleToday->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 2),
                buttonScheduleNow  ->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter,
        }) | size(WIDTH, EQUAL, 24);
    });

    // HistoryList

    Component rendererScheduleHistoryList = Renderer(containerScheduleHistoryList ,[&] {
        return vbox({
            text("Appointment List") | center | bold,
            separator(),

            hbox({
                text("ID") | size(WIDTH, EQUAL, 12),
                text("Status") | size(WIDTH, EQUAL, 10),
                text("Date") | size(WIDTH, EQUAL, 12),
                text("Time") | size(WIDTH, EQUAL, 7),
                text("Services") | size(WIDTH, EQUAL, 12),
            }) | bold,
            separator(),
            containerScheduleHistoryList->Render() | vscroll_indicator | frame,
            separator(),
            filler(),
            separator(),
            text("Total: " + std::to_string(countSchedule)) | size(WIDTH, EQUAL, 20) | align_right,
        });
    });

    // Detail schedule
    std::string errorScheduleDetailDone = "";
    Component buttonScheduleDetailBack = Button("Back", [&] {
        errorScheduleDetailDone = "";
        reloadScheduleHistoryList();
        tabDetail = 0;
    }, buttonOptionAll);
    Component buttonScheduleDetailDone = Button("Done", [&] {
        errorScheduleDetailDone = "";
        try {
            callDoneAppointment(detailScheduleID);
            detailScheduleStatus = callGetAppointmentStatusByID(detailScheduleID);
        } catch (int errorCode) {
            switch (errorCode)
            {
                case ERROR_CODE::APPOINTMENT_IS_CANCELLED:
                    errorScheduleDetailDone = "Appointment is cancelled";
                    break;
                default:
                    break;
            }
        }
    }, buttonOptionAll);

    Component rendererTabDetail = Renderer(Container::Horizontal({
        buttonScheduleDetailBack,
        buttonScheduleDetailDone,
    }, &selectedButtonScheduleDetail),[&] {
        auto textService = [&](int i) {
            return hbox({
                text("  "  + detailScheduleServices[i]) | size(WIDTH, EQUAL, 20),
            });
        };
        Elements textServiceList;
        for (int i = 0; i < detailScheduleServices.size(); ++i) {
            textServiceList.push_back(textService(i));
        }
        auto textError = [&](std::string error) {
            if (error.size() == 0) {
                return text("") | size(HEIGHT, EQUAL, 0);
            }
            return paragraph(error) | align_right | color(Color::Red);
        };
        return vbox({
            text("Detail Appointment") | center,
            separator(),
            text("Appointment ID") | bold,
            text("  "  + detailScheduleID),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Status") | bold,
            text("  "  + detailScheduleStatus),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Customer") | bold,
            text("  "  + detailScheduleCustomerName),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Stylist") | bold,
            text("  "  + detailScheduleStylist),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Services") | size(WIDTH, EQUAL, 20) | bold,
            vbox(textServiceList),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Date and time") | size(WIDTH, EQUAL, 40) | bold,
            text("  Date: " + detailScheduleDate) | size(WIDTH, EQUAL, 40),
            text("  Time: " + detailScheduleTime) | size(WIDTH, EQUAL, 40),
            filler(),
            separator(),
            hbox({
                buttonScheduleDetailBack->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 5),
                buttonScheduleDetailDone->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter,
            textError(errorScheduleDetailDone),
        }) | center | borderRounded | size(WIDTH, EQUAL, 40);
    });

    Component rendererScheduleHistoryAll = Renderer(Container::Tab({
        rendererScheduleHistoryList,
        rendererTabDetail,
    }, &tabDetail),
    [&] {
        if (tabDetail == 1) {
            return dbox({
                rendererScheduleHistoryList->Render(),
                rendererTabDetail->Render() | clear_under | center,
            });
        }
        return rendererScheduleHistoryList->Render();
    });

    Component tabSchedule = Renderer(
        Container::Horizontal({
            rendererScheduleFilter,
            rendererScheduleHistoryAll,
        }), [&] {
        return hbox({
            rendererScheduleFilter->Render(),
            separator(),
            rendererScheduleHistoryAll->Render(),
        });
    });

    #pragma endregion

    // Service done Tab
    #pragma region serviceDone
    std::vector<std::string> serviceDoneIDList;
    int countServiceDone = 0;

    // ServiceDone filter
    int filterServiceDoneDay;
    int filterServiceDoneMonth;
    int filterServiceDoneYear;
    std::string filterServiceDoneCustomerID;
    bool filterServiceDoneRating[STAR_COUNT] = {true, true, true, true, true, true}; // 1-5 stars
    bool filterServiceDoneStatus[2] = {true, true}; // 0: False, 1: True
    bool *filterServiceDoneServices = new bool[serviceCount];
    std::vector<std::string> filterServiceDoneDays = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    std::vector<std::string> filterServiceDoneMonths = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"};
    std::vector<std::string> filterServiceDoneYears = {"----", "2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029", "2030"};

    auto resetServiceDoneFilter = [&] {
        filterServiceDoneDay = 0;
        filterServiceDoneMonth = 0;
        filterServiceDoneYear = 0;
        filterServiceDoneCustomerID = "";
        std::fill(filterServiceDoneRating, filterServiceDoneRating + STAR_COUNT, true);
        std::fill(filterServiceDoneStatus, filterServiceDoneStatus + 2, true);
        std::fill(filterServiceDoneServices, filterServiceDoneServices + serviceCount, true);
    };

    int detailTabServiceDone = 0;
    std::string detailServiceDoneID;
    std::string detailServiceDoneCustomerID;
    std::string detailServiceDoneRating;
    std::string detailServiceDoneDate;
    std::string detailServiceDoneStatus;
    std::string detailServiceDoneService;

    serviceDoneIDList = callGetServiceDoneIDList(filterServiceDoneDay, filterServiceDoneMonth, filterServiceDoneYear, filterServiceDoneCustomerID, currentUserID, filterServiceDoneRating, filterServiceDoneStatus, filterServiceDoneServices, countServiceDone);

    auto setDetailServiceDone = [&] (std::string id) {
        detailServiceDoneID = id;
        detailServiceDoneCustomerID = callGetServiceDoneCustomerIDByID(id);
        detailServiceDoneRating = callGetServiceDoneRatingByID(id);
        detailServiceDoneDate = callGetServiceDoneDateByID(id);
        detailServiceDoneStatus = callGetServiceDoneStatusByID(id);
        detailServiceDoneService = callGetServiceDoneServiceByID(id);
    };

    Component containerServiceDoneList = Container::Vertical({});
    auto reloadServiceDoneList = [&] {
        containerServiceDoneList->DetachAllChildren();
        for (int i = 0; i < serviceDoneIDList.size(); ++i) {
            std::string id = serviceDoneIDList[i];
            Component c = Button("Detail", [&, id] {
                detailTabServiceDone = 1;
                setDetailServiceDone(id);
            }, buttonOptionTab);

            std::string rating = callGetServiceDoneRatingByID(id);
            std::string date = callGetServiceDoneDateByID(id);
            std::string status = callGetServiceDoneStatusByID(id);
            std::string service = callGetServiceDoneServiceByID(id);

            containerServiceDoneList->Add(Renderer(
                c, [&, c, id, rating, date, status, service] {
                return hbox({
                    text(id) | size(WIDTH, EQUAL, 12),
                    text(rating + whiteStar) | size(WIDTH, EQUAL, 8),
                    text(date) | size(WIDTH, EQUAL, 12),
                    text(status) | size(WIDTH, EQUAL, 9),
                    text(service) | size(WIDTH, EQUAL, 12),
                    separator(),
                    c->Render() | center | size(WIDTH, EQUAL, 8),
                });
            }));
        }
    };
    reloadServiceDoneList();
    resetServiceDoneFilter();

    MenuOption menuOptionServiceDoneMonth = MenuOption(menuOptionAll);
    menuOptionServiceDoneMonth.on_change = [&] () {
        int maxDay = 31;
        if (filterServiceDoneMonth != 0) {
            Datetime dt(0, 0, filterServiceDoneDay, filterServiceDoneMonth, 2020 + filterServiceDoneYear);
            maxDay = dt.MonthDays();
            if (filterServiceDoneDay > maxDay) {
                filterServiceDoneDay = maxDay;
            }
        }
        filterServiceDoneDays.clear();
        filterServiceDoneDays.push_back("--");
        for (int i = 1; i <= maxDay; ++i) {
            filterServiceDoneDays.push_back(std::to_string(i).size() == 1 ? "0" + std::to_string(i) : std::to_string(i));
        }
    };

    Component menuServiceDoneFilterDay = Menu(&filterServiceDoneDays, &filterServiceDoneDay, menuOptionAll);
    Component menuServiceDoneFilterMonth = Menu(&filterServiceDoneMonths, &filterServiceDoneMonth, menuOptionServiceDoneMonth);
    Component menuServiceDoneFilterYear = Menu(&filterServiceDoneYears, &filterServiceDoneYear, menuOptionAll);

    Component rendererServiceDoneFilterDate = Renderer(Container::Horizontal({
        menuServiceDoneFilterDay,
        menuServiceDoneFilterMonth,
        menuServiceDoneFilterYear,
    }), [&] {
        return hbox({
            menuServiceDoneFilterDay->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menuServiceDoneFilterMonth->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menuServiceDoneFilterYear->Render() | size(WIDTH, EQUAL, 4) | frame | size(HEIGHT, EQUAL, 1),
        });
    });

    // InputOption inputOptionAll = InputOption::Default();
    // inputOptionAll.multiline = false;
    Component inputServiceDoneCustomerID = Input(&filterServiceDoneCustomerID, "Customer ID", inputOptionAll);

    Component containerServiceDoneRating = Container::Vertical({});
    for (int i = 0; i < STAR_COUNT; ++i) {
        containerServiceDoneRating->Add(Checkbox(std::to_string(i) + whiteStar, &filterServiceDoneRating[i]));
    }

    Component containerServiceDoneStatus = Container::Vertical({
        Checkbox("True", &filterServiceDoneStatus[1]),
        Checkbox("False", &filterServiceDoneStatus[0]),
    });

    Component containerServiceDoneServices = Container::Vertical({});
    for (int i = 0; i < serviceCount; ++i) {
        containerServiceDoneServices->Add(Checkbox(services[i], &filterServiceDoneServices[i]) | size(WIDTH, EQUAL, 20));
    }

    Component buttonServiceDoneFilter = Button("Filter", [&] {
        serviceDoneIDList = callGetServiceDoneIDList(filterServiceDoneDay, filterServiceDoneMonth, filterServiceDoneYear, filterServiceDoneCustomerID, currentUserID, filterServiceDoneRating, filterServiceDoneStatus, filterServiceDoneServices, countServiceDone);
        reloadServiceDoneList();
    }, buttonOptionAll);
    Component buttonServiceDoneResetFilter = Button("Reset", [&] {
        resetServiceDoneFilter();
    }, buttonOptionAll);

    int childServiceDoneFilter = 0;
    Component containerServiceDoneFilter = Container::Vertical({
        rendererServiceDoneFilterDate,
        inputServiceDoneCustomerID,
        containerServiceDoneRating,
        containerServiceDoneStatus,
        containerServiceDoneServices,
        Container::Horizontal({
            buttonServiceDoneFilter,
            buttonServiceDoneResetFilter,
        }),
    }, &childServiceDoneFilter);

    containerServiceDoneFilter |= CatchEvent([&] (Event event) {
        bool check = (event == Event::Tab) && (childServiceDoneFilter < 5); // 6 is the number of small component in containerServiceDoneFilter
        if (check) {
            childServiceDoneFilter++;
        }
        return check;
    });
    containerServiceDoneFilter |= CatchEvent([&] (Event event) {
        bool check = (event == Event::TabReverse) && (childServiceDoneFilter > 0);
        if (check) {
            childServiceDoneFilter--;
        }
        return check;
    });

    Component rendererServiceDoneFilter = Renderer(containerServiceDoneFilter, [&] {
        return vbox({
            text("Filter") | center,
            separator(),
            hbox({
                text("Date") | size(WIDTH, EQUAL, 12),
                rendererServiceDoneFilterDate->Render() | size(WIDTH, EQUAL, 10),
            }),
            hbox({
                text("Customer ID") | size(WIDTH, EQUAL, 12),
                inputServiceDoneCustomerID->Render() | size(WIDTH, EQUAL, 12),
            }),
            text("Rating"),
            containerServiceDoneRating->Render(),
            text("isBook"),
            containerServiceDoneStatus->Render(),
            text("Services"),
            containerServiceDoneServices->Render(),
            hbox({
                buttonServiceDoneFilter->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 2),
                buttonServiceDoneResetFilter->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter
        });
    });

    // ServiceDone list
    Component rendererServiceDoneList = Renderer(containerServiceDoneList ,[&] {
        return vbox({
            hbox({
                text("ID") | size(WIDTH, EQUAL, 12),
                text("Rating") | size(WIDTH, EQUAL, 8),
                text("Date") | size(WIDTH, EQUAL, 12),
                text("isBook") | size(WIDTH, EQUAL, 9),
                text("Service") | size(WIDTH, EQUAL, 12),
            }) | bold,
            separator(),
            containerServiceDoneList->Render() | vscroll_indicator | frame,
            separator(),
            filler(),
            separator(),
            text("Total: " + std::to_string(countServiceDone)) | size(WIDTH, EQUAL, 20) | align_right,
        });
    });

    // ServiceDone detail
    Component buttonServiceDoneDetailBack = Button("Back", [&] {
        detailTabServiceDone = 0;
    }, buttonOptionAll);

    Component rendererServiceDoneDetail = Renderer(Container::Vertical({
        buttonServiceDoneDetailBack,
    }), [&] {
        return vbox({
            text("Service Done Detail") | center,
            separator(),
            text("ID") | bold,
            text("  " + detailServiceDoneID),
            text("Customer") | bold,
            text("  " + callGetMemberNameByID(detailServiceDoneCustomerID) + " (" + detailServiceDoneCustomerID + ")"),
            text("Rating") | bold,
            text("  " + detailServiceDoneRating + whiteStar),
            text("Date") | bold,
            text("  " + detailServiceDoneDate),
            text("isBook") | bold,
            text("  " + detailServiceDoneStatus),
            text("Service") | bold,
            text("  " + detailServiceDoneService),
            separator(),
            buttonServiceDoneDetailBack->Render() | center,
        }) | borderRounded | size(WIDTH, EQUAL, 40);
    });

    Component containerTabServiceDone = Renderer(Container::Tab({
        rendererServiceDoneList,
        rendererServiceDoneDetail,
    }, &detailTabServiceDone), [&] {
        auto document = rendererServiceDoneList->Render();
        if (detailTabServiceDone == 1) {
            document = dbox({
                rendererServiceDoneList->Render(),
                rendererServiceDoneDetail->Render() | clear_under | center,
            });
        }
        return document;
    });

    Component tabServiceDone = Renderer(
        Container::Horizontal({
            rendererServiceDoneFilter,
            containerTabServiceDone,
        }), [&] {
        return hbox({
            rendererServiceDoneFilter->Render(),
            separator(),
            containerTabServiceDone->Render(),
        });
    });
    #pragma endregion

    // Profile tab
    #pragma region profile
    std::string firstname;
    std::string lastname;
    std::string username;
    std::string oldPassword;
    std::string newPassword;
    std::string confirmpassword;
    std::string phonenumber;
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
        callGetCurrentUserPersonInfo(phonenumber, gender);
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

    InputOption inputOptionPassword(inputOptionProfile);
    inputOptionPassword.password = true;

    Component inputOldpassword = Input(&oldPassword, "Old Password", inputOptionPassword);
    Component inputNewpassword = Input(&newPassword, "New Password", inputOptionPassword);
    Component inputConfirmpassword = Input(&confirmpassword, "Confirm Password", inputOptionPassword);

    Component inputPhonenumber = Input(&phonenumber, "Phone Number", inputOptionProfile);

    inputOldpassword |= getComponentDecorator("password");
    inputNewpassword |= getComponentDecorator("password");
    inputConfirmpassword |= getComponentDecorator("password");
    inputPhonenumber |= getComponentDecorator("phonenumber", phonenumber);

    std::vector <std::string> genders = {"Male", "Female"};
    Component radioboxGender = Radiobox(&genders, &gender);

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
            callUpdateCurrentPersonInfo(phonenumber, gender);
        }
        catch (int errorCode) {
            switch (errorCode)
            {
                case ERROR_CODE::UPDATE_PROFILE_PERSONINFO_EMPTY:
                    errorUpdateProfilePersoninfoEmpty = "Phonenumber is empty";
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
            inputPhonenumber->TakeFocus();
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
    Component containerUpdateName = Renderer([&] {
        return vbox({
            hbox({text("First Name      : "), text(firstname)}),
            hbox({text("Last Name       : "), text(lastname) }),
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
        radioboxGender,
        containerButtonsPersoninfo,
    }), [&] {
        return vbox({
            hbox({text("Phone Number    : "), inputPhonenumber->Render()}),
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

    // customer screen
    auto screen = ScreenInteractive::FixedSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    int exit = 0;

    // Navigation tab buttons
    #pragma region
    int selectedTab = 0;


    Component buttonHome = Button("Home", [&] {
        selectedTab = 0;
    }, buttonOptionTab);
    Component buttonSchedule = Button("Schedule", [&] {
        listScheduleID = callGetApointmentIDList(filterScheduleDay, filterScheduleMonth, filterScheduleYear, 0, 0, filterScheduleServices, filterScheduleStatus, filterScheduleCustomerID, currentUserID, countSchedule);
        reloadScheduleHistoryList();
        selectedTab = 1;
    }, buttonOptionTab);
    Component buttonServiceDone = Button("Service Done", [&] {
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
        buttonServiceDone,
        buttonProfile,
        buttonLogout,
    });
    Component rendererButtons = Renderer(containerButtons, [&] {
        return vbox({
            text("Welcome " + nameCurrentUser) | hcenter,
            filler() | size(HEIGHT, EQUAL, 2),
            separator(),
            buttonHome->Render(),
            separator(),
            buttonSchedule->Render(),
            separator(),
            buttonServiceDone->Render(),
            separator(),
            buttonProfile->Render(),
            separator(),
            filler(),
            separator(),
            buttonLogout->Render()
        }) | borderRounded | size(WIDTH, EQUAL, 30);
    });
    #pragma endregion

    Component containerTabs = Container::Tab({
        tabHome,
        tabSchedule,
        tabServiceDone,
        tabProfile,
    }, &selectedTab) | borderRounded | size(WIDTH, EQUAL, 90);
    // All
    Component containerAll = Container::Horizontal({
        rendererButtons,
        containerTabs,
    });

    // exit dialog
    #pragma region
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

    // delete
    delete[] filterServiceDoneServices;
}

void screenAdmin()
{
    // function variables
    #pragma region
    std::string currentUserID = callGetCurrentUserID();
    std::vector<std::string> services;
    int serviceCount = callGetServiceList(services);
    std::vector <std::string> listGenders = {"Male", "Female"};
    std::string name = callGetCurrentUserName();

    ButtonOption buttonOptionAll;
    buttonOptionAll.transform = [](const EntryState& s) {
        auto element = text(s.label) | center | borderRounded;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    ButtonOption buttonOptionTab;
    buttonOptionTab.transform = [](const EntryState& s) {
        auto element = text(s.label) | center ;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    MenuOption menuOptionAll;
    menuOptionAll.direction = Direction::Up;
    menuOptionAll.entries_option.transform = [](const EntryState& s) {
        auto element = text(s.label) | center;
        if (s.focused) {
            element = element | inverted;
        }
        return element;
    };
    #pragma endregion

    // customer screen
    auto screen = ScreenInteractive::FixedSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    int exit = 0;

    // Home tab
    Component tabHome = Renderer([&] {
        return text("Home") | center ;
    });

    // Statistics tab
    int selectedStatisticsTab = 0;
    bool firstTime = true;
    std::vector<std::string> listStatistics = {"Customer Count Statistics", "Service Statistics", "Stylist Statistics"};
    Component dropdownStatistics = Dropdown({
        .radiobox = {
            .entries = &listStatistics,
            .selected = &selectedStatisticsTab,
            .on_change = [&] () {
                firstTime = true;
            }
        },
        .transform = [](bool open, Element checkbox, Element radiobox) {
            if (open) {
                const int maxHeight = 2;
                return vbox({
                    checkbox | inverted,
                    radiobox | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, maxHeight),
                });
            }
            return vbox({checkbox, filler()});
        }
    });

    int statisticsDay = 0;
    int statisticsMonth = 0;
    int statisticsYear = 0;
    std::vector<std::string> listStatisticsDay = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    std::vector<std::string> listStatisticsMonth = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"};
    std::vector<std::string> listStatisticsYear = {"----", "2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029", "2030"};

    Component menuStatisticsDay = Menu(&listStatisticsDay, &statisticsDay, menuOptionAll);
    MenuOption menuOptionStatisticsMonth(menuOptionAll);
    menuOptionStatisticsMonth.on_change = [&] () {
        int maxDay = 31;
        if (statisticsMonth != 0) {
            Datetime dt(0, 0, statisticsDay, statisticsMonth, 2020 + statisticsYear);
            maxDay = dt.MonthDays();
            if (statisticsDay > maxDay) {
                statisticsDay = maxDay;
            }
        }
        listStatisticsDay.clear();
        listStatisticsDay.push_back("--");
        for (int i = 1; i <= maxDay; ++i) {
            listStatisticsDay.push_back(std::to_string(i).size() == 1 ? "0" + std::to_string(i) : std::to_string(i));
        }
    };
    Component menuStatisticsMonth = Menu(&listStatisticsMonth, &statisticsMonth, menuOptionStatisticsMonth);
    Component menuStatisticsYear = Menu(&listStatisticsYear, &statisticsYear, menuOptionAll);
    Component rendererStatisticDate = Renderer(Container::Horizontal({
        menuStatisticsDay,
        menuStatisticsMonth,
        menuStatisticsYear,
    }), [&] {
        return hbox({
            menuStatisticsDay->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menuStatisticsMonth->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menuStatisticsYear->Render() | size(WIDTH, EQUAL, 4) | frame | size(HEIGHT, EQUAL, 1),
        });
    });

    std::string errorStatisticsDate = "";
    std::function<void()> reloadCustumerCountStatistics;
    std::function<void()> reloadServiceStatistics;
    std::function<void()> reloadStylistStatistics;
    Component buttonStatisticsView = Button("View", [&] {
        errorStatisticsDate = "";
        if (statisticsYear == 0) {
            errorStatisticsDate = "Please select a time period";
            return;
        } else if (statisticsDay > 0 && statisticsMonth == 0) {
            errorStatisticsDate = "Please select month";
            return;
        }
        if (selectedStatisticsTab == 0) {
            reloadCustumerCountStatistics();
        } else if (selectedStatisticsTab == 1) {
            reloadServiceStatistics();
        } else if (selectedStatisticsTab == 2) {
            reloadStylistStatistics();
        }
        firstTime = false;
    }, buttonOptionTab);
    Component buttonStatisticsReset = Button("Reset", [&] {
        statisticsDay = 0;
        statisticsMonth = 0;
        statisticsYear = 0;
    }, buttonOptionTab);


    Component rendererStatisticsOption = Renderer(
        Container::Horizontal({
            dropdownStatistics,
            rendererStatisticDate,
            buttonStatisticsView,
            buttonStatisticsReset,
        }), [&] {
        auto textError = [&](std::string error) {
            if (error.size() == 0) {
                return text("") | size(HEIGHT, EQUAL, 0);
            }
            return paragraph(error) | align_right | color(Color::Red);
        };
        return vbox({
            hbox({
                dropdownStatistics->Render() | size(WIDTH, EQUAL, 30),
                separator(),
                hbox({
                    text("Time Period: ") | size(WIDTH, EQUAL, 13),
                    rendererStatisticDate->Render() | size(WIDTH, EQUAL, 10),
                }) | hcenter,
                filler(),
                separator() | align_right,
                buttonStatisticsView->Render() | size(WIDTH, EQUAL, 10) | align_right,
                separator() | align_right,
                buttonStatisticsReset->Render() | size(WIDTH, EQUAL, 10) | align_right,
            }),
            textError(errorStatisticsDate),
        });
    });

    // Customer Count Statistics
    std::vector<int> customerCountStatistics;
    int maxCustomerCount;
    float customerCountAverage;
    float customerCountStandardDeviation;
    std::string dayUnit;
    int widthGraph;
    reloadCustumerCountStatistics = [&] {
        if (statisticsMonth == 0) {
            widthGraph = 52;
            dayUnit = "Week";
        } else {
            widthGraph = Datetime(0, 0, 1, statisticsMonth, 2020 + statisticsYear).MonthDays() * 2;
            dayUnit = "Day";
        }
        customerCountStatistics = callGetCustomerCountStatistics(statisticsDay, statisticsMonth, statisticsYear);
        customerCountAverage = callGetAverage(customerCountStatistics);
        customerCountStandardDeviation = callGetStandardDeviation(customerCountStatistics);
        maxCustomerCount = *std::max_element(customerCountStatistics.begin(), customerCountStatistics.end());
    };

    auto graphCustomerCountStatistics = [&] (int width, int height) {
        int step = width / customerCountStatistics.size();
        float scale = maxCustomerCount > 0? (float)height / maxCustomerCount: 0;
        std::vector<int> scaledCustomerCountStatistics;
        for (int i = 0; i < customerCountStatistics.size(); ++i) {
            for (int j = 0; j < step; ++j) {
                scaledCustomerCountStatistics.push_back(std::round(customerCountStatistics[i] * scale));
            }
        }
        return scaledCustomerCountStatistics;
    };

    Component rendererCustomerCountStatistics = Renderer([&] {
        return vbox({
            hbox({
                text("Average: " + std::to_string(customerCountAverage)),
                filler() | size(WIDTH, EQUAL, 5),
                text("Standard Deviation: " + std::to_string(customerCountStandardDeviation)),
                filler() | size(WIDTH, EQUAL, 5),
                text("Unit: per" + dayUnit),
            }) | hcenter,
            separator(),
            text("Max: " + std::to_string(maxCustomerCount)) | center,
            graph(graphCustomerCountStatistics) | size(HEIGHT, EQUAL, 20) | size(WIDTH, EQUAL, widthGraph) | borderRounded | center,
            text("Min: 0") | center,
        });
    });

    // Service Statistics
    std::vector<std::vector<std::string>> dataService;
    auto tableService = Table();
    Element tableServiceElement;

    auto ignoreNan = [] (float value) {
        return std::isnan(value)? 0: value;
    };

    reloadServiceStatistics = [&] () {
        std::vector<int> serviceCustomerCountStatistics = callGetServiceCustomerCountStatistics(statisticsDay, statisticsMonth, statisticsYear);
        std::vector<float> serviceFrequencyStatistics = callGetServiceFrequencyStatistics(statisticsDay, statisticsMonth, statisticsYear);
        std::vector<int> serviceRateCountStatistics = callGetServiceRateCountStatistics(statisticsDay, statisticsMonth, statisticsYear);
        std::vector<float> serviceRateAverageStatistics = callGetServiceRateAverageStatistics(statisticsDay, statisticsMonth, statisticsYear);
        dataService.clear();

        dataService.push_back({"Service", "CustomerCount", "Frequency", "RateCount", "RateAvg"});
        for (int i = 1; i < SERVICES_COUNT; ++i) {
            std::vector<std::string> row;
            row.push_back(services[i - 1]);
            row.push_back(std::to_string(serviceCustomerCountStatistics[i]));
            row.push_back(std::to_string(ignoreNan(serviceFrequencyStatistics[i])));
            row.push_back(std::to_string(serviceRateCountStatistics[i]));
            row.push_back(std::to_string(ignoreNan(serviceRateAverageStatistics[i])));
            dataService.push_back(row);
        }
        tableService = Table(dataService);
        tableService.SelectAll().Border(LIGHT);
        tableService.SelectAll().DecorateCells(center);
        tableService.SelectAll().DecorateCells(size(WIDTH, GREATER_THAN, 12));
        tableService.SelectRow(0).Decorate(bold);
        tableService.SelectAll().SeparatorVertical(LIGHT);
        tableService.SelectRow(0).Border(LIGHT);

        tableServiceElement = tableService.Render();
    };

    Component rendererServiceStatistics = Renderer([&] {
        return tableServiceElement;
    });

    // Stylist Statistics
    std::vector<std::string> stylistIDlistStatistics = callGetStylistIDList();
    int stylistListSize = stylistIDlistStatistics.size();
    std::vector<std::string> stylistNameStatistics;
    for (int i = 0; i < stylistListSize; ++i) {
        stylistNameStatistics.push_back(callGetMemberNameByID(stylistIDlistStatistics[i]) + " (" + stylistIDlistStatistics[i] + ")");
    }
    std::vector<std::vector<std::string>> dataStylist;
    std::vector<std::vector<std::string>> dataStylistName;
    auto tableStylist = Table();
    auto tableStylistName = Table();
    Element tableStylistElement;
    Element tableStylistNameElement;

    int sortOption = 0;
    std::vector<std::string> listSortOptions = {"None", "CustomerCount", "Frequency", "RateAvg"};
    for (int i = 1; i < SERVICES_COUNT; ++i) {
        listSortOptions.push_back("CC(" + services[i - 1] + ")");
        listSortOptions.push_back("RA(" + services[i - 1] + ")");
    }
    Component dropdownSortOption = Dropdown({
        .radiobox = {
            .entries = &listSortOptions,
            .selected = &sortOption,
        },
        .transform = [](bool open, Element checkbox, Element radiobox) {
            if (open) {
                const int maxHeight = 5;
                return vbox({
                    checkbox | inverted,
                    radiobox | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, maxHeight),
                });
            }
            return vbox({checkbox, filler()});
        }
    });

    int sliderValue = 0;
    Component sliderStatistics = Slider("", &sliderValue, 0, 100, 1);

    reloadStylistStatistics = [&] () {
        dataStylist.clear();
        dataStylist.push_back({"Stylist", "CustomerCount(CC)", "Frequency", "RateAvg(RA)"});
        for (int i = 1; i < SERVICES_COUNT; ++i) {
            dataStylist[0].push_back( "CC(" +services[i - 1] + ")");
            dataStylist[0].push_back( "RA(" +services[i - 1] + ")");
        }

        std::vector<int> all = callGetCustomerCountStatistics(statisticsDay, statisticsMonth, statisticsYear);
        int sum = 0;
        for (int i = 0; i < all.size(); i++)
            sum += all[i];
        int stylistCustomerCountStatistics;
        float stylistFrequencyStatistics;
        float stylistRateAverageStatistics;
        for (int i = 0; i < stylistListSize; ++i) {
            stylistCustomerCountStatistics = callGetStylistCustomerCountStatistics(stylistIDlistStatistics[i], statisticsDay, statisticsMonth, statisticsYear);
            stylistFrequencyStatistics = stylistCustomerCountStatistics / (float)sum;
            stylistRateAverageStatistics = callGetStylistRateAverageStatistics(stylistIDlistStatistics[i], statisticsDay, statisticsMonth, statisticsYear);
            std::vector<std::string> row;
            row.push_back(stylistNameStatistics[i]);
            row.push_back(std::to_string(stylistCustomerCountStatistics));
            row.push_back(std::to_string(ignoreNan(stylistFrequencyStatistics)));
            row.push_back(std::to_string(ignoreNan(stylistRateAverageStatistics)));
            std::vector<int> stylistServiceCustomerCountStatistics = callGetStylistServiceCustomerCountStatistics(stylistIDlistStatistics[i], statisticsDay, statisticsMonth, statisticsYear);
            std::vector<float> stylistServiceRateAverageStatistics = callGetStylistServiceRateAverageStatistics(stylistIDlistStatistics[i], statisticsDay, statisticsMonth, statisticsYear);
            for (int j = 1; j < SERVICES_COUNT; ++j) {
                row.push_back(std::to_string(stylistServiceCustomerCountStatistics[j]));
                row.push_back(std::to_string(ignoreNan(stylistServiceRateAverageStatistics[j])));
            }
            dataStylist.push_back(row);
        }
        if (sortOption > 0) {
            std::sort(dataStylist.begin() + 1, dataStylist.end(), [&] (std::vector<std::string> a, std::vector<std::string> b) {
                return std::stof(a[sortOption]) > std::stof(b[sortOption]);
            });
        }

        dataStylistName.clear();
        for (int i = 0; i <= stylistListSize; ++i) {
            std::vector<std::string> row;
            row.push_back(dataStylist[i][0]);
            dataStylistName.push_back(row);
            dataStylist[i].erase(dataStylist[i].begin());
        }

        tableStylist = Table(dataStylist);
        tableStylist.SelectAll().Border(LIGHT);
        tableStylist.SelectAll().DecorateCells(center);
        tableStylist.SelectAll().DecorateCells(size(WIDTH, GREATER_THAN, 12));
        tableStylist.SelectRow(0).Decorate(bold);
        tableStylist.SelectAll().SeparatorVertical(LIGHT);
        tableStylist.SelectRow(0).Border(LIGHT);

        tableStylistElement = tableStylist.Render();

        tableStylistName = Table(dataStylistName);

        tableStylistName.SelectAll().Border(LIGHT);
        tableStylistName.SelectAll().DecorateCells(center);
        tableStylistName.SelectAll().DecorateCells(size(WIDTH, GREATER_THAN, 40));
        tableStylistName.SelectRow(0).Decorate(bold);
        tableStylistName.SelectAll().SeparatorVertical(LIGHT);
        tableStylistName.SelectRow(0).Border(LIGHT);

        tableStylistNameElement = tableStylistName.Render();
    };

    Component rendererStylistStatistics = Renderer(
        Container::Vertical({
            dropdownSortOption,
            sliderStatistics,
        }), [&] {
        return vbox({
            hbox({
                text("Sort by: ") | size(WIDTH, EQUAL, 10),
                dropdownSortOption->Render(),
            }) | hcenter,
            sliderStatistics->Render() | hcenter,
            hbox({
                tableStylistNameElement,
                tableStylistElement | focusPositionRelative(sliderValue / (float)100,  0) | frame | size(WIDTH, EQUAL, 40),
                vbox({}) | borderLight,
            }),
        });
    });

    // Statistics Tab
    Component tabStatisticsOption = Container::Tab({
        rendererCustomerCountStatistics,
        rendererServiceStatistics,
        rendererStylistStatistics,
    }, &selectedStatisticsTab);

    Component tabStatistics = Renderer(
        Container::Vertical({
            rendererStatisticsOption,
            tabStatisticsOption,
        }), [&] {
        Element last;
        if (selectedStatisticsTab == 0 && !firstTime) {
            last = rendererCustomerCountStatistics->Render();
        } else if (selectedStatisticsTab == 1 && !firstTime) {
            last = rendererServiceStatistics->Render();
        } else if (selectedStatisticsTab == 2 && !firstTime) {
            last = rendererStylistStatistics->Render();
        } else {
            last = text("Click View") | center;
        }
        return vbox({
            rendererStatisticsOption->Render(),
            separator(),
            last | center,
        });
    });

    // Appointment tab
    #pragma region Appointment

    std::vector<std::string> listAppointmentID;
    int countAppointment = 0;

    // HistoryList
    std::string detailAppointmentID;
    std::string detailAppointmentCustomerName;
    std::string detailAppointmentStatus;
    std::string detailAppointmentDate;
    std::string detailAppointmentTime;
    std::vector<std::string> detailAppointmentServices;
    std::string detailAppointmentStylist;
    int tabDetail = 0;

    auto setDetailAppointment = [&] (std::string id) {
        detailAppointmentCustomerName = callGetAppointmentCustomerNameByID(id) + " (ID: " + callGetAppointmentCustomerIDByID(id) + ")";
        detailAppointmentStatus = callGetAppointmentStatusByID(id);
        detailAppointmentDate = callGetAppointmentDateByID(id);
        detailAppointmentTime = callGetAppointmentTimeByID(id);
        detailAppointmentServices = callGetAppointmentServicesByID(id);
        detailAppointmentStylist = callGetAppointmentStylistByID(id);
        if (detailAppointmentStylist != "null") {
            detailAppointmentStylist += " (ID: " + callGetAppointmentStylistIDByID(id) + ")";
        }
    };

    int filterAppointmentDay;
    int filterAppointmentMonth;
    int filterAppointmentYear;
    int filterAppointmentHour;
    int filterAppointmentMinute;
    bool *filterAppointmentServices = new bool[serviceCount];
    int filterAppointmentStatus;
    std::string filterAppointmentCustomerID;
    std::string filterAppointmentStylistID;
    Component containerAppointmentHistoryList = Container::Vertical({});
    auto reloadAppointmentHistoryList = [&] () {
        listAppointmentID = callGetApointmentIDList(filterAppointmentDay, filterAppointmentMonth, filterAppointmentYear, filterAppointmentHour, filterAppointmentMinute, filterAppointmentServices, filterAppointmentStatus, filterAppointmentCustomerID, filterAppointmentStylistID, countAppointment);
        containerAppointmentHistoryList->DetachAllChildren();
        for (int i = 0; i < listAppointmentID.size(); ++i) {
            std::string id = listAppointmentID[i];
            Component c = Button("Detail", [&, id] {
                tabDetail = 1;
                detailAppointmentID = id;
                setDetailAppointment(detailAppointmentID);
            }, buttonOptionTab);

            std::string status = callGetAppointmentStatusByID(id);
            std::string date = callGetAppointmentDateByID(id);
            std::string time = callGetAppointmentTimeByID(id);
            std::vector<std::string> servicesList = callGetAppointmentServicesByID(id);
            std::string services = "";
            for (int j = 0; j < servicesList.size(); ++j) {
                services += servicesList[j];
                if (j < servicesList.size() - 1) {
                    services += ", ";
                }
            }
            containerAppointmentHistoryList->Add(Renderer(
                c, [&, c, id, status, date, time, services] {
                return hbox({
                    text(id) | size(WIDTH, EQUAL, 12),
                    text(status) | size(WIDTH, EQUAL, 8),
                    text(date) | size(WIDTH, EQUAL, 12),
                    text(time) | size(WIDTH, EQUAL, 7),
                    text(services) | size(WIDTH, EQUAL, 14),
                    separator(),
                    c->Render() | center | size(WIDTH, EQUAL, 8),
                });
            }));
        }
    };
    reloadAppointmentHistoryList();

    Component rendererAppointmentHistoryList = Renderer(containerAppointmentHistoryList ,[&] {
        return vbox({
            hbox({
                text("ID") | size(WIDTH, EQUAL, 12),
                text("Status") | size(WIDTH, EQUAL, 8),
                text("Date") | size(WIDTH, EQUAL, 12),
                text("Time") | size(WIDTH, EQUAL, 7),
                text("Services") | size(WIDTH, EQUAL, 14),
            }) | bold,
            separator(),
            containerAppointmentHistoryList->Render() | vscroll_indicator | frame,
            separator(),
            filler(),
            separator(),
            text("Total: " + std::to_string(countAppointment)) | size(WIDTH, EQUAL, 20) | align_right,
        });
    });

    // Detail appointment
    std::string errorAppointmentChangeStatus = "";
    Component buttonAppointmentDetailCancel = Button("Cancel", [&] {
        errorAppointmentChangeStatus = "";
        try {
            callCancelAppointment(detailAppointmentID);
        }
        catch (int errorCode) {
            switch (errorCode)
            {
                case ERROR_CODE::APPOINTMENT_IS_DONE:
                    errorAppointmentChangeStatus = "Appointment is done";
                    break;

                default:
                    break;
            }
        }
        detailAppointmentStatus = callGetAppointmentStatusByID(detailAppointmentID);
    }, buttonOptionAll);

    Component buttonAppointmentDetailBack = Button("Back", [&] {
        errorAppointmentChangeStatus = "";
        reloadAppointmentHistoryList();
        tabDetail = 0;
    }, buttonOptionAll);

    Component buttonAppointmentDetailDone = Button("Done", [&] {
        errorAppointmentChangeStatus = "";
        try {
            callDoneAppointment(detailAppointmentID);
        }
        catch (int errorCode) {
            switch (errorCode)
            {
                case ERROR_CODE::APPOINTMENT_IS_CANCELLED:
                    errorAppointmentChangeStatus = "Appointment is cancelled";
                    break;
                case ERROR_CODE::APPOINTMENT_HAS_NULL_STYLIST:
                    errorAppointmentChangeStatus = "Appointment has null stylist";
                    break;
                default:
                    break;
            }
        }
        detailAppointmentStatus = callGetAppointmentStatusByID(detailAppointmentID);
    }, buttonOptionAll);

    int selectedButtonsAppointmentChooseStylist = 0;
    int selectedButtonsAppointmentDropdown = 0;
    Component buttonAppointmentDetailChoose = Button("Choose stylist", [&] {
        if (detailAppointmentStylist == "null") {
            tabDetail = 2;
            selectedButtonsAppointmentChooseStylist = 0;
            selectedButtonsAppointmentDropdown = 0;
        } else if (detailAppointmentStatus == "Cancel") {
            errorAppointmentChangeStatus = "Appointment is cancelled";
        }
        else {
            errorAppointmentChangeStatus = "Stylist has been chosen";
        }
    }, buttonOptionAll);

    int selectedButtonsAppointment = 0;
    Component rendererTabDetail = Renderer(Container::Vertical({
        Container::Horizontal({
            buttonAppointmentDetailBack,
            buttonAppointmentDetailDone,
            buttonAppointmentDetailCancel,
        }, &selectedButtonsAppointment),
        buttonAppointmentDetailChoose,
        }),[&] {

        auto textService = [&](int i) {
            return hbox({
                text("  " + detailAppointmentServices[i]) | size(WIDTH, EQUAL, 20)
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
            text("  " + detailAppointmentID),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Customer") | bold,
            text("  " + detailAppointmentCustomerName),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Stylist") | bold,
            text("  " + detailAppointmentStylist),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Services") | size(WIDTH, EQUAL, 20) | bold,
            vbox(textServiceList),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Status") | bold,
            text("  " + detailAppointmentStatus),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Date and time") | size(WIDTH, EQUAL, 40) | bold,
            text("  Date: " + detailAppointmentDate) | size(WIDTH, EQUAL, 40),
            text("  Time: " + detailAppointmentTime) | size(WIDTH, EQUAL, 40),
            filler(),
            separator(),
            hbox({
                buttonAppointmentDetailBack->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 4),
                buttonAppointmentDetailDone->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 4),
                buttonAppointmentDetailCancel->Render() | size(WIDTH, EQUAL, 10),
            })| hcenter,
            buttonAppointmentDetailChoose->Render() | size(WIDTH, EQUAL, 20) | center,
            text(errorAppointmentChangeStatus) | align_right | color(Color::Red) | size(HEIGHT, EQUAL, errorAppointmentChangeStatus == "" ? 0 : 1),
        }) | center | borderRounded | size(WIDTH, EQUAL, 40);
    });

    // Choose a stylist
    int selectedStylist = 0;
    std::vector<std::string> stylistIDs = callGetStylistIDList();
    std::vector<std::string> stylistNames;
    for (const auto& id : stylistIDs) {
        stylistNames.push_back(callGetMemberNameByID(id) + " (ID: " + id + ")");
    }

    Component dropdownStylist = Dropdown({
        .radiobox = {.entries = &stylistNames, .selected = &selectedStylist},
        .transform = [](bool open, Element checkbox, Element radiobox) {
            if (open) {
                const int maxHeight = 5;
                return vbox({
                    checkbox | inverted,
                    radiobox | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, maxHeight),
                });
            }
            return vbox({checkbox, filler()}) ;
        },
    });

    Component buttonChooseStylist = Button("Choose", [&] {
        callAssignStylistToAppointment(detailAppointmentID, stylistIDs[selectedStylist]);
        detailAppointmentStylist = stylistNames[selectedStylist];
        // ~ setDetailAppointment(detailAppointmentID);

        tabDetail = 1;
    }, buttonOptionAll);
    Component buttonCancelChooseStylist = Button("Cancel", [&] {
        tabDetail = 1;
    }, buttonOptionAll);

    Component rendererTabChooseStylist = Renderer(Container::Vertical({
        dropdownStylist,
        Container::Horizontal({
            buttonCancelChooseStylist,
            buttonChooseStylist,
        }, &selectedButtonsAppointmentChooseStylist),
    }, &selectedButtonsAppointmentDropdown), [&] {
        return vbox({
            text("Choose a Stylist") | center,
            separator(),
            dropdownStylist->Render() | size(WIDTH, EQUAL, 48) | borderRounded,
            filler(),
            hbox({
                buttonCancelChooseStylist->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 5),
                buttonChooseStylist->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter,
        }) | borderRounded | size(WIDTH, EQUAL, 50);
    });

    Component rendererAppointmentHistoryAll = Renderer(Container::Tab({
        rendererAppointmentHistoryList,
        rendererTabDetail,
        rendererTabChooseStylist
    }, &tabDetail),
    [&] {
        auto document = rendererAppointmentHistoryList->Render();
        if (tabDetail == 1) {
            document = dbox({
                rendererAppointmentHistoryList->Render(),
                rendererTabDetail->Render() | clear_under | center,
            });
        } else if (tabDetail == 2) {
            document = dbox({
                rendererAppointmentHistoryList->Render(),
                rendererTabDetail->Render() | clear_under | center | dim,
                rendererTabChooseStylist->Render() | clear_under | center,
            });
        }
        return document;
    });

    // Filter

    std::vector<std::string> appointmentDays = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    std::vector<std::string> appointmentMonths = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"};
    std::vector<std::string> appointmentYears = {"----", "2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029", "2030"};
    std::vector<std::string> appointmentHours = {"--", "0", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"};
    std::vector<std::string> appointmentMinutes = {"--", "0", "30"};
    std::vector<std::string> appointmentStatus = {"All", "Done", "Waiting", "Cancel"};

    auto resetAppointmentFilter = [&] {
        filterAppointmentDay = 0;
        filterAppointmentMonth = 0;
        filterAppointmentYear = 0;
        filterAppointmentHour = 0;
        filterAppointmentMinute = 0;
        filterAppointmentStatus = 0;
        filterAppointmentCustomerID = "";
        filterAppointmentStylistID = "";
        std::fill(filterAppointmentServices, filterAppointmentServices + serviceCount, true);
    };
    resetAppointmentFilter();
    reloadAppointmentHistoryList();

    Component menuAppointmentFilterDay = Menu(&appointmentDays, &filterAppointmentDay, menuOptionAll);
    MenuOption menuOptionAppointmentMonth = MenuOption(menuOptionAll);
    menuOptionAppointmentMonth.on_change = ([&] () {
        int maxDay = 31;
        if (filterAppointmentMonth > 0) {
            Datetime dt = Datetime(0, 0, filterAppointmentDay, filterAppointmentMonth, filterAppointmentYear);
            maxDay = dt.MonthDays();
            if (filterAppointmentDay > maxDay) {
                filterAppointmentDay = maxDay;
            }
        }
        appointmentDays.clear();
        appointmentDays.push_back("--");
        for (int i = 1; i <= maxDay; ++i) {
            appointmentDays.push_back(std::to_string(i).size() == 1 ? "0" + std::to_string(i) : std::to_string(i));
        }
    });

    Component menuAppointmentFilterMonth = Menu(&appointmentMonths, &filterAppointmentMonth, menuOptionAppointmentMonth);
    Component menuAppointmentFilterYear = Menu(&appointmentYears, &filterAppointmentYear, menuOptionAll);
    Component menuAppointmentFilterHour = Menu(&appointmentHours, &filterAppointmentHour, menuOptionAll);
    Component menuAppointmentFilterMinute = Menu(&appointmentMinutes, &filterAppointmentMinute, menuOptionAll);

    Component rendererAppointmentFilterDate = Renderer(Container::Horizontal({
        menuAppointmentFilterDay,
        menuAppointmentFilterMonth,
        menuAppointmentFilterYear,
    }), [&] {
        return hbox({
            menuAppointmentFilterDay->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menuAppointmentFilterMonth->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menuAppointmentFilterYear->Render() | size(WIDTH, EQUAL, 4) | frame | size(HEIGHT, EQUAL, 1),
        });
    });

    Component rendererAppointmentFilterTime = Renderer(
        Container::Horizontal({
            menuAppointmentFilterHour,
            menuAppointmentFilterMinute,
        }), [&] {
        return hbox({
            menuAppointmentFilterHour->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text(":") | size(WIDTH, EQUAL, 1),
            menuAppointmentFilterMinute->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
        });
    });

    int selectedAppointmentFilterService = 0;
    Component containerFilterServices = Container::Vertical({}, &selectedAppointmentFilterService);
    for (int i = 0; i < serviceCount; ++i) {
        filterAppointmentServices[i] = true;
        containerFilterServices->Add(Checkbox(services[i], &filterAppointmentServices[i]) | size(WIDTH, EQUAL, 20));
    }

    Component radioboxAppointmentStatus = Radiobox(appointmentStatus, &filterAppointmentStatus);
    InputOption inputOptionAll = InputOption::Default();
    inputOptionAll.multiline = false;
    Component inputCustomerID = Input(&filterAppointmentCustomerID, "Customer ID", inputOptionAll);
    Component inputStylistID = Input(&filterAppointmentStylistID, "Stylist ID", inputOptionAll);

    Component buttonAppointmentFilter = Button("Filter", [&] {
        listAppointmentID = callGetApointmentIDList(filterAppointmentDay, filterAppointmentMonth, filterAppointmentYear, filterAppointmentHour, filterAppointmentMinute, filterAppointmentServices, filterAppointmentStatus, filterAppointmentCustomerID, filterAppointmentStylistID, countAppointment);
        reloadAppointmentHistoryList();
    }, buttonOptionAll);
    Component buttonAppointmentResetFilter = Button("Reset", [&] {
        resetAppointmentFilter();
    }, buttonOptionAll);

    int childAppointmentFilter = 0;
    Component containerAppointmentFilter = Container::Vertical({
        rendererAppointmentFilterDate,
        rendererAppointmentFilterTime,
        containerFilterServices,
        radioboxAppointmentStatus,
        inputCustomerID,
        inputStylistID,
        Container::Horizontal({
            buttonAppointmentFilter,
            buttonAppointmentResetFilter,
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
                text("Date") | size(WIDTH, EQUAL, 12),
                rendererAppointmentFilterDate->Render() ,
            }),
            hbox({
                text("Time") | size(WIDTH, EQUAL, 12),
                rendererAppointmentFilterTime->Render() ,
            }),
            hbox({
                text("Customer ID") | size(WIDTH, EQUAL, 12),
                inputCustomerID->Render() | size(WIDTH, EQUAL, 12),
            }),
            hbox({
                text("Stylist ID") | size(WIDTH, EQUAL, 12),
                inputStylistID->Render() | size(WIDTH, EQUAL, 12),
            }),
            text("Services"),
            containerFilterServices->Render(),
            text("Status"),
            radioboxAppointmentStatus->Render(),
            filler(),
            hbox({
                buttonAppointmentFilter->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 2),
                buttonAppointmentResetFilter->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter
        });
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

    std::vector<std::string> customerIDList;
    int countCustomer = 0;

    // Customer variables
    std::string customerFilterName;
    bool customerFilterGender[2] = {true, true}; // 0: Male, 1: Female

    int detailTabCustomer = 0;
    std::string detailCustomerID;
    std::string detailCustomerName;
    std::string detailCustomerGender;
    std::string detailCustomerPhonenumber;

    auto resetCustomerFilter = [&] {
        customerFilterName = "";
        customerFilterGender[0] = true;
        customerFilterGender[1] = true;
    };
    auto setDetailCustomer = [&] (std::string id) {
        detailCustomerName = callGetMemberNameByID(id);
        detailCustomerGender = callGetMemberGenderByID(id);
        detailCustomerPhonenumber = callGetMemberPhoneByID(id);
    };

    Component containerCustomerList = Container::Vertical({});
    int selectedButtonCustomerDetail = 0;
    auto reloadCustomerList = [&] () {
        customerIDList = callGetCustomerIDList(customerFilterGender, customerFilterName, countCustomer);
        containerCustomerList->DetachAllChildren();
        for (int i = 0; i < customerIDList.size(); ++i) {
            std::string cid = customerIDList[i];
            Component c = Button("Detail", [&, cid] {
                detailCustomerID = cid;
                setDetailCustomer(detailCustomerID);
                detailTabCustomer = 1;
                selectedButtonCustomerDetail = 0;
            }, buttonOptionTab);

            std::string id = customerIDList[i];
            std::string name = callGetMemberNameByID(id);
            std::string gender = callGetMemberGenderByID(id);
            std::string phonenumber = callGetMemberPhoneByID(id);
            containerCustomerList->Add(Renderer(
                c, [&, c, id, name, gender, phonenumber] {
                return hbox({
                    text(id) | size(WIDTH, EQUAL, 12),
                    text(name) | size(WIDTH, EQUAL, 25),
                    text(gender) | size(WIDTH, EQUAL, 8),
                    text(phonenumber) | size(WIDTH, EQUAL, 12),
                    separator(),
                    c->Render() | center | size(WIDTH, EQUAL, 8),
                });
            }));
        }
    };

    resetCustomerFilter();
    reloadCustomerList();

    // Customer Filter
    Component checkboxCustomerGenderMale = Checkbox("Male", &customerFilterGender[0]);
    Component checkboxCustomerGenderFemale = Checkbox("Female", &customerFilterGender[1]);
    Component containerCustomerGender = Container::Vertical({
        checkboxCustomerGenderMale,
        checkboxCustomerGenderFemale,
    });

    InputOption inputOptionCustomerAll = InputOption::Default();
    inputOptionCustomerAll.multiline = false;
    Component inputCustomerName = Input(&customerFilterName, "Name", inputOptionCustomerAll);

    Component buttonCustomerFilter = Button("Filter", [&] {
        reloadCustomerList();
    }, buttonOptionAll);
    Component buttonCustomerResetFilter = Button("Reset", [&] {
        resetCustomerFilter();
    }, buttonOptionAll);

    int childCustomerFilter = 0;
    Component containerCustomerFilter = Container::Vertical({
        containerCustomerGender,
        inputCustomerName,
        Container::Horizontal({
            buttonCustomerFilter,
            buttonCustomerResetFilter,
        }),
    }, &childCustomerFilter);

    containerCustomerFilter |= CatchEvent([&] (Event event) {
        bool check = (event == Event::Tab) && (childCustomerFilter < 3); // 4 is the number of small component in containerCustomerFilter
        if (check) {
            childCustomerFilter++;
        }
        return check;
    });
    containerCustomerFilter |= CatchEvent([&] (Event event) {
        bool check = (event == Event::TabReverse) && (childCustomerFilter > 0);
        if (check) {
            childCustomerFilter--;
        }
        return check;
    });

    Component rendererCustomerFilter = Renderer(containerCustomerFilter, [&] {
        return vbox({
            text("Filter") | center,
            separator(),
            text("Gender"),
            containerCustomerGender->Render(),
            hbox({
                text("Name: "),
                inputCustomerName->Render(),
            }),
            filler(),
            hbox({
                buttonCustomerFilter->Render() | size(WIDTH, EQUAL, 10),
                buttonCustomerResetFilter->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter
        }) | size(WIDTH, EQUAL, 20);
    });

    // Customer list

    Component rendererCustomerList = Renderer(
        Container::Vertical({
            containerCustomerList,
        }), [&] {
        return vbox({
            hbox({
                text("ID") | size(WIDTH, EQUAL, 12),
                text("Name") | size(WIDTH, EQUAL, 25),
                text("Gender") | size(WIDTH, EQUAL, 8),
                text("Phonenumber") | size(WIDTH, EQUAL, 12),
            }) | bold,
            separator(),
            containerCustomerList->Render() | vscroll_indicator | frame,
            separator(),
            filler(),
            separator(),
            hbox({
                text("Total: " + std::to_string(countCustomer)),
                filler() | size(WIDTH, EQUAL, 5),
            }) | align_right,
        });
    });


    // Customer detail
    Component buttonCustomerDetailBack = Button("Back", [&] {
        detailTabCustomer = 0;
    }, buttonOptionAll);
    Component buttonCustomerDetailDelete = Button("Delete", [&] {
        callDeleteCustomer(detailCustomerID);
        reloadCustomerList();
        detailTabCustomer = 0;
    }, buttonOptionAll);

    Component rendererCustomerDetail = Renderer(Container::Vertical({
        buttonCustomerDetailBack,
        buttonCustomerDetailDelete,
    }, &selectedButtonCustomerDetail), [&] {
        return vbox({
            text("Customer Detail") | center,
            separator(),
            text("ID") | bold,
            text("  " + detailCustomerID),
            text("Name") | bold,
            text("  " + detailCustomerName),
            text("Gender") | bold,
            text("  " + detailCustomerGender),
            text("Phonenumber") | bold,
            text("  " + detailCustomerPhonenumber),
            text("Username") | bold,
            text("  " + callGetMemberUsernameByID(detailCustomerID)), // This style must be concerned
            separator(),
            hbox({
                buttonCustomerDetailBack->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 5),
                buttonCustomerDetailDelete->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter,
        }) | borderRounded | size(WIDTH, EQUAL, 40);
    });

    Component containerTabCustomer = Renderer(Container::Tab({
        rendererCustomerList,
        rendererCustomerDetail,
    }, &detailTabCustomer), [&] {
        auto document = rendererCustomerList->Render();
        if (detailTabCustomer == 1) {
            document = dbox({
                rendererCustomerList->Render(),
                rendererCustomerDetail->Render() | clear_under | center,
            });
        }
        return document;
    });

    Component tabCustomer = Renderer(
        Container::Horizontal({
            rendererCustomerFilter,
            containerTabCustomer,
        }), [&] {
        return hbox({
            rendererCustomerFilter->Render(),
            separator(),
            containerTabCustomer->Render(),
        });
    });
    #pragma endregion

    // Stylist tab
    #pragma region Stylist

    std::vector<std::string> stylistIDList;
    int countStylist = 0;
    int selectedAddStylist = 0;
    int selectedUpdateStylist = 0;

    // Stylist filter
    bool stylistFilterGender[2] = {true, true}; // 0: Male, 1: Female
    std::string stylistFilterName;

    auto resetStylistFilter = [&] {
        stylistFilterGender[0] = true;
        stylistFilterGender[1] = true;
        stylistFilterName = "";
    };
    resetStylistFilter();

    int selectedTabStylist = 0;
    std::string detailStylistID;
    std::string detailStylistName;
    std::string detailStylistGender;
    std::string detailStylistPhonenumber;
    std::string detailStylistUsername;

    auto setDetailStylist = [&] (std::string id) {
        detailStylistName = callGetMemberNameByID(id);
        detailStylistGender = callGetMemberGenderByID(id);
        detailStylistPhonenumber = callGetMemberPhoneByID(id);
        detailStylistUsername = callGetMemberUsernameByID(id);
    };

    Component containerStylistList = Container::Vertical({});
    int selectedContainerAddStylistButton = 0;
    Component buttonAddStylist = Button("Add Stylist", [&] {
        selectedContainerAddStylistButton = 0;
        selectedAddStylist = 0;
        selectedTabStylist = 3;
    }, buttonOptionAll);

    auto reloadStylistList = [&] () {
        stylistIDList = callGetStylistIDList(stylistFilterGender, stylistFilterName,countStylist);
        containerStylistList->DetachAllChildren();
        for (int i = 0; i < stylistIDList.size(); ++i) {
            std::string id = stylistIDList[i];

            Component c = Button("Detail", [&, id] {
                selectedTabStylist = 1;
                detailStylistID = id;
                setDetailStylist(detailStylistID);
            }, buttonOptionTab);
            c |= CatchEvent([&] (Event event) {
                bool check = event == Event::Tab;
                if (check) {
                    buttonAddStylist->TakeFocus();
                }
                return check;
            });

            std::string name = callGetMemberNameByID(id);
            std::string gender = callGetMemberGenderByID(id);
            std::string phonenumber = callGetMemberPhoneByID(id);

            containerStylistList->Add(Renderer(
                c, [&, c, id, name, gender, phonenumber] {
                return hbox({
                    text(id) | size(WIDTH, EQUAL, 12),
                    text(name) | size(WIDTH, EQUAL, 25),
                    text(gender) | size(WIDTH, EQUAL, 8),
                    text(phonenumber) | size(WIDTH, EQUAL, 12),
                    separator(),
                    c->Render() | center | size(WIDTH, EQUAL, 8),
                });
            }));
        }
    };
    reloadStylistList();

    Component rendererStylistList = Renderer(
        Container::Vertical({
            containerStylistList,
            buttonAddStylist,
        }), [&] {
        return vbox({
            hbox({
                text("ID") | size(WIDTH, EQUAL, 12),
                text("Name") | size(WIDTH, EQUAL, 25),
                text("Gender") | size(WIDTH, EQUAL, 8),
                text("Phonenumber") | size(WIDTH, EQUAL, 12),
            }) | bold,
            separator(),
            containerStylistList->Render() | vscroll_indicator | frame,
            separator(),
            filler(),
            separator(),
            hbox({
                text("Total: " + std::to_string(countStylist)),
                filler(),
                buttonAddStylist->Render() | size(WIDTH, EQUAL, 15),
            })
        });
    });


    // Stylist filter
    Component checkboxStylistGenderMale = Checkbox("Male", &stylistFilterGender[0]);
    Component checkboxStylistGenderFeMale = Checkbox("Female", &stylistFilterGender[1]);
    Component containerStylistGender = Container::Vertical({
        checkboxStylistGenderMale,
        checkboxStylistGenderFeMale,
    });

    InputOption inputOptionStylistAll = InputOption::Default();
    inputOptionStylistAll.multiline = false;
    Component inputStylistName = Input(&stylistFilterName, "Name", inputOptionStylistAll);

    Component buttonStylistFilter = Button("Filter", [&] {
        reloadStylistList();
    }, buttonOptionAll);
    Component buttonStylistResetFilter = Button("Reset", [&] {
        resetStylistFilter();
    }, buttonOptionAll);

    int childStylistFilter = 0;
    Component containerStylistFilter = Container::Vertical({
        containerStylistGender,
        inputStylistName,
        Container::Horizontal({
            buttonStylistFilter,
            buttonStylistResetFilter,
        }),
    }, &childStylistFilter);

    containerStylistFilter |= CatchEvent([&] (Event event) {
        bool check = (event == Event::Tab) && (childStylistFilter < 3); // 4 is the number of small component in containerStylistFilter
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
            text("Gender"),
            containerStylistGender->Render(),
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

    // Detail Stylist
    std::string stylistUpdateFirstName;
    std::string stylistUpdateLastName;
    int stylistUpdateGender;
    std::string stylistUpdatePhonenumber;
    std::string stylistUpdateUsername;
    std::string stylistUpdatePassword;

    auto resetUpdateStylist = [&] {
        stylistUpdateFirstName = callGetMemberFirstNameByID(detailStylistID);
        stylistUpdateLastName = callGetMemberLastNameByID(detailStylistID);
        stylistUpdateGender = detailStylistGender == "Male"? 0 : 1;
        stylistUpdatePhonenumber = detailStylistPhonenumber;
        stylistUpdateUsername = detailStylistUsername;
        stylistUpdatePassword = "";
    };

    Component buttonStylistDetailBack = Button("Back", [&] {
        reloadStylistList();
        selectedTabStylist = 0;
    }, buttonOptionAll);
    int selectedContainerUpdateStylistButton = 0;
    Component buttonStylistDetailModify = Button("Update", [&] {
        resetUpdateStylist();
        selectedContainerUpdateStylistButton = 0;
        selectedUpdateStylist = 0;
        selectedTabStylist = 2;
    }, buttonOptionAll);
    Component buttonStylistDetailDelete = Button("Delete", [&] {
        callDeleteStylist(detailStylistID);
        reloadStylistList();
        selectedTabStylist = 0;
    }, buttonOptionAll);

    int selectedButtonsStylistDetail = 0;
    Component rendererTabStylistDetail = Renderer(Container::Horizontal({
        buttonStylistDetailBack,
        buttonStylistDetailModify,
        buttonStylistDetailDelete,
    }, &selectedButtonsStylistDetail) ,[&] {
        return vbox({
            text("Detail stylist") | center,
            separator(),
            text("Stylist ID") | bold,
            text("    " + detailStylistID),
            text("Name") | bold,
            text("    " + detailStylistName),
            text("Gender") | bold,
            text("    " + detailStylistGender),
            text("Phone Number") | bold,
            text("    " + detailStylistPhonenumber),
            text("Username") | bold,
            text("    " + detailStylistUsername),
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
    Component inputUpdateStylistPhonenumber = Input(&stylistUpdatePhonenumber, "Phone Number", inputOptionStylistAll);
    Component inputUpdateStylistUsername = Input(&stylistUpdateUsername, "Username", inputOptionStylistAll);
    Component inputUpdateStylistPassword = Input(&stylistUpdatePassword, "Password", inputOptionStylistAll);
    inputUpdateStylistFirstName |= getComponentDecorator("name");
    inputUpdateStylistLastName |= getComponentDecorator("name");
    inputUpdateStylistPhonenumber |= getComponentDecorator("phonenumber", stylistUpdatePhonenumber);
    inputUpdateStylistUsername |= getComponentDecorator("username");
    inputUpdateStylistPassword |= getComponentDecorator("password");

    Component buttonUpdateStylistBack = Button("Back", [&] {
        selectedTabStylist = 1;
    }, buttonOptionAll);
    Component buttonUpdateStylistReset = Button("Reset", [&] {
        resetUpdateStylist();
    }, buttonOptionAll);
    Component buttonUpdateStylist = Button("Update", [&] {
        callUpdateStylist(detailStylistID, stylistUpdateFirstName, stylistUpdateLastName, stylistUpdateGender, stylistUpdatePhonenumber, stylistUpdateUsername, stylistUpdatePassword);
        setDetailStylist(detailStylistID);
        selectedTabStylist = 1;
    }, buttonOptionAll);

    Component containerUpdateStylist = Container::Vertical({
        inputUpdateStylistFirstName,
        inputUpdateStylistLastName,
        radioboxUpdateStylistGender,
        inputUpdateStylistPhonenumber,
        inputUpdateStylistUsername,
        inputUpdateStylistPassword,
        Container::Horizontal({
            buttonUpdateStylistBack,
            buttonUpdateStylistReset,
            buttonUpdateStylist,
        }, &selectedContainerUpdateStylistButton),
    }, &selectedUpdateStylist);

    containerUpdateStylist |= CatchEvent([&] (Event event) {
        bool check = (event == Event::Tab) && (selectedUpdateStylist < 6); // 7 is the number of small component in containerUpdateStylist
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
            text("Gender"),
            radioboxUpdateStylistGender->Render(),
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
                buttonUpdateStylistBack->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 5),
                buttonUpdateStylistReset->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 5),
                buttonUpdateStylist->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter,
        }) | size(WIDTH, EQUAL, 40) | borderRounded;
    });

    // Add Stylist
    std::string stylistAddFirstName;
    std::string stylistAddLastName;
    int stylistAddGender;
    std::string stylistAddPhonenumber;
    std::string stylistAddUsername;
    std::string stylistAddPassword;

    auto resetAddStylist = [&] {
        stylistAddFirstName = "";
        stylistAddLastName = "";
        stylistAddGender = 0;
        stylistAddPhonenumber = "";
        stylistAddUsername = "";
        stylistAddPassword = "";
    };
    resetAddStylist();

    Component inputAddStylistFirstName = Input(&stylistAddFirstName, "First Name", inputOptionStylistAll);
    Component inputAddStylistLastName = Input(&stylistAddLastName, "Last Name", inputOptionStylistAll);
    Component radioboxAddStylistGender = Radiobox(listGenders, &stylistAddGender);
    Component inputAddStylistPhonenumber = Input(&stylistAddPhonenumber, "Phone Number", inputOptionStylistAll);
    Component inputAddStylistUsername = Input(&stylistAddUsername, "Username", inputOptionStylistAll);
    Component inputAddStylistPassword = Input(&stylistAddPassword, "Password", inputOptionStylistAll);
    inputAddStylistFirstName |= getComponentDecorator("name");
    inputAddStylistLastName |= getComponentDecorator("name");
    inputAddStylistPhonenumber |= getComponentDecorator("phonenumber", stylistAddPhonenumber);
    inputAddStylistUsername |= getComponentDecorator("username");
    inputAddStylistPassword |= getComponentDecorator("password");

    Component buttonAddStylistBack = Button("Back", [&] {
        selectedTabStylist = 0;
    }, buttonOptionAll);
    Component buttonAddStylistReset = Button("Reset", [&] {
        resetAddStylist();
    }, buttonOptionAll);
    Component buttonAddStylistAdd = Button("Add", [&] {
        callAddStylist(stylistAddFirstName, stylistAddLastName, stylistAddGender, stylistAddPhonenumber, stylistAddUsername, stylistAddPassword);
        stylistIDList = callGetStylistIDList(stylistFilterGender, stylistFilterName, countStylist);
        reloadStylistList();
        resetAddStylist();
        selectedTabStylist = 0;
    }, buttonOptionAll);

    Component containerAddStylist = Container::Vertical({
        inputAddStylistFirstName,
        inputAddStylistLastName,
        radioboxAddStylistGender,
        inputAddStylistPhonenumber,
        inputAddStylistUsername,
        inputAddStylistPassword,
        Container::Horizontal({
            buttonAddStylistBack,
            buttonAddStylistReset,
            buttonAddStylistAdd,
        }, &selectedContainerAddStylistButton),
    }, &selectedAddStylist);

    containerAddStylist |= CatchEvent([&] (Event event) {
        bool check = (event == Event::Tab) && (selectedAddStylist < 6); // 7 is the number of small component in containerAddStylist
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
            text("Gender"),
            radioboxAddStylistGender->Render(),
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
                buttonAddStylistBack->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 5),
                buttonAddStylistReset->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 5),
                buttonAddStylistAdd->Render() | size(WIDTH, EQUAL, 10),
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
    #pragma region Profile
    std::string profileFirstname;
    std::string profileLastname;
    std::string profileUsername;
    std::string profileOldPassword;
    std::string profileNewPassword;
    std::string profileConfirmpassword;
    std::string profilePhonenumber;
    int profileGender = 0;

    auto resetName = [&] {
        callGetCurrentUserName(profileFirstname, profileLastname);
    };
    auto resetPassword = [&] {
        profileOldPassword = "";
        profileNewPassword = "";
        profileConfirmpassword = "";
    };
    auto resetPersonInfo = [&] {
        callGetCurrentUserPersonInfo(profilePhonenumber, profileGender);
    };
    auto resetProfile = [&] {
        resetName();
        resetPassword();
        resetPersonInfo();
    };
    profileUsername = callGetCurrentUserUsername();
    resetProfile();

    const int PROFILE_ALIGN_WIDTH = 50;

    InputOption inputOptionProfile = InputOption::Default();
    inputOptionProfile.placeholder = "";
    inputOptionProfile.multiline = false;

    Component inputFirstname = Input(&profileFirstname, "First Name", inputOptionProfile);
    Component inputLastname = Input(&profileLastname, "Last Name", inputOptionProfile);

    InputOption inputOptionPassword(inputOptionProfile);
    inputOptionPassword.password = true;

    Component inputOldpassword = Input(&profileOldPassword, "Old Password", inputOptionPassword);
    Component inputNewpassword = Input(&profileNewPassword, "New Password", inputOptionPassword);
    Component inputConfirmpassword = Input(&profileConfirmpassword, "Confirm Password", inputOptionPassword);

    Component inputPhonenumber = Input(&profilePhonenumber, "Phone Number", inputOptionProfile);

    inputFirstname |= getComponentDecorator("name");
    inputLastname |= getComponentDecorator("name");
    inputOldpassword |= getComponentDecorator("password");
    inputNewpassword |= getComponentDecorator("password");
    inputConfirmpassword |= getComponentDecorator("password");
    inputPhonenumber |= getComponentDecorator("phonenumber", profilePhonenumber);

    Component radioboxGender = Radiobox(&listGenders, &profileGender);

    std::string errorUpdateProfileFirstnameEmpty = "";
    std::string errorUpdateProfileLastnameEmpty  = "";
    Component buttonNameUpdate = Button("Update", [&] {
        errorUpdateProfileFirstnameEmpty = "";
        errorUpdateProfileLastnameEmpty = "";
        try {
            callUpdateCurrentUserName(profileFirstname, profileLastname);
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
            callUpdateCurrentPassword(profileOldPassword, profileNewPassword, profileConfirmpassword);
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
            callUpdateCurrentPersonInfo(profilePhonenumber, profileGender);
        }
        catch (int errorCode) {
            switch (errorCode)
            {
                case ERROR_CODE::UPDATE_PROFILE_PERSONINFO_EMPTY:
                    errorUpdateProfilePersoninfoEmpty = "Phonenumber is empty";
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
            inputPhonenumber->TakeFocus();
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
        radioboxGender,
        containerButtonsPersoninfo,
    }), [&] {
        return vbox({
            hbox({text("Phone Number    : "), inputPhonenumber->Render()}),
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
            text(profileUsername),
            text("Name") | bold,
            containerUpdateName->Render(),
            text("Password") | bold,
            containerUpdatePassword->Render(),
            text("Personal Information") | bold,
            containerUpdatePersoninfo->Render(),
        }) | size(WIDTH, EQUAL, PROFILE_ALIGN_WIDTH) | center;
    });
    #pragma endregion

    // ServiceDone tab
    #pragma region ServiceDone

    std::vector<std::string> serviceDoneIDList;
    int countServiceDone = 0;

    // ServiceDone filter
    int filterServiceDoneDay;
    int filterServiceDoneMonth;
    int filterServiceDoneYear;
    std::string filterServiceDoneCustomerID;
    std::string filterServiceDoneStylistID;
    bool filterServiceDoneRating[STAR_COUNT] = {true, true, true, true, true, true}; // 0-5 stars
    bool filterServiceDoneStatus[2] = {true, true}; // 0: False, 1: True
    bool *filterServiceDoneServices = new bool[serviceCount];
    std::vector<std::string> serviceDoneFilterDays = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    std::vector<std::string> serviceDoneFilterMonths = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"};
    std::vector<std::string> serviceDoneFilterYears = {"----", "2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029", "2030"};

    auto resetServiceDoneFilter = [&] {
        filterServiceDoneDay = 0;
        filterServiceDoneMonth = 0;
        filterServiceDoneYear = 0;
        filterServiceDoneCustomerID = "";
        filterServiceDoneStylistID = "";
        std::fill(std::begin(filterServiceDoneRating), std::end(filterServiceDoneRating), true);
        std::fill(std::begin(filterServiceDoneStatus), std::end(filterServiceDoneStatus), true);
        std::fill(filterServiceDoneServices, filterServiceDoneServices + serviceCount, true);
    };

    int detailTabServiceDone = 0;
    std::string detailServiceDoneID;
    std::string detailServiceDoneCustomerID;
    std::string detailServiceDoneStylistID;
    std::string detailServiceDoneRating;
    std::string detailServiceDoneDate;
    std::string detailServiceDoneStatus;
    std::string detailServiceDoneService;

    auto setDetailServiceDone = [&] (std::string id) {
        detailServiceDoneCustomerID = callGetServiceDoneCustomerIDByID(id);
        detailServiceDoneStylistID = callGetServiceDoneStylistIDByID(id);
        detailServiceDoneRating = callGetServiceDoneRatingByID(id);
        detailServiceDoneDate = callGetServiceDoneDateByID(id);
        detailServiceDoneStatus = callGetServiceDoneStatusByID(id);
        detailServiceDoneService = callGetServiceDoneServiceByID(id);
    };

    Component containerServiceDoneList = Container::Vertical({});
    auto reloadServiceDoneList = [&] {
        serviceDoneIDList = callGetServiceDoneIDList(filterServiceDoneDay, filterServiceDoneMonth, filterServiceDoneYear, filterServiceDoneCustomerID, filterServiceDoneStylistID, filterServiceDoneRating, filterServiceDoneStatus, filterServiceDoneServices, countServiceDone);
        containerServiceDoneList->DetachAllChildren();
        for (int i = 0; i < serviceDoneIDList.size(); ++i) {
            std::string sid = serviceDoneIDList[i];
            Component c = Button("Detail", [&, sid] {
                detailServiceDoneID = sid;
                setDetailServiceDone(detailServiceDoneID);
                detailTabServiceDone = 1;
            }, buttonOptionTab);

            std::string id = serviceDoneIDList[i];
            std::string rating = callGetServiceDoneRatingByID(id);
            std::string date = callGetServiceDoneDateByID(id);
            std::string status = callGetServiceDoneStatusByID(id);
            std::string service = callGetServiceDoneServiceByID(id);
            containerServiceDoneList->Add(Renderer(
                c, [&, c, id, rating, date, status, service] {
                return hbox({
                    text(id) | size(WIDTH, EQUAL, 12),
                    text(rating + whiteStar) | size(WIDTH, EQUAL, 8),
                    text(date) | size(WIDTH, EQUAL, 12),
                    text(status) | size(WIDTH, EQUAL, 9),
                    text(service) | size(WIDTH, EQUAL, 12),
                    separator(),
                    c->Render() | center | size(WIDTH, EQUAL, 8),
                });
            }));
        }
    };
    resetServiceDoneFilter();

    // Filter components
    MenuOption menuOptionserviceDoneMonth = MenuOption(menuOptionAll);
    menuOptionserviceDoneMonth.on_change = [&] {
        int maxDay = 31;
        if (filterServiceDoneMonth > 0) {
            Datetime dt = Datetime(0, 0, filterServiceDoneDay, filterServiceDoneMonth, filterServiceDoneYear);
            maxDay = dt.MonthDays();
            if (filterServiceDoneDay > maxDay) {
                filterServiceDoneDay = maxDay;
            }
        }
        serviceDoneFilterDays.clear();
        serviceDoneFilterDays.push_back("--");
        for (int i = 1; i <= maxDay; ++i) {
            serviceDoneFilterDays.push_back(std::to_string(i).size() == 1 ? "0" + std::to_string(i) : std::to_string(i));
        }
    };

    Component menuServiceDoneFilterDay = Menu(&serviceDoneFilterDays, &filterServiceDoneDay, menuOptionAll);
    Component menuServiceDoneFilterMonth = Menu(&serviceDoneFilterMonths, &filterServiceDoneMonth, menuOptionAppointmentMonth);
    Component menuServiceDoneFilterYear = Menu(&serviceDoneFilterYears, &filterServiceDoneYear, menuOptionAll);

    Component rendererServiceDoneFilterDate = Renderer(Container::Horizontal({
        menuServiceDoneFilterDay,
        menuServiceDoneFilterMonth,
        menuServiceDoneFilterYear,
    }), [&] {
        return hbox({
            menuServiceDoneFilterDay->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menuServiceDoneFilterMonth->Render() | size(WIDTH, EQUAL, 2) | frame | size(HEIGHT, EQUAL, 1),
            text("/") | size(WIDTH, EQUAL, 1),
            menuServiceDoneFilterYear->Render() | size(WIDTH, EQUAL, 4) | frame | size(HEIGHT, EQUAL, 1),
        });
    });

    Component inputServiceDoneCustomerID = Input(&filterServiceDoneCustomerID, "Customer ID", inputOptionAll);
    Component inputServiceDoneStylistID = Input(&filterServiceDoneStylistID, "Stylist ID", inputOptionAll);

    Component containerServiceDoneRating = Container::Vertical({});
    for (int i = 0; i < STAR_COUNT; ++i) {
        containerServiceDoneRating->Add(Checkbox(std::to_string(i + 1) + whiteStar, &filterServiceDoneRating[i]) | size(WIDTH, EQUAL, 20));
    }

    Component containerServiceDoneStatus = Container::Vertical({
        Checkbox("True", &filterServiceDoneStatus[1]),
        Checkbox("False", &filterServiceDoneStatus[0]),
    });

    Component containerServiceDoneServices = Container::Vertical({});
    for (int i = 0; i < serviceCount; ++i) {
        containerServiceDoneServices->Add(Checkbox(services[i], &filterServiceDoneServices[i]) | size(WIDTH, EQUAL, 20));
    }

    Component buttonServiceDoneFilter = Button("Filter", [&] {
        reloadServiceDoneList();
    }, buttonOptionAll);
    Component buttonServiceDoneResetFilter = Button("Reset", [&] {
        resetServiceDoneFilter();
    }, buttonOptionAll);

    int childServiceDoneFilter = 0;
    Component containerServiceDoneFilter = Container::Vertical({
        rendererServiceDoneFilterDate,
        inputServiceDoneCustomerID,
        inputServiceDoneStylistID,
        containerServiceDoneRating,
        containerServiceDoneStatus,
        containerServiceDoneServices,
        Container::Horizontal({
            buttonServiceDoneFilter,
            buttonServiceDoneResetFilter,
        }),
    }, &childServiceDoneFilter);

    containerServiceDoneFilter |= CatchEvent([&] (Event event) {
        bool check = (event == Event::Tab) && (childServiceDoneFilter < 6); // 7 is the number of small component in containerServiceDoneFilter
        if (check) {
            childServiceDoneFilter++;
        }
        return check;
    });
    containerServiceDoneFilter |= CatchEvent([&] (Event event) {
        bool check = (event == Event::TabReverse) && (childServiceDoneFilter > 0);
        if (check) {
            childServiceDoneFilter--;
        }
        return check;
    });

    Component rendererServiceDoneFilter = Renderer(containerServiceDoneFilter, [&] {
        return vbox({
            text("Filter") | center,
            separator(),
            hbox({
                text("Date") | size(WIDTH, EQUAL, 12),
                rendererServiceDoneFilterDate->Render() | size(WIDTH, EQUAL, 10),
            }),
            hbox({
                text("CustomerID ") | size(WIDTH, EQUAL, 12),
                inputServiceDoneCustomerID->Render() | size(WIDTH, EQUAL, 12),
            }),
            hbox({
                text("StylistID") | size(WIDTH, EQUAL, 12),
                inputServiceDoneStylistID->Render() | size(WIDTH, EQUAL, 12),
            }),
            text("Rating"),
            containerServiceDoneRating->Render(),
            text("isBook"),
            containerServiceDoneStatus->Render(),
            text("Services"),
            containerServiceDoneServices->Render(),
            filler(),
            hbox({
                buttonServiceDoneFilter->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 2),
                buttonServiceDoneResetFilter->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter
        });
    });

    // ServiceDone list
    Component rendererServiceDoneList = Renderer(
        Container::Vertical({
            containerServiceDoneList,
        }), [&] {
        return vbox({
            hbox({
                text("ID") | size(WIDTH, EQUAL, 12),
                text("Rating") | size(WIDTH, EQUAL, 8),
                text("Date") | size(WIDTH, EQUAL, 12),
                text("isBook") | size(WIDTH, EQUAL, 9),
                text("Service") | size(WIDTH, EQUAL, 12),
            }) | bold,
            separator(),
            containerServiceDoneList->Render() | vscroll_indicator | frame,
            separator(),
            filler(),
            separator(),
            text("    Total: " + std::to_string(countServiceDone) + "     ") | align_right,
        });
    });

    // ServiceDone detail
    Component buttonServiceDoneDetailBack = Button("Back", [&] {
        detailTabServiceDone = 0;
    }, buttonOptionAll);

    Component rendererServiceDoneDetail = Renderer(Container::Vertical({
        buttonServiceDoneDetailBack,
    }), [&] {
        return vbox({
            text("Service Done Detail") | center,
            separator(),
            text("ID") | bold,
            text("  " + detailServiceDoneID),
            text("Customer") | bold,
            text("  " + callGetMemberNameByID(detailServiceDoneCustomerID) + " (" + detailServiceDoneCustomerID + ")"),
            text("Stylist") | bold,
            text("  " + callGetMemberNameByID(detailServiceDoneStylistID) + " (" + detailServiceDoneStylistID + ")"),
            text("Rating") | bold,
            text("  " + detailServiceDoneRating + whiteStar),
            text("Date") | bold,
            text("  " + detailServiceDoneDate),
            text("isBook") | bold,
            text("  " + detailServiceDoneStatus),
            text("Service") | bold,
            text("  " + detailServiceDoneService),
            separator(),
            buttonServiceDoneDetailBack->Render() | size(WIDTH, EQUAL, 10) | hcenter,
        }) | borderRounded | size(WIDTH, EQUAL, 40);
    });

    Component containerTabServiceDone = Renderer(Container::Tab({
        rendererServiceDoneList,
        rendererServiceDoneDetail,
    }, &detailTabServiceDone), [&] {
        auto document = rendererServiceDoneList->Render();
        if (detailTabServiceDone == 1) {
            document = dbox({
                rendererServiceDoneList->Render(),
                rendererServiceDoneDetail->Render() | clear_under | center,
            });
        }
        return document;
    });

    Component tabServiceDone = Renderer(
        Container::Horizontal({
            rendererServiceDoneFilter,
            containerTabServiceDone,
        }), [&] {
        return hbox({
            rendererServiceDoneFilter->Render(),
            separator(),
            containerTabServiceDone->Render(),
        });
    });
    #pragma endregion

    #pragma region 
    // Navigation tab buttons
    int selectedTab = 0;
    Component buttonHome = Button("Home", [&] {
        selectedTab = 0;
    }, buttonOptionTab);
    Component buttonStatistics = Button("Statistics", [&] {
        selectedTab = 1;
    }, buttonOptionTab);
    Component buttonAppointment = Button("Appointment", [&] {
        reloadAppointmentHistoryList();
        selectedTab = 2;
    }, buttonOptionTab);
    Component buttonServiceDone = Button("Service Done", [&] {
        reloadServiceDoneList();
        selectedTab = 3;
    }, buttonOptionTab);
    Component buttonCustomer = Button("Customer", [&] {
        reloadCustomerList();
        selectedTab = 4;
    }, buttonOptionTab);
    Component buttonStylist = Button("Stylist", [&] { 
        reloadStylistList();
        selectedTab = 5;
    }, buttonOptionTab);

    Component buttonProfile = Button("Profile", [&] { 
        selectedTab = 6;
    }, buttonOptionTab);

    Component buttonLogout = Button("Logout", [&] {
        exit = 1;
    }, buttonOptionTab);
    Component containerButtons = Container::Vertical({
        buttonHome,
        buttonStatistics,
        buttonAppointment,
        buttonServiceDone,
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
            buttonServiceDone->Render(),
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

    Component containerTabs = Container::Tab({
        tabHome,
        tabStatistics,
        tabAppointment,
        tabServiceDone,
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

    // delete all dynamic memory
    delete[] filterServiceDoneServices;
    delete[] filterAppointmentServices;
}