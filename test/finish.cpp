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

void welcome_screen()
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
            currentScreen = login_screen;
            screen.Exit();
        }, myButtonOption) | size(WIDTH, EQUAL, 20),
        Button("Register", [&]{
            currentScreen = register_screen;
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

void call_login(std::string username, std::string password, int role)
{
    if (username.empty() && password.empty()) 
        throw ERROR_CODE::LOGIN_USER_AND_PASS_EMPTY;
    if (username.empty()) 
        throw ERROR_CODE::LOGIN_USERNAME_EMPTY;
    if (password.empty()) 
        throw ERROR_CODE::LOGIN_PASSWORD_EMPTY;
}

std::vector<std::string> get_stylist()
{
    std::vector<std::string> stylist = {
        "None",
        "Stylist 1",
        "Stylist 2",
        "Stylist 3",
        "Stylist 4",
        "Stylist 5",
    };
    return stylist;
}

void login_screen()
{
    // Error handle
    std::string username_error = "";
    std::string password_error = "";
    std::string role_error     = "";
    auto error_handle = [&] (int error_code) {
        switch (error_code)
        {
            case ERROR_CODE::LOGIN_USERNAME_EMPTY:
                username_error = "Username is empty";
                break;
            case ERROR_CODE::LOGIN_PASSWORD_EMPTY:
                password_error = "Password is empty";
                break;
            case ERROR_CODE::LOGIN_USER_AND_PASS_EMPTY:
                username_error = "Username is empty";
                password_error = "Password is empty";
                break;
            default:
                break;
        }
    };

    // login screen
    auto screen = ScreenInteractive::FixedSize(120, 30);

    std::string username;
    std::string password;
    int role = 0;

    // Input components
    InputOption myInputOption = InputOption::Default();
    myInputOption.insert = true;

    InputOption username_option(myInputOption);
    Component input_username = Input(&username, "username", username_option);

    InputOption password_option(myInputOption);
    password_option.password = true;
    Component input_password = Input(&password, "password", password_option);

    // Radio
    std::vector<std::string> roles = {
        "Stylist",
        "Customer",
        "Admin"
    };
    Component radio = Radiobox(&roles, &role);

    // Button
    ButtonOption myButtonOption;
    myButtonOption.transform = [](const EntryState& s) {
        auto element = text(s.label) | center | borderRounded;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    Component login_button = Button("Login", [&] {
        username_error = "";
        password_error = "";
        role_error     = "";
        try {
            call_login(username, password, role);
            screen.Exit();
            currentScreen = customer_screen;
        }
        catch (int error_code) {
            error_handle(error_code);
        }
    }, myButtonOption) | size(WIDTH, EQUAL, 15);
    Component back_button  = Button("Back" , [&] {
        currentScreen = welcome_screen;
        screen.Exit();
    }, myButtonOption) | size(WIDTH, EQUAL, 15);
    int selected_children = 1;
    Component buttons = Container::Horizontal({
        back_button,
        login_button,
    }, &selected_children);

    // Event
    input_username |= CatchEvent([&] (Event event) {
        bool check = event == Event::Return;
        if (check) {
            input_password->TakeFocus();
        }
        return check;
    });
    input_password |= CatchEvent([&] (Event event) {
    bool check = event == Event::Return;
    if (check) {
        radio->TakeFocus();
    }
    return check;
    });
    radio |= CatchEvent([&] (Event event) {
        bool check = event == Event::Tab;
        if (check) {
            buttons->TakeFocus();
        }
        return check;
    });

    // All
    Component container = Container::Vertical({
        input_username,
        input_password,
        radio,
        buttons,
    });

    auto login = Renderer(container, [&] {
        HIDE_CURSOR;
        return vbox({
            text("Enter username") | hcenter,
            input_username->Render() | borderRounded | size(WIDTH, EQUAL, 30),
            text(username_error) | align_right | size(HEIGHT, EQUAL, username_error.size() > 0 ? 1 : 0) | color(Color::Red),
            text("Enter password") | hcenter,
            input_password->Render() | borderRounded | size(WIDTH, EQUAL, 30),
            text(password_error) | align_right | size(HEIGHT, EQUAL, password_error.size() > 0 ? 1 : 0) | color(Color::Red),
            text("Select role") | hcenter,
            radio->Render(),
            buttons->Render(),
        }) | center;
    });

    screen.Loop(login);
}

void register_screen()
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
    InputOption myInputOption = InputOption::Default();
    myInputOption.placeholder = "";

    InputOption firstname_option(myInputOption);
    Component input_firstname = Input(&firstname, "First Name", firstname_option);

    InputOption lastname_option(myInputOption);
    Component input_lastname = Input(&lastname, "Last Name", lastname_option);

    InputOption username_option(myInputOption);
    Component input_username = Input(&username, "Username", username_option);

    InputOption password_option(myInputOption);
    password_option.password = true;
    Component input_password = Input(&password, "Password", password_option);

    InputOption confirmpassword_option(myInputOption);
    confirmpassword_option.password = true;
    Component input_confirmpassword = Input(&confirmpassword, "Confirm Password", confirmpassword_option);

    InputOption phonenum_option(myInputOption);
    Component input_phonenum = Input(&phonenum, "Phone Number", phonenum_option);

    InputOption age_option(myInputOption);
    Component input_age = Input(&age, "Age", age_option);

    ComponentDecorator numberOnly = CatchEvent([&](Event event) {
        return event.is_character() && !std::isdigit(event.character()[0]);
    });
    input_phonenum |= numberOnly;
    input_phonenum |=  CatchEvent([&](Event event) {
        return event.is_character() && phonenum.size() >= 11;
    });
    input_age |= numberOnly;
    input_age |= CatchEvent([&](Event event) {
        return event.is_character() && age.size() >= 3;
    });


    int selected_input = 0;
    Component inputs = Container::Vertical({
        input_firstname,
        input_lastname,
        input_username,
        input_password,
        input_confirmpassword,
        input_phonenum,
        input_age,
    }, &selected_input);
    inputs |= CatchEvent([&] (Event event) {
        bool check = event == Event::Return;
        if (check && selected_input < 6) {
            selected_input++;
            // inputs->ChildAt(selected_input);
        }
        return check;
    });
    inputs |= CatchEvent([&] (Event event){
        bool check = event == Event::TabReverse;
        if (check && selected_input > 0) {
            selected_input--;
        }
        return check;
    });

    // Radio
    std::vector <std::string> genders = {"Male", "Female"};
    Component radio_gender = Radiobox(&genders, &gender);

    std::vector <std::string> roles = {"Customer"};
    Component radio_role = Radiobox(&roles, &role);

    // Button
    ButtonOption myButtonOption;
    myButtonOption.transform = [](const EntryState& s) {
        auto element = text(s.label) | center | borderRounded;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    Component register_button = Button("Register", [&] {}, myButtonOption) | size(WIDTH, EQUAL, 15);
    Component back_button  = Button("Back" , [&] {
        currentScreen = welcome_screen;
        screen.Exit();
    }, myButtonOption) | size(WIDTH, EQUAL, 15);
    int selected_children = 1;
    Component buttons = Container::Horizontal({
        back_button,
        register_button,
    }, &selected_children);

    auto register_renderer = Renderer(Container::Vertical({inputs, radio_gender, radio_role, buttons,}), [&] {
        return vbox({
        vbox({
            hbox({text("First Name      : "), input_firstname->Render()}),
            hbox({text("Last Name       : "), input_lastname->Render()}),
            hbox({text("Username        : "), input_username->Render()}),
            hbox({text("Password        : "), input_password->Render()}),
            hbox({text("Confirm Password: "), input_confirmpassword->Render()}),
            hbox({text("Phone Number    : "), input_phonenum->Render()}),
            hbox({text("Age             : "), input_age->Render()}),
            hbox({text("Gender          : "), radio_gender->Render()}),
            hbox({text("Role            : "), radio_role->Render()}),
        }) | size(WIDTH, EQUAL, 50) | borderRounded,
        buttons->Render() | center
        }) | center;
    });
    
    try {
        screen.Loop(register_renderer);
    }
    catch (...) {
        std::cerr << "Error" << '\n';
    }
}
