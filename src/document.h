#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// estructura dels links
typedef struct Links {
  int documentId; // ID del document de destinació
  char *title;
  char *linkText;     //text de l'enllaç
  struct Links *next; // punter al següent enllaç
} Links;

// estructura de document
typedef struct Document {
  int id;                //ID del document
  char *title;           //títol del document
  char *body;            //cos del document
  Links *links;          //llista d'enllaços
  struct Document *next; //punter al seguent document
  double relevanceScore;  //puntuació de rellevància per a ordenar documents
} Document;

Document *document_desserialize(
    char *path); // declarar de la funció document_desserialize
Document *loadAllDocuments(
    const char *directoryPath); // declarar la funció que carrega documents

void freeDocument(Document *document); // declarar de freeDocument
void freeLinks(Links *link);           // declarar de freeLinks

Links *LinksInit(); //inicialitzar llista de enllaços

void LinksAdd(Links **links, int documentId, char *linkText); // declarar LinksAdd

#endif // DOCUMENT_H
