#include <iostream>
#include "define.h"
#include "Database.h"
#include "serviceDone.h"
#include "Customer.h"

using namespace std;

int main() {
    Database<serviceDone>& dbServiceDone = Database<serviceDone>::Connect(SERVICE_DONE_FILE);
    dbServiceDone.Query("feedback","hehe").Query("workerID","null").Show();
    cout << "======================================\n";
    dbServiceDone.Show();
    cout << "======================================\n";
    dbServiceDone.Query("feedback","huhu").Show();
    cout << "======================================\n";
    vector<serviceDone> vt = dbServiceDone.Query("feedback","hehe").GetResults();
    for (const auto& x : vt) {
        cout << x << '\n';
    }
    return 0;
}