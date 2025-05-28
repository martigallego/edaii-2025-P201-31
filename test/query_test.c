#include <stdlib.h>
#include "../src/document.h"
#include "../src/query.h" // This is where Consulta, iniciaConsultaDesDeString, alliberaConsulta are defined
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
  doc->titol = strdup(title);
  // copiar cos
  doc->cos = strdup(body);
  // inicialitzar enllaços a NULL
  doc->enllacos = NULL;
  // inicialitzar següent document a NULL
  doc->seguent = NULL;
  return doc;
}

// test 1: crear una llista de consultes a partir d'una cadena
void test_iniciaConsultaDesDeString() {
  runningtest("test_iniciaConsultaDesDeString");
  {
  // Use Consulta and iniciaConsultaDesDeString as defined in query.h
  Consulta *query = iniciaConsultaDesDeString("test query example");
  assert(query != NULL);
  // The structure member is paraulaClau, not keyword
  assert(strcmp(query->paraulaClau, "test") == 0);
  assert(query->seguent != NULL);
  assert(strcmp(query->seguent->paraulaClau, "query") == 0);
  assert(query->seguent->seguent != NULL);
  assert(strcmp(query->seguent->seguent->paraulaClau, "example") == 0);
  assert(query->seguent->seguent->seguent == NULL);
  // Use alliberaConsulta as defined in query.h
  alliberaConsulta(query);
  }
  successtest();
}

// test 2: alliberar la llista de consultes
void test_alliberaConsulta() {
  runningtest("test_alliberaConsulta");
  {
  // Use Consulta and iniciaConsultaDesDeString
  Consulta *query = iniciaConsultaDesDeString("free test");
  // Use alliberaConsulta
  alliberaConsulta(query);
  }
  successtest();
}

// If you need a test for search, you should use cercaDocumentsAmbIndexInvertit
// as declared in query.h, which requires a HashMap.
// The linear search function is explicitly marked as deprecated in query.h.
// So, removing test_linearSearchDocuments for now.

void query_tests(){
  running("QUERY TESTS");
  {
  test_iniciaConsultaDesDeString();
  test_alliberaConsulta();
  // Removed test_linearSearchDocuments as it uses a deprecated function
  }
  success();
}