#include "document.h" 
#include <stdio.h>   
#include <stdlib.h>  
#include <string.h>   
#include <assert.h>
#include <dirent.h>   

// Implementació de les funcions declarades a document.h

// inicialitzar llista de enllaços
// Funció que inicialitza una llista d'enllaços buida.
Enllacos *iniciaEnllacos() {
    return NULL; // Retorna NULL, indicant que la llista d'enllaços està buida.
}

// allibera la memòria d'una llista de links
// Funció que allibera tota la memòria ocupada per una llista enllaçada d'estructures Enllacos.
void alliberaEnllacos(Enllacos *enllac) {
    while (enllac != NULL) { // Bucle que recorre cada element de la llista mentre no sigui NULL.
        Enllacos *temp = enllac; // Guarda el punter a l'enllaç actual per poder alliberar-lo després.
        enllac = enllac->seguent; // Avança al següent enllaç de la llista.
        if (temp->textEnllac) free(temp->textEnllac); // Si el text de l'enllaç existeix, l'allibera.
        if (temp->titolEnllac) free(temp->titolEnllac); // Assegurem l'alliberament del títol de l'enllaç si s'ha assignat memòria.
        free(temp); // Allibera la memòria de l'estructura Enllacos actual.
    }
}

// allibera la memòria d'un document
// Funció que allibera tota la memòria ocupada per una estructura Document i els seus components.
void alliberaDocument(Document *document) {
    if (document == NULL) return; // Si el punter al document és NULL, no fa res i surt de la funció.
    free(document->titol); // Allibera la memòria assignada per al títol del document.
    free(document->cos); // Allibera la memòria assignada per al cos del document.
    alliberaEnllacos(document->enllacos); // Crida a la funció per alliberar la llista d'enllaços associada al document.
    free(document); // Allibera la memòria de l'estructura Document pròpiament dita.
}

// afegir enllaç a la llista
// Funció que afegeix un nou enllaç a una llista d'enllaços existent.
void afegeixEnllac(Enllacos **cap, int idDocumentDesti, const char *textEnllac) { // Usar const char* per seguretat (indica que el contingut no serà modificat).
    Enllacos *nouEnllac = (Enllacos *)malloc(sizeof(Enllacos)); // Assigna memòria per a la nova estructura Enllacos.
    if (nouEnllac == NULL) { // Comprova si l'assignació de memòria ha fallat.
        perror("Error en assignar memòria per a nou enllaç"); // Imprimeix un missatge d'error al stderr.
        // En un programa real, potser voldríem gestionar l'error d'una altra manera que sortir.
        exit(EXIT_FAILURE); // Surt del programa amb un codi d'error.
    }
    nouEnllac->idDocumentDesti = idDocumentDesti; // Assigna l'ID del document de destinació al nou enllaç.
    nouEnllac->textEnllac = strdup(textEnllac); // Duplica el text de l'enllaç i l'assigna al nou enllaç.
    if (nouEnllac->textEnllac == NULL) { // Comprova si la duplicació de la cadena ha fallat.
        perror("Error en duplicar el text de l'enllaç"); // Imprimeix un missatge d'error.
        free(nouEnllac); // Allibera la memòria del nou node ja que el text no es va poder duplicar.
        exit(EXIT_FAILURE); // Surt del programa amb un codi d'error.
    }
    nouEnllac->titolEnllac = NULL; // Per defecte, no hi ha títol d'enllaç al dataset actual, s'inicialitza a NULL.
    nouEnllac->seguent = NULL; // El nou enllaç és l'últim de moment, així que el seu punter 'seguent' és NULL.

    if (*cap == NULL) { // Si la llista està buida (el cap és NULL).
        *cap = nouEnllac; // El nou enllaç es converteix en el primer i únic element de la llista.
    } else { // Si la llista no està buida.
        Enllacos *actual = *cap; // Crea un punter 'actual' i el posiciona al principi de la llista.
        while (actual->seguent != NULL) { // Recorre la llista fins a arribar a l'últim element.
            actual = actual->seguent; // Avança al següent element.
        }
        actual->seguent = nouEnllac; // Afegeix el nou enllaç al final de la llista.
    }
}


// Funció per deserialitzar un document d'un fitxer
// Funció que llegeix un fitxer de text i construeix una estructura Document amb la informació.
Document *deserialitzaDocument(char *camins) {
    FILE *file = fopen(camins, "r"); // Obre el fitxer especificat pel 'camins' en mode lectura ("r").
    if (!file) { // Comprova si l'obertura del fitxer ha fallat.
        perror("No s'ha pogut obrir el fitxer del document"); // Imprimeix un missatge d'error.
        return NULL; // Retorna NULL indicant un error.
    }

    Document *document = (Document *)malloc(sizeof(Document)); // Assigna memòria per a la nova estructura Document.
    if (!document) { // Comprova si l'assignació de memòria ha fallat.
        perror("Error en assignar memòria per al document"); // Imprimeix un missatge d'error.
        fclose(file); // Tanca el fitxer.
        return NULL; // Retorna NULL indicant un error.
    }
    memset(document, 0, sizeof(Document)); // Inicialitza tots els bytes de l'estructura Document a 0 per seguretat (important per a punters).

    char buffer[2048]; // Declara un buffer per llegir línies completes del fitxer, amb una mida generosa.

    // --- Lectura del ID ---
    // S'espera que la primera línia contingui només el número de l'ID seguit d'un salt de línia.
    if (fgets(buffer, sizeof(buffer), file) == NULL) { // Llegeix la primera línia del fitxer al buffer.
        fprintf(stderr, "Error llegint ID del document (fgets fallida o EOF) en %s\n", camins); // Imprimeix un error si no es pot llegir la línia.
        alliberaDocument(document); // Allibera la memòria del document parcialment creat.
        fclose(file); // Tanca el fitxer.
        return NULL; // Retorna NULL.
    }
    // Intentem llegir l'enter directament del buffer
    if (sscanf(buffer, "%d", &document->id) != 1) { // Intenta extreure un enter del buffer i assignar-lo a document->id.
        fprintf(stderr, "Error llegint ID del document (format incorrecte) en %s. Contingut de la línia: '%s'\n", camins, buffer); // Imprimeix un error si el format no coincideix.
        alliberaDocument(document); // Allibera la memòria.
        fclose(file); // Tanca el fitxer.
        return NULL; // Retorna NULL.
    }

    // --- Lectura del Títol ---
    // S'espera que la segona línia contingui "Title: " seguit del títol.
    if (fgets(buffer, sizeof(buffer), file) == NULL) { // Llegeix la segona línia del fitxer (presumiblement el títol).
        fprintf(stderr, "Error llegint títol del document (fgets fallida o EOF) en %s\n", camins); // Imprimeix un error si no es pot llegir.
        alliberaDocument(document); // Allibera la memòria.
        fclose(file); // Tanca el fitxer.
        return NULL; // Retorna NULL.
    }
    char *title_prefix_str = "Title: "; // Defineix la cadena de prefix esperada per al títol.
    if (strncmp(buffer, title_prefix_str, strlen(title_prefix_str)) == 0) { // Comprova si la línia comença amb "Title: ".
        document->titol = strdup(buffer + strlen(title_prefix_str)); // Si té el prefix, duplica la resta de la cadena (el títol real).
    } else {
        // Si no comença amb "Title: ", assumim que tota la línia és el títol.
        // Aquesta és una solució més flexible davant formats lleugerament diferents.
        document->titol = strdup(buffer); // Si no hi ha prefix, duplica tota la línia com a títol.
    }
    if (document->titol == NULL) { // Comprova si la duplicació de la cadena del títol ha fallat.
        perror("Error duplicant el títol"); // Imprimeix un error.
        alliberaDocument(document); // Allibera la memòria.
        fclose(file); // Tanca el fitxer.
        return NULL; // Retorna NULL.
    }
    // Elimina el salt de línia final del títol
    document->titol[strcspn(document->titol, "\n")] = 0; // Troba el primer salt de línia al títol i el reemplaça per un terminador nul.


    // --- Lectura del cos (body) ---
    // Llegim línia a línia fins a trobar la línia "Links:" o el final del fitxer.
    char *cos_acumulat = NULL; // Punter per emmagatzemar el contingut del cos acumulat dinàmicament.
    size_t cos_len = 0; // Mida actual del cos acumulat.
    char line[2048]; // Buffer per a cada línia individual llegida del cos.
    long current_pos_before_line; // Variable per guardar la posició actual del fitxer abans de llegir una línia.

    while ( (current_pos_before_line = ftell(file)) != -1 && fgets(line, sizeof(line), file) != NULL) { // Bucle que llegeix línies del fitxer fins al final o fins a trobar "Links:".
        // Si la línia és "Links:\n" (o només "Links:" si el '\n' és eliminat abans de la comparació)
        // hem arribat a la secció d'enllaços. Reculem i sortim del bucle del cos.
        if (strcmp(line, "Links:\n") == 0) { // Comprova si la línia llegida és exactament "Links:" seguida d'un salt de línia.
            fseek(file, current_pos_before_line, SEEK_SET); // Si és "Links:", recula el punter del fitxer a la posició abans de llegir aquesta línia.
            break; // Surt del bucle de lectura del cos.
        }

        // Si no és la línia "Links:", és part del cos.
        size_t current_line_len = strlen(line); // Obté la longitud de la línia actual.

        // Reassignar memòria per al cos, incloent el nou text i un null terminator
        cos_acumulat = (char*) realloc(cos_acumulat, cos_len + current_line_len + 1); // Reassigna memòria per al cos acumulat per incloure la nova línia.
        if (!cos_acumulat) { // Comprova si la reassignació de memòria ha fallat.
            perror("Error reassignant memòria per al cos del document"); // Imprimeix un error.
            alliberaDocument(document); // Allibera la memòria.
            fclose(file); // Tanca el fitxer.
            // Si cos_acumulat ja no és NULL, allibera'l abans de sortir.
            if (cos_acumulat) free(cos_acumulat); // Assegura que la memòria prèviament assignada al cos s'allibera.
            return NULL; // Retorna NULL.
        }
        // Copiar la línia al final del cos acumulat
        strcpy(cos_acumulat + cos_len, line); // Copia la línia actual al final de la cadena del cos acumulat.
        cos_len += current_line_len; // Actualitza la longitud total del cos.
    }

    // Un cop acabat el bucle, processar el cos acumulat
    if (cos_acumulat) { // Si s'ha acumulat contingut per al cos.
        // Eliminar qualsevol salt de línia o espai en blanc al final del cos
        while (cos_len > 0 && (cos_acumulat[cos_len - 1] == '\n' || cos_acumulat[cos_len - 1] == ' ')) { // Bucle per eliminar salts de línia o espais finals.
            cos_acumulat[--cos_len] = '\0'; // Reemplaça el caràcter final amb un terminador nul i decrementa la longitud.
        }
        cos_acumulat[cos_len] = '\0'; // Assegurar el terminador nul final (per si el bucle anterior no va fer cap canvi).
    } else {
        // Si no hi ha cos, assignar una cadena buida per evitar NULL (o un error)
        cos_acumulat = strdup(""); // Si el cos està buit, assigna una cadena buida.
        if (cos_acumulat == NULL) { // Comprova si la duplicació de la cadena buida ha fallat (poc probable).
             perror("Error assignant cadena buida per al cos"); // Imprimeix un error.
             alliberaDocument(document); // Allibera la memòria.
             fclose(file); // Tanca el fitxer.
             return NULL; // Retorna NULL.
        }
    }
    document->cos = cos_acumulat; // Assigna la cadena del cos acumulat al camp 'cos' del document.


    // --- Lectura de la línia "Links:" i els enllaços ---
    // Intentem llegir la línia "Links:". Si el bucle del cos va trencar per ella, ara la llegirem.
    if (fgets(buffer, sizeof(buffer), file) == NULL || strcmp(buffer, "Links:\n") != 0) { // Llegeix la línia i comprova si és "Links:\n".
        // Si no es llegeix "Links:\n" (o s'arriba al final del fitxer), assumim que no hi ha secció d'enllaços.
        document->enllacos = NULL; // Si no es troba la capçalera "Links:", la llista d'enllaços es deixa a NULL.
    } else {
        // Si hem trobat "Links:\n", procedim a llegir els enllaços un per un.
        Enllacos *enllacos = iniciaEnllacos(); // Inicialitza una nova llista d'enllaços.
        // Llegeix enllaços fins que la línia sigui buida o arribi al final del fitxer
        while (fgets(line, sizeof(line), file) != NULL && strlen(line) > 1) { // Llegeix línies mentre no s'arribi al final i la línia no estigui buida.
            int idEnllac; // Variable per a l'ID de l'enllaç de destinació.
            char textEnllac[512]; // Buffer per al text de l'enllaç.

            // Intentem parsejar la línia de l'enllaç
            if (sscanf(line, "Link: %d, Text: %[^\n]", &idEnllac, textEnllac) == 2) { // Intenta extreure l'ID i el text de l'enllaç de la línia.
                afegeixEnllac(&enllacos, idEnllac, textEnllac); // Si s'extreuen correctament, afegeix l'enllaç a la llista.
            } else {
                // Si la línia no coincideix amb el format esperat, imprimim un avís.
                fprintf(stderr, "Avís: Format d'enllaç inesperat en %s: '%s' (ignorat)\n", camins, line); // Imprimeix un avís d'error de format.
            }
        }
        document->enllacos = enllacos; // Assigna la llista d'enllaços creada al camp 'enllacos' del document.
    }

    fclose(file); // Tanca el fitxer.
    return document; // Retorna el punter al document deserialitzat.
}


// Carrega tots els documents d'un directori donat
// Funció que carrega tots els documents presents en un directori i els retorna com una llista enllaçada.
Document *carregaTotsElsDocuments(const char *rutaDirectori) {
    DIR *dir; // Punter a l'estructura de directori.
    struct dirent *ent; // Punter a l'estructura d'entrada del directori (per cada fitxer/subdirectori).
    Document *documents = NULL; // Llista enllaçada dels documents carregats, inicialment buida.
    int documents_carregats_count = 0; // Comptador de documents carregats amb èxit.

    // Intentar obrir el directori
    if ((dir = opendir(rutaDirectori)) != NULL) { // Intenta obrir el directori especificat per 'rutaDirectori'.
        printf("Carregant documents des de: %s\n", rutaDirectori); // Informa de quin directori s'estan carregant els documents.
        // Recórrer cada entrada al directori
        while ((ent = readdir(dir)) != NULL) { // Llegeix cada entrada del directori fins que no queden més.
            // Ignorar les entrades de directori actual i parent (".", "..")
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) { // Si l'entrada és "." o "..", la ignora.
                continue; // Passa a la següent entrada.
            }

            // Construir el camí complet al fitxer
            char camiComplet[1024]; // Buffer per emmagatzemar el camí complet del fitxer.
            snprintf(camiComplet, sizeof(camiComplet), "%s/%s", rutaDirectori, ent->d_name); // Construeix el camí complet (e.g., "datasets/wikipedia12/0.txt").

            // Deserialitzar el document
            Document *document = deserialitzaDocument(camiComplet); // Crida a deserialitzaDocument per carregar el document actual.
            if (document != NULL) { // Si el document s'ha carregat amb èxit (no és NULL).
                // Afegir el document al principi de la llista enllaçada
                document->seguent = documents; // El punter 'seguent' del nou document apunta al que fins ara era el primer de la llista.
                documents = document; // El nou document es converteix en el cap de la llista.
                documents_carregats_count++; // Incrementa el comptador de documents carregats.
            } else {
                // Si deserialitzaDocument retorna NULL, vol dir que hi ha hagut un error
                fprintf(stderr, "No s'ha pogut carregar el document: %s\n", ent->d_name); // Imprimeix un missatge d'error si el document no es pot carregar.
            }
        }
        closedir(dir); // Tanca el directori un cop finalitzada la lectura.
        printf("Documents carregats amb èxit. Total: %d\n", documents_carregats_count); // Informa del nombre total de documents carregats.
    } else {
        // Error en obrir el directori
        perror("No s'ha pogut obrir el directori"); // Imprimeix un error si no s'ha pogut obrir el directori.
        return NULL; // Retorna NULL indicant un error.
    }
    return documents; // Retorna el punter al primer document de la llista enllaçada (el cap).
}