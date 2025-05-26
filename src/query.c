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
        // Asegúrate de que el substring coincida y que no sea parte de una palabra más larga.
        // Ej: buscar "apple" no debe coincidir con "pineapple".
        // Para una búsqueda simple "contains", strncasecmp es suficiente.
        // Para "palabra completa", necesitarías chequear boundaries.
        if (strncasecmp(haystack, needle, needle_len) == 0) {
            // Opcional: verificar que sea una palabra completa. Esto haría la búsqueda más precisa.
            // if ((haystack == needle || !isalnum(*(haystack - 1))) && (!isalnum(*(haystack + needle_len)))) {
            //     return 1;
            // }
            return 1; // Para la lógica actual de "contiene"
        }
    }
    return 0;
}

// ESTA FUNCIÓN SE DEBE REEMPLAZAR O DEJAR DE USAR EN FAVOR DE LA NUEVA BÚSQUEDA
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
            // Normalizar la palabra clave de la consulta para buscarla
            char *normalized_q_keyword = normalizeWord(q->keyword);
            if (normalized_q_keyword == NULL || strlen(normalized_q_keyword) == 0) {
                allKeywordsFound = 0; // Si la normalización resulta en una palabra vacía, no se puede buscar.
                if (normalized_q_keyword) free(normalized_q_keyword);
                break;
            }

            // Aquí seguimos usando strcasestr_contains, que es menos eficiente que el hashmap.
            // Esta función DEBERÍA SER REEMPLAZADA por la lógica del hashmap.
            if (!strcasestr_contains(currentDoc->title, normalized_q_keyword) &&
                !strcasestr_contains(currentDoc->body, normalized_q_keyword)) {
                allKeywordsFound = 0;
                free(normalized_q_keyword);
                break;
            }
            free(normalized_q_keyword);
        }
        if (allKeywordsFound) {
            Document *copy = (Document *)malloc(sizeof(Document));
            if (!copy) {
                // Si la asignación falla, liberar los resultados ya obtenidos
                Document *temp_results = results;
                while(temp_results != NULL) {
                    Document *next_temp = temp_results->next;
                    freeDocument(temp_results); // freeDocument libera título, cuerpo, links, y la estructura
                    temp_results = next_temp;
                }
                return NULL;
            }
            copy->id = currentDoc->id;
            copy->title = strdup(currentDoc->title);
            copy->body = strdup(currentDoc->body);
            copy->links = NULL; // No copiamos enlaces para simplificar la copia superficial de resultados.
                                // Si se necesitan, se debe implementar una copia profunda aquí.
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


// NUEVA FUNCIÓN: busca documentos utilizando el índice invertido
Document *searchDocumentsWithReverseIndex(HashMap *reverseIndex, Document *allDocuments, Query *query, int maxResults) {
    if (!reverseIndex || !allDocuments || !query) return NULL;

    // Paso 1: Obtener las listas de DocumentIdNode para cada palabra clave de la consulta.
    // Necesitamos la intersección de estas listas.
    DocumentIdNode *first_keyword_doc_ids = NULL;
    Query *q_current = query;
    while (q_current != NULL) {
        char *normalized_keyword = normalizeWord(q_current->keyword);
        if (normalized_keyword == NULL || strlen(normalized_keyword) == 0) {
            // Si una palabra clave se normaliza a vacío, no puede haber resultados.
            if (first_keyword_doc_ids) freeDocumentIdList(first_keyword_doc_ids); // Liberar si ya había algo
            return NULL;
        }

        DocumentIdNode *current_keyword_doc_ids = findWordInHashMap(reverseIndex, normalized_keyword);
        free(normalized_keyword); // Liberar la palabra normalizada

        if (current_keyword_doc_ids == NULL) {
            // Si una palabra clave no está en el índice, la intersección es vacía.
            if (first_keyword_doc_ids) freeDocumentIdList(first_keyword_doc_ids);
            return NULL;
        }

        if (first_keyword_doc_ids == NULL) {
            // Si es la primera palabra clave, copiar su lista de IDs
            DocumentIdNode *src = current_keyword_doc_ids;
            DocumentIdNode *dest_head = NULL;
            DocumentIdNode *dest_tail = NULL;
            while(src != NULL) {
                DocumentIdNode *newNode = (DocumentIdNode *)malloc(sizeof(DocumentIdNode));
                if (!newNode) {
                    perror("malloc DocumentIdNode");
                    freeDocumentIdList(dest_head);
                    return NULL;
                }
                newNode->documentId = src->documentId;
                newNode->next = NULL;
                if (dest_head == NULL) {
                    dest_head = newNode;
                    dest_tail = newNode;
                } else {
                    dest_tail->next = newNode;
                    dest_tail = newNode;
                }
                src = src->next;
            }
            first_keyword_doc_ids = dest_head;
        } else {
            // Si no es la primera palabra clave, realizar la intersección con la lista actual
            DocumentIdNode *intersection_head = NULL;
            DocumentIdNode *intersection_tail = NULL;

            DocumentIdNode *ptr1 = first_keyword_doc_ids;
            while (ptr1 != NULL) {
                DocumentIdNode *ptr2 = current_keyword_doc_ids;
                while (ptr2 != NULL) {
                    if (ptr1->documentId == ptr2->documentId) {
                        // Encontrado un ID común, añadirlo a la intersección
                        DocumentIdNode *newNode = (DocumentIdNode *)malloc(sizeof(DocumentIdNode));
                        if (!newNode) {
                            perror("malloc intersection node");
                            freeDocumentIdList(intersection_head);
                            freeDocumentIdList(first_keyword_doc_ids);
                            return NULL;
                        }
                        newNode->documentId = ptr1->documentId;
                        newNode->next = NULL;
                        if (intersection_head == NULL) {
                            intersection_head = newNode;
                            intersection_tail = newNode;
                        } else {
                            intersection_tail->next = newNode;
                            intersection_tail = newNode;
                        }
                        break; // Ya encontramos este ID, pasar al siguiente de ptr1
                    }
                    ptr2 = ptr2->next;
                }
                ptr1 = ptr1->next;
            }
            freeDocumentIdList(first_keyword_doc_ids); // Liberar la lista anterior
            first_keyword_doc_ids = intersection_head; // Actualizar con la nueva intersección
            if (first_keyword_doc_ids == NULL) { // Si la intersección es vacía, no hay más resultados posibles
                return NULL;
            }
        }
        q_current = q_current->next;
    }

    // Paso 2: Recuperar los documentos completos basándose en los IDs resultantes (intersección)
    Document *results = NULL;
    Document *last_result_node = NULL;
    int count = 0;

    // Para encontrar los documentos por ID de forma eficiente,
    // podríamos construir un pequeño mapa de ID -> Document* si allDocuments es muy grande,
    // pero para este Lab, iterar allDocuments para cada ID es aceptable si el número de resultados es pequeño.
    // Una forma más eficiente sería tener un array o hashmap de todos los documentos indexados por ID.

    DocumentIdNode *id_node = first_keyword_doc_ids;
    while (id_node != NULL && (maxResults <= 0 || count < maxResults)) {
        Document *doc_ptr = allDocuments;
        while (doc_ptr != NULL) {
            if (doc_ptr->id == id_node->documentId) {
                // Documento encontrado, crear una copia para los resultados
                Document *copy = (Document *)malloc(sizeof(Document));
                if (!copy) {
                    perror("malloc result document copy");
                    freeDocumentIdList(first_keyword_doc_ids);
                    // Liberar los resultados ya creados antes de retornar NULL
                    Document *temp_res = results;
                    while(temp_res != NULL) {
                        Document *next_temp = temp_res->next;
                        freeDocument(temp_res);
                        temp_res = next_temp;
                    }
                    return NULL;
                }
                copy->id = doc_ptr->id;
                copy->title = strdup(doc_ptr->title);
                copy->body = strdup(doc_ptr->body);
                copy->links = NULL; // No se copian los enlaces para simplificar el resultado.
                copy->next = NULL;

                if (results == NULL) {
                    results = copy;
                    last_result_node = copy;
                } else {
                    last_result_node->next = copy;
                    last_result_node = copy;
                }
                count++;
                break; // Encontrado el documento, pasar al siguiente ID de la intersección
            }
            doc_ptr = doc_ptr->next;
        }
        id_node = id_node->next;
    }

    freeDocumentIdList(first_keyword_doc_ids); // Liberar la lista de IDs temporales de la intersección

    return results;
}


// muestra la llista de documents (para resultados de búsqueda)
void printDocuments(Document *documents) {
    if (!documents) {
        printf("No hi ha documents per mostrar.\n");
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
    // Recorrer en el orden correcto para mostrar las más recientes primero
    // O desde el último_indice - N hasta el último_indice
    for (int i = 0; i < MAX_LAST_QUERIES; i++) {
        // Calcular el índice para mostrar en orden cronológico (más reciente primero)
        int actual_idx = (lastQueryIndex - 1 - i + MAX_LAST_QUERIES) % MAX_LAST_QUERIES;
        if (lastQueries[actual_idx] != NULL) {
            printf(" * ");
            Query *q = lastQueries[actual_idx];
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