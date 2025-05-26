#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h> // Per a size_t
#include "document.h" // Necessari per a l'estructura Document

// --- Estructura per a la llista d'IDs de documents (dins de cada entrada del hashmap) ---
// Cada node emmagatzema un ID de document on apareix una paraula
typedef struct DocumentIdNode {
    int documentId;               // ID del document
    struct DocumentIdNode *next;  // Punter al següent node de la llista
} DocumentIdNode;

// --- Estructura per a una entrada al Hashmap ---
// Mapeja una paraula (keyword) a una llista de DocumentIdNode (on apareix la paraula)
typedef struct HashEntry {
    char *word;                  // La paraula (clau del hashmap)
    DocumentIdNode *docIds;      // Llista enllaçada d'IDs de documents on es troba la paraula
    struct HashEntry *next;      // Punter per a manejar col·lisions (encadenament)
} HashEntry;

// --- Estructura per al Hashmap complet ---
typedef struct HashMap {
    HashEntry **entries;        // Array de punters a HashEntry (les "cubetes" del hashmap)
    size_t size;                // Mida de l'array de punters (nombre de cubetes)
    size_t num_elements;        // Nombre total de paraules úniques emmagatzemades
} HashMap;


// --- Declaracions de funcions del Hashmap ---

// Inicialitza un nou hashmap amb una mida donada
HashMap *createHashMap(size_t size);

// Allibera tota la memòria associada al hashmap
void freeHashMap(HashMap *map);

// Funció hash per convertir una paraula en un índex
unsigned int hash(const char *word, size_t map_size);

// Insereix un ID de document a la llista d'IDs d'una paraula.
// Crea la paraula al hashmap si no existeix.
void insertWordIntoHashMap(HashMap *map, const char *word, int documentId);

// Normalitza una paraula (a minúscules i elimina caràcters no alfanumèrics)
char *normalizeWord(const char *word);

// Construeix l'índex invertit a partir de la llista de documents
HashMap *buildReverseIndex(Document *documents);

// Cerca una paraula al hashmap i retorna la llista d'IDs de documents associada
DocumentIdNode *findWordInHashMap(HashMap *map, const char *word);

// Funció auxiliar per alliberar una llista de DocumentIdNode
void freeDocumentIdList(DocumentIdNode *head);

// Funció auxiliar per afegir un ID a una llista de DocumentIdNode (evitant duplicats)
DocumentIdNode *addDocumentId(DocumentIdNode *head, int documentId);

#endif // HASHMAP_H
