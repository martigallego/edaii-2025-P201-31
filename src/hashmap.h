#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h> // Per a size_t
#include "document.h" // Necessari per a l'estructura Document

// --- Estructura per a la llista d'IDs de documents (dins de cada entrada del hashmap) ---
// Cada node emmagatzema un ID de document on apareix una paraula
typedef struct NodeIdDocument {
    int idDocument;               // ID del document
    struct NodeIdDocument *seguent;  // Punter al següent node de la llista
} NodeIdDocument;

// --- Estructura per a una entrada al Hashmap ---
// Mapeja una paraula (paraulaClau) a una llista de NodeIdDocument (on apareix la paraula)
typedef struct EntradaHash {
    char *paraula;                  // La paraula (clau del hashmap)
    NodeIdDocument *idsDocuments;      // Llista enllaçada d'IDs de documents on es troba la paraula
    struct EntradaHash *seguent;      // Punter per a manejar col·lisions (encadenament)
} EntradaHash;

// --- Estructura per al Hashmap complet ---
typedef struct HashMap {
    EntradaHash **entrades;        // Array de punters a EntradaHash (les "cubetes" del hashmap)
    size_t mida;                // Mida de l'array de punters (nombre de cubetes)
    size_t num_elements;        // Nombre total de paraules úniques emmagatzemades
} HashMap;


// --- Declaracions de funcions del Hashmap ---

// Inicialitza un nou hashmap amb una mida donada
HashMap *creaHashMap(size_t mida);

// Allibera tota la memòria associada al hashmap
void alliberaHashMap(HashMap *mapa);

// Funció hash per convertir una paraula en un índex
unsigned int hash(const char *paraula, size_t mida_mapa);

// Insereix un ID de document a la llista d'IDs d'una paraula.
// Crea la paraula al hashmap si no existeix.
void insereixParaulaEnHashMap(HashMap *mapa, const char *paraula, int idDocument);

// Normalitza una paraula (a minúscules i elimina caràcters no alfanumèrics)
char *normalitzaParaula(const char *paraula);

// Construeix l'índex invertit a partir de la llista de documents
HashMap *construeixIndexInvertit(Document *documents);

// Cerca una paraula al hashmap i retorna la llista d'IDs de documents associada
NodeIdDocument *trobaParaulaEnHashMap(HashMap *mapa, const char *paraula);

// Funció auxiliar per alliberar una llista de NodeIdDocument
void alliberaLlistaIdsDocuments(NodeIdDocument *cap);

// Funció auxiliar per afegir un ID a una llista de NodeIdDocument (evitant duplicats)
NodeIdDocument *afegeixIdDocument(NodeIdDocument *cap, int idDocument);

#endif // HASHMAP_H