#include "document.h" //cridar a document.h
#include "sample_lib.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void createaleak() {
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}

int main() {
  // printf("*****************\nWelcome to EDA 2!\n*****************\n");

  // how to import and call a function
  // printf("Factorial of 4 is %d\n", fact(4));

  // uncomment and run "make v" to see how valgrind detects memory leaks hola
  // createaleak();

  const char *directoryPath =
      "./datasets/wikipedia12"; // ruta del directori on es troben els documents

  // Crida la funció per llegir documents del directori
  loadAllDocuments(directoryPath);

  Document *documents = loadAllDocuments(directoryPath);

  // Comprovar si s'han carregat documents
  if (documents == NULL) {
    printf("No s'han trobat documents al directori especificat.\n");
    return 1; // Retornar un codi d'error
  }

  // Iterar sobre la llista de documents i mostrar la informació
  Document *current = documents;
  while (current != NULL) {
    printf("ID: %d\n", current->id);
    printf("Title: %s\n", current->title);
    printf("Body: %s\n", current->body);

    // Mostrar enllaços
    Links *linkCurrent =
        current->links; // Punter per recórrer la llista d'enllaços
    while (linkCurrent != NULL) {
      printf("Link ID: %d, Text: %s\n", linkCurrent->documentId,
             linkCurrent->linkText);
      linkCurrent = linkCurrent->next; // Passar al següent enllaç
    }

    current = current->next; // Passar al següent document
    printf("\n");            // Espai entre documents
  }

  // Alliberar la memòria dels documents carregats
  current = documents;
  while (current != NULL) {
    Document *temp = current;
    current = current->next;
    freeDocument(temp); // Alliberar cada document
  }

  return 0; // Retornar 0 per indicar que tot ha anat bé
}
