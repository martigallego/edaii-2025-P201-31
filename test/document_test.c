#include "document.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

// test la deserialització del document a partir d'un fitxer de mostra
void test_document_desserialize() {
  Document *doc = document_desserialize(
      "datasets/wikipedia12/0.txt"); // deserialitzar document del fitxer de
                                     // mostra
  assert(doc != NULL);  // omprovar que el document no és NULL
  assert(doc->id == 0); // comprovar que l'id és 0
  assert(doc->title != NULL &&
         strlen(doc->title) >
             0); // comprovar que el títol no és NULL i té longitud > 0
  assert(doc->body != NULL &&
         strlen(doc->body) >
             0); // comprovar que el cos no és NULL i té longitud > 0
  assert(doc->links != NULL); // comprovar que hi ha enllaços (assumint que el
                              // fitxer de mostra en té)
  freeDocument(doc); // alliberar memòria del document
  printf("test_document_desserialize passed\n"); // imprimir missatge de test
                                                 // passat
}

// test linked list of documents loading
void test_loadAllDocuments() {
  Document *docs = loadAllDocuments(
      "datasets/wikipedia12"); // carregar tots els documents del directori
  assert(docs != NULL);        // comprovar que la llista no és NULL
  int contador = 0;
  Document *current = docs;
  while (current != NULL) { // iterar per la llista de documents
    // comprovar que el títol no és NULL
    assert(current->title != NULL);
    assert(current->body != NULL); // comprovar que el cos no és NULL
    contador++;                    // aumentar contador
    current = current->next;
  }
  // comprovar que s'ha carregat almenys un document
  assert(contador > 0);
  // alliberar tots els documents
  current = docs;
  while (current != NULL) {
    Document *temp = current;
    current = current->next;
    freeDocument(temp); // alliberar document
  }
  printf("test_loadAllDocuments passed\n"); // imprimir missatge de test passat
}

// test de la llista enllaçada d'enllaços afegits i alliberats
void test_links_list() {
  Links *links = NULL; // inicialitzar llista d'enllaços a NULL
  LinksAdd(&links, 1,
           "Link text 1"); // afegir enllaç amb id 1 i text "Link text 1"
  LinksAdd(&links, 2,
           "Link text 2"); // afegir enllaç amb id 2 i text "Link text 2"
  assert(links != NULL);   // comprovar que la llista no és NULL
  assert(links->documentId ==
         2); // comprovar que el primer enllaç té id 2 (afegit últim)
  assert(strcmp(links->linkText, "Link text 2") ==
         0); // comprovar que el text del primer enllaç és correcte
  assert(links->next != NULL);          // comprovar que hi ha un segon enllaç
  assert(links->next->documentId == 1); // comprovar que el segon enllaç té id 1
  freeLinks(links);                     // alliberar la llista d'enllaços
  printf("test_links_list passed\n");   // imprimir missatge de test passat
}

// funció principal per executar els tests
int main() {
  test_document_desserialize(); // test de deserialització de document
  test_loadAllDocuments();      // test de càrrega de documents
  test_links_list();            // test de llista d'enllaços
  printf("all document tests passed.\n"); // imprimir missatge final de tots els
                                          // tests passats
  return 0;
}
