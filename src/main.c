#include "document.h"
#include "query.h"
#include "hashmap.h"
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> // Per a INT_MAX

// Mostrar tots els documents carregats
void lab1_imprimeixDocuments(Document *documents) {
    Document *actual = documents;
    int index = 1;
    while (actual != NULL) {
        printf("[%d] ID: %d\n", index, actual->id);
        printf("    Títol: %s\n", actual->titol); // 'title' -> 'titol'
        printf("    Cos: %.150s%s\n", actual->cos, strlen(actual->cos) > 150 ? "..." : ""); // 'body' -> 'cos'
        Enllacos *enllacActual = actual->enllacos; // 'Links' -> 'Enllacos', 'links' -> 'enllacos'
        while (enllacActual != NULL) {
            printf("    Enllaç ID: %d, Text: %s\n", enllacActual->idDocumentDesti, enllacActual->textEnllac); // 'documentId' -> 'idDocumentDesti', 'linkText' -> 'textEnllac'
            enllacActual = enllacActual->seguent; // 'next' -> 'seguent'
        }
        printf("\n");
        actual = actual->seguent; // 'next' -> 'seguent'
        index++;
    }
}

// Mostrar un document per índex
void mostraDocumentPerIndex(Document *documents) {
    int index;
    printf("Introdueix l'índex del document a mostrar (0 per sortir): ");
    if (scanf("%d", &index) != 1) {
        printf("Entrada invàlida. Tornant al menú.\n");
        int c; while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    if (index == 0) {
        int c; while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    Document *actual = documents;
    int indexActual = 1;
    while (actual != NULL && indexActual < index) {
        actual = actual->seguent; // 'next' -> 'seguent'
        indexActual++;
    }
    if (actual == NULL) {
        printf("Índex invàlid. El document no existeix.\n");
    } else {
        printf("\n--- Informació del Document [%d] ---\n", index);
        printf("Document ID: %d\n", actual->id);
        printf("Títol: %s\n", actual->titol); // 'title' -> 'titol'
        printf("Cos:\n%s\n", actual->cos); // 'body' -> 'cos'
        Enllacos *enllacActual = actual->enllacos; // 'Links' -> 'Enllacos', 'links' -> 'enllacos'
        if (enllacActual != NULL) {
            printf("Enllaços:\n");
            while (enllacActual != NULL) {
                printf("    - Enllaç ID: %d, Text: %s\n", enllacActual->idDocumentDesti, enllacActual->textEnllac); // 'documentId' -> 'idDocumentDesti', 'linkText' -> 'textEnllac'
                enllacActual = enllacActual->seguent; // 'next' -> 'seguent'
            }
        } else {
            printf("No hi ha enllaços per a aquest document.\n");
        }
        printf("--------------------------------------\n");
    }
    int c; while ((c = getchar()) != '\n' && c != EOF);
}

// Cerca per paraules clau
void lab2_querySearch(Document *documents, HashMap *indexInvertit) { // 'reverseIndex' -> 'indexInvertit'
    char entrada[256];
    while (1) {
        printf("\nIntroduir la consulta (ENTER per sortir): ");
        if (fgets(entrada, sizeof(entrada), stdin) == NULL) break;
        entrada[strcspn(entrada, "\n")] = 0;
        if (strlen(entrada) == 0) break;

        Consulta *consulta = iniciaConsultaDesDeString(entrada); // 'Query' -> 'Consulta', 'initQueryFromString' -> 'iniciaConsultaDesDeString'
        if (consulta == NULL) {
            printf("Consulta invàlida.\n");
            continue;
        }

        afegeixUltimaConsulta(iniciaConsultaDesDeString(entrada)); // 'addLastQuery' -> 'afegeixUltimaConsulta'

        Document *resultats = cercaDocumentsAmbIndexInvertit(indexInvertit, documents, consulta, 5); // 'results' -> 'resultats', 'searchDocumentsWithReverseIndex' -> 'cercaDocumentsAmbIndexInvertit'
        if (resultats == NULL) {
            printf("No s'ha trobat cap document.\n");
        } else {
            printf("\n--- Resultats de la cerca ---\n");
            imprimeixDocuments(resultats); // 'printDocuments' -> 'imprimeixDocuments'
            printf("----------------------------\n");
        }

        mostraUltimesConsultes(); // 'showLastQueries' -> 'mostraUltimesConsultes'
        alliberaConsulta(consulta); // 'freeQuery' -> 'alliberaConsulta'

        Document *resultatActual = resultats; // 'currentResult' -> 'resultatActual'
        while (resultatActual != NULL) {
            Document *temp = resultatActual;
            resultatActual = resultatActual->seguent; // 'next' -> 'seguent'
            alliberaDocument(temp); // 'freeDocument' -> 'alliberaDocument'
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
    int numDatasets = sizeof(datasets) / sizeof(datasets[0]);

    printf("Selecciona el dataset a carregar:\n");
    for (int i = 0; i < numDatasets; i++) {
        printf("%d. %s\n", i + 1, datasets[i]);
    }
    printf("Tria una opció (1-%d): ", numDatasets);

    int opcioDataset;
    if (scanf("%d", &opcioDataset) != 1 || opcioDataset < 1 || opcioDataset > numDatasets) {
        printf("Opció invàlida. Sortint.\n");
        return 1;
    }
    int c; while ((c = getchar()) != '\n' && c != EOF);

    const char *rutaDirectori = datasets[opcioDataset - 1]; // 'directoryPath' -> 'rutaDirectori'
    Document *documents = carregaTotsElsDocuments(rutaDirectori); // 'loadAllDocuments' -> 'carregaTotsElsDocuments'

    if (documents == NULL) {
        printf("No s'han trobat documents o error obrint directori.\n");
        return 1;
    }
    printf("\nDataset carregat: %s\n", rutaDirectori);

    HashMap *indexInvertit = construeixIndexInvertit(documents); // 'reverseIndex' -> 'indexInvertit', 'buildReverseIndex' -> 'construeixIndexInvertit'
    if (indexInvertit == NULL) {
        printf("Error creant l'índex invertit.\n");
        Document *d = documents;
        while (d != NULL) {
            Document *temp = d;
            d = d->seguent; // 'next' -> 'seguent'
            alliberaDocument(temp); // 'freeDocument' -> 'alliberaDocument'
        }
        return 1;
    }

    GrafDirigit *graf = construirGrafDirigitDeDocuments(documents); // 'graf' -> 'graf', 'construirGrafDirigitDeDocuments'
    if (graf == NULL) {
        printf("Error creant el graf dirigit.\n");
        Document *d = documents;
        while (d != NULL) {
            Document *temp = d;
            d = d->seguent; // 'next' -> 'seguent'
            alliberaDocument(temp); // 'freeDocument' -> 'alliberaDocument'
        }
        alliberaHashMap(indexInvertit); // 'freeHashMap' -> 'alliberaHashMap'
        return 1;
    }

    while (1) {
        printf("\n--- Menú Principal ---\n");
        printf("1. Mostrar documents\n");
        printf("2. Veure document per índex\n");
        printf("3. Fer cerca per paraules clau\n");
        printf("4. Mostrar documents per rellevància\n");
        printf("0. Sortir del programa\n");
        printf("Tria una opció: ");

        int opcio;
        if (scanf("%d", &opcio) != 1) {
            printf("Entrada invàlida.\n");
            int c_clean; while ((c_clean = getchar()) != '\n' && c_clean != EOF);
            continue;
        }
        int c_clean; while ((c_clean = getchar()) != '\n' && c_clean != EOF);

        switch (opcio) {
            case 1:
                lab1_imprimeixDocuments(documents);
                break;
            case 2:
                mostraDocumentPerIndex(documents);
                break;
            case 3:
                lab2_querySearch(documents, indexInvertit);
                break;
            case 4:
                printf("Quants documents vols mostrar? (0 per tots): ");
                int maxResultats; // 'maxResults' -> 'maxResultats'
                if (scanf("%d", &maxResultats) == 1) {
                    imprimirDocumentsPerRellevancia(graf, documents, maxResultats);
                } else {
                    printf("Entrada invàlida.\n");
                }
                int c_flush; while ((c_flush = getchar()) != '\n' && c_flush != EOF);
                break;
            case 0:
                printf("Sortint del programa. Alliberant memòria...\n");
                Document *d = documents;
                while (d != NULL) {
                    Document *temp = d;
                    d = d->seguent; // 'next' -> 'seguent'
                    alliberaDocument(temp); // 'freeDocument' -> 'alliberaDocument'
                }
                alliberaHashMap(indexInvertit); // 'freeHashMap' -> 'alliberaHashMap'
                alliberarGrafDirigit(graf);
                return 0;
            default:
                printf("Opció desconeguda.\n");
        }
    }

    return 0;
}