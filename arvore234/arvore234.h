typedef int ItemArv ;

typedef struct nodo *ApNodo ;

typedef struct nodo {
    ItemArv item [ 3 ] ;
    ApNodo ap [ 4 ] ;
    int numItem ;
    ApNodo pai ;
} Nodo ;

typedef ApNodo Arv234 ;