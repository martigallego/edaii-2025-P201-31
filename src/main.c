#include "document.h"       // Gestió de documents (estructura Document, càrrega, deserialització)
#include "graph.h"          // Graf dirigit per a rellevància i enllaços entre documents
#include "hashmap.h"        // Índex invertit per a cerques eficients
#include "query.h"          // Funcions per processar consultes i cerques
#include <limits.h>         // Límits de tipus de dades (ex: INT_MAX)
#include <stdio.h>          // Entrada/sortida estàndard (printf, scanf, etc.)
#include <stdlib.h>         // Funcions bàsiques (malloc, free, exit)
#include <string.h>         // Manipulació de cadenes (strcmp, strdup, etc.)

// Funció per ordenar documents per ID (ordre creixent)
Document* ordenaDocumentsPerId(Document* cap) {
    if (cap == NULL) return NULL;  // Si la llista és buida, retorna NULL

    Document* actual = cap;  // Punter per recórrer la llista
    while (actual != NULL) {
        Document* min = actual;  // Assumeix que l'actual és el mínim
        Document* altre = actual->seguent;  // Punter per comparar amb la resta

        while (altre != NULL) {  // Busca el document amb ID més petit
            if (altre->id < min->id) {
                min = altre;  // Actualitza el mínim trobat
            }
            altre = altre->seguent;  // Avança al següent document
        }

        // Intercanvia dades (no els nodes) si s'ha trobat un ID més petit
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

        actual = actual->seguent;  // Passa al següent document
    }

    return cap;  // Retorna la llista ordenada
}

// Funció per mostrar tots els documents carregats
void lab1_imprimeixDocuments(Document *documents) {
    Document *actual = documents;  // Punter per recórrer la llista
    int index = 1;  // Comptador per numerar els documents
    while (actual != NULL) {
        printf("[%d] ID: %d\n", index, actual->id);  // Mostra ID
        printf("    Títol: %s\n", actual->titol);    // Mostra títol
        // Mostra els primers 150 caràcters del cos (o menys si és més curt)
        printf("    Cos: %.150s%s\n", actual->cos,
               strlen(actual->cos) > 150 ? "..." : "");
        printf("\n");
        actual = actual->seguent;  // Passa al següent document
        index++;  // Incrementa l'índex
    }
}

// Funció per mostrar un document específic segons el seu ID
void mostraDocumentPerId(Document *documents) {
    char entrada[64];  // Buffer per a l'entrada de l'usuari
    printf("Introdueix l'ID del document a mostrar (ENTER per sortir): ");

    if (fgets(entrada, sizeof(entrada), stdin) == NULL || entrada[0] == '\n') {
        return;  // Si l'usuari prem ENTER, torna al menú
    }

    int id;
    if (sscanf(entrada, "%d", &id) != 1) {  // Intenta llegir un número
        printf("Entrada invàlida. Torna al menú.\n");
        return;
    }

    Document *actual = documents;  // Punter per buscar el document
    while (actual != NULL) {
        if (actual->id == id) {  // Si troba el document amb l'ID especificat
            printf("\n--- Informació del Document ID %d ---\n", actual->id);
            printf("Títol: %s\n", actual->titol);
            printf("Cos:\n%s\n", actual->cos);
            printf("--------------------------------------\n");
            return;
        }
        actual = actual->seguent;  // Passa al següent document
    }

    printf("No s'ha trobat cap document amb ID %d.\n", id);  // Missatge si no es troba
}

// Funció per realitzar cerques per paraules clau
void lab2_querySearch(Document *documents, HashMap *indexInvertit,
                      GrafDirigit *graf) {
    char entrada[256];  // Buffer per a la consulta de l'usuari
    while (1) {
        printf("\nIntroduir la consulta (ENTER per sortir): ");
        if (fgets(entrada, sizeof(entrada), stdin) == NULL) break;  // Llegeix l'entrada
        entrada[strcspn(entrada, "\n")] = 0;  // Elimina el salt de línia
        if (strlen(entrada) == 0) break;  // Si és buida, surt

        Consulta *consulta = iniciaConsultaDesDeString(entrada);  // Processa la consulta
        if (consulta == NULL) {
            printf("Consulta invàlida.\n");
            continue;
        }

        // Compara els mètodes de cerca (amb/sense índex invertit)
        comparaMetodesBusqueda(indexInvertit, documents, consulta);
        // Guarda la consulta a l'historial
        afegeixUltimaConsulta(iniciaConsultaDesDeString(entrada));

        // Realitza la cerca amb índex invertit
        Document *resultats = cercaDocumentsAmbIndexInvertit(
            indexInvertit, documents, consulta, 1000);

        if (resultats == NULL) {
            printf("No s'ha trobat cap document.\n");
        } else {
            printf("\n--- Resultats de la cerca ---\n");
            // Mostra els resultats ordenats per rellevància
            imprimirDocumentsPerRellevancia(graf, resultats, 10);

            int n_ordenats = 0;
            // Ordena els documents per rellevància
            Document **ordenats =
                ordenaDocumentsPerRellevancia(resultats, &n_ordenats);
            // Mostra els documents ordenats
            for (int i = 0; i < n_ordenats; i++) {
                printf("\n--- Document ID: %d ---\n", ordenats[i]->id);
                printf("Títol: %s\n", ordenats[i]->titol);
                printf("Cos complet:\n%s\n", ordenats[i]->cos);
            }
            free(ordenats);  // Allibera l'array de documents ordenats

            printf("----------------------------\n");
        }

        mostraUltimesConsultes();  // Mostra l'historial de consultes
        alliberaConsulta(consulta);  // Allibera la memòria de la consulta

        // Allibera la memòria dels resultats
        Document *resultatActual = resultats;
        while (resultatActual != NULL) {
            Document *temp = resultatActual;
            resultatActual = resultatActual->seguent;
            alliberaDocument(temp);
        }
    }
}

int main() {
    // Array amb les rutes als diferents datasets disponibles
    const char *datasets[] = {
        "./datasets/wikipedia12",
        "./datasets/wikipedia270",
        "./datasets/wikipedia540",
        "./datasets/wikipedia5400"
    };
    int numDatasets = sizeof(datasets) / sizeof(datasets[0]);  // Calcula el nombre de datasets

    // Mostra el menú de selecció de dataset
    printf("Selecciona el dataset a carregar:\n");
    for (int i = 0; i < numDatasets; i++) {
        printf("%d. %s\n", i + 1, datasets[i]);
    }
    printf("Tria una opció (1-%d): ", numDatasets);

    int opcioDataset;
    // Llegeix la selecció de l'usuari
    if (scanf("%d", &opcioDataset) != 1 || opcioDataset < 1 || opcioDataset > numDatasets) {
        printf("Opció invàlida. Sortint.\n");
        return 1;
    }
    // Neteja el buffer d'entrada
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    const char *rutaDirectori = datasets[opcioDataset - 1];  // Obté la ruta del dataset seleccionat
    // Carrega tots els documents del directori
    Document *documents = carregaTotsElsDocuments(rutaDirectori);

    if (documents == NULL) {
        printf("No s'han trobat documents o error obrint directori.\n");
        return 1;
    }

    documents = ordenaDocumentsPerId(documents);  // Ordena els documents per ID
    printf("\nDataset carregat: %s\n", rutaDirectori);

    // Construeix l'índex invertit per a cerques eficients
    HashMap *indexInvertit = construeixIndexInvertit(documents);
    if (indexInvertit == NULL) {
        printf("Error creant l'índex invertit.\n");
        return 1;
    }

    // Construeix el graf dirigit per a calcular rellevància
    GrafDirigit *graf = construirGrafDirigitDeDocuments(documents);

    // Mostra informació de verificació del graf
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

    // Menú principal
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
            lab1_imprimeixDocuments(documents);  // Mostra tots els documents
            break;
        case 2:
            mostraDocumentPerId(documents);  // Mostra un document específic
            break;
        case 3:
            lab2_querySearch(documents, indexInvertit, graf);  // Cerca per paraules clau
            break;
        case 0:
            printf("Sortint del programa. Alliberant memòria...\n");
            // Allibera la memòria de tots els documents
            Document *d = documents;
            while (d != NULL) {
                Document *temp = d;
                d = d->seguent;
                alliberaDocument(temp);
            }
            alliberaHashMap(indexInvertit);  // Allibera l'índex invertit
            alliberarGrafDirigit(graf);     // Allibera el graf
            return 0;  // Surt del programa
        default:
            printf("Opció desconeguda.\n");
        }
    }

    return 0;
}