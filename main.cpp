#include <iostream>
#include "Database.h"
#include "serviceDone.h"

using namespace std;

void test() {
    Database<serviceDone>& dbServiceDone = Database<serviceDone>::Connect(SERVICE_DONE_FILE);
    //dbServiceDone.Show();
    //dbServiceDone.Query("feedback","hehe").Show();
    vector<serviceDone> vt = dbServiceDone.Query("feedback","hehe").GetResults();
    for (const serviceDone &v : vt) v.Show();
    vector<serviceDone> dt = dbServiceDone.GetResults();
    for (const serviceDone &v : dt) v.Show();
}


int main() {
    test();
    return 0;
}