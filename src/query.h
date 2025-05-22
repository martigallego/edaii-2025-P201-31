// estructura per a la consulta (query)
typedef struct Query {
  char *keyword;      // paraula clau
  struct Query *next; // següent element
} Query;

// inicialitzar llista Query a partir d'una cadena de paraules separades per
// espais
Query *initQueryFromString(const char *queryString);

// alliberar memòria de la llista Query
void freeQuery(Query *query);

// cerca lineal a través de la llista de documents per trobar documents que
// contenen totes les paraules clau retorna una llista enllaçada de documents
// que coincideixen (fins a maxResults)
typedef struct Document Document;
Document *linearSearchDocuments(Document *documents, Query *query,
                                int maxResults);

// imprimir documents de la llista a la CLI
void printDocuments(Document *documents);

// afegir una nova consulta a la cua de les últimes consultes
void addLastQuery(Query *query);

// mostrar les últimes 3 consultes per la CLI
void showLastQueries();
