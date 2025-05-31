#ifndef GRAPH_H
#define GRAPH_H

#include "document.h"

// Estructura d’una aresta (connexió sortint entre dos nodes)
typedef struct Aresta {
  int idDesti; // ID del document destí al qual apunta aquesta aresta
  struct Aresta *seguent; // Punter a la següent aresta (llista encadenada)
} Aresta;

// Estructura d’un node del graf, que representa un document
typedef struct NodeGrau {
  int idDocument;  // ID únic del document
  int grauEntrant; // Nombre d’arestes entrants (referències rebudes)
  int grauSortint; // Nombre d’arestes sortints (referències fetes)
  double puntuacioRellevancia; // Valor de rellevància calculat a partir del
                               // grau entrant
  struct NodeGrau *seguent; // Punter al següent node dins la mateixa posició de
                            // la taula hash

  Aresta *arestesSortints; // Llista d’arestes sortints des d’aquest node
} NodeGrau;

// Estructura principal del graf dirigit
typedef struct {
  size_t mida;       // Mida de la taula hash (nombre de posicions)
  int totalNodes;    // Nombre total de nodes afegits
  int totalEnllacos; // Nombre total d’enllaços (arestes)
  NodeGrau **nodes;  // Taula hash amb llistes encadenades de nodes
} GrafDirigit;

// Prototips de funcions
GrafDirigit *construirGrafDirigitDeDocuments(
    Document
        *documents); // Construeix el graf a partir d’una llista de documents
void calcularPuntuacionsRellevancia(
    GrafDirigit *graf); // Calcula la puntuació de rellevància dels nodes
int obtenirGrauEntrantDocument(
    GrafDirigit *graf, int idDocument); // Retorna el grau entrant d’un document
double obtenirPuntuacioRellevanciaDocument(
    GrafDirigit *graf,
    int idDocument); // Retorna la puntuació de rellevància d’un document
void imprimirDocumentsPerRellevancia(
    GrafDirigit *graf, Document *documents,
    int maxResultats); // Imprimeix els documents ordenats per rellevància
void alliberarGrafDirigit(
    GrafDirigit *graf); // Allibera la memòria ocupada pel graf

Document **ordenaDocumentsPerRellevancia(Document *llista, int *nombre);

#endif // GRAPH_H              // Fi del guardià d’inclusió
