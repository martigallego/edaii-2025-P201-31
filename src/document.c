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
  FILE *f = fopen(path, "r"); // obre el fitxer en mode lectura
  assert(f != NULL); // comprova que el fitxer s'ha obert correctament

  Document *document = (Document *)malloc(sizeof(Document)); // reserva memoria per a la estructura del document

  char buffer[262144]; // buffer per llegir el fitxer
  int bufferSize = 262144; // tamany del buffer
  int bufferIdx = 0; // index del buffer
  char ch; // caràcter actual

  // parse id
  while((ch = fgetc(f)) != '\n') { // llegir fins a trobar un salt de línia
    assert(bufferIdx < bufferSize); // comprova que el buffer no està ple
    buffer[bufferIdx++] = ch; // caràcter a l'índex actual del buffer
  }
  buffer[bufferIdx++] = '\0'; // acaba la cadena
  document->id = atoi(buffer); // converteix la cadena a un número -- atoi converteix una cadena de caràcters (char*) en un enter (int)

  // parse title
  bufferIdx = 0; // reset buffer index for title
  while((ch = fgetc(f)) != '\n') {
    assert(bufferIdx < bufferSize); // comprova que no sobrepassem la mida del buffer
    buffer[bufferIdx++] = ch; // guarda el caràcter llegit dins el buffer i incrementa l'índex
  }
  buffer[bufferIdx++] = '\0'; // afegeix el caràcter nul al final per tancar la cadena
  document->title = strdup(buffer); // assigna el contingut del buffer com a títol del document (còpia dinàmica)

  //cos
  char linkBuffer[64]; // buffer per llegir els enllaços
  int linkBufferSize = 64; // mida del buffer
  int linkBufferIdx = 0; // índex del buffer
  bool analitzarLink = false; //saber si estem llegint un enllaç
  Links *links = LinksInit(); // inicialitza la llista d'enllaços

  bufferIdx = 0;
  while((ch = fgetc(f)) != EOF) {
    assert(bufferIdx < bufferSize); // comprova que no sobrepassem la mida del buffer
    buffer[bufferIdx++] = ch; // caràcter a l'índex actual del buffer
    if(analitzarLink) {
      if(ch == ')') { // end of link
        analitzarLink = false; // deixem de llegir el link
        assert(linkBufferIdx < linkBufferSize); // comprova que no sobrepassem la mida del buffer
        linkBuffer[linkBufferIdx++] = '\0'; // acaba la cadena
        int linkId = atoi(linkBuffer); // converteix la cadena a un número

        // afegeix l'enllaç a la llista
        //crear una cadena temporal per al text de l'enllaç
        //calcular la posició inicial del text de l'enllaç
        int startPos = bufferIdx - linkBufferIdx - strlen(linkBuffer) - 2; // 2 per a les parèntesis
        int linkTextLength = linkBufferIdx; // longitud del text de l'enllaç

        if(startPos < 0) startPos = 0; // si la posició inicial és negativa, posa-la a 0
        if(linkTextLength < 0) linkTextLength = 0; // si la longitud és negativa, posa-la a 0

        char *linkText = (char *)malloc(linkTextLength + 1); // reserva memòria per al text de l'enllaç
        strncpy(linkText, buffer + startPos, linkTextLength); // copia el text de l'enllaç
        linkText[linkTextLength] = '\0'; // acaba la cadena
 
        LinksAdd(&links, linkId, linkText); // afegeix el text de l'enllaç

        linkBufferIdx = 0;
      } else if (ch != '(') { // skip first parenthesis of the link
        assert(linkBufferIdx < linkBufferSize);
        linkBuffer[linkBufferIdx++] = ch;
      }
    } else if (ch == '[') { // start of link
      analitzarLink = true; // comença a llegir el link
      linkBufferIdx = 0; // reset link buffer index
    }
  }
  assert(bufferIdx < bufferSize); // comprova que no sobrepassem la mida del buffer
  buffer[bufferIdx++] = '\0'; // acaba la cadena

  char *body =(char *)malloc(sizeof(char) * bufferIdx);
  strcpy(body, buffer);

  document->body = body;   // assignar cos al document
  document->links = links; // assginar els links al document
  fclose(f);               // tancar el arxiu
  return document;         // retornar el document
}

// funció que llegeix tots els documents d'una carpeta i retorna una linked list de DOCUMENT
Document *loadAllDocuments(const char *directoryPath) {
  DIR *dir;             // punter per accedir al directori
  struct dirent *entry; // punter a una estructura dirent que representa una entrada del directori
  Document *documents = NULL; // inicialitza la llista de documents com a NULL

  // intentar obrir el directori especificat
  dir = opendir(directoryPath);
  if(dir == NULL) { // comprovar si el directori s'ha obert correctament
    perror("no es pot obrir el directori"); // mostra un missatge d'error si no es pot obrir
    return NULL; // torna NULL si no es pot obrir el directori
  }

  // itero sobre cada entrada del directori
  while ((entry = readdir(dir)) != NULL) {
    //ignorar entrades . i .. que representen el directori actual i el pare
    if (entry->d_name[0] == '.') { // comprovar si l'entrada és . o ..
      continue; // passar a la següent entrada
    }

    // crear el camí complet del fitxer
    char filePath[512]; 
    snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath,
             entry->d_name); // crear el path complet del fitxer

    //comprovar si és un fitxer regular
    struct stat fileStat; // estructura per obtenir informació sobre el fitxer
    if(stat(filePath, &fileStat) == 0 && S_ISREG(fileStat.st_mode)) { 
      // cridar a document_desserialize per cada fitxer
      Document *document = document_desserialize(filePath);
      if(document !=
          NULL){ // comprovar si el document s'ha deserialitzat correctament
        // afegir el document a la llista
        document->next = documents; // afegir al principi de la llista
        documents = document;       // actualitzar el cap de la llista

        // mostrar la informació del document
        /*
        printf("ID: %d\n", document->id);
        printf("Title: %s\n", document->title);
        printf("Body: %s\n", document->body);

        // mostrar enllaços
        Links *current = document->links; // punter per recórrer la llista d'enllaços
        while (current) {    // iterar sobre cada enllaç
          printf("Link ID: %d, Text: %s\n", current->documentId,current->linkText);
          current = current->next; // passar al següent enllaç
        }
        */
      }else{
        printf("error al llegir el document: %s\n", filePath); // mostrar missatge d'error si no es pot llegir el document
      }
    }
  }

  closedir(dir); // tancar el directori un cop s'han llegit totes les entrades
  return documents; // retorna la llista de documents carregats
}