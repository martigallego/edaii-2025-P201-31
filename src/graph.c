#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MIDA_INICIAL_GRAF 10007

unsigned int hashGraf(int idDocument, size_t midaGraf) {
    return (unsigned int)idDocument % midaGraf;
}

GrafDirigit *crearGrafDirigit(size_t mida) {
    GrafDirigit *graf = (GrafDirigit *)malloc(sizeof(GrafDirigit));
    if (!graf) {
        perror("Error creant el graf dirigit");
        exit(EXIT_FAILURE);
    }

    graf->mida = mida;
    graf->totalNodes = 0;
    graf->totalEnllacos = 0;
    graf->nodes = (NodeGrau **)calloc(mida, sizeof(NodeGrau *));
    if (!graf->nodes) {
        perror("Error assignant memòria per als nodes del graf");
        free(graf);
        exit(EXIT_FAILURE);
    }

    return graf;
}

NodeGrau *cercarNodeEnGraf(GrafDirigit *graf, int idDocument) {
    unsigned int index = hashGraf(idDocument, graf->mida);
    NodeGrau *actual = graf->nodes[index];
    while (actual != NULL) {
        if (actual->idDocument == idDocument) return actual;
        actual = actual->seguent;
    }
    return NULL;
}

void afegirNodeAGraf(GrafDirigit *graf, int idDocument) {
    if (cercarNodeEnGraf(graf, idDocument) != NULL) return;

    NodeGrau *nouNode = (NodeGrau *)malloc(sizeof(NodeGrau));
    if (!nouNode) {
        perror("Error creant nou node del graf");
        exit(EXIT_FAILURE);
    }

    nouNode->idDocument = idDocument;
    nouNode->grauEntrant = 0;
    nouNode->grauSortint = 0;
    nouNode->puntuacioRellevancia = 0.0;
    nouNode->seguent = NULL;
    nouNode->arestesSortints = NULL;

    unsigned int index = hashGraf(idDocument, graf->mida);
    nouNode->seguent = graf->nodes[index];
    graf->nodes[index] = nouNode;

    graf->totalNodes++;
}

void afegirArestaAGraf(GrafDirigit *graf, int idDocOrigen, int idDocDesti) {
    afegirNodeAGraf(graf, idDocOrigen);
    afegirNodeAGraf(graf, idDocDesti);

    NodeGrau *nodeOrigen = cercarNodeEnGraf(graf, idDocOrigen);
    NodeGrau *nodeDesti = cercarNodeEnGraf(graf, idDocDesti);

    if (nodeOrigen == NULL || nodeDesti == NULL) return;

    // Afegim nova aresta a la llista d’adjacència
    Aresta *nova = malloc(sizeof(Aresta));
    nova->idDesti = idDocDesti;
    nova->seguent = nodeOrigen->arestesSortints;
    nodeOrigen->arestesSortints = nova;

    nodeOrigen->grauSortint++;
    nodeDesti->grauEntrant++;
    graf->totalEnllacos++;
}

GrafDirigit *construirGrafDirigitDeDocuments(Document *documents) {
    GrafDirigit *graf = crearGrafDirigit(MIDA_INICIAL_GRAF);

    Document *docActual = documents;
    while (docActual != NULL) {
        afegirNodeAGraf(graf, docActual->id);
        docActual = docActual->seguent;
    }

    docActual = documents;
    while (docActual != NULL) {
        Enllacos *e = docActual->enllacos;
        while (e != NULL) {
            afegirArestaAGraf(graf, docActual->id, e->idDocumentDesti);
            e = e->seguent;
        }
        docActual = docActual->seguent;
    }

    calcularPuntuacionsRellevancia(graf);
    return graf;
}

void calcularPuntuacionsRellevancia(GrafDirigit *graf) {
    if (graf->totalNodes == 0) return;
    int maxGrauEntrant = 0;

    for (size_t i = 0; i < graf->mida; i++) {
        NodeGrau *actual = graf->nodes[i];
        while (actual != NULL) {
            if (actual->grauEntrant > maxGrauEntrant)
                maxGrauEntrant = actual->grauEntrant;
            actual = actual->seguent;
        }
    }

    for (size_t i = 0; i < graf->mida; i++) {
        NodeGrau *actual = graf->nodes[i];
        while (actual != NULL) {
            actual->puntuacioRellevancia = (maxGrauEntrant > 0)
                ? (double)actual->grauEntrant / maxGrauEntrant : 0.0;
            actual = actual->seguent;
        }
    }
}

int obtenirGrauEntrantDocument(GrafDirigit *graf, int idDocument) {
    NodeGrau *n = cercarNodeEnGraf(graf, idDocument);
    return (n != NULL) ? n->grauEntrant : -1;
}

double obtenirPuntuacioRellevanciaDocument(GrafDirigit *graf, int idDocument) {
    NodeGrau *n = cercarNodeEnGraf(graf, idDocument);
    return (n != NULL) ? n->puntuacioRellevancia : 0.0;
}

int compararDocumentsRellevancia(const void *a, const void *b) {
    Document *docA = *(Document **)a;
    Document *docB = *(Document **)b;
    if (docA->puntuacioRellevancia < docB->puntuacioRellevancia) return 1;
    if (docA->puntuacioRellevancia > docB->puntuacioRellevancia) return -1;
    return 0;
}

void imprimirDocumentsPerRellevancia(GrafDirigit *graf, Document *documents, int maxResultats) {
    int total = 0;
    Document *d = documents;
    while (d != NULL) { total++; d = d->seguent; }
    if (total == 0) return;

    Document **array = malloc(sizeof(Document *) * total);
    d = documents;
    for (int i = 0; i < total; i++) {
        array[i] = d;
        NodeGrau *n = cercarNodeEnGraf(graf, d->id);
        d->puntuacioRellevancia = (n != NULL) ? n->puntuacioRellevancia : 0.0;
        d = d->seguent;
    }

    qsort(array, total, sizeof(Document *), compararDocumentsRellevancia);
    for (int i = 0; i < total && i < maxResultats; i++) {
        printf("ID: %d, Puntuació: %.3f, Títol: %s\n", array[i]->id, array[i]->puntuacioRellevancia, array[i]->titol);
    }
    free(array);
}

void alliberarGrafDirigit(GrafDirigit *graf) {
    if (!graf) return;
    for (size_t i = 0; i < graf->mida; i++) {
        NodeGrau *n = graf->nodes[i];
        while (n != NULL) {
            NodeGrau *seguent = n->seguent;
            Aresta *a = n->arestesSortints;
            while (a != NULL) {
                Aresta *tmp = a;
                a = a->seguent;
                free(tmp);
            }
            free(n);
            n = seguent;
        }
    }
    free(graf->nodes);
    free(graf);
}
