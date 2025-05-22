#include "sample_lib_test.c"
#include "query_test.c"
#include "document_test.c"
#include "utils.h"
#include <stdio.h>

int main() {
  {
  // Call all test modules you want to run here
  sample_lib_test();
  //tests query --> string, free,linear
  query_tests();
  //tests documents --> desserialize,load,links
  document_tests();
  }
  allsuccess();
}