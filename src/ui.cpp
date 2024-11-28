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
    // function variables
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

    // Home Tab
    #pragma region
    Component tabHome = Renderer([&] {
        return text("Home") | center ;
    });
    #pragma endregion

    // Appointment Tab
    #pragma region
    // flog << "Screen Customer: Appointment Tab\n";
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

    std::string messageCheckBusy = "";
    auto checkBusy = [&] () {
        // flog << "Screen Customer: Appointment Tab: Check Busy\n";
        messageCheckBusy = callCheckStylistBusy(toStylistID(selectedStylist), selectedDay, selectedMonth, selectedYear, selectedHour, selectedMinute);
        // flog << "End Screen Customer: Appointment Tab: Check Busy\n";
    };
    checkBusy();

    // stylist 
    std::vector<std::string> appointmentStylists;
    appointmentStylists.push_back("null");
    for (int i = 0; i < appointmentStylistIDs.size(); ++i) {
        appointmentStylists.push_back(callGetMemberNameByID(appointmentStylistIDs[i]) + " (" + appointmentStylistIDs[i] + ")");
    }

    Component dropdownStylists = Dropdown({
        // .checkbox = checkboxOptionAll,
        .radiobox = {
            .entries = &appointmentStylists, 
            .selected = &selectedStylist,
            .on_change = checkBusy,
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


    MenuOption menuOptionAll;
    menuOptionAll.direction = Direction::Up;
    menuOptionAll.entries_option.transform = [](const EntryState& s) {
        auto element = text(s.label) | center;
        if (s.focused) {
            element = element | inverted;
        }
        return element;
    };
    menuOptionAll.on_change = checkBusy;

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
        checkBusy();
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
    inputRequirement |= CatchEvent([&] (Event event) {
        bool check = (event == Event::Return );
        if (check) {
            requirement = requirement + "\n";
        }
        return false;
    });

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
            callCreateNewAppointment(selectedServices, toStylistID(selectedStylist), selectedDay, selectedMonth, selectedYear, selectedHour, selectedMinute, requirement);
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
                dropdownStylists,
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

            Element textBusy;
            if (messageCheckBusy.size() > 0) {
                textBusy = paragraph(messageCheckBusy) | color(Color::Red);
            } else {
                textBusy = text("Can choose now") | color(Color::Green);
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
                // textError(errorDateTime),
                textBusy | align_right,
                text("Reqirements") | bold,
                inputRequirement->Render(),
                buttonNext->Render()| align_right,
            }) | size(WIDTH, EQUAL, 40) | borderRounded;
    });

    // Comfirm appointment
    auto resetAppointment = [&] {
        for (int i = 0; i < serviceCount; ++i) {
            selectedServices[i] = false;
            selectedStylist = 0;
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
                return text("  ") | size(HEIGHT, EQUAL, 0);
            }
            return text(services[i]) | size(WIDTH, EQUAL, 20);
        };
        Elements textServiceList;
        for (int i = 0; i < serviceCount; ++i) {
            textServiceList.push_back(textService(i));
        }
        Elements ps;
        std::string s = requirement;
        while (s.find("\n") != std::string::npos) {
            ps.push_back(paragraph(s.substr(0, s.find("\n"))) | size(WIDTH, EQUAL, 40));
            s = s.substr(s.find("\n") + 1);
        }
        ps.push_back(paragraph(s));
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
            text("Date: " + days[selectedDay] + "/" + months[selectedMonth] + "/" + years[selectedYear]),
            text("Time: " + hours[selectedHour] + ":" + minutes[selectedMinute]),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Requirement: ") | size(WIDTH, EQUAL, 40) | bold,
            vbox(ps),
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
    // flog << "Screen Customer: service Done Tab\n";

    std::vector<std::string> serviceDoneIDList;
    int countServiceDone = 0;

    // ServiceDone filter
    int filterServiceDoneDay;
    int filterServiceDoneMonth;
    int filterServiceDoneYear;
    std::string filterServiceDoneStylistID;
    bool filterServiceDoneRating[5] = {true, true, true, true, true}; // 1-5 stars
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
        std::fill(filterServiceDoneRating, filterServiceDoneRating + 5, true);
        std::fill(filterServiceDoneStatus, filterServiceDoneStatus + 2, true);
        std::fill(filterServiceDoneServices, filterServiceDoneServices + serviceCount, true);
    };
    resetServiceDoneFilter();

    MenuOption menuOptionServiceDoneMonth = MenuOption(menuOptionAll);
    menuOptionServiceDoneMonth.on_change = ([&] () {
        int curYear = 2021 + filterServiceDoneYear;
        int curMonth = 1 + filterServiceDoneMonth;
        int curDay = 1 + filterServiceDoneDay;
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
            filterServiceDoneDay = maxDay - 1;
        }
        filterSericeDoneDays.clear();
        filterSericeDoneDays.push_back("--");
        for (int i = 1; i <= maxDay; ++i) {
            filterSericeDoneDays.push_back(std::to_string(i).size() == 1 ? "0" + std::to_string(i) : std::to_string(i));
        }
    });

    Component menuServiceDoneFilterDay = Menu(&filterSericeDoneDays, &filterServiceDoneDay, menuOptionAll);
    Component menuServiceDoneFilterMonth = Menu(&filterSericeDoneMonths, &filterServiceDoneMonth, menuOptionMonth);
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

    InputOption inputOptionAll = InputOption::Default();
    inputOptionAll.multiline = false;
    Component inputServiceDoneStylistID = Input(&filterServiceDoneStylistID, "Stylist ID", inputOptionAll);

    Component containerServiceDoneRating = Container::Vertical({});
    for (int i = 0; i < 5; ++i) {
        containerServiceDoneRating->Add(Checkbox(std::to_string(i + 1) + whiteStar, &filterServiceDoneRating[i]));
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
        serviceDoneIDList = callGetServiceDoneIDList(filterServiceDoneDay, filterServiceDoneMonth, filterServiceDoneYear, currentUserID, filterServiceDoneStylistID, filterServiceDoneRating, filterServiceDoneStatus, filterServiceDoneServices, countServiceDone);
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
                text("Date") | size(WIDTH, EQUAL, 10),
                rendererServiceDoneFilterDate->Render() | size(WIDTH, EQUAL, 10),
            }),
            hbox({
                text("StylistID") | size(WIDTH, EQUAL, 10),
                inputServiceDoneStylistID->Render() | size(WIDTH, EQUAL, 10),
            }),
            text("Rating"),
            containerServiceDoneRating->Render(),
            text("Status"),
            containerServiceDoneStatus->Render(),
            text("Services"),
            containerServiceDoneServices->Render(),
            hbox({
                buttonServiceDoneFilter->Render() | size(WIDTH, EQUAL, 10),
                buttonServiceDoneResetFilter->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter
        });
    });

    // ServiceDone list
    int detailTabServiceDone = 0;
    std::string detailServiceDoneID;
    std::string detailServiceDoneStylistID;
    std::string detailServiceDoneRating;
    std::string detailServiceDoneDate;
    std::string detailServiceDoneStatus;
    std::string detailServiceDoneService;

    serviceDoneIDList = callGetServiceDoneIDList(filterServiceDoneDay, filterServiceDoneMonth, filterServiceDoneYear, currentUserID, filterServiceDoneStylistID, filterServiceDoneRating, filterServiceDoneStatus, filterServiceDoneServices, countServiceDone);

    auto setDetailServiceDone = [&] (std::string id) {
        // flog << "Screen Customer: Service Done Tab: Set Detail\n";
        detailServiceDoneID = id;
        // flog << "  ID\n";
        detailServiceDoneStylistID = callGetServiceDoneStylistIDByID(id);
        // flog << "  StylistID\n";
        detailServiceDoneRating = callGetServiceDoneRatingByID(id);
        // flog << "  Rating\n";
        detailServiceDoneDate = callGetServiceDoneDateByID(id);
        // flog << "  Date\n";
        detailServiceDoneStatus = callGetServiceDoneStatusByID(id);
        // flog << "  Status\n";
        detailServiceDoneService = callGetServiceDoneServiceByID(id);
        // flog << "End Screen Customer: Service Done Tab: Set Detail\n";
    };

    
    Component containerServiceDoneList = Container::Vertical({});
    auto reloadServiceDoneList = [&] () {
        containerServiceDoneList->DetachAllChildren();
            for (int i = 0; i < serviceDoneIDList.size(); ++i) {
            std::string id = serviceDoneIDList[i];
            Component c = Button("Detail", [&, id] {
                // flog << "Screen Customer: Service Done Tab: Detail\n";
                detailTabServiceDone = 1;
                setDetailServiceDone(id);
                // flog << "End Screen Customer: Service Done Tab: Detail\n";
            }, buttonOptionTab);
            
            std::string rating = callGetServiceDoneRatingByID(id);
            std::string date = callGetServiceDoneDateByID(id);
            std::string status = callGetServiceDoneStatusByID(id);
            std::string service = callGetServiceDoneServiceByID(id);

            containerServiceDoneList->Add(Renderer(
                c, [&, c, id, rating, date, status, service] {
                return hbox({
                    text(id) | size(WIDTH, EQUAL, 10),
                    text(rating + whiteStar) | size(WIDTH, EQUAL, 10),
                    text(date) | size(WIDTH, EQUAL, 15),
                    text(status) | size(WIDTH, EQUAL, 10),
                    text(service) | size(WIDTH, EQUAL, 12),
                    separator(),
                    c->Render() | center | size(WIDTH, EQUAL, 8),
                    
                });
            }));
        }
    };
    reloadServiceDoneList();

    Component rendererServiceDoneList = Renderer(containerServiceDoneList ,[&] {
        return vbox({
            hbox({
                text("ID") | size(WIDTH, EQUAL, 10),
                text("Rating") | size(WIDTH, EQUAL, 10),
                text("Date") | size(WIDTH, EQUAL, 15),
                text("Status") | size(WIDTH, EQUAL, 10),
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
        reloadServiceDoneList();

        detailTabServiceDone = 0;
    }, buttonOptionAll);

    std::vector<std::string> starRating;
    int hoverButtonStar = 0;
    Component buttonServiceDoneRating = Button("Rating", [&] {
        starRating = {blackStar, blackStar, blackStar, blackStar, blackStar};
        hoverButtonStar = 0;
        
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
            text("Status") | bold,
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

    int starCount = 0;
    Component rendererServiceDoneRating = Container::Horizontal({}, &hoverButtonStar);
    
    for (int i = 0; i < 5; ++i) {
        ButtonOption buttonOptionStar;
        buttonOptionStar.transform = [&, i](const EntryState& s) {
            auto element = text(starRating[i]) | center | borderRounded;
            if (s.focused) {
                element |= inverted;
            }
            return element;
        };
        Component b = Button("", [&, i] {
            for (int j = 0; j < 5; ++j) {
                starRating[j] = (j <= i ? whiteStar : blackStar);
            }
            starCount = i + 1;
        }, buttonOptionStar);
        rendererServiceDoneRating->Add(b);
    }
    
    int selectedButtonsTabServiceDoneRating = 0;
    Component buttonServiceDoneRatingBack = Button("Back", [&] {
        detailTabServiceDone = 1;
        selectedButtonsTabServiceDoneRating = 0;
    }, buttonOptionTab);
    Component buttonServiceDoneRatingDone = Button("Done", [&] {
        detailTabServiceDone = 1;
        callRateServiceDone(detailServiceDoneID, starCount);
        detailServiceDoneRating = std::to_string(starCount);
        // setDetailServiceDone(detailServiceDoneID);

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
    // flog << "Screen Customer: History Tab\n";
    std::vector<std::string> listAppointmentID;

    // History list
    // flog << "Screen Customer: History List\n";
    int tabDetail = 0;
    int selectedButtonDetail = 0;

    std::string detailAppointmentID;
    std::string detailAppointmentCustomerName;
    std::string detailAppointmentStatus;
    std::string detailAppointmentDate;
    std::string detailAppointmentTime;
    std::vector<std::string> detailAppointmentServices;
    std::string detailAppointmentStylist;
    std::string detailAppointmentRequirement;

    auto setDetailAppointment = [&] (std::string id) {
        // flog << "Screen Customer: History Tab: Set Detail\n";
        detailAppointmentCustomerName = callGetAppointmentCustomerNameByID(id);
        // flog << "  CustomerName\n";
        detailAppointmentStatus = callGetAppointmentStatusByID(id);
        // flog << "  Status\n";
        detailAppointmentDate = callGetAppointmentDateByID(id);
        // flog << "  Date\n";
        detailAppointmentTime = callGetAppointmentTimeByID(id);
        // flog << "  Time\n";
        detailAppointmentServices = callGetAppointmentServicesByID(id);
        // flog << "  Services\n";
        detailAppointmentStylist = callGetAppointmentStylistByID(id);
        // flog << "  Stylist\n";
        if (detailAppointmentStylist != "null") {
            detailAppointmentStylist += " (ID: " + callGetAppointmentStylistIDByID(id) + ")";
        }
        detailAppointmentRequirement = callGetAppointmentRequirementByID(id);
        // flog << "  Requirement\n";
    };

    // Components componentHistoryList = {};
    // for (int i = 0; i < listAppointmentID.size(); ++i) {
    //     std::string id = listAppointmentID[i];
    //     Component buttonDetail = Button("Detail", [&, id] {
    //         tabDetail = 1;
    //         detailAppointmentID = id;
    //         setDetailAppointment(detailAppointmentID);
    //     }, buttonOptionTab);
    //     componentHistoryList.push_back(buttonDetail);
    // }

    Component containerHistoryList = Container::Vertical({});

    auto reloadHistoryList = [&] () {
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
                    text(date) | size(WIDTH, EQUAL, 14),
                    text(time) | size(WIDTH, EQUAL, 7),
                    text(services) | size(WIDTH, EQUAL, 16),
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
                text("Date") | size(WIDTH, EQUAL, 14),
                text("Time") | size(WIDTH, EQUAL, 7),
                text("Services") | size(WIDTH, EQUAL, 16),
            }) | bold,
            separator(),
            containerHistoryList->Render() | vscroll_indicator | frame ,
            separator(),
        });
    });

    // Detail appointment
    int selectedButtonsDetail = 0;
    Component buttonDetailCancel = Button("Cancel", [&] {
        callCancelAppointment(detailAppointmentID);
        detailAppointmentStatus = callGetAppointmentStatusByID(detailAppointmentID);
    }, buttonOptionNext);
    Component buttonDetailBack = Button("Back", [&] {
        reloadHistoryList();
        tabDetail = 0;
    }, buttonOptionNext);
    Component rendererTabDetail = Renderer(Container::Horizontal({
        buttonDetailBack,
        buttonDetailCancel
    }, &selectedButtonsAppointment) ,[&] {
        auto textService = [&](int i) {
            return hbox({
                text(detailAppointmentServices[i]) | size(WIDTH, EQUAL, 20),
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
            text("Status") | bold,
            text(detailAppointmentStatus),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Customer name") | bold,
            text(detailAppointmentCustomerName),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Services") | bold,
            vbox(textServiceList),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Stylist") | bold,
            text(detailAppointmentStylist),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Date and time") | bold,
            text("Date: " + detailAppointmentDate),
            text("Time: " + detailAppointmentTime),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Requirement: ") | bold,
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

    // filter History
    int filterHistoryDay;
    int filterHistoryMonth;
    int filterHistoryYear;
    bool *filterHistoryServices = new bool[serviceCount];
    int filterHistoryStatus = 0; // 0: all, 1: done, 2: pending, 3: cancel
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
        int curYear = 2020 + filterHistoryYear;
        int curMonth = filterHistoryMonth;
        int curDay = filterHistoryDay;
        bool isLeapYear = (curYear % 4 == 0 && curYear % 100 != 0) || (curYear % 400 == 0);
        int maxDay; 
        if (filterHistoryYear == 0) {
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
        historyDays.clear();
        historyDays.push_back("--");
        for (int i = 1; i <= maxDay; ++i) {
            historyDays.push_back(std::to_string(i).size() == 1 ? "0" + std::to_string(i) : std::to_string(i));
        }
    });

    // flog << "  break point 1\n";
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
    
    // flog << "  break point 2\n";
    int selectedFilterService = 0;
    Component containerFilterServices = Container::Vertical({}, &selectedFilterService);
    for (int i = 0; i < serviceCount; ++i) {
        // flog << "    count: " << i << "\n";
        filterHistoryServices[i] = true;
        containerFilterServices->Add(Checkbox(services[i], &filterHistoryServices[i]) | size(WIDTH, EQUAL, 20));
    }
    
    std::vector<std::string> filterStatus = {"All", "Done", "Pending", "Cancel"};
    Component radioboxStatus = Radiobox(filterStatus, &filterHistoryStatus);
    
    // flog << "  break point 2.5\n";
    Component buttonFilter = Button("Filter", [&] {
        listAppointmentID = callGetCurrentUserAppointmentIDList(filterHistoryDay, filterHistoryMonth, filterHistoryYear, filterHistoryServices, filterHistoryStatus);
        reloadHistoryList();
    }, buttonOptionNext);
    
    Component buttonResetFilter = Button("Reset", [&] {
        resetFilter();
    }, buttonOptionNext);

    // flog << "  break point 3\n";
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

    // flog << "  break point 4\n";
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
    // flog << "Screen Customer: Profile Tab\n";
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
            callUpdateCurrentPersonInfo(phonenumber, gender);
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
    auto screen = ScreenInteractive::FixedSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    int  exit = 0;

    // Navigation tab buttons
    
    int selectedTab = 0;
    
    Component buttonHome = Button("Home", [&] {selectedTab = 0;}, buttonOptionTab);
    Component buttonAppointment = Button("Appointment", [&] {selectedTab = 1;}, buttonOptionTab);
    Component buttonServiceDone = Button("Service Done", [&] {selectedTab = 2;}, buttonOptionTab);
    Component buttonHistory = Button("History", [&] {
        listAppointmentID = callGetCurrentUserAppointmentIDList(filterHistoryDay, filterHistoryMonth, filterHistoryYear, filterHistoryServices, filterHistoryStatus);
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
    // function variables
    std::string currentUserID = callGetCurrentUserID();
    std::vector<std::string> services;
    int serviceCount = callGetServiceList(services);

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

    // customer screen
    auto screen = ScreenInteractive::FixedSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    int exit = 0;

    // Navigation tab buttons
    #pragma region 
    std::string nameCurrentUser = callGetCurrentUserName();
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
    std::vector<std::string> scheduleStatus = {"All", "Done", "Pending", "Cancel"};

    auto resetScheduleFilter = [&] {
        filterScheduleDay = 0;
        filterScheduleMonth = 0;
        filterScheduleYear = 0;
        filterScheduleStatus = 0;
        filterScheduleCustomerID = "";
        std::fill(filterScheduleServices, filterScheduleServices + serviceCount, true);
    };

    resetScheduleFilter();
    // MenuOption menuOptionAll;
    // menuOptionAll.direction = Direction::Up;
    // menuOptionAll.entries_option.transform = [](const EntryState& s) {
    //     auto element = text(s.label) | center;
    //     if (s.focused) {
    //         element = element | inverted;
    //     }
    //     return element;
    // };

    Component menuScheduleFilterDay = Menu(&scheduleDays, &filterScheduleDay, menuOptionAll);
    MenuOption menuOptionScheduleMonth = MenuOption(menuOptionAll);
    menuOptionScheduleMonth.on_change = ([&] () {
        int curYear = 2020 + filterScheduleYear;
        int curMonth = filterScheduleMonth;
        int curDay = filterScheduleDay;
        bool isLeapYear = (curYear % 4 == 0 && curYear % 100 != 0) || (curYear % 400 == 0);
        int maxDay;
        if (filterScheduleYear == 0) {
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
            filterScheduleDay = maxDay;
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
    Component inputCustomerID = Input(&filterScheduleCustomerID, "CustomerID", inputOptionAll);

    // ButtonOption buttonOptionAll;
    // buttonOptionAll.transform = [](const EntryState& s) {
    //     auto element = text(s.label) | center | borderRounded;
    //     if (s.focused) {
    //         element |= inverted;
    //     }
    //     return element;
    // };
    Component buttonScheduleFilter = Button("Filter", [&] {
        listScheduleID = callGetApointmentIDList(filterScheduleDay, filterScheduleMonth, filterScheduleYear, 0, 0, filterScheduleServices, filterScheduleStatus, filterScheduleCustomerID, currentUserID, countSchedule);        
    }, buttonOptionAll);
    Component buttonScheduleResetFilter = Button("Reset", [&] {
        resetScheduleFilter();
    }, buttonOptionAll);

    Component buttonScheduleToday = Button("Filter Today", [&] {
        listScheduleID = callGetApointmentIDList(-1, -1, -1, 0, 0, filterScheduleServices, filterScheduleStatus, filterScheduleCustomerID, currentUserID, countSchedule);        
    }, buttonOptionAll);
    Component buttonScheduleNow   = Button(" Filter Now ", [&] {
        listScheduleID = callGetApointmentIDList(-1, -1, -1, -1, -1, filterScheduleServices, filterScheduleStatus, filterScheduleCustomerID, currentUserID, countSchedule);        
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
                text("Date") | size(WIDTH, EQUAL, 10),
                rendererScheduleFilterDate->Render() | size(WIDTH, EQUAL, 10),
            }),
            text("Services"),
            containerFilterServices->Render(),
            text("Status"),
            radioboxScheduleStatus->Render(),
            hbox({
                text("CustomerID") | size(WIDTH, EQUAL, 11),
                inputCustomerID->Render() | size(WIDTH, EQUAL, 10),
            }),
            filler(),
            hbox({
                buttonScheduleFilter->Render() | size(WIDTH, EQUAL, 10),
                buttonScheduleResetFilter->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter,
            buttonScheduleToday->Render() | hcenter,
            buttonScheduleNow  ->Render() | hcenter,
        }) | size(WIDTH, EQUAL, 21);
    });

    // HistoryList
    std::string detailScheduleID;
    std::string detailScheduleCustomerName;
    std::string detailScheduleStatus;
    std::string detailScheduleDate;
    std::string detailScheduleTime;
    std::vector<std::string> detailScheduleServices;
    std::string detailScheduleStylist;
    std::string detailScheduleRequirement;
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
        detailScheduleRequirement = callGetAppointmentRequirementByID(id);
    };

    listScheduleID = callGetApointmentIDList(filterScheduleDay, filterScheduleMonth, filterScheduleYear, 0, 0, filterScheduleServices, filterScheduleStatus, filterScheduleCustomerID, currentUserID, countSchedule);

    Component containerScheduleHistoryList = Container::Vertical({});
    for (int i = 0; i < listScheduleID.size(); ++i) {
        std::string id = listScheduleID[i];
        Component c = Button("Detail", [&, id] {
            tabDetail = 1;
            setDetailSchedule(id);
            detailScheduleID = id;
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
                text(id) | size(WIDTH, EQUAL, 9),
                text(status) | size(WIDTH, EQUAL, 10),
                text(date) | size(WIDTH, EQUAL, 14),
                text(time) | size(WIDTH, EQUAL, 7),
                text(services) | size(WIDTH, EQUAL, 16),
                separator(),
                c->Render() | center | size(WIDTH, EQUAL, 8),
            });
        }));
    }

    Component rendererScheduleHistoryList = Renderer(containerScheduleHistoryList ,[&] {
        return vbox({
            text("Appointment List") | center | bold,
            separator(),
            
            hbox({
                text("ID") | size(WIDTH, EQUAL, 9),
                text("Status") | size(WIDTH, EQUAL, 10),
                text("Date") | size(WIDTH, EQUAL, 14),
                text("Time") | size(WIDTH, EQUAL, 7),
                text("Services") | size(WIDTH, EQUAL, 16),
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
    Component buttonScheduleDetailBack = Button("Back", [&] {
        tabDetail = 0;
    }, buttonOptionAll);

    Component rendererTabDetail = Renderer(Container::Vertical({
        buttonScheduleDetailBack,
    }),[&] {
        auto textService = [&](int i) {
            return hbox({
                text(detailScheduleServices[i]) | size(WIDTH, EQUAL, 20),
            });
        };
        Elements textServiceList;
        for (int i = 0; i < detailScheduleServices.size(); ++i) {
            textServiceList.push_back(textService(i));
        }
        return vbox({
            text("Detail Appointment") | center,
            separator(),
            text("Appointment ID") | bold,
            text(detailScheduleID),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Customer") | bold,
            text(detailScheduleCustomerName),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Stylist") | bold,
            text(detailScheduleStylist) | size(WIDTH, EQUAL, 20),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Services") | size(WIDTH, EQUAL, 20) | bold,
            vbox(textServiceList),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Date and time") | size(WIDTH, EQUAL, 40) | bold,
            text("Date: " + detailScheduleDate) | size(WIDTH, EQUAL, 40),
            text("Time: " + detailScheduleTime) | size(WIDTH, EQUAL, 40),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Requirement: ") | size(WIDTH, EQUAL, 40) | bold,
            paragraph(detailScheduleRequirement) | size(WIDTH, EQUAL, 40),
            filler(),
            separator(),
            buttonScheduleDetailBack->Render() | center,
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
    bool filterServiceDoneRating[5] = {true, true, true, true, true}; // 1-5 stars
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
        std::fill(filterServiceDoneRating, filterServiceDoneRating + 5, true);
        std::fill(filterServiceDoneStatus, filterServiceDoneStatus + 2, true);
        std::fill(filterServiceDoneServices, filterServiceDoneServices + serviceCount, true);
    };
    resetServiceDoneFilter();

    MenuOption menuOptionServiceDoneMonth = MenuOption(menuOptionAll);
    menuOptionServiceDoneMonth.on_change = [&] () {
        int curYear = 2020 + filterServiceDoneYear;
        int curMonth = filterServiceDoneMonth;
        int curDay = filterServiceDoneDay;
        bool isLeapYear = (curYear % 4 == 0 && curYear % 100 != 0) || (curYear % 400 == 0);
        int maxDay; 
        if (filterServiceDoneYear == 0) {
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
            filterServiceDoneDay = maxDay;
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
    Component inputServiceDoneCustomerID = Input(&filterServiceDoneCustomerID, "CustomerID", inputOptionAll);

    Component containerServiceDoneRating = Container::Vertical({});
    for (int i = 0; i < 5; ++i) {
        containerServiceDoneRating->Add(Checkbox(std::to_string(i + 1) + " Star", &filterServiceDoneRating[i]));
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
                text("Date") | size(WIDTH, EQUAL, 10),
                rendererServiceDoneFilterDate->Render() | size(WIDTH, EQUAL, 10),
            }),
            hbox({
                text("CustomerID") | size(WIDTH, EQUAL, 11),
                inputServiceDoneCustomerID->Render() | size(WIDTH, EQUAL, 9),
            }),
            text("Rating"),
            containerServiceDoneRating->Render(),
            text("Status"),
            containerServiceDoneStatus->Render(),
            text("Services"),
            containerServiceDoneServices->Render(),
            hbox({
                buttonServiceDoneFilter->Render() | size(WIDTH, EQUAL, 10),
                buttonServiceDoneResetFilter->Render() | size(WIDTH, EQUAL, 10),
            }) | hcenter
        });
    });

    // ServiceDone list
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
                text(id) | size(WIDTH, EQUAL, 10),
                text(rating) | size(WIDTH, EQUAL, 10),
                text(date) | size(WIDTH, EQUAL, 15),
                text(status) | size(WIDTH, EQUAL, 10),
                text(service) | size(WIDTH, EQUAL, 12),
                separator(),
                c->Render() | center | size(WIDTH, EQUAL, 8),
            });
        }));
    }

    Component rendererServiceDoneList = Renderer(containerServiceDoneList ,[&] {
        return vbox({
            hbox({
                text("ID") | size(WIDTH, EQUAL, 10),
                text("Rating") | size(WIDTH, EQUAL, 10),
                text("Date") | size(WIDTH, EQUAL, 15),
                text("Status") | size(WIDTH, EQUAL, 10),
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
            text("  " + detailServiceDoneRating),
            text("Date") | bold,
            text("  " + detailServiceDoneDate),
            text("Status") | bold,
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
    Component inputAge = Input(&age, "Age", inputOptionProfile);

    ComponentDecorator disableInput = CatchEvent([&](Event event) {
        return true;
    });

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

