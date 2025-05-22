#include "../src/document.h"
#include "../src/query.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

// funció auxiliar per crear un document de prova
Document *createTestDocument(int id, const char *title, const char *body) {
  // reservar memòria per al document
  Document *doc = (Document *)malloc(sizeof(Document));
  // assignar id
  doc->id = id;
  // copiar títol
  doc->title = strdup(title);
  // copiar cos
  doc->body = strdup(body);
  // inicialitzar enllaços a NULL
  doc->links = NULL;
  // inicialitzar següent document a NULL
  doc->next = NULL;
  return doc;
}

// test 1: crear una llista de consultes a partir d'una cadena
void test_initQueryFromString() {
  runningtest("test_initQueryFromString");
  {
  Query *query = initQueryFromString(
      "exemple de consulta de prova"); // inicialitzar la llista de consultes
                                       // amb la cadena "exemple de consulta de
                                       // prova"
  assert(query != NULL); // comprovar que la llista no és NULL
  assert(strcmp(query->keyword, "test") ==
         0); // comprovar que la primera paraula clau és "test"
  assert(query->next != NULL); // comprovar que hi ha un següent node
  // comprovar que la segona paraula clau és "query"
  assert(strcmp(query->next->keyword, "query") == 0);
  // comprovar que hi ha un tercer node
  assert(query->next->next != NULL);
  // comprovar que la tercera paraula clau és "example"
  assert(strcmp(query->next->next->keyword, "example") == 0);
  // comprovar que no hi ha més nodes
  assert(query->next->next->next == NULL);
  // alliberar la llista de consultes
  freeQuery(query);
  // imprimir missatge de test passat
  }
  successtest();
}

// test 2: alliberar la llista de consultes
void test_freeQuery() {
  runningtest("test_freeQuery");
  {
  // inicialitzar la llista de consultes amb la cadena "free test"
  Query *query = initQueryFromString("free test");
  // alliberar la llista de consultes
  freeQuery(query);
  }
  successtest();
}

// test 3: cerca lineal amb documents i consultes
void test_linearSearchDocuments() {
  runningtest("test_linearSearchDocuments");
  {
  // crear document de prova 1
  Document *doc1 = createTestDocument(
      1, "Hola mundo",
      "aquest es un document de prova."); // crear document de prova 1
  Document *doc2 = createTestDocument(
      2, "Otro Doc", "contenido diferente."); // crear document de prova 2

  doc1->next = doc2; // enllaçar doc1 amb doc2

  // inicialitzar la consulta amb la paraula "test"
  Query *query = initQueryFromString("test");
  // fer la cerca lineal amb doc1 com a llista de documents
  Document *resultats = linearSearchDocuments(doc1, query, 5);
  // comprovar que s'han trobat resultats
  assert(resultats != NULL);
  // comprovar que el primer resultat és el document 1
  assert(resultats->id == 1);
  // comprovar que només hi ha un resultat
  assert(resultats->next == NULL);

  // alliberar la consulta
  freeQuery(query);
  // alliberar documents
  freeDocument(doc1);
  freeDocument(doc2);
  }
  successtest();
}

void query_tests(){
  running("query_tests");
  {
  test_initQueryFromString();
  test_freeQuery();
  test_linearSearchDocuments();
  }
  success();
}
