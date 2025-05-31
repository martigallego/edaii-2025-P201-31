#include "graph.h" // Inclou la capçalera amb les estructures i funcions declarades pel graf
#include <limits.h> // Biblioteca amb constants com INT_MAX
#include <stdio.h> // Biblioteca estàndard per entrada/sortida (fgets, printf, perror...)
#include <stdlib.h> // Biblioteca per gestió de memòria dinàmica (malloc, free, exit...)
#include <string.h> // Biblioteca per manipular cadenes (strchr, strcmp...)

#define MIDA_INICIAL_GRAF                                                      \
  10007 // Constant per la mida inicial del graf (nombre primer per millor
        // dispersió hash)

// Funcio de hash per a identificadors de documents
unsigned int hashGraf(int idDocument, size_t midaGraf) {
  return (unsigned int)idDocument %
         midaGraf; // Retorna l'índex calculat per taula hash
}

// Crea i inicialitza un nou graf dirigit
GrafDirigit *crearGrafDirigit(size_t mida) {
  GrafDirigit *graf = (GrafDirigit *)malloc(
      sizeof(GrafDirigit)); // Reserva memòria per la estructura GrafDirigit
  if (!graf) {              // Comprova si malloc ha fallat
    perror("Error creant el graf dirigit"); // Missatge d'error
    exit(EXIT_FAILURE);                     // Finalitza el programa amb error
  }

  graf->mida = mida;       // Assigna la mida inicial
  graf->totalNodes = 0;    // Inicialitza el recompte de nodes a 0
  graf->totalEnllacos = 0; // Inicialitza el recompte d'enllaços a 0
  graf->nodes = (NodeGrau **)calloc(
      mida, sizeof(NodeGrau *)); // Reserva la taula hash de punters a NodeGrau
  if (!graf->nodes) {            // Comprova si calloc ha fallat
    perror("Error assignant memòria per als nodes del graf");
    free(graf); // Allibera la memòria del graf abans de sortir
    exit(EXIT_FAILURE);
  }

  return graf; // Retorna el punter al graf creat
}

// Busca un node dins del graf donat el seu id
NodeGrau *cercarNodeEnGraf(GrafDirigit *graf, int idDocument) {
  unsigned int index =
      hashGraf(idDocument, graf->mida);  // Calcula la posició a la taula hash
  NodeGrau *actual = graf->nodes[index]; // Apunta al primer node de la llista
                                         // en aquesta posició
  while (actual != NULL) {               // Recorre la llista
    if (actual->idDocument == idDocument)
      return actual;          // Si troba l'ID, el retorna
    actual = actual->seguent; // Passa al següent node
  }
  return NULL; // Si no troba el node, retorna NULL
}

// Afegeix un nou node al graf si no existeix prèviament
void afegirNodeAGraf(GrafDirigit *graf, int idDocument) {
  if (cercarNodeEnGraf(graf, idDocument) != NULL)
    return; // Si el node ja existeix, no fa res

  NodeGrau *nouNode =
      (NodeGrau *)malloc(sizeof(NodeGrau)); // Reserva memòria per al nou node
  if (!nouNode) {                           // Comprova si malloc ha fallat
    perror("Error creant nou node del graf");
    exit(EXIT_FAILURE);
  }

  // Inicialització dels camps del nou node
  nouNode->idDocument = idDocument; // Assigna ID
  nouNode->grauEntrant = 0;         // Inicialitza grau entrant
  nouNode->grauSortint = 0;         // Inicialitza grau sortint
  nouNode->puntuacioRellevancia =
      0.0;                         // Inicialitza la puntuació de rellevància
  nouNode->seguent = NULL;         // Encara no apunta a cap altre node
  nouNode->arestesSortints = NULL; // Sense arestes sortints encara

  unsigned int index =
      hashGraf(idDocument, graf->mida); // Calcula index a taula hash
  nouNode->seguent =
      graf->nodes[index];       // Enllaça el nou node a la capçalera actual
  graf->nodes[index] = nouNode; // Fa que el nou node sigui la nova capçalera

  graf->totalNodes++; // Actualitza el recompte de nodes
}

// Afegeix una aresta al graf entre dos documents
void afegirArestaAGraf(GrafDirigit *graf, int idDocOrigen, int idDocDesti) {
  afegirNodeAGraf(graf, idDocOrigen); // Garanteix que el node origen existeix
  afegirNodeAGraf(graf, idDocDesti);  // Garanteix que el node destí existeix

  NodeGrau *nodeOrigen =
      cercarNodeEnGraf(graf, idDocOrigen);                  // Busca node origen
  NodeGrau *nodeDesti = cercarNodeEnGraf(graf, idDocDesti); // Busca node destí

  if (nodeOrigen == NULL || nodeDesti == NULL)
    return; // Si falta algun, no fa res

  Aresta *nova = malloc(sizeof(Aresta)); // Reserva memòria per a nova aresta
  nova->idDesti = idDocDesti;            // Assigna l'ID del document destí
  nova->seguent =
      nodeOrigen
          ->arestesSortints; // Insereix al principi de la llista d'arestes
  nodeOrigen->arestesSortints = nova; // Actualitza la capçalera de la llista

  nodeOrigen->grauSortint++; // Incrementa grau sortint del node origen
  nodeDesti->grauEntrant++;  // Incrementa grau entrant del node destí
  graf->totalEnllacos++;     // Actualitza nombre total d'enllaços
}

// Construeix el graf a partir de la llista de documents
GrafDirigit *construirGrafDirigitDeDocuments(Document *documents) {
  GrafDirigit *graf = crearGrafDirigit(MIDA_INICIAL_GRAF); // Crea el graf

  Document *docActual = documents;        // Inicialitza recorregut de documents
  while (docActual != NULL) {             // Per cada document
    afegirNodeAGraf(graf, docActual->id); // Afegeix el node al graf
    docActual = docActual->seguent;       // Passa al següent document
  }

  docActual = documents; // Torna a l'inici per afegir les arestes
  while (docActual != NULL) {
    Enllacos *e =
        docActual->enllacos; // Llegeix la llista d'enllaços del document
    while (e != NULL) {
      afegirArestaAGraf(graf, docActual->id,
                        e->idDocumentDesti); // Afegeix aresta
      e = e->seguent;                        // Passa al següent enllaç
    }
    docActual = docActual->seguent; // Passa al següent document
  }

  calcularPuntuacionsRellevancia(graf); // Calcula puntuacions de rellevància
  return graf;                          // Retorna el graf
}

// Calcula la puntuació de rellevància de cada node segons el seu grau entrant
void calcularPuntuacionsRellevancia(GrafDirigit *graf) {
  if (graf->totalNodes == 0)
    return;               // Si no hi ha nodes, surt
  int maxGrauEntrant = 0; // Variable per al màxim grau entrant

  // Troba el grau entrant màxim de tots els nodes
  for (size_t i = 0; i < graf->mida; i++) {
    NodeGrau *actual = graf->nodes[i];
    while (actual != NULL) {
      if (actual->grauEntrant > maxGrauEntrant)
        maxGrauEntrant = actual->grauEntrant; // Actualitza el màxim si cal
      actual = actual->seguent;
    }
  }

  // Calcula puntuació de rellevància normalitzada
  for (size_t i = 0; i < graf->mida; i++) {
    NodeGrau *actual = graf->nodes[i];
    while (actual != NULL) {
      actual->puntuacioRellevancia =
          (maxGrauEntrant > 0) ? (double)actual->grauEntrant / maxGrauEntrant
                               : 0.0; // Assigna puntuació normalitzada
      actual = actual->seguent;
    }
  }
}

// Retorna el grau entrant d'un document concret
int obtenirGrauEntrantDocument(GrafDirigit *graf, int idDocument) {
  NodeGrau *n = cercarNodeEnGraf(graf, idDocument); // Busca node pel seu ID
  return (n != NULL) ? n->grauEntrant : -1; // Retorna grau o -1 si no existeix
}

// Retorna la puntuació de rellevància d'un document
double obtenirPuntuacioRellevanciaDocument(GrafDirigit *graf, int idDocument) {
  NodeGrau *n = cercarNodeEnGraf(graf, idDocument); // Busca node pel seu ID
  return (n != NULL) ? n->puntuacioRellevancia
                     : 0.0; // Retorna puntuació o 0.0 si no existeix
}

// Funcio de comparació per a qsort, ordena descendentment per puntuació
int compararDocumentsRellevancia(const void *a, const void *b) {
  Document *docA = *(Document **)a;
  Document *docB = *(Document **)b;
  if (docA->puntuacioRellevancia < docB->puntuacioRellevancia)
    return 1;
  if (docA->puntuacioRellevancia > docB->puntuacioRellevancia)
    return -1;
  return 0;
}

// Imprimeix els documents ordenats per rellevància
void imprimirDocumentsPerRellevancia(GrafDirigit *graf, Document *documents,
                                     int maxResultats) {
  int total = 0;
  Document *d = documents;
  while (d != NULL) {
    total++;
    d = d->seguent;
  } // Compta total de documents
  if (total == 0)
    return;

  Document **array =
      malloc(sizeof(Document *) * total); // Reserva array per ordenar
  d = documents;
  for (int i = 0; i < total; i++) {
    array[i] = d;
    NodeGrau *n =
        cercarNodeEnGraf(graf, d->id); // Busca node per obtenir puntuació
    d->puntuacioRellevancia =
        (n != NULL) ? n->puntuacioRellevancia : 0.0; // Assigna puntuació
    d = d->seguent;
  }

  qsort(array, total, sizeof(Document *),
        compararDocumentsRellevancia); // Ordena per rellevància
  for (int i = 0; i < total && i < maxResultats; i++) {
    printf("ID: %d, Puntuació: %.3f, Títol: %s\n", array[i]->id,
           array[i]->puntuacioRellevancia, array[i]->titol); // Mostra resultat
  }
  free(array); // Allibera memòria
}

// Allibera tota la memòria utilitzada pel graf
void alliberarGrafDirigit(GrafDirigit *graf) {
  if (!graf)
    return;
  for (size_t i = 0; i < graf->mida; i++) {
    NodeGrau *n = graf->nodes[i];
    while (n != NULL) {
      NodeGrau *seguent = n->seguent; // Guarda el següent node
      Aresta *a = n->arestesSortints; // Recorre la llista d'arestes sortints
      while (a != NULL) {
        Aresta *tmp = a;
        a = a->seguent;
        free(tmp); // Allibera aresta
      }
      free(n); // Allibera node
      n = seguent;
    }
  }
  free(graf->nodes); // Allibera la taula hash
  free(graf);        // Allibera la struct principal
}

Document **ordenaDocumentsPerRellevancia(Document *llista, int *nombre) {
  // Comptar quants documents hi ha
  int n = 0;
  Document *temp = llista;
  while (temp != NULL) {
    n++;
    temp = temp->seguent;
  }
  *nombre = n;

  // Crear array de punters
  Document **array = malloc(n * sizeof(Document *));
  temp = llista;
  for (int i = 0; i < n; i++) {
    array[i] = temp;
    temp = temp->seguent;
  }

  // Ordenar amb qsort
  qsort(array, n, sizeof(Document *), compararDocumentsRellevancia);
  return array;
}
