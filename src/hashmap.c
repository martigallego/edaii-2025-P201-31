#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Per a isalnum, tolower
// Constant per a la mida inicial del hashmap.
// Un nombre primer ajuda a distribuir millor els elements i a reduir les col·lisions.
#define MIDA_INICIAL_HASHMAP 10007 // Exemple: un nombre primer gran, triat per una bona distribució.

// --- Funcions auxiliars per a NodeIdDocument ---

// Allibera la memòria d'una llista de NodeIdDocument
// Aquesta funció recorre una llista enllaçada de NodeIdDocument i allibera la memòria de cada node.
void alliberaLlistaIdsDocuments(NodeIdDocument *cap) {
    NodeIdDocument *actual = cap;          // Comença pel primer node de la llista.
    while (actual != NULL) {               // Mentre hi hagi nodes a la llista.
        NodeIdDocument *temp = actual;     // Guarda el punter al node actual per poder alliberar-lo.
        actual = actual->seguent;          // Avança al següent node de la llista.
        free(temp);                        // Allibera la memòria del node actual.
    }
}

// Afegeix un idDocument a la llista de NodeIdDocument, evitant duplicats.
// Retorna el nou cap de la llista.
// Aquesta funció afegeix un ID de document a una llista, només si no hi és ja present.
NodeIdDocument *afegeixIdDocument(NodeIdDocument *cap, int idDocument) {
    // Comprova si l'ID ja existeix a la llista per evitar duplicats.
    NodeIdDocument *actual = cap; // Punter per recórrer la llista.
    while (actual != NULL) { // Recorre la llista.
        if (actual->idDocument == idDocument) { // Si l'ID ja es troba a la llista.
            return cap; // L'ID ja està a la llista, no fem res i retornem el cap original.
        }
        actual = actual->seguent; // Avança al següent node.
    }

    // Si no existeix, crea un nou node i l'afegeix a l'inici de la llista.
    NodeIdDocument *nouNode = (NodeIdDocument *)malloc(sizeof(NodeIdDocument)); // Assigna memòria per al nou node.
    if (nouNode == NULL) { // Comprova si l'assignació de memòria ha fallat.
        perror("Error en assignar memòria per NodeIdDocument"); // Imprimeix un missatge d'error.
        exit(EXIT_FAILURE); // Surt del programa amb un codi d'error.
    }
    nouNode->idDocument = idDocument;      // Assigna l'ID del document al nou node.
    nouNode->seguent = cap;                // El punter 'seguent' del nou node apunta a l'antic cap de la llista (afegint al principi).
    return nouNode;                        // Retorna el nou node, que ara és el cap de la llista.
}


// --- Funcions del Hashmap ---

// Inicialitza un nou hashmap amb una mida donada
// Aquesta funció assigna memòria i inicialitza una nova estructura HashMap.
HashMap *creaHashMap(size_t mida) {
    HashMap *mapa = (HashMap *)malloc(sizeof(HashMap)); // Reserva memòria per a l'estructura principal del hashmap.
    if (mapa == NULL) { // Comprova si l'assignació de memòria ha fallat.
        perror("Error en assignar memòria per HashMap"); // Imprimeix un missatge d'error.
        exit(EXIT_FAILURE); // Surt del programa.
    }
    mapa->mida = mida;                     // Assigna la mida de la taula hash al camp 'mida' del hashmap.
    mapa->num_elements = 0;                // Inicialitza el comptador d'elements (paraules úniques) a 0.
    mapa->entrades = (EntradaHash **)calloc(mida, sizeof(EntradaHash *)); // Reserva memòria per a l'array de punters a EntradaHash (les "cubetes" del hashmap) i l'inicialitza a NULL.
    if (mapa->entrades == NULL) { // Comprova si l'assignació de memòria per a les entrades ha fallat.
        perror("Error en assignar memòria per les entrades del HashMap"); // Imprimeix un missatge d'error.
        free(mapa); // Allibera la memòria de l'estructura del hashmap ja assignada.
        exit(EXIT_FAILURE); // Surt del programa.
    }
    return mapa;                           // Retorna el punter al hashmap creat.
}

// Allibera tota la memòria associada al hashmap
// Aquesta funció allibera de forma segura tota la memòria dinàmica assignada al hashmap,
// incloent les paraules i les llistes d'IDs de documents.
void alliberaHashMap(HashMap *mapa) {
    if (mapa == NULL) return; // Si el punter al mapa és NULL, no fa res.

    for (size_t i = 0; i < mapa->mida; i++) { // Itera sobre cada cubeta (posició) de la taula hash.
        EntradaHash *actual = mapa->entrades[i]; // Comença pel primer node de la llista encadenada en aquesta cubeta.
        while (actual != NULL) {               // Mentre hi hagi entrades en aquesta llista.
            EntradaHash *temp = actual;        // Guarda l'entrada actual per alliberar-la.
            actual = actual->seguent;          // Avança a la següent entrada de la llista encadenada.
            free(temp->paraula);               // Allibera la memòria de la cadena de la paraula.
            alliberaLlistaIdsDocuments(temp->idsDocuments); // Crida a la funció per alliberar la llista d'IDs de documents associada a aquesta paraula.
            free(temp);                        // Allibera la memòria de l'estructura EntradaHash.
        }
    }
    free(mapa->entrades);                      // Allibera l'array de punters (les cubetes) del hashmap.
    free(mapa);                                // Allibera l'estructura principal del hashmap.
}

// Funció hash simple (FNV-1a adaptada per a cadenes)
// Crèdits: Basada en la funció FNV-1a (Fowler-Noll-Vo hash)
// Aquesta funció pren una paraula i la mida del hashmap, i calcula un índex hash per a aquesta paraula.
unsigned int hash(const char *paraula, size_t mida_mapa) {
    unsigned int valor_hash = 2166136261U;     // FNV_PRIME_32_INIT: Valor inicial per al hash FNV-1a de 32 bits.
    for (int i = 0; paraula[i] != '\0'; i++) { // Itera sobre cada caràcter de la paraula fins al terminador nul.
        valor_hash ^= (unsigned char)paraula[i]; // XOR amb el caràcter actual (per barrejar bits).
        valor_hash *= 16777619U;                 // Multiplica pel primer FNV (per a més barreja).
    }
    return valor_hash % mida_mapa;               // Retorna l'índex final, assegurant que estigui dins dels límits de la mida del mapa.
}

// Insereix un ID de document a la llista d'IDs d'una paraula al hashmap.
// Si la paraula no existeix, crea una nova entrada.
// Aquesta funció afegeix una paraula i el seu ID de document associat al hashmap.
void insereixParaulaEnHashMap(HashMap *mapa, const char *paraula, int idDocument) {
    unsigned int index = hash(paraula, mapa->mida); // Calcula l'índex hash per a la paraula donada.

    // Cerca si la paraula ja existeix en aquesta cubeta (llista encadenada).
    EntradaHash *actual = mapa->entrades[index]; // Punter per recórrer la llista a la cubeta.
    while (actual != NULL) { // Recorre la llista.
        if (strcmp(actual->paraula, paraula) == 0) { // Si la paraula actual coincideix amb la paraula que volem inserir.
            // Paraula trobada, afegeix el idDocument a la seva llista (evitant duplicats).
            actual->idsDocuments = afegeixIdDocument(actual->idsDocuments, idDocument); // Afegeix l'ID a la llista existent.
            return; // Surt de la funció, ja que la paraula ja estava i l'ID s'ha afegit.
        }
        actual = actual->seguent; // Avança a la següent entrada en cas de col·lisió.
    }

    // Si la paraula no existeix a la cubeta, crea una nova entrada.
    EntradaHash *novaEntrada = (EntradaHash *)malloc(sizeof(EntradaHash)); // Assigna memòria per a la nova entrada.
    if (novaEntrada == NULL) { // Comprova si l'assignació de memòria ha fallat.
        perror("Error en assignar memòria per EntradaHash"); // Imprimeix un error.
        exit(EXIT_FAILURE); // Surt del programa.
    }
    novaEntrada->paraula = strdup(paraula); // Copia la paraula (s'alliberarà més tard).
    if (novaEntrada->paraula == NULL) { // Comprova si la duplicació de la cadena ha fallat.
        perror("Error en assignar memòria per la paraula de EntradaHash"); // Imprimeix un error.
        free(novaEntrada); // Allibera la memòria de l'entrada creada.
        exit(EXIT_FAILURE); // Surt del programa.
    }
    novaEntrada->idsDocuments = NULL; // Inicialitza la llista d'IDs de documents com a buida.
    novaEntrada->idsDocuments = afegeixIdDocument(novaEntrada->idsDocuments, idDocument); // Afegeix el primer ID a aquesta nova llista.

    // Afegeix la nova entrada a l'inici de la llista encadenada de la cubeta (com a un pre-append).
    novaEntrada->seguent = mapa->entrades[index]; // El punter 'seguent' de la nova entrada apunta al que era el primer en aquesta cubeta.
    mapa->entrades[index] = novaEntrada; // La nova entrada es converteix en el primer element d'aquesta cubeta.
    mapa->num_elements++; // Incrementa el nombre total d'elements (paraules úniques) al hashmap.
}

// Cerca una paraula al hashmap i retorna la llista d'IDs de documents associada
// Aquesta funció s'utilitza per obtenir la llista de documents on apareix una paraula.
NodeIdDocument *trobaParaulaEnHashMap(HashMap *mapa, const char *paraula) {
    unsigned int index = hash(paraula, mapa->mida); // Calcula l'índex hash per a la paraula buscada.
    EntradaHash *actual = mapa->entrades[index]; // Punter per recórrer la llista en la cubeta corresponent.
    while (actual != NULL) { // Recorre la llista.
        if (strcmp(actual->paraula, paraula) == 0) { // Si la paraula de l'entrada actual coincideix amb la buscada.
            return actual->idsDocuments;              // Paraula trobada, retorna la seva llista d'IDs de documents.
        }
        actual = actual->seguent; // Avança a la següent entrada.
    }
    return NULL;                                      // Paraula no trobada al hashmap, retorna NULL.
}

// Normalitza una paraula: converteix a minúscules i elimina caràcters no alfanumèrics.
// Retorna una nova cadena que s'ha de alliberar amb free().
// Aquesta funció neteja una paraula per a la seva inserció al hashmap (tokenització).
char *normalitzaParaula(const char *paraula) {
    if (paraula == NULL) return NULL; // Si la paraula d'entrada és NULL, retorna NULL.

    // Aproximació: una paraula normalitzada no serà més llarga que l'original.
    // +1 per al terminador null.
    char *normalitzada = (char *)malloc(strlen(paraula) + 1); // Assigna memòria per a la paraula normalitzada.
    if (normalitzada == NULL) { // Comprova si l'assignació de memòria ha fallat.
        perror("Error en assignar memòria per paraula normalitzada"); // Imprimeix un error.
        exit(EXIT_FAILURE); // Surt del programa.
    }

    int j = 0; // Índex per a la cadena 'normalitzada'.
    for (int i = 0; paraula[i] != '\0'; i++) { // Itera sobre cada caràcter de la paraula original.
        // Només copia caràcters alfanumèrics i els converteix a minúscules.
        if (isalnum((unsigned char)paraula[i])) { // Comprova si el caràcter és alfanumèric (lletra o número).
            normalitzada[j++] = tolower((unsigned char)paraula[i]); // Converteix a minúscula i copia al buffer 'normalitzada'.
        }
    }
    normalitzada[j] = '\0'; // Assegura que la cadena normalitzada estigui terminada amb un caràcter nul.

    // Si la paraula normalitzada és una cadena buida (ex. la paraula original només era puntuació),
    // podem retornar NULL o una cadena buida. Ara retornem NULL.
    if (j == 0) { // Si després de normalitzar la paraula resultant és buida.
        free(normalitzada); // Allibera la memòria assignada.
        return NULL; // Retorna NULL.
    }

    return normalitzada; // Retorna la nova cadena normalitzada.
}

// Construeix l'índex invertit a partir de la llista de documents
// Aquesta funció processa tots els documents, extreu les paraules del títol i el cos,
// les normalitza i les insereix al hashmap, creant així l'índex invertit.
HashMap *construeixIndexInvertit(Document *documents) {
    // Escollim una mida inicial per al hashmap.
    // Podria ser dinàmic o basat en el nombre esperat de paraules úniques.
    HashMap *indexInvertit = creaHashMap(MIDA_INICIAL_HASHMAP); // Crea un nou hashmap amb la mida inicial predefinida.

    Document *documentActual = documents; // Punter per recórrer la llista de documents.
    while (documentActual != NULL) { // Itera sobre cada document de la llista.
        // Tokenitza el títol del document.
        char *copiaTitol = strdup(documentActual->titol); // Fa una còpia del títol (strdup assigna memòria, que s'haurà d'alliberar).
        if (copiaTitol == NULL) { perror("strdup titol"); exit(EXIT_FAILURE); } // Comprova si strdup ha fallat.
        char *token = strtok(copiaTitol, " \t\n.,;!?-:()\"'"); // Utilitza strtok per dividir el títol en tokens (paraules) basant-se en delimitadors.
        while (token != NULL) { // Mentre hi hagi tokens disponibles.
            char *normalitzada = normalitzaParaula(token); // Normalitza el token (minúscules, sense puntuació).
            if (normalitzada != NULL && strlen(normalitzada) > 0) { // Assegura que no sigui una paraula buida després de normalitzar.
                insereixParaulaEnHashMap(indexInvertit, normalitzada, documentActual->id); // Insereix la paraula normalitzada i l'ID del document al hashmap.
                free(normalitzada); // Allibera la memòria de la paraula normalitzada (ja que strdup la va assignar).
            }
            token = strtok(NULL, " \t\n.,;!?-:()\"'"); // Obté el següent token.
        }
        free(copiaTitol); // Allibera la memòria de la còpia del títol.

        // Tokenitza el cos del document (procés similar al del títol).
        char *copiaCos = strdup(documentActual->cos); // Fa una còpia del cos del document.
        if (copiaCos == NULL) { perror("strdup cos"); exit(EXIT_FAILURE); } // Comprova si strdup ha fallat.
        token = strtok(copiaCos, " \t\n.,;!?-:()\"'"); // Divideix el cos en tokens.
        while (token != NULL) { // Mentre hi hagi tokens.
            char *normalitzada = normalitzaParaula(token); // Normalitza el token.
            if (normalitzada != NULL && strlen(normalitzada) > 0) { // Comprova que sigui una paraula vàlida.
                insereixParaulaEnHashMap(indexInvertit, normalitzada, documentActual->id); // Insereix al hashmap.
                free(normalitzada); // Allibera la paraula normalitzada.
            }
            token = strtok(NULL, " \t\n.,;!?-:()\"'"); // Obté el següent token.
        }
        free(copiaCos); // Allibera la memòria de la còpia del cos.

        documentActual = documentActual->seguent; // Passa al següent document de la llista.
    }
    printf("Índex invertit construït amb %zu paraules úniques.\n", indexInvertit->num_elements); // Imprimeix el nombre de paraules úniques indexades.
    return indexInvertit; // Retorna el punter a l'índex invertit construït.
}