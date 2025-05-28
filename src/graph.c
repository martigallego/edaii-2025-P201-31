#include "graph.h"    
#include <stdio.h>   
#include <stdlib.h>  
#include <string.h>  
#include <limits.h> 

#define MIDA_INICIAL_GRAF 10007   // Defineix una constant per a la mida inicial de la taula hash del graf. S'utilitza un nombre primer per millorar la distribució i reduir col·lisions.

// Funció hash per obtenir un índex a partir de l’ID del document
// Aquesta funció pren l'ID d'un document i la mida de la taula hash del graf,
// i retorna un índex dins d'aquesta taula.
unsigned int hashGraf(int idDocument, size_t midaGraf) {
    return (unsigned int)idDocument % midaGraf; // Calcula l'índex usant l'operador mòdul.
}

// Crea un graf dirigit buit amb una mida determinada
// Aquesta funció assigna memòria i inicialitza una nova estructura GrafDirigit.
GrafDirigit *crearGrafDirigit(size_t mida) {
    GrafDirigit *graf = (GrafDirigit *)malloc(sizeof(GrafDirigit));   // Reserva memòria per a l'estructura principal del graf.
    if (!graf) {  // Comprova si l'assignació de memòria ha fallat.
        perror("Error creant el graf dirigit"); // Imprimeix un missatge d'error al stderr.
        exit(EXIT_FAILURE); // Surt del programa amb un codi d'error.
    }

    graf->mida = mida; // Assigna la mida de la taula hash al camp 'mida' del graf.
    graf->totalNodes = 0; // Inicialitza el comptador de nodes a 0.
    graf->totalEnllacos = 0; // Inicialitza el comptador total d'enllaços a 0.
    graf->nodes = (NodeGrau **)calloc(mida, sizeof(NodeGrau *));   // Reserva memòria per a l'array de punters a NodeGrau (la taula hash) i l'inicialitza a NULL.
    if (!graf->nodes) {  // Comprova si l'assignació de memòria per a l'array de nodes ha fallat.
        perror("Error assignant memòria per als nodes del graf"); // Imprimeix un missatge d'error.
        free(graf); // Allibera la memòria de l'estructura del graf ja assignada.
        exit(EXIT_FAILURE); // Surt del programa amb un codi d'error.
    }

    return graf;  // Retorna el punter al graf dirigit creat.
}

// Busca un node en el graf a partir del seu ID
// Aquesta funció cerca un NodeGrau específic dins del graf utilitzant el seu ID.
NodeGrau *cercarNodeEnGraf(GrafDirigit *graf, int idDocument) {
    unsigned int index = hashGraf(idDocument, graf->mida);   // Calcula l'índex de la taula hash on podria estar el node.
    NodeGrau *actual = graf->nodes[index];   // Accedeix al primer node de la llista encadenada en aquest índex.

    while (actual != NULL) {  // Recorre la llista encadenada (en cas de col·lisions de hash).
        if (actual->idDocument == idDocument) { // Si l'ID del node actual coincideix amb l'ID buscat.
            return actual;  // Retorna el punter al node trobat.
        }
        actual = actual->seguent; // Avança al següent node de la llista.
    }
    return NULL;  // Retorna NULL si el node no s'ha trobat al graf.
}

// Afegeix un nou node al graf si no existeix ja
// Aquesta funció crea i afegeix un node representant un document al graf si encara no hi és.
void afegirNodeAGraf(GrafDirigit *graf, int idDocument) {
    if (cercarNodeEnGraf(graf, idDocument) != NULL) { // Comprova si el node amb aquest ID ja existeix al graf.
        return;   // Si el node ja existeix, no fa res i surt de la funció.
    }

    NodeGrau *nouNode = (NodeGrau *)malloc(sizeof(NodeGrau));   // Reserva memòria per al nou node.
    if (!nouNode) { // Comprova si l'assignació de memòria ha fallat.
        perror("Error creant nou node del graf"); // Imprimeix un missatge d'error.
        exit(EXIT_FAILURE); // Surt del programa amb un codi d'error.
    }

    // Inicialitza el nou node amb valors per defecte.
    nouNode->idDocument = idDocument; // Assigna l'ID al nou node.
    nouNode->grauEntrant = 0; // Inicialitza el grau entrant a 0.
    nouNode->grauSortint = 0; // Inicialitza el grau sortint a 0.
    nouNode->puntuacioRellevancia = 0.0; // Inicialitza la puntuació de rellevància a 0.0.
    nouNode->seguent = NULL; // El nou node, en ser afegit al principi de la llista, no té un 'seguent' inicial.

    // Afegeix el node al principi de la llista encadenada (hash table chaining).
    unsigned int index = hashGraf(idDocument, graf->mida); // Calcula l'índex de la taula hash.
    nouNode->seguent = graf->nodes[index]; // El punter 'seguent' del nou node apunta al node que abans era el primer en aquesta posició.
    graf->nodes[index] = nouNode; // El nou node es converteix en el primer d'aquesta posició de la taula hash.

    graf->totalNodes++;   // Incrementa el comptador total de nodes al graf.
}

// Afegeix una aresta dirigida entre dos nodes (documents)
// Aquesta funció afegeix una connexió dirigida d'un document origen a un document destí,
// actualitzant els graus d'entrada i sortida.
void afegirArestaAGraf(GrafDirigit *graf, int idDocOrigen, int idDocDesti) {
    afegirNodeAGraf(graf, idDocOrigen);   // Assegura que el node d'origen existeix al graf (el crea si no existeix).
    afegirNodeAGraf(graf, idDocDesti);    // Assegura que el node de destí existeix al graf (el crea si no existeix).

    NodeGrau *nodeOrigen = cercarNodeEnGraf(graf, idDocOrigen); // Troba el node d'origen.
    if (nodeOrigen) nodeOrigen->grauSortint++;   // Si el node d'origen existeix, incrementa el seu grau sortint (ha afegit una aresta que surt d'ell).

    NodeGrau *nodeDesti = cercarNodeEnGraf(graf, idDocDesti); // Troba el node de destí.
    if (nodeDesti) nodeDesti->grauEntrant++;   // Si el node de destí existeix, incrementa el seu grau entrant (ha afegit una aresta que entra a ell).

    graf->totalEnllacos++;   // Incrementa el nombre total d’enllaços (arestes) al graf.
}

// Construeix un graf dirigit a partir d’una llista de documents amb enllaços
// Aquesta funció és la principal per poblar el graf a partir de tots els documents carregats.
GrafDirigit *construirGrafDirigitDeDocuments(Document *documents) {
    GrafDirigit *graf = crearGrafDirigit(MIDA_INICIAL_GRAF);   // Inicialitza un nou graf dirigit amb la mida predefinida.

    printf("Construint el graf dirigit de documents...\n"); // Missatge informatiu.

    // Primera passada: afegeix tots els documents com a nodes al graf.
    Document *docActual = documents; // Punter per recórrer la llista de documents.
    while (docActual != NULL) { // Recorre cada document de la llista.
        afegirNodeAGraf(graf, docActual->id); // Afegeix el document actual com a node al graf.
        docActual = docActual->seguent; // Avança al següent document.
    }

    // Segona passada: afegeix totes les arestes basant-se en els enllaços dels documents.
    docActual = documents; // Reinicia el punter al principi de la llista de documents.
    while (docActual != NULL) { // Recorre cada document de nou.
        Enllacos *enllacActual = docActual->enllacos; // Punter per recórrer la llista d'enllaços del document actual.
        while (enllacActual != NULL) { // Recorre cada enllaç d'aquest document.
            afegirArestaAGraf(graf, docActual->id, enllacActual->idDocumentDesti);   // Afegeix una aresta del document actual (origen) al document de destí de l'enllaç.
            enllacActual = enllacActual->seguent; // Avança al següent enllaç.
        }
        docActual = docActual->seguent; // Avança al següent document.
    }

    // Calcula la rellevància de tots els nodes un cop el graf està completament construït.
    calcularPuntuacionsRellevancia(graf);

    printf("Graf construït: %d nodes, %d arestes\n", graf->totalNodes, graf->totalEnllacos); // Missatge final amb el resum del graf.
    return graf; // Retorna el punter al graf construït.
}

// Calcula la rellevància dels nodes segons el grau entrant
// Aquesta funció calcula i normalitza la puntuació de rellevància per a cada node del graf.
void calcularPuntuacionsRellevancia(GrafDirigit *graf) {
    if (graf->totalNodes == 0) return; // Si no hi ha nodes al graf, no hi ha res a calcular.

    int maxGrauEntrant = 0; // Variable per trobar el grau entrant màxim de tot el graf.

    // Troba el grau entrant màxim entre tots els nodes.
    for (size_t i = 0; i < graf->mida; i++) { // Itera sobre cada posició de la taula hash.
        NodeGrau *actual = graf->nodes[i]; // Punter per recórrer la llista encadenada a cada posició.
        while (actual != NULL) { // Recorre tots els nodes en aquesta posició.
            if (actual->grauEntrant > maxGrauEntrant) { // Si el grau entrant del node actual és més gran que el màxim trobat fins ara.
                maxGrauEntrant = actual->grauEntrant; // Actualitza el valor del grau entrant màxim.
            }
            actual = actual->seguent; // Avança al següent node de la llista.
        }
    }

    // Normalitza les puntuacions de rellevància de cada node.
    for (size_t i = 0; i < graf->mida; i++) { // Itera de nou sobre cada posició de la taula hash.
        NodeGrau *actual = graf->nodes[i]; // Punter per recórrer els nodes.
        while (actual != NULL) { // Recorre tots els nodes.
            if (maxGrauEntrant > 0) { // Per evitar la divisió per zero si no hi ha cap enllaç entrant a cap document.
                actual->puntuacioRellevancia = (double)actual->grauEntrant / maxGrauEntrant; // Calcula la puntuació normalitzada.
            } else {
                actual->puntuacioRellevancia = 0.0; // Si no hi ha enllaços entrants, la puntuació és 0.
            }
            actual = actual->seguent; // Avança al següent node.
        }
    }
}

// Retorna el grau entrant d’un document concret
// Funció per obtenir el nombre d'enllaços que apunten a un document específic.
int obtenirGrauEntrantDocument(GrafDirigit *graf, int idDocument) {
    NodeGrau *node = cercarNodeEnGraf(graf, idDocument); // Busca el node corresponent a l'ID del document.
    if (node) return node->grauEntrant; // Si el node es troba, retorna el seu grau entrant.
    return -1;    // Retorna -1 si el document no es troba al graf.
}

// Retorna la puntuació de rellevància d’un document concret
// Funció per obtenir la puntuació de rellevància normalitzada d'un document específic.
double obtenirPuntuacioRellevanciaDocument(GrafDirigit *graf, int idDocument) {
    NodeGrau *node = cercarNodeEnGraf(graf, idDocument); // Busca el node corresponent a l'ID del document.
    if (node) return node->puntuacioRellevancia; // Si el node es troba, retorna la seva puntuació de rellevància.
    return 0.0;   // Si el document no es troba, retorna 0.0 com a puntuació per defecte.
}

// Funció de comparació per ordenar els documents per rellevància (descendent)
// Aquesta funció s'utilitza amb qsort per ordenar un array de punters a Document.
int compararDocumentsRellevancia(const void *a, const void *b) {
    Document *docA = *(Document **)a; // Converteix el punter genèric 'a' a un punter a punter a Document, i després desreferencia per obtenir el punter al Document A.
    Document *docB = *(Document **)b; // Converteix el punter genèric 'b' a un punter a punter a Document, i després desreferencia per obtenir el punter al Document B.

    double puntuacioA = docA->puntuacioRellevancia; // Obté la puntuació de rellevància del document A.
    double puntuacioB = docB->puntuacioRellevancia; // Obté la puntuació de rellevància del document B.

    if (puntuacioA < puntuacioB) return 1; // Si A té menys puntuació que B, B va abans que A (ordenació descendent).
    if (puntuacioA > puntuacioB) return -1; // Si A té més puntuació que B, A va abans que B (ordenació descendent).
    return 0; // Si les puntuacions són iguals, l'ordre no importa.
}

// Mostra els documents ordenats per rellevància, amb límit màxim
// Aquesta funció recull tots els documents, els assigna les seves puntuacions de rellevància,
// els ordena i imprimeix els 'maxResultats' primers.
void imprimirDocumentsPerRellevancia(GrafDirigit *graf, Document *documents, int maxResultats) {
    int totalDocs = 0; // Comptador per saber el nombre total de documents.
    Document *actualDoc = documents; // Punter per recórrer la llista de documents.
    while (actualDoc != NULL) { // Recorre tota la llista per comptar els documents.
        totalDocs++; // Incrementa el comptador.
        actualDoc = actualDoc->seguent; // Avança al següent document.
    }

    if (totalDocs == 0) { // Si no hi ha documents, imprimeix un missatge i surt.
        printf("No hi ha documents per mostrar.\n");
        return;
    }

    Document **arrayDocs = malloc(sizeof(Document *) * totalDocs); // Assigna memòria per a un array de punters a Document.
    if (!arrayDocs) { // Comprova si l'assignació ha fallat.
        perror("Error allocant array per ordenació"); // Imprimeix un error.
        return; // Surt de la funció.
    }

    // Omple l’array amb els punters als documents i assigna les seves puntuacions de rellevància.
    actualDoc = documents; // Reinicia el punter al principi de la llista de documents.
    int i = 0; // Índex per a l'array.
    while (actualDoc != NULL) { // Recorre la llista de documents.
        arrayDocs[i] = actualDoc; // Emmagatzema el punter al document a l'array.
        NodeGrau *node = cercarNodeEnGraf(graf, actualDoc->id); // Busca el node corresponent al document al graf.
        if (node) { // Si el node es troba.
            actualDoc->puntuacioRellevancia = node->puntuacioRellevancia; // Assigna la puntuació de rellevància del node del graf al document.
        } else {
            actualDoc->puntuacioRellevancia = 0.0; // Si el node no es troba al graf, assigna 0.0 de puntuació.
        }
        i++; // Incrementa l'índex.
        actualDoc = actualDoc->seguent; // Avança al següent document.
    }

    // Ordena l'array de documents per rellevància utilitzant qsort i la funció de comparació.
    qsort(arrayDocs, totalDocs, sizeof(Document *), compararDocumentsRellevancia);

    int mostrats = 0; // Comptador de resultats mostrats.
    int limit = maxResultats <= 0 ? totalDocs : maxResultats; // Defineix el límit de resultats a mostrar (tots si maxResultats és 0 o negatiu).
    printf("\nDocuments ordenats per rellevància:\n"); // Capçalera per a la sortida.
    for (i = 0; i < totalDocs && mostrats < limit; i++) { // Itera sobre l'array ordenat fins al límit.
        Document *doc = arrayDocs[i]; // Obté el punter al document actual.
        printf("ID: %d, Puntuació: %.3f, Títol: %s\n", doc->id, doc->puntuacioRellevancia, doc->titol); // Imprimeix la informació del document.
        mostrats++; // Incrementa el comptador de mostrats.
    }

    free(arrayDocs);    // Allibera la memòria temporal de l'array de punters.
}

// Allibera tota la memòria utilitzada pel graf
// Aquesta funció allibera de forma segura tota la memòria dinàmica assignada al graf.
void alliberarGrafDirigit(GrafDirigit *graf) {
    if (!graf) return; // Si el punter al graf és NULL, no fa res.

    for (size_t i = 0; i < graf->mida; i++) { // Itera sobre cada posició de la taula hash.
        NodeGrau *actual = graf->nodes[i]; // Punter per recórrer la llista encadenada.
        while (actual != NULL) { // Recorre cada node de la llista.
            NodeGrau *temp = actual; // Guarda el punter al node actual per alliberar-lo.
            actual = actual->seguent; // Avança al següent node.
            free(temp);   // Allibera la memòria de l'estructura NodeGrau.
        }
    }

    free(graf->nodes);    // Allibera la memòria de l'array de punters (la taula hash).
    free(graf);           // Allibera la memòria de l'estructura principal del graf.
}