#ifndef CONSULTA_H
#define CONSULTA_H

// Inclou definicions de l'estructura Document
#include "document.h"
// Inclou definicions per a la taula de hash utilitzada com a índex invertit
#include "hashmap.h"

// Estructura que representa una consulta amb una paraula clau
typedef struct Consulta {
  char *paraulaClau;          // Paraula clau de la consulta
  struct Consulta *seguent;     // Punter a la següent consulta (llista enllaçada)
} Consulta;

// Inicialitza una estructura Consulta a partir d'una cadena d'entrada
Consulta *iniciaConsultaDesDeString(const char *entrada);

// Allibera la memòria d'una estructura Consulta
void alliberaConsulta(Consulta *consulta);

// Aquesta funció ja no s'utilitza; substituïda per una versió millorada
// Document *cercaDocumentsLineal(Document *documents, Consulta *consulta, int maxResultats);

// Cerca documents utilitzant l'índex invertit i una llista de consultes
Document *cercaDocumentsAmbIndexInvertit(HashMap *indexInvertit, Document *totsElsDocuments, Consulta *consulta, int maxResultats);

// Imprimeix una llista de documents (ID, títol, etc.)
void imprimeixDocuments(Document *documents);

// Desa la consulta més recent en una llista d’historial
void afegeixUltimaConsulta(Consulta *consulta);

// Mostra les últimes consultes realitzades
void mostraUltimesConsultes(void);

// Funció per comparar temps de cerca amb / sense reverse-index
void comparaMetodesBusqueda(HashMap *indexInvertit, Document *documents, Consulta *consulta);

#endif // CONSULTA_H