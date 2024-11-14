#include <iostream>
#include "Datetime.h"
// Thư viện Time chưa hoàn thiện, thiếu phần check xem lịch có nằm ở quá khứ hay không
using namespace std;

int main() {
  Datetime hehe = Datetime::Now();
  hehe.Show();
  return 0;
}