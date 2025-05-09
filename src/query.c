#include "query.h"
#include "document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//funció que elimina espais al principi i al final d'una cadena
 void retallarEspaisBlanc(char *str) {
    char *end;
    while(isspace((unsigned char)*str)) str++; //eliminar espais al principi amb la funció isspace (busca espais en blanc)
    if(*str == 0) //si la cadena és buida després d'eliminar espais, sortir
        return;
    end = str + strlen(str) - 1; //punter a l'últim caràcter
    while(end > str && isspace((unsigned char)*end)) end--; //eliminar espais al final
    *(end+1) = 0; //posar terminador nul després del darrer caràcter vàlid
}

//funció que crea una llista enllaçada de paraules clau a partir d'una cadena de text
Query *initQueryFromString(const char *queryString) {
    if(queryString == NULL) return NULL; //si la cadena és NULL, retornar NULL
    char *copia = strdup(queryString); //fer una còpia de la cadena per modificar-la
    if(copia == NULL) return NULL; //si no es pot copiar, retornar NULL
    Query *head = NULL; //punter al primer element de la llista
    Query *tail = NULL; //punter a l'últim element de la llista
    char *paraula_a_buscar = strtok(copia, " "); //obtenir la primera paraula separada per espais
    while(paraula_a_buscar != NULL) { //mentre hi hagi paraules
        retallarEspaisBlanc(paraula_a_buscar); //eliminar espais al principi i final de la paraula
        if(strlen(paraula_a_buscar) > 0) { //si la paraula no és buida
            Query *newNode = (Query *)malloc(sizeof(Query)); //crear un nou node
            newNode->keyword = strdup(paraula_a_buscar); //copiar la paraula al node
            newNode->next = NULL; //posar següent a NULL
            if(head == NULL) { //si la llista està buida
                head = newNode; //assignar el nou node com a cap
                tail = newNode; //assignar el nou node com a cua
            } else {
                tail->next = newNode; //afegir el nou node al final
                tail = newNode; //actualitzar la cua
            }
        }
        paraula_a_buscar = strtok(NULL, " "); //obtenir la següent par  aula
    }
    free(copia); //alliberar la còpia de la cadena
    return head; //retornar la llista creada
}

//funció que allibera la memòria de la llista de paraules clau
void freeQuery(Query *query) {
    while(query != NULL) { //mentre hi hagi nodes
        Query *temp = query; //guardar el node actual
        query = query->next; //avançar al següent
        free(temp->keyword); //alliberar la paraula
        free(temp); //alliberar el node
    }
}

//funció que comprova si un document conté una paraula clau (sense importar majúscules/minúscules)
static int documentContainsKeyword(Document *doc, const char *keyword) {
    if(doc == NULL || keyword == NULL) return 0; //si algun és NULL, retornar 0
    char *titleLower = strdup(doc->title); //copiar títol
    char *bodyLower = strdup(doc->body); //copiar cos
    if(!titleLower || !bodyLower) { //si no es pot copiar, alliberar i retornar 0
        free(titleLower); 
        free(bodyLower);
        return 0;
    }
    for(char *p = titleLower; *p; ++p) *p = tolower(*p); //convertir títol a minúscules
    for(char *p = bodyLower; *p; ++p) *p = tolower(*p); //convertir cos a minúscules
    char keywordLower[256];
    strncpy(keywordLower, keyword, 255); //copiar paraula clau
    keywordLower[255] = '\0'; //assegurar terminador nul
    for(char *p = keywordLower; *p; ++p) *p = tolower(*p); //convertir paraula clau a minúscules
    int found = (strstr(titleLower, keywordLower) != NULL) || (strstr(bodyLower, keywordLower) != NULL); //buscar paraula al títol o cos
    free(titleLower); //alliberar títol
    free(bodyLower); //alliberar cos
    return found; //retornar si s'ha trobat
}

//funció que comprova si un document conté totes les paraules clau de la consulta
static int documentMatchesQuery(Document *doc, Query *query) {
    Query *current = query; //punter a la llista de paraules clau
    while(current != NULL) { //mentre hi hagi paraules
        if(!documentContainsKeyword(doc, current->keyword)) { //si no conté alguna paraula
            return 0; //retornar 0
        }
        current = current->next; //avançar a la següent paraula
    }
    return 1; //si conté totes, retornar 1
}

//funció que fa una cerca lineal a la llista de documents per trobar els que contenen totes les paraules clau
Document *linearSearchDocuments(Document *documents, Query *query, int maxResults) {
    if(documents == NULL || query == NULL || maxResults <= 0) return NULL; //comprovar valors
    Document *resultHead = NULL; //cap de la llista de resultats
    Document *resultTail = NULL; //cua de la llista de resultats
    int count = 0; //comptador de resultats trobats
    Document *current = documents; //punter a la llista de documents
    while(current != NULL && count < maxResults) { //mentre hi hagi documents i no s'hagi arribat al màxim
        if(documentMatchesQuery(current, query)) { //si el document compleix la consulta
            Document *newDoc = current; //agafar el document
            newDoc->next = NULL; //posar següent a NULL
            if(resultHead == NULL) { //si la llista de resultats està buida
                resultHead = newDoc; //assignar cap i cua
                resultTail = newDoc;
            } else {
                resultTail->next = newDoc; //afegir al final
                resultTail = newDoc; //actualitzar cua
            }
            count++; //incrementar comptador
        }
        current = current->next; //avançar al següent document
    }
    return resultHead; //retornar la llista de resultats
}

//funció que imprimeix la llista de documents a la consola
void printDocuments(Document *documents) {
    Document *current = documents; //punter a la llista
    while(current != NULL) { //mentre hi hagi documents
        printf("ID: %d\n", current->id); //imprimir id
        printf("Title: %s\n", current->title); //imprimir títol
        printf("Body: %s\n", current->body); //imprimir cos
        Links *linkCurrent = current->links; //punter a la llista d'enllaços
        while(linkCurrent != NULL) { //mentre hi hagi enllaços
            printf("Link ID: %d, Text: %s\n", linkCurrent->documentId, linkCurrent->linkText); //imprimir enllaç
            linkCurrent = linkCurrent->next; //avançar al següent enllaç
        }
        printf("\n"); //línia en blanc entre documents
        current = current->next; //avançar al següent document
    }
}

/*
Explicació funcions implementades de les llibreries:

strtok --> divideix una cadena en tokens (subcadenes), utilitzant un delimitador.
Sintaxi: char *strtok(char *str, const char *delim);

strlen --> retorna la longitud d'una cadena de caràcters (sense comptar el caràcter nul \0).
Sintaxi: size_t strlen(const char *str);

strcpy --> copia una cadena de caràcters a una altra.
Sintaxi: char *strcpy(char *dest, const char *src);

strncpy --> copia una quantitat especificada de caràcters d'una cadena a una altra.
Sintaxi: char *strncpy(char *dest, const char *src, size_t n);

strstr --> cerca una subcadena dins d'una cadena.
Sintaxi: char *strstr(const char *haystack, const char *needle);

malloc -->assigna memòria dinàmica.
Sintaxi: void *malloc(size_t size);
*/