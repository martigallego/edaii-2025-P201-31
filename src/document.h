//LAB 1 a)
typedef struct{
    int documentId; // ID del document de destinació
    char* title;
    char* linkText; // text de l'enllaç
    struct Link* next; // punter al següent enllaç
  }Link;
  