#include "document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

Links *LinksInit() {
    return NULL; // Inicialitza la llista d'enllaços com a NULL
}

//funcio per alliberar el document sencer, titol, cos, enllacos i estructura
void freeDocument(Document* document) {
    if (document) {
        free(document->title); // allibera el títol
        free(document->body); // allibera el cos
        freeLinks(document->links); // allibera els enllaços
        free(document); // allibera l'estructura del document
    }
}

void LinksAdd(Links **head, int documentId, char *linkText) {
    Links *newLink = (Links *)malloc(sizeof(Links));
    newLink->documentId = documentId;
    newLink->linkText = strdup(linkText);
    newLink->next = *head; // afegeix al principi de la llista
    *head = newLink;
}

//Funcio per llegir un document del dataset i guardr tota la informacio

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
    bufferIdx = 0; //reset buffer index for title
    while ((ch = fgetc(f)) != '\n') {
        assert(bufferIdx < bufferSize);
        buffer[bufferIdx++] = ch;
    }
    buffer[bufferIdx++] = '\0';
    document->title = strdup(buffer); //assignar titol

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

                //afegeix l'enllaç a la llista
                LinksAdd(&links, linkId, buffer + bufferIdx - linkBufferIdx - strlen(linkBuffer) - 2); //afegeix el text de l'enllaç

                linkBufferIdx = 0;
            } else if (ch != '(') { // skip first parenthesis of the link
                assert(linkBufferIdx < linkBufferSize);
                linkBuffer[linkBufferIdx++] = ch;
            } 
        } else if (ch == '[') { // found beginning of link text
            parsingLink = true;
            linkBufferIdx = 0; //reset link buffer index
        }
    }
    buffer[bufferIdx++] = '\0';
    
    //assigna el cos a l'estructura Document
    document->body = (char *)malloc(sizeof(char) * (bufferIdx + 1)); // +1 per al caràcter null
    strcpy(document->body, buffer);
    document->links = links; // assigna la llista d'enllaços al document

    fclose(f); // tanca el fitxer
    return document; // retorna el document
}