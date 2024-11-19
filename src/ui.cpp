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
