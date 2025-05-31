#include "document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>

// Funció per inicialitzar una llista d'enllaços buida
Enllacos *iniciaEnllacos()
{
    return NULL; // Retorna NULL com a indicador de llista buida
}

// Allibera la memòria ocupada per la llista d'enllaços
void alliberaEnllacos(Enllacos *enllac)
{
    while (enllac != NULL)
    {                             // Recorre la llista fins que no quedi cap node
        Enllacos *temp = enllac;  // Desa el node actual
        enllac = enllac->seguent; // Passa al següent node
        if (temp->textEnllac)
            free(temp->textEnllac); // Allibera el text de l'enllaç si existeix
        if (temp->titolEnllac)
            free(temp->titolEnllac); // Allibera el títol si està definit
        free(temp);                  // Allibera el node complet
    }
}

// Allibera la memòria associada a un document
void alliberaDocument(Document *document)
{
    if (document == NULL)
        return;                           // No fa res si el punter és NULL
    free(document->titol);                // Allibera la memòria del títol
    free(document->cos);                  // Allibera la memòria del cos
    alliberaEnllacos(document->enllacos); // Allibera la llista d'enllaços
    free(document);                       // Allibera l'estructura principal del document
}

// Afegeix un nou enllaç a la llista d'enllaços
void afegeixEnllac(Enllacos **cap, int idDocumentDesti, const char *textEnllac)
{
    if (textEnllac == NULL)
    { // Comprovació: si el text és NULL
        printf("[ERROR] Text de l'enllaç és NULL per id destí: %d\n", idDocumentDesti);
        return; // No es continua
    }

    Enllacos *nouEnllac = (Enllacos *)malloc(sizeof(Enllacos)); // Reserva memòria per un nou node
    if (nouEnllac == NULL)
    { // Comprova si malloc ha fallat
        perror("Error en assignar memòria per a nou enllaç");
        exit(EXIT_FAILURE); // Surt amb error crític
    }

    nouEnllac->idDocumentDesti = idDocumentDesti; // Assigna ID del document de destí
    nouEnllac->textEnllac = strdup(textEnllac);   // Duplica el text de l'enllaç

    if (nouEnllac->textEnllac == NULL)
    { // Comprovació de strdup
        perror("Error en duplicar el text de l'enllaç");
        free(nouEnllac); // Allibera memòria si ha fallat
        exit(EXIT_FAILURE);
    }

    nouEnllac->titolEnllac = NULL; // Inicialitza el títol com a NULL
    nouEnllac->seguent = NULL;     // Nou enllaç encara no apunta a res

    if (*cap == NULL)
    {                     // Si la llista és buida
        *cap = nouEnllac; // El nou enllaç és el cap
    }
    else
    {
        Enllacos *actual = *cap; // Comença a recórrer la llista
        while (actual->seguent != NULL)
        { // Fins arribar al final
            actual = actual->seguent;
        }
        actual->seguent = nouEnllac; // Afegeix el nou enllaç al final
    }
}

// Deserialitza un fitxer i construeix un objecte Document
Document *deserialitzaDocument(char *camins)
{
    FILE *file = fopen(camins, "r"); // Obre el fitxer en mode lectura
    if (!file) {
        perror("No s'ha pogut obrir el fitxer del document");
        return NULL; // Retorna NULL si no s'ha pogut obrir
    }

    Document *document = malloc(sizeof(Document)); // Reserva memòria pel document
    if (!document) {
        perror("Error en assignar memòria per al document");
        fclose(file); // Tanca fitxer abans de sortir
        return NULL;
    }


    // Llegir l’ID del document
    if (fscanf(file, "%d\n", &document->id) != 1) {
        fprintf(stderr, "Format d'ID incorrecte: %s\n", camins);
        free(document); // Allibera memòria si l'ID no és vàlid
        fclose(file);   // Tanca fitxer
        return NULL;
    }

    // Llegir el títol del document
    // Utilitza getline per gestionar longituds variables
    size_t len_titol = 0;
    document->titol = NULL;
    if (getline(&document->titol, &len_titol, file) == -1) {
        perror("Error llegint el títol");
        free(document);
        fclose(file);
        return NULL;
    }
    // Elimina salt de línia final, si n'hi ha
    if (document->titol[strlen(document->titol) - 1] == '\n')
        document->titol[strlen(document->titol) - 1] = '\0';

    // Llegir tot el "cos" en un buffer dinàmic (cos_acumulat)
    size_t buffer_cap = 4096;                      // Capacitat inicial del buffer
    size_t cos_len = 0;                            // Longitud acumulada del cos
    char *cos_acumulat = malloc(buffer_cap);       // Reserva memòria inicial
    if (!cos_acumulat) {
        perror("Error assignant memòria per al cos");
        free(document->titol);
        free(document);
        fclose(file);
        return NULL;
    }
    cos_acumulat[0] = '\0'; // Inicia cadena buida

    char line[1024]; // Buffer d'entrada per a cada línia
    while (fgets(line, sizeof(line), file)) {
        size_t l = strlen(line); // Mida de la línia llegida
        if (cos_len + l + 1 > buffer_cap) {               // Si s'excedeix la capacitat
            buffer_cap *= 2;                              // Duplica la capacitat
            char *tmp = realloc(cos_acumulat, buffer_cap); // Reassigna memòria
            if (!tmp) {
                perror("Error reassignant memòria per al cos");
                free(cos_acumulat);
                free(document->titol);
                free(document);
                fclose(file);
                return NULL;
            }
            cos_acumulat = tmp; // Assigna el nou punter
        }
        memcpy(cos_acumulat + cos_len, line, l); // Copia la línia al final del buffer
        cos_len += l;                            // Actualitza la longitud
        cos_acumulat[cos_len] = '\0';            // Assegura final de cadena
    }
    // Elimina salts de línia i espais finals del cos
    while (cos_len > 0 && (cos_acumulat[cos_len - 1] == '\n' || cos_acumulat[cos_len - 1] == ' ')) {
        cos_acumulat[--cos_len] = '\0';
    }

    // Assignar document->cos
    // Si no hi ha contingut (cos_len == 0), guardem una cadena buida
    if (cos_len > 0) {
        document->cos = cos_acumulat; // Assigna el cos acumulat
    } else {
        free(cos_acumulat);                    // Allibera buffer si està buit
        document->cos = strdup("");            // Cadena buida per evitar NULL
        if (!document->cos) {
            perror("Error assignant memòria per a cadena buida");
            free(document->titol);
            free(document);
            fclose(file);
            return NULL;
        }
    }

    // Inicialitzar la llista d’enllaços com a buida
    document->enllacos = NULL;

    // Cercar enllaços dins del cos (format: [text](id))
    // Només entrem si document->cos no és cadena buida
    if (document->cos[0] != '\0') {
        char *p = document->cos;
        while ((p = strchr(p, '[')) != NULL) {
            char *endText = strchr(p, ']'); // Fi del text visible
            char *startId = NULL;
            char *endId   = NULL;

            if (endText)
                startId = strchr(endText, '('); // Inici de l'ID
            if (startId)
                endId = strchr(startId, ')');   // Fi de l'ID

            if (endText && startId && endId && endText < startId && startId < endId) {
                // Extreure el "text" dins dels [ ]
                size_t lengthText = (size_t)(endText - p - 1);
                char text[1024];
                if (lengthText >= sizeof(text))
                    lengthText = sizeof(text) - 1;                // Limita la mida si es massa gran
                strncpy(text, p + 1, lengthText);                 // Copia el text
                text[lengthText] = '\0';

                // Extreure l'ID dins dels parèntesis ( )
                int lenId = (int)(endId - startId - 1);
                char idStr[32];
                if (lenId >= (int)sizeof(idStr))
                    lenId = (int)sizeof(idStr) - 1;              // Limita la mida si es massa gran
                strncpy(idStr, startId + 1, (size_t)lenId);     // Copia l'ID
                idStr[lenId] = '\0';

                int id_dest = atoi(idStr); // Converteix ID a enter
                if (id_dest >= 0) {
                    afegeixEnllac(&document->enllacos, id_dest, text); // Afegeix l'enllaç a la llista
                }
                p = endId + 1; // Continua després del ')'
            } else {
                // Si el format no és vàlid, sortim del bucle
                break;
            }
        }
    }

    fclose(file);    // Tanca el fitxer
    return document; // Retorna el document deserialitzat
}

// Llegeix tots els documents d’un directori
Document *carregaTotsElsDocuments(const char *rutaDirectori)
{
    DIR *dir;                          // Punter al directori
    struct dirent *ent;                // Punter a cada entrada del directori
    Document *documents = NULL;        // Cap de la llista de documents
    Document *ultim = NULL;            // Per anar afegint al final
    int documents_carregats_count = 0; // Comptador

    if ((dir = opendir(rutaDirectori)) != NULL)
    { // Intenta obrir el directori
        while ((ent = readdir(dir)) != NULL)
        { // Recorre cada entrada
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            {
                continue; // Omet "." i ".."
            }

            char camiComplet[1024]; // Crea ruta completa del fitxer
            snprintf(camiComplet, sizeof(camiComplet), "%s/%s", rutaDirectori, ent->d_name);

            Document *document = deserialitzaDocument(camiComplet); // Deserialitza el fitxer actual

            if (document != NULL)
            {
                if (documents == NULL)
                { // Primer document carregat
                    documents = document;
                    ultim = document;
                }
                else
                {
                    ultim->seguent = document; // Afegeix el document al final de la llista
                    ultim = document;
                }
                documents_carregats_count++; // Incrementa el comptador
            }
            else
            {
                fprintf(stderr, "No s'ha pogut carregar el document: %s\n", ent->d_name);
            }
        }
        closedir(dir); // Tanca el directori
        printf("Documents carregats amb èxit. Total: %d\n", documents_carregats_count);
    }
    else
    {
        perror("No s'ha pogut obrir el directori"); // Error si no es pot obrir
        return NULL;
    }

    return documents; // Retorna la llista enllaçada de documents
}
