#include "../src/document.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

// Incloure les llibreries necessàries per redirigir la sortida estàndard
#include <unistd.h>
#include <fcntl.h>

void test_document_desserialize() {
  runningtest("test_document_desserialize");
  {
  int stdout_fd = dup(STDOUT_FILENO);
  int dev_null = open("/dev/null", O_WRONLY);
  dup2(dev_null, STDOUT_FILENO);
  close(dev_null);

  Document *doc = deserialitzaDocument("datasets/wikipedia12/0.txt");

  dup2(stdout_fd, STDOUT_FILENO);
  close(stdout_fd);

  assert(doc != NULL);
  assert(doc->id == 0);
  assert(doc->titol != NULL && strlen(doc->titol) > 0);
  assert(doc->cos != NULL && strlen(doc->cos) > 0);
  successtest();
  alliberaDocument(doc);
  }
}

void test_carregaTotsElsDocuments() {
  runningtest("test_carregaTotsElsDocuments");
  {
  int stdout_fd = dup(STDOUT_FILENO);
  int dev_null = open("/dev/null", O_WRONLY);
  dup2(dev_null, STDOUT_FILENO);
  close(dev_null);

  Document *docs = carregaTotsElsDocuments("datasets/wikipedia12");

  dup2(stdout_fd, STDOUT_FILENO);
  close(stdout_fd);
  assert(docs != NULL);
  int contador = 0;
  Document *current = docs;
  while (current != NULL) {
    assert(current->titol != NULL);
    assert(current->cos != NULL);
    contador++;
    current = current->seguent;
  }
  assert(contador > 0);
  current = docs;
  while (current != NULL) {
    Document *temp = current;
    current = current->seguent;
    alliberaDocument(temp);
  }
  successtest();
}
}
// test de la llista enllaçada d'enllaços afegits i alliberats
void test_enllacos_list() {
  runningtest("test_enllacos_list");
  {
  Enllacos *enllacos = NULL; // inicialitzar llista d'enllaços a NULL
  afegeixEnllac(&enllacos, 1,"Link text 1"); // afegir enllaç amb id 1 i text "Link text 1"
  afegeixEnllac(&enllacos, 2,"Link text 2"); //afegir enllaç amb id 2 i text "Link text 2"
  assert(enllacos != NULL);   //comprovar que la llista no és NULL
  assert(enllacos->idDocumentDesti == 1); // comprovar que el primer enllaç té id 1 (afegit primer)
  assert(strcmp(enllacos->textEnllac, "Link text 1") == 0); //comprovar que el text del primer enllaç és correcte
  assert(enllacos->seguent != NULL);   //comprovar que hi ha un segon enllaç
  assert(enllacos->seguent->idDocumentDesti == 2); //comprovar que el segon enllaç té id 2
  alliberaEnllacos(enllacos); //alliberar la llista d'enllaços
  }
  successtest();
}

//test de la llista enllaçada de documents: compta nodes 
void test_document_linked_list_count() {
    runningtest("test_document_linked_list_count");
    Document *list = NULL;
    for (int i = 3; i > 0; i--) {
        Document *doc = (Document*)malloc(sizeof(Document));
        doc->id = i; 
        doc->titol = NULL; 
        doc->cos = NULL;
        doc->enllacos = NULL;
        doc->seguent = list;  
        list = doc; 
    }
    int count = 0;
    Document *current = list; 
    while (current != NULL) {
        count++;
        current = current->seguent; 
    }
    assertEqualsInt(count, 3);
    current = list;
    while (current != NULL) {
        Document *temp = current;
        current = current->seguent; 
        free(temp);
    }
    successtest();
}

void test_document_linked_list_order() {
    runningtest("test_document_linked_list_order");
    Document *list = NULL;
    for (int i = 3; i > 0; i--) {
        Document *doc = (Document*)malloc(sizeof(Document));
        doc->id = i;
        doc->titol = NULL;
        doc->cos = NULL;
        doc->enllacos = NULL;
        doc->seguent = list;
        list = doc;
    }
    assertEqualsInt(list->id, 1);
    assertEqualsInt(list->seguent->id, 2);
    assertEqualsInt(list->seguent->seguent->id, 3);
    Document *current = list;
    while (current != NULL) {
        Document *temp = current;
        current = current->seguent;
        free(temp);
    }
    successtest();
}

void test_document_linked_list_add_remove() {
    runningtest("test_document_linked_list_add_remove");
    Document *list = NULL;
    for (int i = 1; i <= 3; i++) {
        Document *doc = (Document*)malloc(sizeof(Document));
        doc->id = i;
        doc->titol = NULL;
        doc->cos = NULL;
        doc->enllacos = NULL;
        doc->seguent = list; 
        list = doc;
    }
    Document *prev = NULL;
    Document *current = list;
    while (current != NULL) {
        if (current->id == 2) {
            if (prev == NULL) {
                list = current->seguent;
            } else {
                prev->seguent = current->seguent; 
            }
            free(current);
            break;
        }
        prev = current;
        current = current->seguent;
    }
    int count = 0;
    current = list;
    while (current != NULL) {
        count++;
        current = current->seguent;
    }
    assertEqualsInt(count, 2);
    current = list;
    while (current != NULL) {
        Document *temp = current;
        current = current->seguent; // move to next node
        free(temp);
    }
    successtest();
}

void document_tests(){
  running("DOCUMENT TESTS");
  {
  test_document_desserialize();
  test_carregaTotsElsDocuments();
  test_enllacos_list();
  test_document_linked_list_count();
  test_document_linked_list_order();
  test_document_linked_list_add_remove();
  }
  success();
}
