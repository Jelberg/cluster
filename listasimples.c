#include <stdio.h>
#include <stdlib.h>


/*
----------------------------------------------

	          MANEJO DE LISTAS SIMPLES

----------------------------------------------

*/

typedef struct{
char valor;
char valor2;
struct lista *prox;
} lista;

lista *crear (char x, char x2){
	lista *aux = malloc(sizeof(lista));
	aux -> valor = x;
	aux -> valor2 = x2;
	aux -> prox = NULL;
	return aux;
};

void insertaCol ( lista **p , char x, char x2){
  lista *t = *p ;
  if ( (*p) == NULL)
  	  *p= crear (x,x2);
  else {
	  while ( t -> prox != NULL)
	  t = t-> prox;
	  t->prox = crear (x,x2);
  };
};

void buscar (lista **p, int x){
	lista *t= *p;
	if (*p){
		while ((t->valor != x) && (t->prox != NULL))
		t= t->prox;
		if (t->valor == x) printf ("El numero %d SE ENCUENTRA\n",x) ;
		else printf ("NO SE ENCUENTRA EL NUMERO\n");
	}; 
};

void muestra(lista *p){
	lista *t = p;
	while (t){
		printf("%c %c-> ",t->valor, t->valor2);
		t = t->prox;
	}
	printf("\n");
}

int compara (char caracter1, char caracter2){
	if (caracter1 > caracter2) return 1;
	else if (caracter1 == caracter2) return -1;
	else return 0;
}

void ordenaLista(lista **p){
	lista *pivote, *sig;
	char nvo, nvo2;
	
	pivote = *p;
	while (pivote){
		sig = pivote->prox; 
	
		//printf("%d\n",pivote->valor);
		
		while(sig){
		
			if (pivote->valor > sig->valor){
				
				nvo = sig->valor;
				sig->valor=pivote->valor;
				pivote->valor=nvo;

				nvo2 = sig->valor2;
				sig->valor2=pivote->valor2;
				pivote->valor2=nvo2;
				
				muestra(*p);
			}
			else 
				if(pivote->valor == sig->valor){
					if (pivote->valor2 > sig->valor2){
						nvo2 = sig->valor2;
						sig->valor2=pivote->valor2;
						pivote->valor2=nvo2;
						muestra(*p);
					}
			}
			sig =sig->prox;
		}

		pivote=pivote->prox;
		
	}
}


void main ()
{
 	lista *g = NULL;
 
 	
 	insertaCol(&g,'e','a');
 	insertaCol(&g,'b','b');
 	insertaCol(&g,'a','c');
 	insertaCol(&g,'k','d');
 	insertaCol(&g,'a','b');
 	insertaCol(&g,'f','e');
 	insertaCol(&g,'j','f');
 	//insertaCol(&g, 'c');
    //insertaCol(&g, 'e');

 	muestra(g);
	
	
 	ordenaLista(&g);
 	printf("Lista ordenada\n");
 	muestra(g);
 
 	printf("-----------FIN DEL PROGRAMA -------------\n");
}


