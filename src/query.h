#ifndef QUERY_H
#define QUERY_H

#include "document.h"
#include "hashmap.h"

typedef struct Query {
  char *keyword;
  struct Query *next;
} Query;

Query *initQueryFromString(const char *input);
void freeQuery(Query *query);

// Esta función será reemplazada/mejorada por la nueva búsqueda
// Document *linearSearchDocuments(Document *documents, Query *query, int maxResults);

// Nueva función de búsqueda que utiliza el índice invertido
Document *searchDocumentsWithReverseIndex(HashMap *reverseIndex, Document *allDocuments, Query *query, int maxResults);

void printDocuments(Document *documents);

void addLastQuery(Query *query);
void showLastQueries(void);

#endif // QUERY_H