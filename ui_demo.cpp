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
    // function variables
    std::string currentUserID = callGetCurrentUserID();
    std::vector<std::string> services;
    int serviceCount = callGetServiceList(services);
    std::vector <std::string> listGenders = {"Male", "Female"};

    // test stuff
    std::string name = "John Doe";
    std::ofstream log;
    log.open("log.txt");

    // customer screen
    auto screen = ScreenInteractive::FixedSize(SCREEN_WIDTH, SCREEN_HEIGHT);
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
    Component buttonServiceDone = Button("Service Done", [&] {
        selectedTab = 3;
    }, buttonOptionTab);
    Component buttonCustomer = Button("Customer", [&] {
        selectedTab = 4;
    }, buttonOptionTab);
    Component buttonStylist = Button("Stylist", [&] { 
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

    // Home tab
    Component tabHome = Renderer([&] {
        return text("Home") | center ;
    });

    // Statistics tab
    Component tabStatistics = Renderer([&] {
        return text("Statistics") | center;
    });

    // Appointment tab
    #pragma region Appointment

    std::vector<std::string> listAppointmentID;
    int countAppointment = 0;

    // Filter
    int filterAppointmentDay;
    int filterAppointmentMonth;
    int filterAppointmentYear;
    int filterAppointmentHour;
    int filterAppointmentMinute;
    bool *filterAppointmentServices = new bool[serviceCount];
    int filterAppointmentStatus;
    std::string filterAppointmentCustomerID;
    std::string filterAppointmentStylistID;
    std::vector<std::string> appointmentDays = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    std::vector<std::string> appointmentMonths = {"--", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"};
    std::vector<std::string> appointmentYears = {"----", "2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029", "2030"};
    std::vector<std::string> appointmentHours = {"--", "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"};
    std::vector<std::string> appointmentMinutes = {"--", "00", "30"};
    std::vector<std::string> appointmentStatus = {"All", "Done", "Pending", "Cancel"};
    

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
    MenuOption menuOptionAll;
    menuOptionAll.direction = Direction::Up;
    menuOptionAll.entries_option.transform = [](const EntryState& s) {
        auto element = text(s.label) | center;
        if (s.focused) {
            element = element | inverted;
        }
        return element;
    };

    Component menuAppointmentFilterDay = Menu(&appointmentDays, &filterAppointmentDay, menuOptionAll);
    MenuOption menuOptionAppointmentMonth = MenuOption(menuOptionAll);
    menuOptionAppointmentMonth.on_change = ([&] () {
        int curYear = 2020 + filterAppointmentYear;
        int curMonth = filterAppointmentMonth;
        int curDay = filterAppointmentDay;
        bool isLeapYear = (curYear % 4 == 0 && curYear % 100 != 0) || (curYear % 400 == 0);
        int maxDay;
        if (filterAppointmentYear == 0) {
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
            filterAppointmentDay = maxDay;
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
    for (int i = 0; i < SERVICES_COUNT; ++i) {
        filterAppointmentServices[i] = true;
        containerFilterServices->Add(Checkbox(services[i], &filterAppointmentServices[i]) | size(WIDTH, EQUAL, 20));
    }

    Component radioboxAppointmentStatus = Radiobox(appointmentStatus, &filterAppointmentStatus);
    InputOption inputOptionAll = InputOption::Default();
    inputOptionAll.multiline = false;
    Component inputCustomerID = Input(&filterAppointmentCustomerID, "Customer ID", inputOptionAll);
    Component inputStylistID = Input(&filterAppointmentStylistID, "Stylist ID", inputOptionAll);

    ButtonOption buttonOptionAll;
    buttonOptionAll.transform = [](const EntryState& s) {
        auto element = text(s.label) | center | borderRounded;
        if (s.focused) {
            element |= inverted;
        }
        return element;
    };
    Component buttonAppointmentFilter = Button("Filter", [&] {
        listAppointmentID = callGetApointmentIDList(filterAppointmentDay, filterAppointmentMonth, filterAppointmentYear, filterAppointmentHour, filterAppointmentMinute, filterAppointmentServices, filterAppointmentStatus, filterAppointmentCustomerID, filterAppointmentStylistID, countAppointment);        
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
                text("Date") | size(WIDTH, EQUAL, 10),
                rendererAppointmentFilterDate->Render() | size(WIDTH, EQUAL, 10),
            }),
            hbox({
                text("Time") | size(WIDTH, EQUAL, 10),
                rendererAppointmentFilterTime->Render() | size(WIDTH, EQUAL, 10),
            }),
            text("Services"),
            containerFilterServices->Render(),
            text("Status"),
            radioboxAppointmentStatus->Render(),
            hbox({
                text("Customer ID") | size(WIDTH, EQUAL, 12),
                inputCustomerID->Render() | size(WIDTH, EQUAL, 12),
            }),
            hbox({
                text("Stylist ID") | size(WIDTH, EQUAL, 12),
                inputStylistID->Render() | size(WIDTH, EQUAL, 12),
            }),
            filler(),
            hbox({
                buttonAppointmentFilter->Render() | size(WIDTH, EQUAL, 10),
                buttonAppointmentResetFilter->Render() | size(WIDTH, EQUAL, 10),
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
    std::string detailAppointmentStylist;
    std::string detailAppointmentRequirement;
    int tabDetail = 0;

    auto setDetailAppointment = [&] (std::string id) {
        detailAppointmentCustomerName = callGetAppointmentCustomerNameByID(id) + " (ID: " + callGetAppointmentCustomerIDByID(id) + ")";
        detailAppointmentStatus = callGetAppointmentStatusByID(id);
        detailAppointmentDate = callGetAppointmentDateByID(id);
        detailAppointmentTime = callGetAppointmentTimeByID(id);
        detailAppointmentServices = callGetAppointmentServicesByID(id);
        detailAppointmentStylist = callGetAppointmentStylistByID(id);
        if (detailAppointmentStylist != "None") {
            detailAppointmentStylist += " (ID: " + callGetAppointmentStylistIDByID(id) + ")";
        }
        detailAppointmentRequirement = callGetAppointmentRequirementByID(id);
    };

    listAppointmentID = callGetApointmentIDList(filterAppointmentDay, filterAppointmentMonth, filterAppointmentYear, filterAppointmentHour, filterAppointmentMinute, filterAppointmentServices, filterAppointmentStatus, filterAppointmentCustomerID, filterAppointmentStylistID, countAppointment);

    Component containerAppointmentHistoryList = Container::Vertical({});
    auto reloadAppointmentHistoryList = [&] () {
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
                    text(id) | size(WIDTH, EQUAL, 9),
                    text(status) | size(WIDTH, EQUAL, 10),
                    text(date) | size(WIDTH, EQUAL, 12),
                    text(time) | size(WIDTH, EQUAL, 7),
                    text(services) | size(WIDTH, EQUAL, 15),
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
                text("ID") | size(WIDTH, EQUAL, 9),
                text("Status") | size(WIDTH, EQUAL, 10),
                text("Date") | size(WIDTH, EQUAL, 12),
                text("Time") | size(WIDTH, EQUAL, 7),
                text("Services") | size(WIDTH, EQUAL, 15),
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
    std::string errorAppointmentDetailStylistChosen = "";
    Component buttonAppointmentDetailCancel = Button("Cancel", [&] {
        callCancelAppointment(detailAppointmentID);
        detailAppointmentStatus = callGetAppointmentStatusByID(detailAppointmentID);
    }, buttonOptionAll);
    Component buttonAppointmentDetailBack = Button("Back", [&] {
        errorAppointmentDetailStylistChosen = "";
        reloadAppointmentHistoryList();

        tabDetail = 0;
    }, buttonOptionAll);
    Component buttonAppointmentDetailDone = Button("Done", [&] {
        callDoneAppointment(detailAppointmentID);
        detailAppointmentStatus = callGetAppointmentStatusByID(detailAppointmentID);
    }, buttonOptionAll);
    Component buttonAppointmentDetailChoose = Button("Choose stylist", [&] {
        if (detailAppointmentStylist == "None") {
            tabDetail = 2;
        }
        else {
            errorAppointmentDetailStylistChosen = "Stylist has been chosen";
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
                text(detailAppointmentServices[i]) | size(WIDTH, EQUAL, 20)
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
            text("Customer") | bold,
            text(detailAppointmentCustomerName),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Stylist") | bold,
            text(detailAppointmentStylist) | size(WIDTH, EQUAL, 20),
            filler() | size(HEIGHT, EQUAL, 1),
            text("Services") | size(WIDTH, EQUAL, 20) | bold,
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
                buttonAppointmentDetailDone->Render() | size(WIDTH, EQUAL, 10),
                filler() | size(WIDTH, EQUAL, 5),
                buttonAppointmentDetailCancel->Render() | size(WIDTH, EQUAL, 10),
            })| hcenter,
            buttonAppointmentDetailChoose->Render() | size(WIDTH, EQUAL, 20) | center,
            text(errorAppointmentDetailStylistChosen) | align_right | color(Color::Red) | size(HEIGHT, EQUAL, errorAppointmentDetailStylistChosen == "" ? 0 : 1),
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
        }),
    }), [&] {
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

    // Customer filter
    std::string customerFilterName;
    std::string customerFilterAge;
    bool customerFilterGender[2] = {true, true}; // 0: Male, 1: Female

    auto resetCustomerFilter = [&] {
        customerFilterName = "";
        customerFilterAge = "";
        customerFilterGender[0] = true;
        customerFilterGender[1] = true;
    };
    resetCustomerFilter();

    Component checkboxCustomerGenderMale = Checkbox("Male", &customerFilterGender[0]);
    Component checkboxCustomerGenderFemale = Checkbox("Female", &customerFilterGender[1]);
    Component containerCustomerGender = Container::Vertical({
        checkboxCustomerGenderMale,
        checkboxCustomerGenderFemale,
    });

    InputOption inputOptionCustomerAll = InputOption::Default();
    inputOptionCustomerAll.multiline = false;
    Component inputCustomerAge = Input(&customerFilterAge, "Age", inputOptionCustomerAll);
    Component inputCustomerName = Input(&customerFilterName, "Name", inputOptionCustomerAll);

    Component buttonCustomerFilter = Button("Filter", [&] {
        customerIDList = callGetCustomerIDList(customerFilterGender, customerFilterName, customerFilterAge, countCustomer);
    }, buttonOptionAll);
    Component buttonCustomerResetFilter = Button("Reset", [&] {
        resetCustomerFilter();
    }, buttonOptionAll);

    int childCustomerFilter = 0;
    Component containerCustomerFilter = Container::Vertical({
        containerCustomerGender,
        inputCustomerAge,
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
                text("Age : "),
                inputCustomerAge->Render(),
            }),
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
    int detailTabCustomer = 0;
    std::string detailCustomerID;
    std::string detailCustomerName;
    std::string detailCustomerGender;
    std::string detailCustomerAge;
    std::string detailCustomerPhonenumber;

    customerIDList = callGetCustomerIDList(customerFilterGender, customerFilterName, customerFilterAge, countCustomer);

    auto setDetailCustomer = [&] (std::string id) {
        detailCustomerName = callGetMemberNameByID(id);
        detailCustomerGender = callGetMemberGenderByID(id);
        detailCustomerAge = callGetMemberAgeByID(id);
        detailCustomerPhonenumber = callGetMemberPhoneByID(id);
    };

    Component containerCustomerList = Container::Vertical({});
    for (int i = 0; i < customerIDList.size(); ++i) {
        std::string cid = customerIDList[i];
        Component c = Button("Detail", [&, cid] {
            detailCustomerID = cid;
            setDetailCustomer(detailCustomerID);
            detailTabCustomer = 1;
        }, buttonOptionTab);
        
        std::string id = customerIDList[i];
        std::string name = callGetMemberNameByID(id);
        std::string gender = callGetMemberGenderByID(id);
        std::string age = callGetMemberAgeByID(id);
        std::string phonenumber = callGetMemberPhoneByID(id);
        containerCustomerList->Add(Renderer(
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

    Component rendererCustomerList = Renderer(
        Container::Vertical({
            containerCustomerList,
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
            containerCustomerList->Render() | vscroll_indicator | frame,
            separator(),
            filler(),
            separator(),
            hbox({
                text("Total: " + std::to_string(countCustomer)), 
                filler(),
            }) | align_right,
        });
    });


    // Customer detail

    Component buttonCustomerDetailBack = Button("Back", [&] {
        detailTabCustomer = 0;
    }, buttonOptionAll);

    Component rendererCustomerDetail = Renderer(Container::Vertical({
        buttonCustomerDetailBack,
    }), [&] {
        return vbox({
            text("Customer Detail") | center,
            separator(),
            text("ID") | bold,
            text("  " + detailCustomerID),
            text("Name") | bold,
            text("  " + detailCustomerName),
            text("Gender") | bold,
            text("  " + detailCustomerGender),
            text("Age") | bold,
            text("  " + detailCustomerAge),
            text("Phonenumber") | bold,
            text("  " + detailCustomerPhonenumber),
            text("Username") | bold,
            text("  " + callGetMemberUsernameByID(detailCustomerID)), // This style must be concerned
            separator(),
            buttonCustomerDetailBack->Render() | center,
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
    
    // Stylist filter
    bool stylistFilterGender[2] = {true, true}; // 0: Male, 1: Female
    std::string stylistFilterName;
    std::string stylistFilterAge;

    auto resetStylistFilter = [&] {
        stylistFilterGender[0] = true;
        stylistFilterGender[1] = true;
        stylistFilterName = "";
        stylistFilterAge = "";
    };
    resetStylistFilter();

    Component checkboxStylistGenderMale = Checkbox("Male", &stylistFilterGender[0]);
    Component checkboxStylistGenderFeMale = Checkbox("Female", &stylistFilterGender[1]);
    Component containerStylistGender = Container::Vertical({
        checkboxStylistGenderMale,
        checkboxStylistGenderFeMale,
    });
    
    InputOption inputOptionStylistAll = InputOption::Default();
    inputOptionStylistAll.multiline = false;
    Component inputStylistAge = Input(&stylistFilterAge, "Age", inputOptionStylistAll);
    Component inputStylistName = Input(&stylistFilterName, "Name", inputOptionStylistAll);

    Component buttonStylistFilter = Button("Filter", [&] {
        stylistIDList = callGetStylistIDList(stylistFilterGender, stylistFilterName, stylistFilterAge, countStylist);
    }, buttonOptionAll);
    Component buttonStylistResetFilter = Button("Reset", [&] {
        resetStylistFilter();
    }, buttonOptionAll);
    
    int childStylistFilter = 0;
    Component containerStylistFilter = Container::Vertical({
        containerStylistGender,
        inputStylistAge,
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
    std::string detailStylistUsername;
    
    stylistIDList = callGetStylistIDList(stylistFilterGender, stylistFilterName, stylistFilterAge, countStylist);

    auto setDetailStylist = [&] (std::string id) {
        detailStylistName = callGetMemberNameByID(id);
        detailStylistGender = callGetMemberGenderByID(id);
        detailStylistAge = callGetMemberAgeByID(id);
        detailStylistPhonenumber = callGetMemberPhoneByID(id);
        detailStylistUsername = callGetMemberUsernameByID(id);
    };

    Component containerStylistList = Container::Vertical({});
    auto reloadStylistList = [&] () {
        containerStylistList->DetachAllChildren();
        for (int i = 0; i < stylistIDList.size(); ++i) {
            std::string sid = stylistIDList[i];
            Component c = Button("Detail", [&, sid] {
                selectedTabStylist = 1;
                detailStylistID = sid;
                setDetailStylist(detailStylistID);
            }, buttonOptionTab);
            
            std::string id = stylistIDList[i];
            std::string name = callGetMemberNameByID(id);
            std::string gender = callGetMemberGenderByID(id);
            std::string age = callGetMemberAgeByID(id);
            std::string phonenumber = callGetMemberPhoneByID(id);
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
    };
    reloadStylistList();

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
                text("Total: " + std::to_string(countStylist)), 
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

    auto resetUpdateStylist = [&] {
        stylistUpdateFirstName = callGetMemberFirstNameByID(detailStylistID);
        stylistUpdateLastName = callGetMemberLastNameByID(detailStylistID);
        stylistUpdateGender = detailStylistGender == "Male"? 0 : 1;
        stylistUpdateAge = detailStylistAge;
        stylistUpdatePhonenumber = detailStylistPhonenumber;
        stylistUpdateUsername = detailStylistUsername;
        stylistUpdatePassword = "";
    };

    Component buttonStylistDetailBack = Button("Back", [&] {
        resetUpdateStylist();

        selectedTabStylist = 0;
    }, buttonOptionAll);
    Component buttonStylistDetailModify = Button("Update", [&] {
        resetUpdateStylist();
        selectedTabStylist = 2;
    }, buttonOptionAll);
    Component buttonStylistDetailDelete = Button("Delete", [&] {
        callDeleteStylist(detailStylistID);
        stylistIDList = callGetStylistIDList(stylistFilterGender, stylistFilterName, stylistFilterAge, countStylist);
        resetUpdateStylist();

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
            text("Age") | bold,
            text("    " + detailStylistAge),
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
    Component inputUpdateStylistAge = Input(&stylistUpdateAge, "Age", inputOptionStylistAll);
    Component inputUpdateStylistPhonenumber = Input(&stylistUpdatePhonenumber, "Phone Number", inputOptionStylistAll);
    Component inputUpdateStylistUsername = Input(&stylistUpdateUsername, "Username", inputOptionStylistAll);
    Component inputUpdateStylistPassword = Input(&stylistUpdatePassword, "Password", inputOptionStylistAll);

    int selectedButtonsUpdateStylist = 0;
    Component buttonUpdateStylistBack = Button("Back", [&] {
        selectedTabStylist = 1;
    }, buttonOptionAll);
    Component buttonUpdateStylistReset = Button("Reset", [&] {
        resetUpdateStylist();
    }, buttonOptionAll);
    Component buttonUpdateStylist = Button("Update", [&] {
        callUpdateStylist(detailStylistID, stylistUpdateFirstName, stylistUpdateLastName, stylistUpdateGender, stylistUpdateAge, stylistUpdatePhonenumber, stylistUpdateUsername, stylistUpdatePassword);
        setDetailStylist(detailStylistID);
        selectedTabStylist = 1;
    }, buttonOptionAll);
    
    int selectedUpdateStylist = 0;
    Component containerUpdateStylist = Container::Vertical({
        inputUpdateStylistFirstName,
        inputUpdateStylistLastName,
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

    auto resetAddStylist = [&] {
        stylistAddFirstName = "";
        stylistAddLastName = "";
        stylistAddGender = 0;
        stylistAddAge = "";
        stylistAddPhonenumber = "";
        stylistAddUsername = "";
        stylistAddPassword = "";
    };
    resetAddStylist();

    Component inputAddStylistFirstName = Input(&stylistAddFirstName, "First Name", inputOptionStylistAll);
    Component inputAddStylistLastName = Input(&stylistAddLastName, "Last Name", inputOptionStylistAll);
    Component radioboxAddStylistGender = Radiobox(listGenders, &stylistAddGender);
    Component inputAddStylistAge = Input(&stylistAddAge, "Age", inputOptionStylistAll);
    Component inputAddStylistPhonenumber = Input(&stylistAddPhonenumber, "Phone Number", inputOptionStylistAll);
    Component inputAddStylistUsername = Input(&stylistAddUsername, "Username", inputOptionStylistAll);
    Component inputAddStylistPassword = Input(&stylistAddPassword, "Password", inputOptionStylistAll);

    Component buttonAddStylistBack = Button("Back", [&] {
        selectedTabStylist = 0;
    }, buttonOptionAll);
    Component buttonAddStylistReset = Button("Reset", [&] {
        resetAddStylist();
    }, buttonOptionAll);
    Component buttonAddStylistAdd = Button("Add", [&] {
        callAddStylist(stylistAddFirstName, stylistAddLastName, stylistAddGender, stylistAddAge, stylistAddPhonenumber, stylistAddUsername, stylistAddPassword);
        stylistIDList = callGetStylistIDList(stylistFilterGender, stylistFilterName, stylistFilterAge, countStylist);
        reloadStylistList();

        selectedTabStylist = 0;
    }, buttonOptionAll);

    int selectedAddStylist = 0;
    Component containerAddStylist = Container::Vertical({
        inputAddStylistFirstName,
        inputAddStylistLastName,
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
        bool check = (event == Event::Tab) && (selectedAddStylist < 7); // 8 is the number of small component in containerAddStylist
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
    #pragma region Profile
    std::string profileFirstname;
    std::string profileLastname;
    std::string profileUsername;
    std::string profileOldPassword;
    std::string profileNewPassword;
    std::string profileConfirmpassword;
    std::string profilePhonenumber;
    std::string profileAge;
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
        callGetCurrentUserPersonInfo(profilePhonenumber, profileAge, profileGender);
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
    Component inputAge = Input(&profileAge, "Age", inputOptionProfile);

    ComponentDecorator eventNumberOnly = CatchEvent([&](Event event) {
        return event.is_character() && !std::isdigit(event.character()[0]);
    });
    inputPhonenumber |= eventNumberOnly;
    inputPhonenumber |=  CatchEvent([&](Event event) {
        return event.is_character() && profilePhonenumber.size() >= 11;
    });
    inputAge |= eventNumberOnly;
    inputAge |= CatchEvent([&](Event event) {
        return event.is_character() && profileAge.size() >= 3;
    });
    ComponentDecorator eventAlphabetOnly = CatchEvent([&](Event event) {
        return event.is_character() && 
            !std::isalpha(event.character()[0]) && 
            !std::isspace(event.character()[0]);
    });
    inputFirstname |= eventAlphabetOnly;
    inputLastname  |= eventAlphabetOnly;

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
            callUpdateCurrentPersonInfo(profilePhonenumber, profileAge, profileGender);
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
    bool filterServiceDoneRating[5] = {true, true, true, true, true}; // 1-5 stars
    bool filterServiceDoneStatus[2] = {true, true}; // 0: False, 1: True
    bool *filterServiceDoneServices = new bool[serviceCount];
    std::vector<std::string> serviceDoneFilterDays = {"--", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    std::vector<std::string> serviceDoneFilterMonths = {"--", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"};
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
    resetServiceDoneFilter();

    MenuOption menuOptionserviceDoneMonth = MenuOption(menuOptionAll);
    menuOptionserviceDoneMonth.on_change = [&] {
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
    for (int i = 0; i < 5; ++i) {
        containerServiceDoneRating->Add(Checkbox(std::to_string(i + 1) + " Star", &filterServiceDoneRating[i]) | size(WIDTH, EQUAL, 20));
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
        serviceDoneIDList = callGetServiceDoneIDList(filterServiceDoneDay, filterServiceDoneMonth, filterServiceDoneYear, filterServiceDoneCustomerID, filterServiceDoneStylistID, filterServiceDoneRating, filterServiceDoneStatus, filterServiceDoneServices, countServiceDone);
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
                text("CustomerID ") | size(WIDTH, EQUAL, 11),
                inputServiceDoneCustomerID->Render() | size(WIDTH, EQUAL, 9),
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
            filler(),
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
    std::string detailServiceDoneStylistID;
    std::string detailServiceDoneRating;
    std::string detailServiceDoneDate;
    std::string detailServiceDoneStatus;
    std::string detailServiceDoneService;

    serviceDoneIDList = callGetServiceDoneIDList(filterServiceDoneDay, filterServiceDoneMonth, filterServiceDoneYear, filterServiceDoneCustomerID, filterServiceDoneStylistID, filterServiceDoneRating, filterServiceDoneStatus, filterServiceDoneServices, countServiceDone);

    auto setDetailServiceDone = [&] (std::string id) {
        detailServiceDoneCustomerID = callGetServiceDoneCustomerIDByID(id);
        detailServiceDoneStylistID = callGetServiceDoneStylistIDByID(id);
        detailServiceDoneRating = callGetServiceDoneRatingByID(id);
        detailServiceDoneDate = callGetServiceDoneDateByID(id);
        detailServiceDoneStatus = callGetServiceDoneStatusByID(id);
        detailServiceDoneService = callGetServiceDoneServiceByID(id);
    };

    Component containerServiceDoneList = Container::Vertical({});
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
                text(id) | size(WIDTH, EQUAL, 10),
                text(rating) | size(WIDTH, EQUAL, 8),
                text(date) | size(WIDTH, EQUAL, 15),
                text(status) | size(WIDTH, EQUAL, 12),
                text(service) | size(WIDTH, EQUAL, 12),
                separator(),
                c->Render() | center | size(WIDTH, EQUAL, 8),
            });
        }));
    }

    Component rendererServiceDoneList = Renderer(
        Container::Vertical({
            containerServiceDoneList,
        }), [&] {
        return vbox({
            hbox({
                text("ID") | size(WIDTH, EQUAL, 7),
                text("Rating") | size(WIDTH, EQUAL, 8),
                text("Date") | size(WIDTH, EQUAL, 15),
                text("Status") | size(WIDTH, EQUAL, 12),
                text("Service") | size(WIDTH, EQUAL, 12),
            }) | bold,
            separator(),
            containerServiceDoneList->Render() | vscroll_indicator | frame,
            separator(),
            filler(),
            separator(),
            text("    Total: " + std::to_string(countServiceDone)) | align_right, 
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
