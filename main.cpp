#include <iostream>
#include "define.h"
#include "Database.h"
#include "serviceDone.h"
#include "Customer.h"

using namespace std;

int main() {
    Database<serviceDone>& dbServiceDone = Database<serviceDone>::Connect(SERVICE_DONE_FILE);
    //dbServiceDone.Show();
    vector<serviceDone> vt = dbServiceDone.Query("feedback","hehe");
    for (auto x : vt) {
        cout << x << '\n';
    }
    return 0;
}