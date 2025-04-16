#include "sample_lib.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "document.h" //cridar a docu.h

void createaleak() {
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}

int main() {
  //printf("*****************\nWelcome to EDA 2!\n*****************\n");

  // how to import and call a function
  //printf("Factorial of 4 is %d\n", fact(4));

  // uncomment and run "make v" to see how valgrind detects memory leaks hola
  // createaleak();

  Document* docu = document_desserialize("./datasets/wikipedia12/2.txt");
  if (docu) {
    printf("ID: %d\n", docu->id);
    printf("Title: %s\n", docu->title);
    printf("Body: %s\n", docu->body);
    
    // Imprimeix els enllaços
    Links* current = docu->links;
    while (current) {
        printf("Link ID: %d, Text: %s\n", current->documentId, current->linkText);
        current = current->next;
    }
    
    freeDocument(docu); // Allibera la memòria
} else {
    printf("Error al llegir el document.\n");
}
return 0;

}
