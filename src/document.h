//LAB 1 a)
typedef struct Link{
    int documentId; // ID del document de destinació
    char* title;
    char* linkText; // text de l'enllaç
    struct Link* next; // punter al següent enllaç
  }Link;

  //estructura de document

typedef struct Document {
  int id; // ID del document
  char* title; // Títol del document
  char* body; // Cos del document
  Link* links; // Llista d'enllaços
} Document;

Document* document_desserialize(char* path);    // Declarar de la funció

