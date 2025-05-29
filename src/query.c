#include "query.h"    
#include <stdio.h>    
#include <stdlib.h>   
#include <string.h>   
#include <ctype.h>    
#include "document.h"
#include "time.h" // Per la funció de calcular el temps 

// Funció auxiliar per crear un nou node de l'estructura Consulta.
// Aquesta funció s'utilitza internament per construir la llista enllaçada de paraules clau.
static Consulta *creaNodeConsulta(const char *paraulaClau) {
    Consulta *node = (Consulta *)malloc(sizeof(Consulta)); // Assigna memòria per al nou node.
    if (!node) return NULL; // Si falla l'assignació, retorna NULL.
    node->paraulaClau = strdup(paraulaClau); // Copia la paraula clau (strdup també assigna memòria).
    node->seguent = NULL;                   // Inicialitza el punter al següent node a NULL.
    return node;                            // Retorna el punter al node creat.
}

// Inicialitza una consulta a partir d'una cadena d'entrada.
// Converteix una cadena de text en una llista enllaçada de paraules clau (tokens).
Consulta *iniciaConsultaDesDeString(const char *entrada) {
    if (entrada == NULL || strlen(entrada) == 0) { // Comprova si la cadena d'entrada és nul·la o buida.
        return NULL; // Si ho és, no hi ha consulta a crear, retorna NULL.
    }

    Consulta *cap = NULL;    // Punter al cap de la llista de la consulta.
    Consulta *actual = NULL; // Punter per recórrer la llista i afegir nous nodes.

    // Duplica la cadena d'entrada per tokenitzar-la, ja que strtok modifica la cadena original.
    char *copiaEntrada = strdup(entrada); // Fa una còpia de la cadena d'entrada.
    if (!copiaEntrada) return NULL; // Si falla la còpia, retorna NULL.

    char *token = strtok(copiaEntrada, " "); // Obté el primer token (paraula) utilitzant l'espai com a delimitador.
    while (token != NULL) { // Mentre hi hagi tokens (paraules).
        Consulta *nouNode = creaNodeConsulta(token); // Crea un nou node de consulta amb el token actual.
        if (!nouNode) { // Si falla la creació del node.
            alliberaConsulta(cap);    // Allibera la memòria de qualsevol node ja creat a la llista.
            free(copiaEntrada);       // Allibera la còpia de la cadena d'entrada.
            return NULL;              // Retorna NULL indicant un error.
        }
        if (cap == NULL) { // Si és el primer node de la llista.
            cap = nouNode;   // El nou node es converteix en el cap.
            actual = nouNode; // El punter 'actual' també apunta al nou node.
        } else { // Si no és el primer node.
            actual->seguent = nouNode; // Enllaça el node anterior amb el nou node.
            actual = nouNode;         // El punter 'actual' avança al nou node.
        }
        token = strtok(NULL, " "); // Obté el següent token (passant NULL per continuar amb la mateixa cadena).
    }
    free(copiaEntrada); // Allibera la memòria de la còpia de la cadena d'entrada.
    return cap;         // Retorna el cap de la llista de la consulta.
}

// Allibera la memòria d'una consulta.
// Recorre la llista enllaçada de Consulta i allibera la memòria de cada node i de la paraula clau.
void alliberaConsulta(Consulta *consulta) {
    while (consulta != NULL) { // Mentre hi hagi nodes a la llista de la consulta.
        Consulta *seguent = consulta->seguent; // Guarda el punter al següent node abans d'alliberar l'actual.
        free(consulta->paraulaClau);            // Allibera la memòria de la cadena de la paraula clau.
        free(consulta);                         // Allibera la memòria del node de consulta.
        consulta = seguent;                     // Avança al següent node.
    }
}

// Funció auxiliar per a comparar cadenes sense tenir en compte majúscules/minúscules (case-insensitive substring search).
// Retorna 1 si 'agulla' es troba a 'paller' (case-insensitive), 0 altrament.
static int strcasestr_contiene(const char *paller, const char *agulla) {
    if (!paller || !agulla) return 0; // Si alguna de les cadenes és NULL, retorna 0.
    size_t llargada_agulla = strlen(agulla); // Obté la longitud de la cadena 'agulla'.
    for (; *paller; paller++) { // Itera sobre la cadena 'paller' caràcter a caràcter.
        // Compara 'llargada_agulla' caràcters de 'paller' amb 'agulla' sense tenir en compte majúscules/minúscules.
        if (strncasecmp(paller, agulla, llargada_agulla) == 0) {
            return 1; // Si es troba una coincidència, retorna 1.
        }
    }
    return 0; // Si no es troba la subcadena, retorna 0.
}

// AQUESTA FUNCIÓ S'HA DE REEMPLAÇAR O DEIXAR DE USAR EN FAVOR DE LA NOVA BÚSQUEDA (amb índex invertit).
// Cerca lineal en la llista de documents segons la consulta.
// Aquesta funció és ineficient per a grans conjunts de dades.
Document *cercaDocumentsLineal(Document *documents, Consulta *consulta, int maxResultats) {
    if (!documents || !consulta) return NULL; // Si no hi ha documents o consulta, retorna NULL.

    Document *resultats = NULL; // Cap de la llista de documents resultants.
    Document *ultim = NULL;     // Punter a l'últim node de la llista de resultats.
    int comptador = 0;          // Comptador de resultats trobats.

    Document *documentActual = documents; // Punter per recórrer la llista de documents.
    // Itera sobre els documents mentre n'hi hagi i no s'hagi superat el límit de resultats.
    while (documentActual != NULL && (maxResultats <= 0 || comptador < maxResultats)) {
        int totesLesParaulesClauTrobades = 1; // Flag per saber si totes les paraules clau de la consulta es troben al document actual.
        for (Consulta *q = consulta; q != NULL; q = q->seguent) { // Itera sobre cada paraula clau de la consulta.
            char *paraula_clau_q_normalitzada = normalitzaParaula(q->paraulaClau); // Normalitza la paraula clau de la consulta.
            if (paraula_clau_q_normalitzada == NULL || strlen(paraula_clau_q_normalitzada) == 0) {
                totesLesParaulesClauTrobades = 0; // Si la normalització resulta en una paraula buida, considerem que no s'ha trobat.
                if (paraula_clau_q_normalitzada) free(paraula_clau_q_normalitzada); // Allibera la memòria si no és NULL.
                break; // Surt del bucle de paraules clau.
            }

            // Comprova si la paraula clau normalitzada es troba al títol o al cos del document actual (sense distinció de majúscules/minúscules).
            if (!strcasestr_contiene(documentActual->titol, paraula_clau_q_normalitzada) &&
                !strcasestr_contiene(documentActual->cos, paraula_clau_q_normalitzada)) {
                totesLesParaulesClauTrobades = 0; // Si no es troba, el document no coincideix amb totes les paraules clau.
                free(paraula_clau_q_normalitzada); // Allibera la memòria.
                break; // Surt del bucle de paraules clau.
            }
            free(paraula_clau_q_normalitzada); // Allibera la memòria de la paraula clau normalitzada.
        }
        if (totesLesParaulesClauTrobades) { // Si totes les paraules clau de la consulta es van trobar al document.
            // Crea una còpia del document trobat per afegir-lo a la llista de resultats.
            Document *copia = (Document *)malloc(sizeof(Document));
            if (!copia) { // Si falla l'assignació de memòria per a la còpia.
                // Allibera els resultats ja acumulats abans de sortir.
                Document *temp_resultats = resultats;
                while(temp_resultats != NULL) {
                    Document *seguent_temp = temp_resultats->seguent;
                    alliberaDocument(temp_resultats);
                    temp_resultats = seguent_temp;
                }
                return NULL; // Retorna NULL indicant un error.
            }
            // Copia les dades del document original al nou document (profundament per títol i cos).
            copia->id = documentActual->id;
            copia->titol = strdup(documentActual->titol);
            copia->cos = strdup(documentActual->cos);
            copia->enllacos = NULL; // Els enllaços no es copien en aquesta cerca simple.
            copia->seguent = NULL;

            if (resultats == NULL) { // Si és el primer resultat.
                resultats = copia;   // La còpia es converteix en el cap dels resultats.
                ultim = copia;       // I també l'últim node.
            } else { // Si ja hi ha resultats.
                ultim->seguent = copia; // Enllaça l'últim node amb la nova còpia.
                ultim = copia;          // La nova còpia es converteix en l'últim node.
            }
            comptador++; // Incrementa el comptador de resultats.
        }
        documentActual = documentActual->seguent; // Passa al següent document de la llista original.
    }
    return resultats; // Retorna la llista de documents que coincideixen amb la consulta.
}


// NOVA FUNCIÓ: busca documents utilitzant l'índex invertit.
// Aquesta funció és molt més eficient per a cerques en grans conjunts de dades.
Document *cercaDocumentsAmbIndexInvertit(HashMap *indexInvertit, Document *totsElsDocuments, Consulta *consulta, int maxResultats) {
    if (!indexInvertit || !totsElsDocuments || !consulta) return NULL; // Comprova si els punters d'entrada són vàlids.

    // Pas 1: Obtenir les llistes de NodeIdDocument per a cada paraula clau de la consulta.
    // Necessitem la intersecció d'aquestes llistes per trobar documents que continguin *totes* les paraules clau.
    NodeIdDocument *ids_docs_resultants = NULL; // Llista que contindrà els IDs dels documents que coincideixen amb la consulta.

    Consulta *consulta_actual = consulta; // Punter per recórrer la llista de paraules clau de la consulta.
    while (consulta_actual != NULL) { // Itera sobre cada paraula clau de la consulta.
        char *paraula_normalitzada = normalitzaParaula(consulta_actual->paraulaClau); // Normalitza la paraula clau.
        if (paraula_normalitzada == NULL || strlen(paraula_normalitzada) == 0) {
            // Si la paraula normalitzada és invàlida o buida, significa que no hi ha resultats per a aquesta paraula clau.
            if (ids_docs_resultants) alliberaLlistaIdsDocuments(ids_docs_resultants); // Allibera qualsevol llista d'IDs ja construïda.
            return NULL; // No hi ha documents que continguin aquesta "paraula", així que no hi ha cap document que contingui totes les paraules de la consulta.
        }

        // Cerca la llista d'IDs de documents associada a la paraula normalitzada a l'índex invertit.
        NodeIdDocument *ids_docs_paraula_clau_actual = trobaParaulaEnHashMap(indexInvertit, paraula_normalitzada);
        free(paraula_normalitzada); // Allibera la memòria de la paraula normalitzada.

        if (ids_docs_paraula_clau_actual == NULL) {
            // Si una de les paraules clau no es troba a l'índex, cap document pot contenir totes les paraules.
            if (ids_docs_resultants) alliberaLlistaIdsDocuments(ids_docs_resultants); // Allibera la llista de resultats acumulats.
            return NULL; // Retorna NULL.
        }

        if (ids_docs_resultants == NULL) {
            // Si és la primera paraula clau, la llista de resultats inicial és una còpia de la llista trobada per aquesta paraula.
            // S'ha de fer una còpia profunda per no dependre de les estructures internes del hashmap.
            NodeIdDocument *src = ids_docs_paraula_clau_actual;
            NodeIdDocument *cap_dest = NULL;
            NodeIdDocument *cua_dest = NULL;
            while(src != NULL) {
                NodeIdDocument *nouNode = (NodeIdDocument *)malloc(sizeof(NodeIdDocument));
                if (!nouNode) {
                    perror("malloc NodeIdDocument per a la còpia");
                    alliberaLlistaIdsDocuments(cap_dest);
                    return NULL;
                }
                nouNode->idDocument = src->idDocument;
                nouNode->seguent = NULL;
                if (cap_dest == NULL) {
                    cap_dest = nouNode;
                    cua_dest = nouNode;
                } else {
                    cua_dest->seguent = nouNode;
                    cua_dest = nouNode;
                }
                src = src->seguent;
            }
            ids_docs_resultants = cap_dest; // La llista de resultats ara és aquesta còpia.
        } else {
            // Si ja tenim una llista de resultats, cal fer la intersecció amb la llista actual de la paraula clau.
            NodeIdDocument *cap_interseccio = NULL; // Cap de la nova llista d'intersecció.
            NodeIdDocument *cua_interseccio = NULL; // Cua de la nova llista d'intersecció.

            NodeIdDocument *ptr1 = ids_docs_resultants; // Punter per recórrer la llista de resultats actual.
            while (ptr1 != NULL) {
                NodeIdDocument *ptr2 = ids_docs_paraula_clau_actual; // Punter per recórrer la llista de la paraula clau actual.
                while (ptr2 != NULL) {
                    if (ptr1->idDocument == ptr2->idDocument) { // Si trobem un ID de document comú.
                        NodeIdDocument *nouNode = (NodeIdDocument *)malloc(sizeof(NodeIdDocument));
                        if (!nouNode) {
                            perror("malloc node interseccio");
                            alliberaLlistaIdsDocuments(cap_interseccio);
                            alliberaLlistaIdsDocuments(ids_docs_resultants); // Allibera la llista de resultats anterior.
                            return NULL;
                        }
                        nouNode->idDocument = ptr1->idDocument;
                        nouNode->seguent = NULL;
                        if (cap_interseccio == NULL) {
                            cap_interseccio = nouNode;
                            cua_interseccio = nouNode;
                        } else {
                            cua_interseccio->seguent = nouNode;
                            cua_interseccio = nouNode;
                        }
                        break; // Trobada la coincidència, no cal seguir buscant en ptr2 per a aquest ptr1.
                    }
                    ptr2 = ptr2->seguent;
                }
                ptr1 = ptr1->seguent;
            }
            alliberaLlistaIdsDocuments(ids_docs_resultants); // Allibera la llista de resultats anterior, ja que hem creat la nova intersecció.
            ids_docs_resultants = cap_interseccio; // La llista de resultats ara és la intersecció.
            if (ids_docs_resultants == NULL) {
                // Si la intersecció és buida, significa que cap document conté *totes* les paraules clau.
                return NULL;
            }
        }
        consulta_actual = consulta_actual->seguent; // Passa a la següent paraula clau de la consulta.
    }

    // Pas 2: Recuperar els documents complets basant-se en els IDs resultants (intersecció).
    Document *resultats = NULL;          // Cap de la llista de documents resultants finals.
    Document *ultim_node_resultat = NULL; // Punter a l'últim node de la llista de resultats.
    int comptador = 0;                   // Comptador de resultats per al límit 'maxResultats'.

    NodeIdDocument *node_id = ids_docs_resultants; // Punter per recórrer la llista d'IDs de documents resultants.
    while (node_id != NULL && (maxResultats <= 0 || comptador < maxResultats)) {
        Document *ptr_doc = totsElsDocuments; // Punter per buscar el document complet a la llista de tots els documents.
        while (ptr_doc != NULL) {
            if (ptr_doc->id == node_id->idDocument) { // Si l'ID del document coincideix.
                // Crea una còpia profunda del document per afegir-lo als resultats.
                Document *copia = (Document *)malloc(sizeof(Document));
                if (!copia) { // Si falla l'assignació de memòria.
                    perror("malloc copia document resultat");
                    alliberaLlistaIdsDocuments(ids_docs_resultants); // Allibera els IDs.
                    // Allibera els documents resultat ja copiats.
                    Document *temp_res = resultats;
                    while(temp_res != NULL) {
                        Document *seguent_temp = temp_res->seguent;
                        alliberaDocument(temp_res);
                        temp_res = seguent_temp;
                    }
                    return NULL; // Retorna NULL.
                }
                copia->id = ptr_doc->id;
                copia->titol = strdup(ptr_doc->titol);
                copia->cos = strdup(ptr_doc->cos);
                copia->enllacos = NULL; // Els enllaços no es copien per simplificació.
                copia->seguent = NULL;
                copia->puntuacioRellevancia = ptr_doc->puntuacioRellevancia; // També copia la puntuació de rellevància.

                if (resultats == NULL) {
                    resultats = copia;
                    ultim_node_resultat = copia;
                } else {
                    ultim_node_resultat->seguent = copia;
                    ultim_node_resultat = copia;
                }
                comptador++;
                break; // Document trobat, passa al següent ID de la llista de resultats.
            }
            ptr_doc = ptr_doc->seguent;
        }
        node_id = node_id->seguent;
    }

    alliberaLlistaIdsDocuments(ids_docs_resultants); // Allibera la llista temporal d'IDs de documents.

    return resultats; // Retorna la llista de documents que coincideixen amb la consulta.
}


// Mostra la llista de documents (per a resultats de cerca).
void imprimeixDocuments(Document *documents) {
    if (!documents) { // Si la llista de documents és buida.
        printf("No hi ha documents per mostrar.\n");
        return;
    }
    int index = 1;      // Comptador per numerar els documents.
    Document *actual = documents; // Punter per recórrer la llista.
    while (actual != NULL) { // Mentre hi hagi documents.
        printf("[%d] ID: %d\n", index, actual->id);        // Imprimeix l'índex i l'ID del document.
        printf("    Títol: %s\n", actual->titol);         // Imprimeix el títol.
        // Imprimeix el cos, truncant-lo si és massa llarg i afegint "..."
        printf("    Cos: %.150s%s\n", actual->cos, strlen(actual->cos) > 150 ? "..." : "");
        actual = actual->seguent; // Passa al següent document.
        index++;                  // Incrementa l'índex.
    }
}

// Constant que defineix el nombre màxim d'últimes consultes a guardar.
#define MAX_ULTIMES_CONSULTES 3

// Array estatic per emmagatzemar les últimes consultes.
// S'inicialitzen a NULL.
static Consulta *ultimesConsultes[MAX_ULTIMES_CONSULTES] = {NULL, NULL, NULL};
// Índex per saber on afegir la pròxima consulta (implementa un buffer circular).
static int indexUltimaConsulta = 0;

// Funció per afegir una consulta a la cua d'últimes consultes.
// Gestiona un historial de consultes de forma circular.
void afegeixUltimaConsulta(Consulta *consulta) {
    // Alliberar la consulta antiga si existeix en aquesta posició abans de sobrescriure-la.
    if (ultimesConsultes[indexUltimaConsulta] != NULL) {
        alliberaConsulta(ultimesConsultes[indexUltimaConsulta]);
    }
    // Copiar la consulta passada per valor (profundament).
    Consulta *copia = NULL;
    if (consulta != NULL) {
        // Crear una nova consulta copiant les paraules clau.
        Consulta *srcActual = consulta; // Punter per recórrer la consulta original.
        Consulta *capCopia = NULL;     // Cap de la nova còpia.
        Consulta *copiaActual = NULL;  // Punter per recórrer i construir la còpia.
        while (srcActual != NULL) { // Itera sobre els nodes de la consulta original.
            Consulta *nouNode = (Consulta *)malloc(sizeof(Consulta)); // Crea un nou node per a la còpia.
            if (!nouNode) { // Si falla l'assignació de memòria.
                alliberaConsulta(capCopia); // Allibera la còpia ja construïda.
                return; // Surt de la funció.
            }
            nouNode->paraulaClau = strdup(srcActual->paraulaClau); // Copia la paraula clau.
            nouNode->seguent = NULL; // Inicialitza el punter 'seguent'.
            if (capCopia == NULL) { // Si és el primer node de la còpia.
                capCopia = nouNode;
                copiaActual = nouNode;
            } else { // Si no és el primer.
                copiaActual->seguent = nouNode; // Enllaça el node anterior de la còpia amb el nou.
                copiaActual = nouNode;         // Avança a la nova còpia.
            }
            srcActual = srcActual->seguent; // Avança al següent node de la consulta original.
        }
        copia = capCopia; // La còpia final és el cap de la nova llista.
    }
    ultimesConsultes[indexUltimaConsulta] = copia; // Guarda la còpia de la consulta a l'historial.
    indexUltimaConsulta = (indexUltimaConsulta + 1) % MAX_ULTIMES_CONSULTES; // Actualitza l'índex per al pròxim cicle (buffer circular).
}

// Mostra les últimes consultes realitzades.
void mostraUltimesConsultes(void) {
    printf("******* Últimes consultes ********\n");
    // Recorre en l'ordre correcte per mostrar les més recents primer.
    for (int i = 0; i < MAX_ULTIMES_CONSULTES; i++) {
        // Calcula l'índex per mostrar en ordre cronològic (la més recent primer).
        // (indexUltimaConsulta - 1) és la posició de l'última consulta afegida.
        // Resta 'i' per anar cap enrere en l'historial.
        // El "+ MAX_ULTIMES_CONSULTES) % MAX_ULTIMES_CONSULTES" gestiona el desbordament negatiu i el cicle.
        int idx_actual = (indexUltimaConsulta - 1 - i + MAX_ULTIMES_CONSULTES) % MAX_ULTIMES_CONSULTES;
        if (ultimesConsultes[idx_actual] != NULL) { // Si hi ha una consulta guardada en aquesta posició.
            printf(" * ");
            Consulta *q = ultimesConsultes[idx_actual]; // Punter a la consulta a imprimir.
            while (q != NULL) { // Itera sobre les paraules clau de la consulta.
                printf("%s", q->paraulaClau); // Imprimeix la paraula clau.
                if (q->seguent != NULL) { // Si hi ha més paraules clau.
                    printf(" "); // Imprimeix un espai entre elles.
                }
                q = q->seguent; // Passa a la següent paraula clau.
            }
            printf(" *\n"); // Tanca la línia de la consulta.
        }
    }
    printf("*********************************\n");
}

// PER EL REPORT: 

// Versió simplificada de cerca lineal (SENSE reverse-index)
// Funció que cerca documents que continguin totes les paraules clau de la consulta, sense utilitzar un índex invertit
Document *cercaDocumentsSenseIndex(Document *documents, Consulta *consulta, int maxResultats) {
    if (!documents || !consulta) return NULL; // Retorna NULL si no hi ha documents o consulta

    Document *resultats = NULL; // Llista de resultats inicialment buida
    Document *ultim = NULL;     // Punter a l'últim document afegit a la llista de resultats
    int comptador = 0;          // Comptador de resultats trobats

    Document *docActual = documents; // Punter al document actual de la llista
    while (docActual != NULL && (maxResultats <= 0 || comptador < maxResultats)) {
        int totesTrobades = 1; // Indicador de si totes les paraules de la consulta han estat trobades
        Consulta *q = consulta; // Punter a la paraula clau actual de la consulta
        while (q != NULL) {
            char *paraulaNormalitzada = normalitzaParaula(q->paraulaClau); // Normalitza la paraula clau
            if (!paraulaNormalitzada || 
                (!strcasestr_contiene(docActual->titol, paraulaNormalitzada) && 
                 !strcasestr_contiene(docActual->cos, paraulaNormalitzada))) {
                // Si la paraula no és trobada ni al títol ni al cos, marquem com a no trobada
                totesTrobades = 0;
                if (paraulaNormalitzada) free(paraulaNormalitzada); // Alliberem memòria
                break; // Sortim del bucle de consulta
            }
            free(paraulaNormalitzada); // Alliberem memòria
            q = q->seguent; // Passem a la següent paraula clau
        }
        if (totesTrobades) { // Si totes les paraules han estat trobades
            Document *copia = (Document *)malloc(sizeof(Document)); // Reservem memòria per una còpia del document
            copia->id = docActual->id; // Copiem l'ID
            copia->titol = strdup(docActual->titol); // Copiem el títol
            copia->cos = strdup(docActual->cos); // Copiem el cos
            copia->seguent = NULL; // Inicialitzem el següent com a NULL
            if (!resultats) resultats = copia; // Si és el primer resultat, inicialitzem la llista
            else ultim->seguent = copia; // Si no, l'afegim al final de la llista
            ultim = copia; // Actualitzem el punter a l'últim
            comptador++; // Incrementem el comptador
        }
        docActual = docActual->seguent; // Passem al següent document
    }
    return resultats; // Retornem la llista de resultats
}

// Funció que compara el temps d'execució entre la cerca amb índex invertit i la cerca sense índex
void comparaMetodesBusqueda(HashMap *indexInvertit, Document *documents, Consulta *consulta) {
    clock_t start, end; // Variables per guardar el temps d'inici i fi
    double tempsAmbIndex, tempsSenseIndex; // Temps en mil·lisegons per a cada mètode

    // --- Búsqueda AMB reverse-index ---
    start = clock(); // Inici del cronòmetre
    Document *resultatsAmbIndex = cercaDocumentsAmbIndexInvertit(indexInvertit, documents, consulta, 5); // Cerca amb índex invertit
    end = clock(); // Final del cronòmetre
    tempsAmbIndex = ((double)(end - start)) / CLOCKS_PER_SEC * 1000; // Converteix a mil·lisegons

    if (resultatsAmbIndex) {
        printf("[Amb reverse-index] Temps: %.2f ms\n", tempsAmbIndex); // Mostra el temps per la cerca amb índex

        // Allibera memòria dels resultats
        Document *temp = resultatsAmbIndex;
        while (temp) {
            Document *next = temp->seguent; // Guarda el següent
            free(temp->titol); // Allibera el títol
            free(temp->cos);   // Allibera el cos
            free(temp);        // Allibera el document
            temp = next;       // Passa al següent
        }
    }

    // --- Búsqueda SENSE reverse-index ---
    start = clock(); // Inici del cronòmetre
    Document *resultatsSenseIndex = cercaDocumentsSenseIndex(documents, consulta, 5); // Cerca sense índex
    end = clock(); // Final del cronòmetre
    tempsSenseIndex = ((double)(end - start)) / CLOCKS_PER_SEC * 1000; // Converteix a mil·lisegons

    if (resultatsSenseIndex) {
        printf("[Sense reverse-index] Temps: %.2f ms\n", tempsSenseIndex); // Mostra el temps per la cerca sense índex

        // Allibera memòria dels resultats
        Document *temp = resultatsSenseIndex;
        while (temp) {
            Document *next = temp->seguent; // Guarda el següent
            free(temp->titol); // Allibera el títol
            free(temp->cos);   // Allibera el cos
            free(temp);        // Allibera el document
            temp = next;       // Passa al següent
        }
    }

    if (resultatsAmbIndex && resultatsSenseIndex) {
        double diferencia_ms = tempsSenseIndex - tempsAmbIndex; // Calcula la diferència de temps
        printf("Diferència: %.2f ms\n", diferencia_ms); // Mostra la diferència de temps
    }
}