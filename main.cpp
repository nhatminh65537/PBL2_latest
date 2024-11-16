#include <iostream>
#include "define.h"
#include "Database.h"
#include "serviceDone.h"
#include "Customer.h"

using namespace std;

int main() {
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

    return 0;
}