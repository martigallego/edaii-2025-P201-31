#include "document.h"
#include "query.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// funció per mostrar tots els documents carregats (LAB1)
void lab1_printDocuments(Document *documents) {
  Document *current = documents;
  int index = 1;
  while (current != NULL) {
    printf("[%d] ID: %d\n", index, current->id);       // mostrar índex i id
    printf("    Title: %s\n", current->title); // mostrar títol
    printf("    Body: %.150s%s\n", current->body, strlen(current->body) > 150 ? "..." : "");   // mostrar primeres 150 chars cos

    Links *linkCurrent = current->links;
    while (linkCurrent != NULL) {
      printf("    Link ID: %d, Text: %s\n", linkCurrent->documentId,
             linkCurrent->linkText); // mostrar enllaços
      linkCurrent = linkCurrent->next;
    }
    printf("\n");
    current = current->next;
    index++;
  }
}

// funció per mostrar un document per índex
void showDocumentByIndex(Document *documents) {
  int index;
  printf("Introdueix l'índex del document a mostrar (0 per sortir): ");
  if (scanf("%d", &index) != 1) {
    printf("Entrada invàlida.\n");
    // Netejar buffer stdin
    int c; while ((c = getchar()) != '\n' && c != EOF);
    return;
  }
  if (index == 0) {
    printf("Sortint de la selecció de documents.\n");
    exit(0);
  }
  Document *current = documents;
  int currentIndex = 1;
  while (current != NULL && currentIndex < index) {
    current = current->next;
    currentIndex++;
  }
  if (current == NULL) {
    printf("Índex invàlid.\n");
  } else {
    printf("Document ID: %d\n", current->id);
    printf("Títol: %s\n", current->title);
    printf("Cos: %s\n", current->body);
    Links *linkCurrent = current->links;
    while (linkCurrent != NULL) {
      printf("Enllaç ID: %d, Text: %s\n", linkCurrent->documentId,
             linkCurrent->linkText);
      linkCurrent = linkCurrent->next;
    }
  }
  // Netejar buffer stdin
  int c; while ((c = getchar()) != '\n' && c != EOF);
}

// funció per implementar la cerca per consulta (LAB2)
void lab2_querySearch(Document *documents) {
  char input[256];
  while (1) {
    printf("Introduir la consulta (ENTER per sortir):"); // demanar consulta
    if (fgets(input, sizeof(input), stdin) == NULL) {
      break; // sortir si error o EOF
    }
    input[strcspn(input, "\n")] = 0; // eliminar salt de línia

    if (strlen(input) == 0) {
      break; // sortir si consulta buida
    }

    Query *query = initQueryFromString(input); // inicialitzar consulta
    if (query == NULL) {
      printf("Consulta invàlida.\n"); // consulta invàlida
      continue;
    }

    // afegir la consulta a la cua de les últimes consultes
    addLastQuery(initQueryFromString(input));

    Document *results =
        linearSearchDocuments(documents, query, 5); // cerca lineal
    if (results == NULL) {
      printf(
          "No s'ha trobat cap document que coincideixi amb la consulta.\n"); // no trobat
    } else {
      printDocuments(results); // mostrar resultats
    }

    // mostrar les últimes 3 consultes
    showLastQueries();

    freeQuery(query); // alliberar consulta
  }
}

int main() {
  const char *datasets[] = {
    "./datasets/wikipedia12",
    "./datasets/wikipedia270",
    "./datasets/wikipedia540",
    "./datasets/wikipedia5400"
  };
  int numDatasets = sizeof(datasets) / sizeof(datasets[0]);

  printf("Selecciona el dataset a carregar:\n");
  for (int i = 0; i < numDatasets; i++) {
    printf("%d. %s\n", i + 1, datasets[i]);
  }
  printf("Tria una opció (1-%d): ", numDatasets);

  int datasetOption;
  if (scanf("%d", &datasetOption) != 1 || datasetOption < 1 || datasetOption > numDatasets) {
    printf("Opció invàlida. Sortint.\n");
    return 1;
  }
  int c; while ((c = getchar()) != '\n' && c != EOF); // netejar buffer

  const char *directoryPath = datasets[datasetOption - 1];

  Document *documents = loadAllDocuments(directoryPath); // carregar documents

  if (documents == NULL) {
    printf("No s'han trobat documents al directori especificat.\n"); // error
    return 1;
  }

  printf("Dataset carregat: %s\n", directoryPath);

  while (1) {
    printf("\nOpcions:\n");
    printf("1. Mostrar documents\n");
    printf("2. Veure document per índex\n");
    printf("3. Fer cerca per paraules clau\n");
    printf("0. Sortir\n");
    printf("Tria una opció: ");

    int option;
    if (scanf("%d", &option) != 1) {
      printf("Entrada invàlida.\n");
      int c; while ((c = getchar()) != '\n' && c != EOF);
      continue;
    }
    int c; while ((c = getchar()) != '\n' && c != EOF); // netejar buffer

    switch (option) {
      case 1:
        lab1_printDocuments(documents);
        break;
      case 2:
        showDocumentByIndex(documents);
        break;
      case 3:
        lab2_querySearch(documents);
        break;
      case 0:
        printf("Sortint del programa.\n");
        // alliberar memoria documents
        Document *current = documents;
        while (current != NULL) {
          Document *temp = current;
          current = current->next;
          freeDocument(temp);
        }
        exit(0);
      default:
        printf("Opció desconeguda.\n");
    }
  }

  return 0;
}
