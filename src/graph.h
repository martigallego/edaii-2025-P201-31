#ifndef GRAFO_H
#define GRAFO_H

#include "document.h" 
#include <stddef.h>   // Per a size_t

// Estructura per representar un node del graf dirigit
typedef struct NodeGrau {
    int idDocument;                // ID del document
    int grauEntrant;               // Nombre d'enllaços que apunten a aquest document (indegree)
    int grauSortint;               // Nombre d'enllaços que surten d'aquest document (outdegree)
    double puntuacioRellevancia;   // Puntuació de rellevància basada en grau entrant
    struct NodeGrau *seguent;      // Punter al següent node (per llista encadenada a la taula hash)
} NodeGrau;

// Estructura principal del graf dirigit
typedef struct GrafDirigit {
    NodeGrau **nodes;      // Array de punters a nodes (per taula hash)
    size_t mida;           // Mida de la taula hash
    int totalNodes;        // Total de documents en el graf
    int totalEnllacos;     // Total d'enllaços en el graf (canviat de 'totalEnllaços' per consistència)
} GrafDirigit;

// --- Funcions principals del graf ---

// Crea un nou graf dirigit amb una mida determinada
GrafDirigit *crearGrafDirigit(size_t mida);

// Construeix el graf dirigit a partir dels documents carregats
GrafDirigit *construirGrafDirigitDeDocuments(Document *documents);

// Allibera la memòria usada pel graf
void alliberarGrafDirigit(GrafDirigit *graf);

// Afegeix un node al graf (si no existeix encara)
void afegirNodeAGraf(GrafDirigit *graf, int idDocument);

// Afegeix una aresta dirigida entre dos documents
void afegirArestaAGraf(GrafDirigit *graf, int idDocOrigen, int idDocDesti);

// Calcula el grau entrant (indegree) de tots els nodes (Aquesta funció ja no s'utilitza directament, la lògica és a calcularPuntuacionsRellevancia o afegirArestaAGraf)
// void calcularGrausEntrants(GrafDirigit *graf); // Comentada perquè la lògica ja està en altres funcions o no és necessària com a funció pública separada.

// Calcula les puntuacions de rellevància basades en el grau entrant
void calcularPuntuacionsRellevancia(GrafDirigit *graf);

// Obté el grau entrant d'un document específic
int obtenirGrauEntrantDocument(GrafDirigit *graf, int idDocument);

// Obté la puntuació de rellevància d'un document
double obtenirPuntuacioRellevanciaDocument(GrafDirigit *graf, int idDocument);

// Imprimeix els documents ordenats per rellevància
void imprimirDocumentsPerRellevancia(GrafDirigit *graf, Document *documents, int maxResultats);

// Cerca un node per ID de document
NodeGrau *cercarNodeEnGraf(GrafDirigit *graf, int idDocument);

// Funció hash per al graf
unsigned int hashGraf(int idDocument, size_t midaGraf);

#endif // GRAFO_H