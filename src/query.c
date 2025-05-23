#include "query.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Helper function to create a new Query node
static Query *createQueryNode(const char *keyword) {
    Query *node = (Query *)malloc(sizeof(Query));
    if (!node) return NULL;
    node->keyword = strdup(keyword);
    node->next = NULL;
    return node;
}

// inicialitza una consulta a partir d'una cadena d'entrada
Query *initQueryFromString(const char *input) {
    if (input == NULL || strlen(input) == 0) {
        return NULL;
    }

    Query *head = NULL;
    Query *current = NULL;

    // Duplicate input to tokenize
    char *inputCopy = strdup(input);
    if (!inputCopy) return NULL;

    char *token = strtok(inputCopy, " ");
    while (token != NULL) {
        Query *newNode = createQueryNode(token);
        if (!newNode) {
            freeQuery(head);
            free(inputCopy);
            return NULL;
        }
        if (head == NULL) {
            head = newNode;
            current = newNode;
        } else {
            current->next = newNode;
            current = newNode;
        }
        token = strtok(NULL, " ");
    }
    free(inputCopy);
    return head;
}

// allibera la memòria d'una consulta
void freeQuery(Query *query) {
    while (query != NULL) {
        Query *next = query->next;
        free(query->keyword);
        free(query);
        query = next;
    }
}

// funció auxiliar per a comparar cadenes sense tenir en compte majúscules/minúscules
static int strcasestr_contains(const char *haystack, const char *needle) {
    if (!haystack || !needle) return 0;
    size_t needle_len = strlen(needle);
    for (; *haystack; haystack++) {
        if (strncasecmp(haystack, needle, needle_len) == 0) {
            return 1;
        }
    }
    return 0;
}

// cerca lineal en la llista de documents segons la consulta
Document *linearSearchDocuments(Document *documents, Query *query, int maxResults) {
    if (!documents || !query) return NULL;

    Document *results = NULL;
    Document *last = NULL;
    int count = 0;

    Document *currentDoc = documents;
    while (currentDoc != NULL && (maxResults <= 0 || count < maxResults)) {
        int allKeywordsFound = 1;
        for (Query *q = query; q != NULL; q = q->next) {
            if (!strcasestr_contains(currentDoc->title, q->keyword) &&
                !strcasestr_contains(currentDoc->body, q->keyword)) {
                allKeywordsFound = 0;
                break;
            }
        }
        if (allKeywordsFound) {
            Document *copy = (Document *)malloc(sizeof(Document));
            if (!copy) break;
            copy->id = currentDoc->id;
            copy->title = strdup(currentDoc->title);
            copy->body = strdup(currentDoc->body);
            copy->links = NULL; // no copiem enllaços per simplificar
            copy->next = NULL;

            if (results == NULL) {
                results = copy;
                last = copy;
            } else {
                last->next = copy;
                last = copy;
            }
            count++;
        }
        currentDoc = currentDoc->next;
    }
    return results;
}

// mostra la llista de documents
void printDocuments(Document *documents) {
    if (!documents) {
        printf("no hi ha documents per mostrar.\n");
        return;
    }
    int index = 1;
    Document *current = documents;
    while (current != NULL) {
        printf("[%d] ID: %d\n", index, current->id);
        printf("    Title: %s\n", current->title);
        printf("    Body: %.150s%s\n", current->body, strlen(current->body) > 150 ? "..." : "");
        current = current->next;
        index++;
    }
}

// funció placeholder per afegir consulta a la cua d'últimes consultes
#define MAX_LAST_QUERIES 3

static Query *lastQueries[MAX_LAST_QUERIES] = {NULL, NULL, NULL};
static int lastQueryIndex = 0;

void addLastQuery(Query *query) {
    // Alliberar la consulta antiga si existeix en aquesta posició
    if (lastQueries[lastQueryIndex] != NULL) {
        freeQuery(lastQueries[lastQueryIndex]);
    }
    // Copiar la consulta passada
    Query *copy = NULL;
    if (query != NULL) {
        // Crear una nova consulta copiant les paraules clau
        Query *currentSrc = query;
        Query *headCopy = NULL;
        Query *currentCopy = NULL;
        while (currentSrc != NULL) {
            Query *newNode = (Query *)malloc(sizeof(Query));
            if (!newNode) {
                freeQuery(headCopy);
                return;
            }
            newNode->keyword = strdup(currentSrc->keyword);
            newNode->next = NULL;
            if (headCopy == NULL) {
                headCopy = newNode;
                currentCopy = newNode;
            } else {
                currentCopy->next = newNode;
                currentCopy = newNode;
            }
            currentSrc = currentSrc->next;
        }
        copy = headCopy;
    }
    lastQueries[lastQueryIndex] = copy;
    lastQueryIndex = (lastQueryIndex + 1) % MAX_LAST_QUERIES;
}

void showLastQueries(void) {
    printf("******* últimes consultes ********\n");
    for (int i = 0; i < MAX_LAST_QUERIES; i++) {
        int idx = (lastQueryIndex + i) % MAX_LAST_QUERIES;
        if (lastQueries[idx] != NULL) {
            printf(" * ");
            Query *q = lastQueries[idx];
            while (q != NULL) {
                printf("%s", q->keyword);
                if (q->next != NULL) {
                    printf(" ");
                }
                q = q->next;
            }
            printf(" *\n");
        }
    }
    printf("*********************************\n");
}
