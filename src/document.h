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
  char* title; // Títol del document
  char* body; // Cos del document
  Links* links; // Llista d'enllaços
} Document;

Document* document_desserialize(char* path);    // Declarar de la funció

