#include <stdio.h>
#include <stdlib.h>


typedef struct{
	char valor;
	struct pila *prox;
} pila;

int vacio (pila *p){
	return ( p == NULL);
};

int tope (pila *p){
	if (!vacio(p))return (p->valor);
	else return 0;
};

void apilar(pila **p, char x){
    pila *t = malloc(sizeof(pila));
	t->valor = x;
	t->prox = *p;
	*p= t;
};

/*void desapilar( pila **p){
	if (! vacio (*p)){
		pila *aux = (*p);
		(*p) = (*p) ->prox;
		delete aux; 
	};
};
*/

void muestra(pila *p){
	pila *t = p;
	int i = 0;
	while (t){
		printf("%c  ",t->valor);
		t = t->prox;
	}
	printf("\n");
}

int compara (char caracter1, char caracter2){
	if (caracter1 > caracter2) return 1;
	else if (caracter1 == caracter2) return -1;
	else return 0;
}

void main ()
{
 	pila *g = NULL;
 
 	
 	apilar(&g,'A');
 	apilar(&g,'6');
 
 	muestra(g);

 	//int a = compara('b','a');
 	//printf("Retorna %d \n",a);

 	printf("-----------FIN DEL PROGRAMA -------------\n");
}