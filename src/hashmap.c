#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Per a isalnum(funcio nomes atmet alfanumeric i ho pasa a minuscula), tolower(funcio de posar tot el minuscula)


#define HASHMAP_INITIAL_SIZE 10007 //establir la mida inicial de l'array que conté les "cubetes" (buckets) del hashmap

// Allibera la memòria d'una llista de DocumentIdNode
void freeDocumentIdList(DocumentIdNode *head) {
    DocumentIdNode *current = head;  //comença pel primer node
    while (current != NULL) {  // mentre hi hagi nodes
        DocumentIdNode *temp = current; //guarda el node actual
        current = current->next;  //passa al següent
        free(temp); //allibera el node actual
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
    newNode->documentId = documentId;  // Assigna l'ID de document
    newNode->next = head;   //Enllaça amb l'antic cap de la llista
    return newNode;   //retorna el nou cap
}


//Funcions del Hashmap

// Inicialitza un nou hashmap amb una mida donada
HashMap *createHashMap(size_t size) {
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    if (map == NULL) {
        perror("Error en assignar memòria per HashMap");
        exit(EXIT_FAILURE);
    }
    map->size = size;  //assigna la mida del hashmap
    map->num_elements = 0;   //inicialitza el comptador d'elements
    map->entrades = (EntradaHash **)calloc(size, sizeof(EntradaHash *)); //calloc inicialitza a NULL
    if (map->entrades == NULL) {
        perror("Error en assignar memòria per les entrades del HashMap");
        free(map);
        exit(EXIT_FAILURE);
    }
    return map;   //retorna el nou hashmap creat
}

// Allibera tota la memòria associada al hashmap
void freeHashMap(HashMap *map) {
    if (map == NULL) return;                   // Si és NULL, no fa res

    for (size_t i = 0; i < map->size; i++) {
        EntradaHash *current = map->entrades[i]; //recorre les entrades del hashmap
        while (current != NULL) {   //mentre hi hagi entrades
            EntradaHash *temp = current;  //guarda l'entrada actual
            current = current->next; //avança a la següent entrada
            free(temp->word);   //allibera la paraula
            freeDocumentIdList(temp->docIds);  //allibera la llista d'IDs de documents
            free(temp);  //allibera l'entrada del hashmap
        }
    }
    free(map->entrades); //allibera la array de punters
    free(map); //allibera la estructura del hashmap
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

//Insereix un ID de document a la llista d'IDs d'una paraula al hashmap.
//si la paraula no existeix, crea una nova entrada.
void insertWordIntoHashMap(HashMap *map, const char *word, int documentId) {
    unsigned int index = hash(word, map->size); //calcula l'índex hash

    //cerca si la paraula ja existeix en aquesta cubeta
    EntradaHash *current = map->entrades[index]; //recorre les entrades de la cubeta
    while (current != NULL) {
        if (strcmp(current->word, word) == 0) {
            // Paraula trobada, afegeix el documentId a la seva llista
            current->docIds = addDocumentId(current->docIds, documentId);
            return;
        }
        current = current->next; //avança a la següent entrada
    }

    //si la paraula no existeix a la cubeta, crea una nova entrada
    EntradaHash *newEntry = (EntradaHash *)malloc(sizeof(EntradaHash)); //crea una nova entrada
    if (newEntry == NULL) { //si no hi ha memòria disponible, retorna
        perror("Error en assignar memòria per EntradaHash");
        exit(EXIT_FAILURE);
    }
    newEntry->word = strdup(word);             //copia la paraula
    if (newEntry->word == NULL) { //si no hi ha memòria disponible, retorna
        perror("Error en assignar memòria per la paraula de EntradaHash");
        free(newEntry); //allibera la nova entrada
        exit(EXIT_FAILURE); //finalitza el programa
    }
    newEntry->docIds = NULL;  //inicialitza la llista d'IDs
    newEntry->docIds = addDocumentId(newEntry->docIds, documentId); //afegeix el primer ID

    //afegeix la nova entrada a l'inici de la llista encadenada de la cubeta
    newEntry->next = map->entrades[index]; //estableix la següent entrada
    map->entrades[index] = newEntry; //actualitza la primera entrada de la cubeta
    map->num_elements++;    //incrementa el nombre d'elements
}

// Cerca una paraula al hashmap i retorna la llista d'IDs de documents associada
DocumentIdNode *findWordInHashMap(HashMap *map, const char *word) {
    unsigned int index = hash(word, map->size); //calcula l'índex hash
    EntradaHash *current = map->entrades[index]; //recorre les entrades de la cubeta
    while (current != NULL) {
        if (strcmp(current->word, word) == 0) { //si la paraula coincideix amb la cerca
            return current->docIds;              // Paraula trobada, retorna la seva llista d'IDs
        }
        current = current->next; //avança a la següent entrada
    }
    return NULL;                                // Paraula no trobada
}

// Normalitza una paraula: converteix a minúscules i elimina caràcters no alfanumèrics.
//retorna una nova cadena que s'ha de alliberar amb free().
char *normalizeWord(const char *word) {
    if (word == NULL) return NULL;              // Si la paraula és NULL, retorna NULL

    // Aproximació: una paraula normalitzada no serà més llarga que l'original.
    // +1 per al terminador null.
    char *normalized = (char *)malloc(strlen(word) + 1); //reserva memòria per la paraula normalitzada
    if (normalized == NULL) { //si no hi ha memòria disponible, retorna
        perror("Error en assignar memòria per paraula normalitzada"); 
        exit(EXIT_FAILURE); //finalitza el programa
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
        free(normalized); //allibera la memòria reservada
        return NULL;
    }

    return normalized; //retorna la paraula normalitzada
}

// Construeix l'índex invertit a partir de la llista de documents
HashMap *buildReverseIndex(Document *documents) {
    // Escollim una mida inicial per al hashmap.
    // Podria ser dinàmic o basat en el nombre esperat de paraules úniques.
    HashMap *reverseIndex = createHashMap(HASHMAP_INITIAL_SIZE);

    Document *currentDoc = documents;
    while (currentDoc != NULL) {
        // Tokenitza el títol
        char *titleCopy = strdup(currentDoc->title); //copia el títol per poder-lo modificar
        if (titleCopy == NULL) { perror("strdup title"); exit(EXIT_FAILURE); }
        // 'token' és un punter a la cadena que representa cada paraula o segment separat pels delimitadors especificats.
        // La funció strtok s'utilitza per dividir la cadena en tokens (paraules) basant-se en aquests delimitadors.
        char *token = strtok(titleCopy, " \t\n.,;!?-:()\"'"); //delimitadors 
        while (token != NULL) {
    char *normalized = normalitzar_paraula(token); //normalitza la paraula
    if (normalized != NULL && strlen(normalized) > 0) { //assegura que no sigui una paraula buida després de normalitzar
        introduir_paraula_hashmap(reverseIndex, normalized, currentDoc->id); //introduir la paraula al hashmap
        free(normalized);                   //allibera la paraula normalitzada
    }
            token = strtok(NULL, " \t\n.,;!?-:()\"'"); //token =  apunta a paraula --> extreta del títol o cos del document
        }
        free(titleCopy);   //allibera la còpia del títol

        // Tokenitza el cos del document
        char *bodyCopy = strdup(currentDoc->body);
        if (bodyCopy == NULL) { perror("strdup body"); exit(EXIT_FAILURE); }
        token = strtok(bodyCopy, " \t\n.,;!?-:()\"'"); // Mateixos delimitadors
        while (token != NULL) { 
            char *normalized = normalizeWord(token); //normalitza la paraula
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
