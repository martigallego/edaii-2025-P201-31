#include "document.h"
#include "query.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// funció per mostrar tots els documents carregats (LAB1)
void lab1_printDocuments(Document *documents) {
  Document *current = documents; // punter a la primera entrada de la llista
  int index = 1; 
  while (current != NULL) {
    printf("[%d] ID: %d\n", index, current->id);       // mostrar índex i id
    printf("    Title: %s\n", current->title); // mostrar títol
    printf("    Body: %.150s%s\n", current->body, strlen(current->body) > 150 ? "..." : "");   // mostrar primeres 150 chars cos

    Links *linkCurrent = current->links;
    while (linkCurrent != NULL) {
      printf("    Link ID: %d, Text: %s\n", linkCurrent->documentId,
             linkCurrent->linkText); // mostrar enllaços
      linkCurrent = linkCurrent->next; // anar al següent enllaç
    }
    printf("\n");
    current = current->next; // anar al següent document
    index++; // incrementar índex
  }
}

// funció per mostrar un document per índex
void showDocumentByIndex(Document *documents) {
  int index;
  printf("Introdueix l'índex del document a mostrar (0 per sortir): ");
  if (scanf("%d", &index) != 1) {
    printf("Entrada invàlida. Torna al menú principal.\n");
    // Netejar buffer stdin
    int c; while ((c = getchar()) != '\n' && c != EOF);
    return;
  }
  if (index == 0) {
    printf("Sortint de la selecció de documents. Tornant al menú principal.\n");
    // Limpiar el buffer por si acaso, aunque aquí scanf ya leyó el número
    int c; while ((c = getchar()) != '\n' && c != EOF);
    return; // <--- ¡ESTE ES EL CAMBIO IMPORTANTE!
  }
  Document *current = documents;
  int currentIndex = 1;
  while (current != NULL && currentIndex < index) {
    current = current->next; // anar al següent document
    currentIndex++;
  }
  if (current == NULL) {
    printf("Índex invàlid. El document no existeix.\n");
  } else {
    printf("\n--- Informació del Document [%d] ---\n", index);
    printf("Document ID: %d\n", current->id); // mostrar id
    printf("Títol: %s\n", current->title); // mostrar títol
    printf("Cos:\n%s\n", current->body); // mostrar cos
    Links *linkCurrent = current->links; // punter a la primera entrada de la llista
    if (linkCurrent != NULL) { // si hi ha enllaços
      printf("Enllaços:\n"); // mostrar enllaços
      while (linkCurrent != NULL) {
        printf("  - Enllaç ID: %d, Text: %s\n", linkCurrent->documentId, linkCurrent->linkText); // mostrar enllaços
        linkCurrent = linkCurrent->next; // anar al següent enllaç
      }
    } else {
      printf("No hi ha enllaços per a aquest document.\n"); 
    }
    printf("--------------------------------------\n"); 
  }
  // Netejar buffer stdin
int c; while ((c = getchar()) != '\n' && c != EOF); // Neteja el buffer d'entrada per eliminar qualsevol caràcter restant després de llegir l'entrada, evitant que afecti futures lectures
}


// funció per implementar la cerca per consulta (LAB2)
//rep el HashMap del índice invertido
void lab2_querySearch(Document *documents, HashMap *reverseIndex) {
  char input[256];
  while (1) {
    printf("\nIntroduir la consulta (ENTER per sortir): "); // demanar consulta
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

    addLastQuery(initQueryFromString(input)); // Se sigue creando una Query doblemente. Podría optimizarse.


    Document *results = searchDocumentsWithReverseIndex(reverseIndex, documents, query, 5); // cerca amb índex invertit
    if (results == NULL) {
      printf(
          "No s'ha trobat cap document que coincideixi amb la consulta.\n"); // no trobat
    } else {
      printf("\n--- Resultats de la cerca ---\n");
      printDocuments(results); // mostrar resultats (esta función está en query.c y no muestra links)
      printf("----------------------------\n");
    }

    // mostrar les últimes 3 consultes
    showLastQueries();

    freeQuery(query); // alliberar consulta
    // También es crucial liberar los 'results' de la búsqueda, ya que son copias.
    Document *currentResult = results; // copia dels resultats
    while (currentResult != NULL) {
        Document *temp = currentResult;  
        currentResult = currentResult->next; 
        freeDocument(temp); // freeDocument libera título, cuerpo, links, y la estructura
    }
  }
}

int main() {
  const char *datasets[] = {
    "./datasets/wikipedia12",
    "./datasets/wikipedia270",
    "./datasets/wikipedia540",
    "./datasets/wikipedia5400"
  };
  int numDatasets = sizeof(datasets) / sizeof(datasets[0]); // calcular num de datasets

  printf("Selecciona el dataset a carregar:\n");
  for (int i = 0; i < numDatasets; i++) { // mostrar datasets
    printf("%d. %s\n", i + 1, datasets[i]); // imprimir dataset
  }
  printf("Tria una opció (1-%d): ", numDatasets);

  int datasetOption; // opción del dataset
  if (scanf("%d", &datasetOption) != 1 || datasetOption < 1 || datasetOption > numDatasets) { // comprobar si la entrada es valida
    printf("Opció invàlida. Sortint.\n");
    return 1; // si no es valida -> sortirr
  }
  int c; while ((c = getchar()) != '\n' && c != EOF); // netejar buffer

  const char *directoryPath = datasets[datasetOption - 1]; //seleccionar dataset

  Document *documents = loadAllDocuments(directoryPath); //carregar documents

  if (documents == NULL) {
    printf("No s'han trobat documents al directori especificat o no s'ha pogut obrir el directori.\n"); //error
    return 1;
  }

  printf("\nDataset carregat: %s\n", directoryPath); //mostrar dataset carregat

  // NUEVO: Construir el índice invertido después de cargar los documentos
  HashMap *reverseIndex = buildReverseIndex(documents);
  if (reverseIndex == NULL) {
      printf("Error al construir el índice invertido. Saliendo.\n");
      // Liberar documentos si el índice no se pudo construir.
      Document *doc_actual = documents;
      while (doc_actual != NULL) {
          Document *temp = doc_actual; 
          doc_actual = doc_actual->next;  // va al seguent document
          freeDocument(temp);
      }
      return 1;
  }


  while (1) {
    printf("\n--- Menú Principal ---\n");
    printf("1. Mostrar documents\n");
    printf("2. Veure document per índex\n");
    printf("3. Fer cerca per paraules clau\n"); 
    printf("0. Sortir del programa\n");
    printf("Tria una opció: ");

    int option;
    if (scanf("%d", &option) != 1) { 
      printf("Entrada invàlida. Si us plau, introdueix un número.\n");
      int c_clean; 
      while ((c_clean = getchar()) != '\n' && c_clean != EOF); // netejar buffer
      continue; 
    }
    int c_clean; while ((c_clean = getchar()) != '\n' && c_clean != EOF); // netejar buffer d'entrada per eliminar caràcters sobrants o restants després de llegir l'entrada

    switch (option) { // switch para manejar les opcions
      case 1:
        lab1_printDocuments(documents); // LAB1: mostrar documents
        break;
      case 2:
        showDocumentByIndex(documents); // veure document per índex
        break;
      case 3:
        lab2_querySearch(documents, reverseIndex); // index invertit
        break;
      case 0:
        printf("Sortint del programa. Alliberant memòria...\n");
        // alliberar memoria documents
        Document *current = documents;
        while (current != NULL) {
          Document *temp = current; // per no trencar la llista
          current = current->next;  //va al seguent document
          freeDocument(temp); //alliberar memoria del document
        }
        //alliberar la memoria del hashmap
        freeHashMap(reverseIndex);
        exit(0);
      default:
        printf("Opció desconeguda. Si us plau, tria una opció vàlida. \n");
    }
  }

  return 0;
}