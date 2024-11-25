#include <iostream>

#ifndef TEST_UI

#include "define.h"
#include "Database.h"
#include "serviceDone.h"
#include "Customer.h"

void test() {
    dbServiceDone.Show();
    cout << "=======================================\n";
    for (auto it : dbServiceDone.indexMapList) {
        cout << it.first << "\n";
        for (auto it2 : it.second) {
            cout << it2.first << " " << it2.second << "\n";
        }
    }
    dbServiceDone.Update("2","customerID","hehe");
    dbServiceDone.Update("2","workerID","hehe");
    dbServiceDone.Update("3","workerID","hehe");
    cout << "=======================================\n";
    for (auto it : dbServiceDone.indexMapList) {
        cout << it.first << "\n";
        for (auto it2 : it.second) {
            cout << it2.first << " " << it2.second << "\n";
        }
    }
    cout << "=======================================\n";
    dbServiceDone.Delete("2");
     for (auto it : dbServiceDone.indexMapList) {
         cout << it.first << "\n";
         for (auto it2 : it.second) {
             cout << it2.first << " " << it2.second << "\n";
         }
    }
}

#else

#include "ui.h"

std::function<void()> currentScreen = screenAdmin;
void test() {
    while(currentScreen != nullptr) {
        currentScreen();
    }
}
#endif

using namespace std;


int main() {
    test();
    return 0;
}