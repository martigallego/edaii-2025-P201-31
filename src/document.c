#include "document.h" 
#include <stdio.h>   
#include <stdlib.h>  
#include <string.h>   
#include <assert.h>
#include <dirent.h>   

// Inicialitza una llista d'enllaços buida
Enllacos *iniciaEnllacos() {
    return NULL;
}

// Allibera una llista d'enllaços
void alliberaEnllacos(Enllacos *enllac) {
    while (enllac != NULL) {
        Enllacos *temp = enllac;
        enllac = enllac->seguent;
        if (temp->textEnllac) free(temp->textEnllac);
        if (temp->titolEnllac) free(temp->titolEnllac);
        free(temp);
    }
}

// Allibera un document i els seus enllaços
void alliberaDocument(Document *document) {
    if (document == NULL) return;
    free(document->titol);
    free(document->cos);
    alliberaEnllacos(document->enllacos);
    free(document);
}

// Afegeix un enllaç a una llista d'enllaços
void afegeixEnllac(Enllacos **cap, int idDocumentDesti, const char *textEnllac) {
    Enllacos *nouEnllac = (Enllacos *)malloc(sizeof(Enllacos));
    if (nouEnllac == NULL) {
        perror("Error en assignar memòria per a nou enllaç");
        exit(EXIT_FAILURE);
    }
    nouEnllac->idDocumentDesti = idDocumentDesti;
    nouEnllac->textEnllac = strdup(textEnllac);
    if (nouEnllac->textEnllac == NULL) {
        perror("Error en duplicar el text de l'enllaç");
        free(nouEnllac);
        exit(EXIT_FAILURE);
    }
    nouEnllac->titolEnllac = NULL;
    nouEnllac->seguent = NULL;

    if (*cap == NULL) {
        *cap = nouEnllac;
    } else {
        Enllacos *actual = *cap;
        while (actual->seguent != NULL) {
            actual = actual->seguent;
        }
        actual->seguent = nouEnllac;
    }
}

// Deserialitza un document a partir del fitxer
Document *deserialitzaDocument(char *camins) {
    FILE *file = fopen(camins, "r");
    if (!file) {
        perror("No s'ha pogut obrir el fitxer del document");
        return NULL;
    }

    Document *document = (Document *)malloc(sizeof(Document));
    if (!document) {
        perror("Error en assignar memòria per al document");
        fclose(file);
        return NULL;
    }
    memset(document, 0, sizeof(Document));

    char buffer[2048];

    // ID
    if (fgets(buffer, sizeof(buffer), file) == NULL || sscanf(buffer, "%d", &document->id) != 1) {
        fprintf(stderr, "Error llegint ID del document en %s\n", camins);
        alliberaDocument(document);
        fclose(file);
        return NULL;
    }

    // Títol
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        fprintf(stderr, "Error llegint títol del document en %s\n", camins);
        alliberaDocument(document);
        fclose(file);
        return NULL;
    }
    char *title_prefix_str = "Title: ";
    if (strncmp(buffer, title_prefix_str, strlen(title_prefix_str)) == 0)
        document->titol = strdup(buffer + strlen(title_prefix_str));
    else
        document->titol = strdup(buffer);
    document->titol[strcspn(document->titol, "\n")] = 0;

    // Cos
    char *cos_acumulat = NULL;
    size_t cos_len = 0;
    char line[2048];
    long current_pos;
    while ((current_pos = ftell(file)) != -1 && fgets(line, sizeof(line), file) != NULL) {
        if (strcmp(line, "Links:\n") == 0) {
            fseek(file, current_pos, SEEK_SET);
            break;
        }
        size_t len = strlen(line);
        cos_acumulat = realloc(cos_acumulat, cos_len + len + 1);
        strcpy(cos_acumulat + cos_len, line);
        cos_len += len;
    }
    if (cos_acumulat) {
        while (cos_len > 0 && (cos_acumulat[cos_len - 1] == '\n' || cos_acumulat[cos_len - 1] == ' '))
            cos_acumulat[--cos_len] = '\0';
        document->cos = cos_acumulat;
    }

    // Enllaços
    document->enllacos = NULL;
    if (fgets(buffer, sizeof(buffer), file) != NULL && strcmp(buffer, "Links:\n") == 0) {
        while (fgets(buffer, sizeof(buffer), file)) {
            int idDesti;
            char text[1024];
            if (sscanf(buffer, "%d|%1023[^\n]", &idDesti, text) == 2) {
                afegeixEnllac(&document->enllacos, idDesti, text);
            }
        }
    }

    fclose(file);
    return document;
}

// Carrega tots els documents d'un directori
Document *carregaTotsElsDocuments(const char *rutaDirectori) {
    DIR *dir;
    struct dirent *ent;
    Document *documents = NULL;
    int documents_carregats_count = 0;

    if ((dir = opendir(rutaDirectori)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                continue;
            }

            char camiComplet[1024];
            snprintf(camiComplet, sizeof(camiComplet), "%s/%s", rutaDirectori, ent->d_name);

            Document *document = deserialitzaDocument(camiComplet);
            if (document != NULL) {
                document->seguent = documents;
                documents = document;
                documents_carregats_count++;
            } else {
                fprintf(stderr, "No s'ha pogut carregar el document: %s\n", ent->d_name);
            }
        }
        closedir(dir);
    } else {
        perror("No s'ha pogut obrir el directori");
        return NULL;
    }

    return documents;
}
