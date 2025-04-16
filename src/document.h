//LAB 1 a)
typedef struct Links{
    int documentId; // ID del document de destinació
    char* title;  
    char* linkText; // text de l'enllaç
    struct Links* next; // punter al següent enllaç
  }Links;

  //estructura de document

typedef struct Document {
  int id; // ID del document
  char* title; // títol del document
  char* body; // cos del document
  Links* links; // llista d'enllaços
} Document;

Document* document_desserialize(char* path);    // declarar de la funció document_desserialize
void freeDocument(Document* document); // declarar de freeDocument
void freeLinks(Links* link); // declarar de freeLinks


