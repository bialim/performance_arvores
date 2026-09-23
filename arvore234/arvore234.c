#include <stdlib.h>
#include <stdio.h>
#include "arvore234.h"


void criaArv234 ( Arv234* ) ;
void freeArv234 ( Arv234 ) ;
void escreveArv234 ( Arv234 ) ;
Arv234 insereArv234 ( ItemArv , Arv234 ) ;
Arv234 buscaArv234 ( ItemArv , Arv234 ) ;
Arv234 removeArv234 ( ItemArv , Arv234 ) ;

int main ( void ) {
    Arv234 a ;
    criaArv234 ( &a ) ;
    escreveArv234 ( a ) ;
    freeArv234 ( a ) ;
    return 0 ;
}

//busca 
Arv234 buscaArv234 ( ItemArv v , Arv234 p ){
    int i;
    if ( p == NULL )
        return NULL;
    i = 0 ;
    while ( i < p->numItem && lt ( p->item [ i ] , v ) )
        i ++ ;
    if ( eq ( v , p->item [ i ] ) )
        return p ;
    return buscaArv234 ( v , p->ap [ i ] );
}

//insere item do base de dados
Arv234 insereArv234( ItemArv v , Arv234 raiz ){
    Arv234 p , pai , esq , dir , novoDir , novoEsq ;
    int i , split ;
    ItemArv splitV , novoSplitV ;

    if ( raiz == NULL ){
        p= criaNodo ( ) ;
        insereItem( v , NULL , NULL , p ) ;
        return p ;
    }
    p= buscaFolha ( v , raiz ) ;

    if ( p == NULL ) // v ja existe 
    return raiz;

    split = insereNoNodo ( v , NULL , NULL , p , &splitV , &esq , &dir ) ;
    p= p->pai ;
    while ( split && p != NULL ){
        split = insereNoNodo ( splitV , esq , dir , p , &novoSplitV , &novoEsq , &novoDir ) ;
        splitV= novoSplitV ;
        esq= novoEsq ; dir= novoDir ;
        p= p->pai ;
    }
    if ( split ){ //split da raiz 
        p= criaNodo ( ) ;
        insereItem ( splitV , esq , dir , p ) ;
        return p;
    }
    return raiz;
}

//insere item no nodo
int insereNoNodo ( ItemArv v , Arv234 esq , Arv234 dir , Arv234 p ,
    ItemArv *splitV , Arv234 *novoEsq , Arv234 *novoDir ){
    if ( p-> numItem < 3 ){
        insereItem ( v , esq , dir , p ) ;
        return 0;
    }
    *splitV= p->item [ 1 ] ;
    *novoEsq= p ;
    p-> numItem= 1;
    if ( lt ( v , *splitV ) )
    insereItem ( v , esq , dir , *novoEsq) ;
    else
    insereItem ( v , esq , dir , *novoDir) ;
    return 1;
}

//insere item em nodo ja existente
int insereItem ( ItemArv v , Arv234 esq , Arv234 dir , Arv234 p ){
    int i ;
    i=p-> numItem ;
    while ( i >0 && lt ( v , p->item [i-1]) ){
        p->item [ i ]= p->item [i-1];
        p->ap [ i +1]= p->ap [ i ] ;
        i--;
    }

    p->item [ i ]= v ;
    p->ap [ i ]= esq ;
    p->ap [ i +1]= dir ;
    p-> numItem++;
    if ( esq != NULL ){
        esq->pai= p ;
        dir->pai= p ;
    }
    return i;
    
}
