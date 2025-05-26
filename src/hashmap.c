#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Per a isalnum, tolower

// Constant per a la mida inicial del hashmap.
// Un nombre primer ajuda a distribuir millor els elements.
#define HASHMAP_INITIAL_SIZE 10007 // Exemple: un nombre primer gran

// --- Funcions auxiliars per a DocumentIdNode ---

// Allibera la memòria d'una llista de DocumentIdNode
void freeDocumentIdList(DocumentIdNode *head) {
    DocumentIdNode *current = head;           // Comença pel primer node
    while (current != NULL) {                  // Mentre hi hagi nodes
        DocumentIdNode *temp = current;       // Guarda el node actual
        current = current->next;               // Passa al següent
        free(temp);                           // Allibera el node actual
    }
}

// Afegeix un documentId a la llista de DocumentIdNode, evitant duplicats.
// Retorna el nou cap de la llista.
DocumentIdNode *addDocumentId(DocumentIdNode *head, int documentId) {
    // Comprova si l'ID ja existeix a la llista per evitar duplicats
    DocumentIdNode *current = head;
    while (current != NULL) {
        if (current->documentId == documentId) {
            return head; // L'ID ja està a la llista, no fem res
        }
        current = current->next;
    }

    // Si no existeix, crea un nou node i l'afegeix a l'inici
    DocumentIdNode *newNode = (DocumentIdNode *)malloc(sizeof(DocumentIdNode));
    if (newNode == NULL) {
        perror("Error en assignar memòria per DocumentIdNode");
        exit(EXIT_FAILURE);
    }
    newNode->documentId = documentId;         // Assigna l'ID de document
    newNode->next = head;                      // Enllaça amb l'antic cap de la llista
    return newNode;                            // Retorna el nou cap
}


// --- Funcions del Hashmap ---

// Inicialitza un nou hashmap amb una mida donada
HashMap *createHashMap(size_t size) {
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    if (map == NULL) {
        perror("Error en assignar memòria per HashMap");
        exit(EXIT_FAILURE);
    }
    map->size = size;                          // Assigna la mida del hashmap
    map->num_elements = 0;                     // Inicialitza el comptador d'elements
    map->entries = (HashEntry **)calloc(size, sizeof(HashEntry *)); // calloc inicialitza a NULL
    if (map->entries == NULL) {
        perror("Error en assignar memòria per les entrades del HashMap");
        free(map);
        exit(EXIT_FAILURE);
    }
    return map;                               // Retorna el nou hashmap creat
}

// Allibera tota la memòria associada al hashmap
void freeHashMap(HashMap *map) {
    if (map == NULL) return;                   // Si és NULL, no fa res

    for (size_t i = 0; i < map->size; i++) {
        HashEntry *current = map->entries[i]; // Comença per la cubeta i
        while (current != NULL) {              // Mentre hi hagi entrades
            HashEntry *temp = current;         // Guarda l'entrada actual
            current = current->next;           // Avança a la següent entrada
            free(temp->word);                   // Allibera la paraula
            freeDocumentIdList(temp->docIds);  // Allibera la llista d'IDs de documents
            free(temp);                        // Allibera l'entrada del hashmap
        }
    }
    free(map->entries);                        // Allibera l'array de punters
    free(map);                                // Allibera la estructura del hashmap
}

// Funció hash simple (FNV-1a adaptada per a cadenes)
// Crèdits: Basada en la funció FNV-1a (Fowler-Noll-Vo hash)
unsigned int hash(const char *word, size_t map_size) {
    unsigned int hash_val = 2166136261U;      // FNV_PRIME_32_INIT
    for (int i = 0; word[i] != '\0'; i++) {
        hash_val ^= (unsigned char)word[i];   // XOR amb el caràcter actual
        hash_val *= 16777619U;                 // Multiplica pel primer FNV
    }
    return hash_val % map_size;                // Retorna l'índex segons la mida
}

// Insereix un ID de document a la llista d'IDs d'una paraula al hashmap.
// Si la paraula no existeix, crea una nova entrada.
void insertWordIntoHashMap(HashMap *map, const char *word, int documentId) {
    unsigned int index = hash(word, map->size); // Calcula l'índex hash

    // Cerca si la paraula ja existeix en aquesta cubeta
    HashEntry *current = map->entries[index];
    while (current != NULL) {
        if (strcmp(current->word, word) == 0) {
            // Paraula trobada, afegeix el documentId a la seva llista
            current->docIds = addDocumentId(current->docIds, documentId);
            return;
        }
        current = current->next;
    }

    // Si la paraula no existeix a la cubeta, crea una nova entrada
    HashEntry *newEntry = (HashEntry *)malloc(sizeof(HashEntry));
    if (newEntry == NULL) {
        perror("Error en assignar memòria per HashEntry");
        exit(EXIT_FAILURE);
    }
    newEntry->word = strdup(word);             // Copia la paraula
    if (newEntry->word == NULL) {
        perror("Error en assignar memòria per la paraula de HashEntry");
        free(newEntry);
        exit(EXIT_FAILURE);
    }
    newEntry->docIds = NULL;                    // Inicialitza la llista d'IDs
    newEntry->docIds = addDocumentId(newEntry->docIds, documentId); // Afegeix el primer ID

    // Afegeix la nova entrada a l'inici de la llista encadenada de la cubeta
    newEntry->next = map->entries[index];
    map->entries[index] = newEntry;
    map->num_elements++;                        // Incrementa el nombre d'elements
}

// Cerca una paraula al hashmap i retorna la llista d'IDs de documents associada
DocumentIdNode *findWordInHashMap(HashMap *map, const char *word) {
    unsigned int index = hash(word, map->size); // Calcula l'índex hash
    HashEntry *current = map->entries[index];
    while (current != NULL) {
        if (strcmp(current->word, word) == 0) {
            return current->docIds;              // Paraula trobada, retorna la seva llista d'IDs
        }
        current = current->next;
    }
    return NULL;                                // Paraula no trobada
}

// Normalitza una paraula: converteix a minúscules i elimina caràcters no alfanumèrics.
// Retorna una nova cadena que s'ha de alliberar amb free().
char *normalizeWord(const char *word) {
    if (word == NULL) return NULL;              // Si la paraula és NULL, retorna NULL

    // Aproximació: una paraula normalitzada no serà més llarga que l'original.
    // +1 per al terminador null.
    char *normalized = (char *)malloc(strlen(word) + 1);
    if (normalized == NULL) {
        perror("Error en assignar memòria per paraula normalitzada");
        exit(EXIT_FAILURE);
    }

    int j = 0;
    for (int i = 0; word[i] != '\0'; i++) {
        // Només copia caràcters alfanumèrics i els converteix a minúscules
        if (isalnum((unsigned char)word[i])) {
            normalized[j++] = tolower((unsigned char)word[i]);
        }
    }
    normalized[j] = '\0';                       // Assegura el terminador nul

    // Si la paraula normalitzada és una cadena buida (ex. només puntuació),
    // podem retornar NULL o una cadena buida. Ara retornem NULL.
    if (j == 0) {
        free(normalized);
        return NULL;
    }

    return normalized;
}

// Construeix l'índex invertit a partir de la llista de documents
HashMap *buildReverseIndex(Document *documents) {
    // Escollim una mida inicial per al hashmap.
    // Podria ser dinàmic o basat en el nombre esperat de paraules úniques.
    HashMap *reverseIndex = createHashMap(HASHMAP_INITIAL_SIZE);

    Document *currentDoc = documents;
    while (currentDoc != NULL) {
        // Tokenitza el títol
        char *titleCopy = strdup(currentDoc->title);
        if (titleCopy == NULL) { perror("strdup title"); exit(EXIT_FAILURE); }
        char *token = strtok(titleCopy, " \t\n.,;!?-:()\"'"); // Delimitadors comuns
        while (token != NULL) {
            char *normalized = normalizeWord(token);
            if (normalized != NULL && strlen(normalized) > 0) { // Assegura que no sigui una paraula buida després de normalitzar
                insertWordIntoHashMap(reverseIndex, normalized, currentDoc->id);
                free(normalized);                   // Allibera la paraula normalitzada
            }
            token = strtok(NULL, " \t\n.,;!?-:()\"'");
        }
        free(titleCopy);                           // Allibera la còpia del títol

        // Tokenitza el cos del document
        char *bodyCopy = strdup(currentDoc->body);
        if (bodyCopy == NULL) { perror("strdup body"); exit(EXIT_FAILURE); }
        token = strtok(bodyCopy, " \t\n.,;!?-:()\"'"); // Mateixos delimitadors
        while (token != NULL) {
            char *normalized = normalizeWord(token);
            if (normalized != NULL && strlen(normalized) > 0) {
                insertWordIntoHashMap(reverseIndex, normalized, currentDoc->id);
                free(normalized);                   // Allibera la paraula normalitzada
            }
            token = strtok(NULL, " \t\n.,;!?-:()\"'");
        }
        free(bodyCopy);                            // Allibera la còpia del cos

        currentDoc = currentDoc->next;             // Passa al següent document
    }
    printf("Índex invertit construït amb %zu paraules úniques.\n", reverseIndex->num_elements);
    return reverseIndex;
}
