#include "document.h"
#include <assert.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// funcio per inicialitzar una llista de enllaços
Links *LinksInit() {
  return NULL; // inicialitza la llista d'enllaços com a NULL
}

// funcio per alliberar links --> text i estructura
void freeLinks(Links *link) {
  while (link != NULL) {  // comprova que el link no es null
    Links *temp = link;   // node actual
    link = link->next;    // passa al node seguent
    free(temp->linkText); // allibera el text de l'enllaç
    free(temp);           // allibera l'estructura de l'enllaç
  }
}

// funcio per alliberar el document sencer, titol, cos, enllacos i estructura
void freeDocument(Document *document) {
  if (document) {
    free(document->title);      // allibera el títol
    free(document->body);       // allibera el cos
    freeLinks(document->links); // allibera els enllaços
    free(document);             // allibera l'estructura del document
  }
}

void LinksAdd(Links **head, int documentId, char *linkText) {
  Links *newLink = (Links *)malloc(sizeof(Links));
  newLink->documentId = documentId;
  newLink->linkText = strdup(linkText);
  newLink->next = *head; // afegeix al principi de la llista
  *head = newLink;
}

// Funcio per llegir un document del dataset i guardr tota la informacio
Document *document_desserialize(char *path) {
  FILE *f = fopen(path, "r");
  assert(f != NULL);

  Document *document = (Document *)malloc(sizeof(Document));

  char buffer[262144];
  int bufferSize = 262144;
  int bufferIdx = 0;
  char ch;

  // parse id
  while ((ch = fgetc(f)) != '\n') {
    assert(bufferIdx < bufferSize);
    buffer[bufferIdx++] = ch;
  }
  buffer[bufferIdx++] = '\0';
  document->id = atoi(buffer);

  // parse title
  bufferIdx = 0; // reset buffer index for title
  while ((ch = fgetc(f)) != '\n') {
    assert(bufferIdx <
           bufferSize); // comprova que no sobrepassem la mida del buffer
    buffer[bufferIdx++] =
        ch; // guarda el caràcter llegit dins el buffer i incrementa l'índex
  }
  buffer[bufferIdx++] =
      '\0'; // afegeix el caràcter nul al final per tancar la cadena
  document->title = strdup(buffer); // assigna el contingut del buffer com a
                                    // títol del document (còpia dinàmica)

  // parse body
  char linkBuffer[64];
  int linkBufferSize = 64;
  int linkBufferIdx = 0;
  bool parsingLink = false;
  Links *links = LinksInit();

  bufferIdx = 0;
  while ((ch = fgetc(f)) != EOF) {
    assert(bufferIdx < bufferSize);
    buffer[bufferIdx++] = ch;
    if (parsingLink) {
      if (ch == ')') { // end of link
        parsingLink = false;
        assert(linkBufferIdx < linkBufferSize);
        linkBuffer[linkBufferIdx++] = '\0';
        int linkId = atoi(linkBuffer);

        // afegeix l'enllaç a la llista
        // Crear una cadena temporal per al text de l'enllaç
        // Calcular la posició inicial del text de l'enllaç
        int startPos = bufferIdx - linkBufferIdx - strlen(linkBuffer) - 2;
        int linkTextLength = linkBufferIdx;

        if (startPos < 0) startPos = 0;
        if (linkTextLength < 0) linkTextLength = 0;

        char *linkText = (char *)malloc(linkTextLength + 1);
        strncpy(linkText, buffer + startPos, linkTextLength);
        linkText[linkTextLength] = '\0';

        LinksAdd(&links, linkId, linkText); // afegeix el text de l'enllaç

        linkBufferIdx = 0;
      } else if (ch != '(') { // skip first parenthesis of the link
        assert(linkBufferIdx < linkBufferSize);
        linkBuffer[linkBufferIdx++] = ch;
      }
    } else if (ch == '[') { // found beginning of link text
      parsingLink = true;
      linkBufferIdx = 0; // reset link buffer index
    }
  }
  assert(bufferIdx < bufferSize);
  buffer[bufferIdx++] = '\0';

  char *body = (char *)malloc(sizeof(char) * bufferIdx);
  strcpy(body, buffer);

  document->body = body;   // assignar cos al document
  document->links = links; // assginar els links al document
  fclose(f);               // tancar el arxiu
  return document;         // retornar el document
}

// funció que llegeix tots els documents d'una carpeta i retorna una linked list
// de Document
Document *loadAllDocuments(const char *directoryPath) {
  DIR *dir;             // punter per accedir al directori
  struct dirent *entry; // punter a una estructura dirent que representa una
                        // entrada del directori
  Document *documents = NULL; // inicialitza la llista de documents com a NULL

  // intentar obrir el directori especificat
  dir = opendir(directoryPath);
  if (dir == NULL) { // comprovar si el directori s'ha obert correctament
    perror("no es pot obrir el directori"); // mostra un missatge d'error si no
                                            // es pot obrir
    return NULL; // torna NULL si no es pot obrir el directori
  }

  // itero sobre cada entrada del directori
  while ((entry = readdir(dir)) != NULL) {
    // ignorar entrades "." i ".." que representen el directori actual i el pare
    if (entry->d_name[0] == '.') {
      continue; // passar a la següent entrada
    }

    // crear el camí complet del fitxer
    char filePath[512];
    snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath,
             entry->d_name); // crear el path complet del fitxer

    // comprovar si és un fitxer regular
    struct stat fileStat;
    if (stat(filePath, &fileStat) == 0 && S_ISREG(fileStat.st_mode)) {
      // cridar a document_desserialize per cada fitxer
      Document *document = document_desserialize(filePath);
      if (document !=
          NULL) { // comprovar si el document s'ha deserialitzat correctament
        // afegir el document a la llista
        document->next = documents; // afegir al principi de la llista
        documents = document;       // actualitzar el cap de la llista

        // mostrar la informació del document
        /*
        printf("ID: %d\n", document->id);
        printf("Title: %s\n", document->title);
        printf("Body: %s\n", document->body);

        // mostrar enllaços
        Links *current =
            document->links; // punter per recórrer la llista d'enllaços
        while (current) {    // iterar sobre cada enllaç
          printf("Link ID: %d, Text: %s\n", current->documentId,
                 current->linkText);
          current = current->next; // passar al següent enllaç
        }
        */
      } else {
        printf("error al llegir el document: %s\n",
               filePath); // mostrar missatge d'error si no es pot llegir el
                          // document
      }
    }
  }

  closedir(dir); // tancar el directori un cop s'han llegit totes les entrades
  return documents; // retorna la llista de documents carregats
}