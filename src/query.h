#ifndef QUERY_H
#define QUERY_H

#include "document.h"

typedef struct Query {
  char *keyword;
  struct Query *next;
} Query;

Query *initQueryFromString(const char *input);
void freeQuery(Query *query);

Document *linearSearchDocuments(Document *documents, Query *query, int maxResults);
void printDocuments(Document *documents);

void addLastQuery(Query *query);
void showLastQueries(void);

#endif // QUERY_H
