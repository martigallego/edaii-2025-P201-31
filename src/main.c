#include "document.h"
#include "graph.h"
#include "hashmap.h"
#include "query.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Ordena la llista de documents per ID (ordre creixent)
Document* ordenaDocumentsPerId(Document* cap) {
    if (cap == NULL) return NULL;

    Document* actual = cap;
    while (actual != NULL) {
        Document* min = actual;
        Document* altre = actual->seguent;

        while (altre != NULL) {
            if (altre->id < min->id) {
                min = altre;
            }
            altre = altre->seguent;
        }

        // Intercanvia dades (no els nodes)
        if (min != actual) {
            int tempId = actual->id;
            char* tempTitol = actual->titol;
            char* tempCos = actual->cos;
            Enllacos* tempEnllacos = actual->enllacos;

            actual->id = min->id;
            actual->titol = min->titol;
            actual->cos = min->cos;
            actual->enllacos = min->enllacos;

            min->id = tempId;
            min->titol = tempTitol;
            min->cos = tempCos;
            min->enllacos = tempEnllacos;
        }

        actual = actual->seguent;
    }

    return cap;
}

// Mostrar tots els documents carregats
void lab1_imprimeixDocuments(Document *documents) {
    Document *actual = documents;
    int index = 1;
    while (actual != NULL) {
        printf("[%d] ID: %d\n", index, actual->id);
        printf("    Títol: %s\n", actual->titol);
        printf("    Cos: %.150s%s\n", actual->cos,
               strlen(actual->cos) > 150 ? "..." : "");
        printf("\n");
        actual = actual->seguent;
        index++;
    }
}

// Mostrar un document per ID
void mostraDocumentPerId(Document *documents) {
    char entrada[64];
    printf("Introdueix l'ID del document a mostrar (ENTER per sortir): ");

    if (fgets(entrada, sizeof(entrada), stdin) == NULL || entrada[0] == '\n') {
        return;
    }

    int id;
    if (sscanf(entrada, "%d", &id) != 1) {
        printf("Entrada invàlida. Torna al menú.\n");
        return;
    }

    Document *actual = documents;
    while (actual != NULL) {
        if (actual->id == id) {
            printf("\n--- Informació del Document ID %d ---\n", actual->id);
            printf("Títol: %s\n", actual->titol);
            printf("Cos:\n%s\n", actual->cos);
            printf("--------------------------------------\n");
            return;
        }
        actual = actual->seguent;
    }

    printf("No s'ha trobat cap document amb ID %d.\n", id);
}

// Cerca per paraules clau
void lab2_querySearch(Document *documents, HashMap *indexInvertit,
                      GrafDirigit *graf) {
    char entrada[256];
    while (1) {
        printf("\nIntroduir la consulta (ENTER per sortir): ");
        if (fgets(entrada, sizeof(entrada), stdin) == NULL)
            break;
        entrada[strcspn(entrada, "\n")] = 0;
        if (strlen(entrada) == 0)
            break;

        Consulta *consulta = iniciaConsultaDesDeString(entrada);
        if (consulta == NULL) {
            printf("Consulta invàlida.\n");
            continue;
        }

        comparaMetodesBusqueda(indexInvertit, documents, consulta);
        afegeixUltimaConsulta(iniciaConsultaDesDeString(entrada));

        Document *resultats = cercaDocumentsAmbIndexInvertit(
            indexInvertit, documents, consulta, 1000);

        if (resultats == NULL) {
            printf("No s'ha trobat cap document.\n");
        } else {
            printf("\n--- Resultats de la cerca ---\n");
            imprimirDocumentsPerRellevancia(graf, resultats, 10);

            int n_ordenats = 0;
            Document **ordenats =
                ordenaDocumentsPerRellevancia(resultats, &n_ordenats);
            for (int i = 0; i < n_ordenats; i++) {
                printf("\n--- Document ID: %d ---\n", ordenats[i]->id);
                printf("Títol: %s\n", ordenats[i]->titol);
                printf("Cos complet:\n%s\n", ordenats[i]->cos);
            }
            free(ordenats);

            printf("----------------------------\n");
        }

        mostraUltimesConsultes();
        alliberaConsulta(consulta);

        Document *resultatActual = resultats;
        while (resultatActual != NULL) {
            Document *temp = resultatActual;
            resultatActual = resultatActual->seguent;
            alliberaDocument(temp);
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
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    const char *rutaDirectori = datasets[opcioDataset - 1];
    Document *documents = carregaTotsElsDocuments(rutaDirectori);

    if (documents == NULL) {
        printf("No s'han trobat documents o error obrint directori.\n");
        return 1;
    }

    documents = ordenaDocumentsPerId(documents);
    printf("\nDataset carregat: %s\n", rutaDirectori);

    HashMap *indexInvertit = construeixIndexInvertit(documents);
    if (indexInvertit == NULL) {
        printf("Error creant l'índex invertit.\n");
        return 1;
    }

    GrafDirigit *graf = construirGrafDirigitDeDocuments(documents);

    printf("\n--- Verificació de nodes i arestes del graf ---\n");
    for (size_t i = 0; i < graf->mida; i++) {
        NodeGrau *n = graf->nodes[i];
        while (n != NULL) {
            printf("Node ID %d -> Entrants: %d | Sortints: %d | Rellevància: %.2f\n",
                   n->idDocument, n->grauEntrant, n->grauSortint,
                   n->puntuacioRellevancia);
            n = n->seguent;
        }
    }

    while (1) {
        printf("\n--- Menú Principal ---\n");
        printf("1. Mostrar documents\n");
        printf("2. Veure document per índex\n");
        printf("3. Fer cerca per paraules clau\n");
        printf("0. Sortir del programa\n");
        printf("Tria una opció: ");

        int opcio;
        if (scanf("%d", &opcio) != 1) {
            printf("Entrada invàlida.\n");
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        while ((c = getchar()) != '\n' && c != EOF);

        switch (opcio) {
        case 1:
            lab1_imprimeixDocuments(documents);
            break;
        case 2:
            mostraDocumentPerId(documents);
            break;
        case 3:
            lab2_querySearch(documents, indexInvertit, graf);
            break;
        case 0:
            printf("Sortint del programa. Alliberant memòria...\n");
            Document *d = documents;
            while (d != NULL) {
                Document *temp = d;
                d = d->seguent;
                alliberaDocument(temp);
            }
            alliberaHashMap(indexInvertit);
            alliberarGrafDirigit(graf);
            return 0;
        default:
            printf("Opció desconeguda.\n");
        }
    }

    return 0;
}
