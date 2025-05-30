#ifndef GRAPH_H
#define GRAPH_H

#include "document.h"

// Estructura d’una aresta (connexió sortint)
typedef struct Aresta {
    int idDesti;
    struct Aresta *seguent;
} Aresta;

// Node del graf (document amb grau i connexions)
typedef struct NodeGrau {
    int idDocument;
    int grauEntrant;
    int grauSortint;
    double puntuacioRellevancia;
    struct NodeGrau *seguent;

    Aresta *arestesSortints;
} NodeGrau;

// Estructura del graf
typedef struct {
    size_t mida;
    int totalNodes;
    int totalEnllacos;
    NodeGrau **nodes;
} GrafDirigit;

GrafDirigit *construirGrafDirigitDeDocuments(Document *documents);
void calcularPuntuacionsRellevancia(GrafDirigit *graf);
int obtenirGrauEntrantDocument(GrafDirigit *graf, int idDocument);
double obtenirPuntuacioRellevanciaDocument(GrafDirigit *graf, int idDocument);
void imprimirDocumentsPerRellevancia(GrafDirigit *graf, Document *documents, int maxResultats);
void alliberarGrafDirigit(GrafDirigit *graf);

#endif // GRAPH_H