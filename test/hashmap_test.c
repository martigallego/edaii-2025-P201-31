#include <stdio.h>
#include <stdlib.h>
#include "../src/hashmap.h"
#include "utils.h"

//test 1: crea i allibera un hashmap 
void test_create_and_free() {
    runningtest("test_create_and_free");
    HashMap *map = creaHashMap(10); // creo un hashmap con 10 slot
    assert(map != NULL); 
    assertEqualsInt(map->mida, 10);      
    assertEqualsInt(map->num_elements, 0);  
    alliberaHashMap(map); //allibear hashmap
    successtest();
}

// funció auxiliar per comptar nodes en la llista NodeIdDocument
int count_nodes(NodeIdDocument *head) {
    int count = 0;
    NodeIdDocument *current = head; // assigna el cap a current
    while (current != NULL) { // mentre current no sigui nul
        count++; //incrementa el comptador
        current = current->seguent; //passar al seguent node
    }
    return count;
}

// test 2: insereix una paraula i recupera els ids de documents
void test_insert_and_find() {
    runningtest("test_insert_and_find");
    HashMap *map = creaHashMap(10); // hashmap de 10 posicions
    insereixParaulaEnHashMap(map, "testword", 1); // insereix la paraula amb id 1
    NodeIdDocument *ids = trobaParaulaEnHashMap(map, "testword"); // recupera els ids de documents
    assert(ids != NULL); // la llista no és nul·la
    assertEqualsInt(ids->idDocument, 1); // el primer id és 1
    assertEqualsInt(count_nodes(ids), 1); // hi ha un node
    alliberaHashMap(map); // allibera el hashmap
    successtest();
}

// test 3: evita duplicats d'ids de documents per a la mateixa paraula
void test_prevent_duplicate_ids() {
    runningtest("test_prevent_duplicate_ids"); 
    HashMap *map = creaHashMap(10); // 10 slots
    insereixParaulaEnHashMap(map, "dupword", 2); // insereix 2
    insereixParaulaEnHashMap(map, "dupword", 2); // inserció duplicada
    NodeIdDocument *ids = trobaParaulaEnHashMap(map, "dupword"); // troba la paraula
    assert(ids != NULL); // s'ha trobat la paraula
    assertEqualsInt(count_nodes(ids), 1); // només hauria d'haver un node
    alliberaHashMap(map); // allibera la memòria
    successtest(); 
}

void hashmap_tests_main() {
    running("HASH MAP TESTS");
    test_create_and_free();
    test_insert_and_find();
    test_prevent_duplicate_ids();
    allsuccess();
}
