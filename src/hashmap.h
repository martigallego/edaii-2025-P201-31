#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h> //def tipus de dades i macros 
#include "document.h" //estructura Document

//estructura per a la llista d'IDs de documents (dins de cada entrada del hashmap)
//cada node emmagatzema un ID de document on apareix una paraula
typedef struct DocumentIdNode {
    int documentId;               //ID docu
    struct DocumentIdNode *next;  //punter al següent node de la llista
} DocumentIdNode;

//Estructura per a una entrada al Hashmap
//mapeja una paraula (keyword) a una llista de DocumentIdNode (on apareix la paraula)
typedef struct EntradaHash {
    char *word;                  //la paraula (clau del hashmap)
    DocumentIdNode *docIds;      //llista enllaçada d'IDs de documents on es troba la paraula
    struct EntradaHash *next;      //punter per a manejar col·lisions (encadenament)
} EntradaHash;

// --- Estructura per al Hashmap complet ---
typedef struct HashMap {
    EntradaHash **entrades;        //array de punters a EntradaHash (les "cubetes" del hashmap)
    size_t size;                //mida de l'array de punters (nombre de cubetes)
    size_t num_elements;        //num total de paraules úniques emmagatzemades
} HashMap;


//Declaracions de funcions del Hashmap

HashMap *createHashMap(size_t size); //inicialitza un nou hashmap amb una mida donada

void freeHashMap(HashMap *map); //allibera tota la memòria associada al hashmap
 
unsigned int hash(const char *word, size_t map_size); //funció hash per convertir una paraula en un índex

// Insereix un ID de document a la llista d'IDs d'una paraula, crea la paraula al hashmap si no existeix.
void introduir_paraula_hashmap(HashMap *map, const char *word, int documentId);

//normalitza una paraula (a minúscules i elimina caràcters no alfanumèrics)
char *normalitzar_paraula(const char *word);

HashMap *buildReverseIndex(Document *documents); //Construeix l'índex invertit a partir de la llista de documents

DocumentIdNode *trobar_paraula_hashmap(HashMap *map, const char *word); // construeix el index invertit a partir de la llista de documents

//funció auxiliar per alliberar una llista de DocumentIdNode
void freeDocumentIdList(DocumentIdNode *head);

// Funció auxiliar per afegir un ID a una llista de DocumentIdNode (evitant duplicats)
DocumentIdNode *addDocumentId(DocumentIdNode *head, int documentId);

#endif // HASHMAP_H
