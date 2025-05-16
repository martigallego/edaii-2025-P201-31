#include "document.h"
#include "query.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//funció per mostrar tots els documents carregats (LAB1)
void lab1_printDocuments(Document *documents) {
  Document *current = documents;
  while (current != NULL) {
    printf("ID: %d\n", current->id); //mostrar id
    printf("Title: %s\n", current->title); //mostrar títol
    printf("Body: %s\n", current->body); //mostrar cos

    Links *linkCurrent = current->links;
    while (linkCurrent != NULL) {
      printf("Link ID: %d, Text: %s\n", linkCurrent->documentId, linkCurrent->linkText); //mostrar enllaços
      linkCurrent = linkCurrent->next;
    }
    printf("\n");
    current = current->next;
  }
}

//funció per implementar la cerca per consulta (LAB2)
void lab2_querySearch(Document *documents) {
  char input[256];
  while (1) {
    printf("Enter query (empty to quit): "); //demanar consulta
    if (fgets(input, sizeof(input), stdin) == NULL) {
      break; //sortir si error o EOF
    }
    input[strcspn(input, "\n")] = 0; //eliminar salt de línia

    if (strlen(input) == 0) {
      break; //sortir si consulta buida
    }

    Query *query = initQueryFromString(input); //inicialitzar consulta
    if (query == NULL) {
      printf("Invalid query.\n"); //consulta invàlida
      continue;
    }

    Document *results = linearSearchDocuments(documents, query, 5); //cerca lineal
    if (results == NULL) {
      printf("No s'ha trobat cap document que coincideixi amb la consulta.\n"); //no trobat
    } else {
      printDocuments(results); //mostrar resultats
    }

    freeQuery(query); //alliberar consulta
  }
}

int main() {
  const char *directoryPath = "./datasets/wikipedia12"; //ruta documents

  Document *documents = loadAllDocuments(directoryPath); //carregar documents

  if (documents == NULL) {
    printf("No s'han trobat documents al directori especificat.\n"); //error
    return 1;
  }

  lab1_printDocuments(documents); //mostrar documents (LAB1)
  lab2_querySearch(documents); //cerca per consulta (LAB2)

  //alliberar memoria documents
  Document *current = documents;
  while (current != NULL) {
    Document *temp = current;
    current = current->next;
    freeDocument(temp);
  }

  return 0;
}
