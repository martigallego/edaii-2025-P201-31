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
    if (!file)
    {
        perror("No s'ha pogut obrir el fitxer del document");
        return NULL; // Retorna NULL si no s'ha pogut obrir
    }

    Document *document = (Document *)malloc(sizeof(Document)); // Reserva memòria pel document
    if (!document)
    {
        perror("Error en assignar memòria per al document");
        fclose(file); // Tanca fitxer abans de sortir
        return NULL;
    }

    memset(document, 0, sizeof(Document)); // Inicialitza el contingut del document a 0

    char buffer[2048]; // Buffer per llegir línies

    // Llegeix l’ID del document
    if (fgets(buffer, sizeof(buffer), file) == NULL || sscanf(buffer, "%d", &document->id) != 1)
    {
        fprintf(stderr, "Error llegint ID del document en %s\n", camins);
        alliberaDocument(document); // Allibera memòria
        fclose(file);               // Tanca fitxer
        return NULL;
    }

    // Llegeix el títol del document
    if (fgets(buffer, sizeof(buffer), file) == NULL)
    {
        fprintf(stderr, "Error llegint títol del document en %s\n", camins);
        alliberaDocument(document);
        fclose(file);
        return NULL;
    }

    char *title_prefix_str = "Title: "; // Prefix opcional al títol
    if (strncmp(buffer, title_prefix_str, strlen(title_prefix_str)) == 0)
        document->titol = strdup(buffer + strlen(title_prefix_str)); // Elimina prefix si hi és
    else
        document->titol = strdup(buffer); // Copia tal qual

    document->titol[strcspn(document->titol, "\n")] = 0; // Elimina salt de línia final

    char *cos_acumulat = NULL; // Buffer acumulador pel cos
    size_t cos_len = 0;        // Longitud total
    char line[2048];           // Buffer temporal per línies

    // Llegeix totes les línies restants del fitxer
    while (fgets(line, sizeof(line), file))
    {
        size_t len = strlen(line); // Mida de la línia llegida
        cos_acumulat = realloc(cos_acumulat, cos_len + len + 1);
        if (!cos_acumulat)
        {
            perror("Error de realloc per al cos del document");
            alliberaDocument(document);
            fclose(file);
            return NULL;
        } // Reassigna memòria
        strcpy(cos_acumulat + cos_len, line); // Afegeix la línia al final del text acumulat
        cos_len += len;                       // Actualitza la mida total
    }

    if (cos_acumulat)
    {
        // Elimina espais en blanc i salts de línia finals
        while (cos_len > 0 && (cos_acumulat[cos_len - 1] == '\n' || cos_acumulat[cos_len - 1] == ' '))
            cos_acumulat[--cos_len] = '\0';
        document->cos = cos_acumulat; // Assigna el cos final
    }

    document->enllacos = NULL; // Inicialitza llista d’enllaços

    // Cerca enllaços dins del cos: format [text](id)
    char *p = document->cos;
    while ((p = strchr(p, '[')) != NULL)
    {                                         // Busca obertura d’enllaç
        char *endText = strchr(p, ']');       // Fi del text visible
        char *startId = strchr(endText, '('); // Inici de l’ID
        char *endId = strchr(startId, ')');   // Fi de l’ID

        if (endText && startId && endId && endText < startId && startId < endId)
        {
            char text[1024]; // Text visible
            char idStr[32];  // ID com a string

            strncpy(text, p + 1, endText - p - 1); // Copia el text entre [ ]
            text[endText - p - 1] = '\0';

            strncpy(idStr, startId + 1, endId - startId - 1); // Copia l’ID entre ( )
            idStr[endId - startId - 1] = '\0';

            int id = atoi(idStr); // Converteix ID a enter
            if (id >= 0)
                afegeixEnllac(&document->enllacos, id, text); // Afegeix l'enllaç al document

            p = endId + 1; // Continua després del ')'
        }
        else
        {
            break; // Si el format no és vàlid, para
        }
    }

    fclose(file);    // Tanca el fitxer
    return document; // Retorna el document carregat
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

            char camiComplet[1024]; // Crea ruta completa
            snprintf(camiComplet, sizeof(camiComplet), "%s/%s", rutaDirectori, ent->d_name);

            Document *document = deserialitzaDocument(camiComplet); // Deserialitza fitxer

            if (document != NULL)
            {
                if (documents == NULL)
                { // Primer document
                    documents = document;
                    ultim = document;
                }
                else
                {
                    ultim->seguent = document; // Afegeix al final
                    ultim = document;
                }
                documents_carregats_count++; // Suma al comptador
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

    return documents; // Retorna la llista de documents
}
