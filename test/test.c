#include "query_test.c"
#include "document_test.c"
#include "utils.h"
#include "hashmap_test.c"
#include <stdio.h>

int main() {
  {
  //tests query --> string, free,linear
  query_tests();
  //tests documents --> desserialize,load,links
  document_tests();
  //tests hashmap
  hashmap_tests_main();
  }
  allsuccess();
}
