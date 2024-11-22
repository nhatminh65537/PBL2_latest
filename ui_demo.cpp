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
