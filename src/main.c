#include "document.h" 
#include "query.h" 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

//funció per mostrar tots els documents carregats (LAB1)
void lab1_printDocuments(Document *documents) {
  Document *current = documents; //inicialitzar pointer al primer document
  int index = 1; // inicialitzar índex del document
  while (current != NULL) { //recórrer tots els documents
    printf("[%d] ID: %d\n", index, current->id); // mostrar índex i ID
    printf("    Title: %s\n", current->title); //mostrar títol
    printf("    Body: %.150s%s\n", current->body, strlen(current->body) > 150 ? "..." : ""); // mostrar fins a 150 caràcters del cos

    Links *linkCurrent = current->links; // inicialitzar pointer a enllaços
    while (linkCurrent != NULL) { // recórrer tots els enllaços
      printf("    Link ID: %d, Text: %s\n", linkCurrent->documentId,
             linkCurrent->linkText); //mostrar ID i text de l’enllaç
      linkCurrent = linkCurrent->next; // passar al següent enllaç
    }
    printf("\n"); //imprimir línia en blanc
    current = current->next; //passar al següent document
    index++; //incrementar índex
  }
}

// funció per mostrar un document per índex
void showDocumentByIndex(Document *documents) {
  int index;
  printf("Introdueix l'índex del document a mostrar (0 per sortir): "); // demanar índex a l’usuari
  if (scanf("%d", &index) != 1) { //comprovar entrada vàlida
    printf("Entrada invàlida.\n");
    int c; while ((c = getchar()) != '\n' && c != EOF); //netejar buffer d’entrada
    return;
  }
  if (index == 0) {
    printf("Sortint de la selecció de documents.\n");
    exit(0); // sortir del programa
  }
  Document *current = documents; // inicialitzar pointer a documents
  int currentIndex = 1;
  while (current != NULL && currentIndex < index) { // buscar el document per índex
    current = current->next;
    currentIndex++;
  }
  if (current == NULL) {
    printf("Índex invàlid.\n"); // mostrar missatge si l’índex no existeix
  } else {
    printf("Document ID: %d\n", current->id); // mostrar ID
    printf("Títol: %s\n", current->title); // mostrar títol
    printf("Cos: %s\n", current->body); // mostrar cos
    Links *linkCurrent = current->links;
    while (linkCurrent != NULL) { // recórrer enllaços del document
      printf("Enllaç ID: %d, Text: %s\n", linkCurrent->documentId,
             linkCurrent->linkText); //mostrar info de l’enllaç
      linkCurrent = linkCurrent->next; //passar al següent enllaç
    }
  }
  int c; while ((c = getchar()) != '\n' && c != EOF); // netejar buffer stdin
}

// funció per implementar la cerca per consulta (LAB2)
void lab2_querySearch(Document *documents) {
  char input[256]; // declarar buffer per la consulta
  while (1) { // repetir fins que es decideixi sortir
    printf("Introduir la consulta (ENTER per sortir):"); // demanar consulta
    if (fgets(input, sizeof(input), stdin) == NULL) {
      break; // sortir si hi ha error o EOF
    }
    input[strcspn(input, "\n")] = 0; //eliminar salt de línia

    if (strlen(input) == 0) {
      break; //sortir si la consulta està buida
    }

    Query *query = initQueryFromString(input); // inicialitzar consulta
    if (query == NULL) {
      printf("Consulta invàlida.\n"); // mostrar error si la consulta no és vàlida
      continue;
    }
    addLastQuery(initQueryFromString(input)); // afegir consulta a l’historial

    Document *results = linearSearchDocuments(documents, query, 5); // fer cerca als documents
    if (results == NULL) {
      printf("No s'ha trobat cap document que coincideixi amb la consulta.\n");
    } else {
      printDocuments(results); // mostrar documents trobats
    }
    showLastQueries(); //mostrar les últimes consultes
    freeQuery(query); // alliberar memòria de la consulta
  }
}

int main() {
  const char *datasets[] = {
    "./datasets/wikipedia12",
    "./datasets/wikipedia270",
    "./datasets/wikipedia540",
    "./datasets/wikipedia5400"
  }; //definir array amb les rutes dels datasets
  int numDatasets = sizeof(datasets) / sizeof(datasets[0]); // calcular quants datasets hi ha

  printf("Selecciona el dataset a carregar:\n");
  for (int i = 0; i < numDatasets; i++) {
    printf("%d. %s\n", i + 1, datasets[i]); //mostrar cada dataset com a opció
  }
  printf("Tria una opció (1-%d): ", numDatasets); //demanar opció

  int datasetOption;
  if (scanf("%d", &datasetOption) != 1 || datasetOption < 1 || datasetOption > numDatasets) {
    printf("Opció invàlida. Sortint.\n"); // mostrar error i sortir si l’opció no és correcta
    return 1;
  }
  int c; while ((c = getchar()) != '\n' && c != EOF); // netejar buffer stdin

  const char *directoryPath = datasets[datasetOption - 1]; // btenir ruta del dataset seleccionat

  Document *documents = loadAllDocuments(directoryPath); //carrregar tots els documents del directori

  if (documents == NULL) {
    printf("No s'han trobat documents al directori especificat.\n"); //mostrar missatge d’error si no es troben documents
    return 1;
  }

  printf("Dataset carregat: %s\n", directoryPath); // confirmar que el dataset s’ha carregat

  while (1) { // mostrar menú principal
    printf("\nOpcions:\n");
    printf("1. Mostrar documents\n");
    printf("2. Veure document per índex\n");
    printf("3. Fer cerca per paraules clau\n");
    printf("0. Sortir\n");
    printf("Tria una opció: ");

    int option;
    if (scanf("%d", &option) != 1) {
      printf("Entrada invàlida.\n");
      int c; while ((c = getchar()) != '\n' && c != EOF); //netejar buffer
      continue;
    }
    int c; while ((c = getchar()) != '\n' && c != EOF); //netejar buffer stdin

    switch (option) {
      case 1:
        lab1_printDocuments(documents); //funció per mostrar documents
        break;
      case 2:
        showDocumentByIndex(documents); //funció per veure document concret
        break;
      case 3:
        lab2_querySearch(documents); //funció per fer consulta
        break;
      case 0:
        printf("Sortint del programa.\n");
        Document *current = documents;
        while (current != NULL) {
          Document *temp = current;
          current = current->next;
          freeDocument(temp); //alliberar memòria de cada document
        }
        exit(0); // sortir del programa
      default:
        printf("Opció desconeguda.\n"); // mostrar missatge per opcions no vàlides
    }
  }

  return 0; //fi del programa
}
