#include "../src/document.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

// Incloure les llibreries necessàries per redirigir la sortida estàndard
#include <unistd.h>
#include <fcntl.h>

// test la deserialització del document a partir d'un fitxer de mostra
void test_document_desserialize() {
  runningtest("test_document_desserialize");
  {
  // Redirigir la sortida estàndard a /dev/null per suprimir la sortida
  int stdout_fd = dup(STDOUT_FILENO); // Desar l'identificador original de stdout
  int dev_null = open("/dev/null", O_WRONLY); // Obrir /dev/null per escriptura
  dup2(dev_null, STDOUT_FILENO); // Redirigir stdout a /dev/null
  close(dev_null); // Tancar l'identificador de /dev/null

  Document *doc = document_desserialize("datasets/wikipedia12/0.txt"); // deserialitzar document del fitxer de
  // Restaurar la sortida estàndard original
  dup2(stdout_fd, STDOUT_FILENO); // Restaurar stdout original
  close(stdout_fd); // Tancar l'identificador desat

  assert(doc != NULL);  // omprovar que el document no és NULL
  assert(doc->id == 0); // comprovar que l'id és 0
  assert(doc->title != NULL && strlen(doc->title) > 0); // comprovar que el títol no és NULL i té longitud > 0
  assert(doc->body != NULL && strlen(doc->body) > 0); // comprovar que el cos no és NULL i té longitud > 0
  assert(doc->links != NULL); // comprovar que hi ha enllaços (assumint que el
                              // fitxer de mostra en té)
  freeDocument(doc); // alliberar memòria del document
  }
  successtest();
}


// Test de la càrrega de la llista enllaçada de documents
void test_loadAllDocuments() {
  runningtest("test_loadAllDocuments");
  {
  // Redirigir la sortida estàndard a /dev/null per suprimir la sortida
  int stdout_fd = dup(STDOUT_FILENO); // Desar l'identificador original de stdout
  int dev_null = open("/dev/null", O_WRONLY); // Obrir /dev/null per escriptura
  dup2(dev_null, STDOUT_FILENO); // Redirigir stdout a /dev/null
  close(dev_null); // Tancar l'identificador de /dev/null

  // Carregar tots els documents del directori de dades
  Document *docs = loadAllDocuments("datasets/wikipedia12"); // carregar tots els documents del directori
  // Restaurar la sortida estàndard original
  dup2(stdout_fd, STDOUT_FILENO); // Restaurar stdout original
  close(stdout_fd); // Tancar l'identificador desat
  assert(docs != NULL);        // Comprovar que la llista no és NULL
  int contador = 0;
  Document *current = docs;
  while (current != NULL) { // Iterar per la llista de documents
    // Comprovar que el títol no és NULL
    assert(current->title != NULL);
    assert(current->body != NULL); // Comprovar que el cos no és NULL
    contador++;                    // Incrementar el comptador
    current = current->next;
  }
  // Comprovar que s'ha carregat almenys un document
  assert(contador > 0);
  // Alliberar tots els documents
  current = docs;
  while (current != NULL) {
    Document *temp = current;
    current = current->next;
    freeDocument(temp); // Alliberar document
  }
  }
  successtest();
}

// test de la llista enllaçada d'enllaços afegits i alliberats
void test_links_list() {
  runningtest("test_linked_list");
  {
  Links *links = NULL; // inicialitzar llista d'enllaços a NULL
  LinksAdd(&links, 1,"Link text 1"); // afegir enllaç amb id 1 i text "Link text 1"
  LinksAdd(&links, 2,"Link text 2"); // afegir enllaç amb id 2 i text "Link text 2"
  assert(links != NULL);   // comprovar que la llista no és NULL
  assert(links->documentId ==2); // comprovar que el primer enllaç té id 2 (afegit últim)
  assert(strcmp(links->linkText, "Link text 2") == 0); // comprovar que el text del primer enllaç és correcte
  assert(links->next != NULL);          // comprovar que hi ha un segon enllaç
  assert(links->next->documentId == 1); // comprovar que el segon enllaç té id 1
  freeLinks(links);                     // alliberar la llista d'enllaços
  }
  successtest();
}

void document_tests(){
  running("document_tests");
  {
  test_document_desserialize();
  test_loadAllDocuments();
  test_links_list();
  }
  success();
}
