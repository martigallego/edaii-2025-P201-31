#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// estructura dels enllaços
typedef struct Enllacos {
  int idDocumentDesti; // ID del document de destinació
  char *titolEnllac;   // títol del document enllaçat (opcional)
  char *textEnllac;    // text de l'enllaç
  struct Enllacos *seguent; // punter al següent enllaç
} Enllacos;

// estructura de document
typedef struct Document {
  int id;                // ID del document
  char *titol;           // títol del document
  char *cos;             // cos del document
  Enllacos *enllacos;    // llista d'enllaços
  struct Document *seguent; // punter al següent document
  double puntuacioRellevancia; // Puntuació de rellevància per a ordenar documents
} Document;

// declarar de la funció deserialitzaDocument
Document *deserialitzaDocument(char *camins); 

// declarar la funció que carrega tots els documents
Document *carregaTotsElsDocuments(const char *rutaDirectori); 

// declarar de alliberaDocument
void alliberaDocument(Document *document); 

// declarar de alliberaEnllacos
void alliberaEnllacos(Enllacos *enllac);    

// inicialitzar llista de enllaços
Enllacos *iniciaEnllacos(); 

// declarar afegeixEnllac
void afegeixEnllac(Enllacos **enllacos, int idDocumentDesti, const char *textEnllac); 

#endif // DOCUMENT_H