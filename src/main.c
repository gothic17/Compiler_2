#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "main.h"
#include "translate.c"
#include "compiler.tab.h"

/* Zmienne globalne */
extern FILE *yyin;

/* Funkcje */
int yyparse(error *errors_list_root);

/********** DRZEWO AST **********/

node *add_node(int number_of_children, char *string, node *children[]) {
	node *p = malloc(sizeof(node) + number_of_children * sizeof(node));

	p->string = strdup(string);
	p->number_of_children = number_of_children;

	int i;

	for (i = 0; i < number_of_children; i++) p->children[i] = children[i];

	p->first_line = yylloc.first_line;
	p->first_column = yylloc.first_column;
	p->last_line = yylloc.last_line;
	p->last_column = yylloc.last_column;

	return p;
}

void add_child(char *operation, node *parent, node *child) {
		/* Dodane ze wzgledu na przesunięcia, jakie pojawiały się przy dodawaniu wielu linii kodu
		 * (np. COMMANDS zamieniało się w krzaki.*/
		parent->string = operation;

		if ((parent->children[parent->number_of_children] = malloc(sizeof(child))) == NULL) yyerror("Brak pamięci");

		parent->children[parent->number_of_children] = child;

		parent->number_of_children++;
}

void free_node(node *p) {
	if(!p) return;

	int i = 0;
	while (i < p->number_of_children) {
				free_node(p->children[i]);
				i++;
	}
	free(p);
}

void post_order (node *p) {
	if (strcmp(p->string, "DECLARE") == 0 && AST_root == NULL) AST_root = p;
	int i = 0;
	if (p->number_of_children != 0) {
		while (i < p->number_of_children) {
			post_order(p->children[i]);
			i++;
		}
		printf("%s\n", p->string);
		//interpret(p);
	}
	else {
		printf("%s\n", p->string);
		//interpret(p);
	}
}

/******** LISTA BŁĘDÓW *******************/

error *create_error(char *string, int first_line, int first_column, int last_line, int last_column) {
	error *new = malloc(sizeof(error));
	new->string = string;
	new->first_line = first_line;
	new->first_column = first_column;
	new->last_line = last_line;
	new->last_column = last_column;

	new->next = NULL;

	return new;
}

/* add_error wstawia element x do listy błędów za elementem, na który wskazuje wskaźnik current*/
void add_error(error *new_error) {
	if(errors_list_root == NULL) {
		errors_list_root = malloc(sizeof(error));
		errors_list_root = new_error;
		errors_list_root->next = NULL;
	}
	else {
		error *current = errors_list_root;
		while(current->next != NULL) {
			current = current->next;
		}

		new_error->next = current->next;
		current->next = new_error;
	}
}
/* remove_error - usuwa z listy błędów (zaczynającej się od korzenia root) błąd x */
void remove_error(error *x) {
   error *temp;
   error *previous;
   temp = errors_list_root;
   while(temp != NULL && temp != x) {
	   previous = temp;
	   temp = temp->next;
   }
   if(temp == errors_list_root) errors_list_root = errors_list_root->next;
   else {
	   previous->next = temp->next;
   }
   if (temp != NULL) free(temp);
}

/* find_error szuka elementu x w liście błędów, zwraca jego adres albo null, gdy lista nie zawiera
 * elementu x.*/
error *find_error(error *x) {
	error *temp = errors_list_root;
	while (temp != NULL && temp != x) {
		temp = temp->next;
	}
	return temp;
}

void print_errors() {
	error *temp = errors_list_root;
	while(temp != NULL) {
		if (strcmp(temp->string, "ROOT_ERROR") != 0) {
			printf("Error: %d.%d - %d.%d - %s\n", temp->first_line, temp->first_column, temp->last_line, temp->last_column, temp->string);
		}
		temp = temp->next;
	}
}


/********* LISTA SYMBOLI **************/

symbol *create_symbol(char *string, char *type, int address, int size, int first_line,
		int first_column, int last_line, int last_column) {

	symbol *new_symbol = malloc(sizeof(symbol));
	new_symbol->string = string;
	new_symbol->type = type;
	new_symbol->address = address;
	new_symbol->size = size;

	new_symbol->first_line = first_line;
	new_symbol->first_column = first_column;
	new_symbol->last_line = last_line;
	new_symbol->last_column = last_column;

	return new_symbol;
}

void add_symbol(symbol *symbol) {
	symbols_list *temp_sublist = symbols_list_root;

	while(temp_sublist->next != NULL) {
		temp_sublist = temp_sublist->next;
	}

	struct symbol *temp_symbol = temp_sublist->data;

	while(temp_symbol->next != NULL) {
		temp_symbol = temp_symbol->next;
	}

	symbol->next = temp_symbol->next;
	temp_symbol->next = symbol;
}


void add_symbols_sublist() {
	symbols_list *new_list = malloc(sizeof(symbols_list));
	symbol *new_symbol = create_symbol("ROOT_SYMBOL", "-", 0,0,0,0,0,0); //Inicjowanie nowej listy symboli symbolem "ROOT_SYMBOL"
	new_list->data = new_symbol;

	symbols_list *temp = symbols_list_root;

	if(symbols_list_root == NULL) {
		symbols_list_root = malloc(sizeof(symbols_list));
		symbols_list_root->next = NULL;
		symbols_list_root->data = new_symbol;
	}
	else {
		while (temp->next != NULL) {
			temp = temp->next;
		}
		new_list->next = temp->next;
		temp->next = new_list;
	}

	//sublists_pointer++;
}

/* remove_error - usuwa z listy błędów (zaczynającej się od korzenia root) błąd x */
void remove_symbols_sublist(symbols_list *x) {
	symbols_list *temp;
	symbols_list *previous;
	temp = symbols_list_root;
	while(temp != NULL && temp != x) {
		previous = temp;
		temp = temp->next;
	}
	if(temp == symbols_list_root) symbols_list_root = symbols_list_root->next;
	else {
		previous->next = temp->next;
	}
	if (temp != NULL) free(temp);
}


/* search_for_symbol - zwraca 1, jesli symbol został znaleziony i 0 w p.p. */
int search_for_symbol(char *symbol_name) {
	symbols_list *temp = symbols_list_root;

	while (temp != NULL) {
		struct symbol *temp_symbol = temp->data;
		while(temp_symbol != NULL) {
			if (strcmp(temp_symbol->string, symbol_name) == 0) return 1;
			temp_symbol = temp_symbol->next;
		}
		temp = temp->next;
	}

	return 0;
}

void print_symbols() {
	symbols_list *symbols_sublist = symbols_list_root;

	while(symbols_sublist != NULL) {
		symbol *temp_symbol = symbols_sublist->data;
		while(temp_symbol != NULL) {
			if(strcmp(temp_symbol->string, "ROOT_SYMBOL") != 0) {
				printf("%*s %*d %*d\n", 5, temp_symbol->string, 5, temp_symbol->address,
						5, temp_symbol->size);
			}
			temp_symbol = temp_symbol->next;
		}
		symbols_sublist = symbols_sublist->next;
	}
}

/********* KOD TRÓJADRESOWY ***************/

void add_four(char *operator1, char *arg1, char *operator2, char *arg2, char *result) {
	four *new_four = malloc(sizeof(four));
	new_four->operator1 = operator1;
	new_four->arg1 = arg1;
	new_four->operator2 = operator2;
	new_four->arg2 = arg2;
	new_four->result = result;

	four *current = fours_root;
	while(current->next != NULL) {
		current = current->next;
	}

	new_four->next = current->next;
	current->next = new_four;
}

// void remove_four [DO ZROBIENIA]

void print_fours() {
	four *temp = fours_root;

	while(temp != NULL) {
		if(strcmp(temp->operator1, "ROOT_FOUR") != 0) {
			printf("%*s %*s %*s %*s %*s\n", 5, temp->operator1, 3, temp->arg1, 4,
					temp->operator2, 3, temp->arg2, 3, temp->result);
		}
		temp = temp->next;
	}
}

/************** MAIN *********************/

void main(int argc, char **argv) {

	/******** Korzeń listy błędów **********/
	errors_list_root = NULL;
	/*error *error0 = create_error("ROOT_ERROR", 0, 0, 0, 0);
	errors_list_root = malloc(sizeof(errors_list));
	errors_list_root->next = NULL;
	errors_list_root->data = error0;*/


	/******* Korzen listy symboli *********/
	symbols_list_root = NULL;


	/****** Korzeń drzewa AST *************/
	AST_root = malloc(sizeof(node));
	AST_root = NULL;


	/***** Korzeń listy czwórek ***********/
	fours_root = malloc(sizeof(four));
	fours_root->operator1 = "ROOT_FOUR";
	fours_root->next = NULL;


	/***** Czytanie z pliku i parser******/
	char temp[4] = {argv[1][strlen(argv[1])-4], argv[1][strlen(argv[1])-3], argv[1][strlen(argv[1])-2], argv[1][strlen(argv[1])-1]};

	FILE *file = fopen(argv[1], "r");

	if (file == NULL) {
		printf("Nie udalo sie otworzyc pliku %s.", argv[1]);
	    exit(0);
	}
	else if (strcmp(temp, ".imp") != 0) {
		printf("Plik %s me złe rozszerzenie.\n", argv[1]);
		exit(0);
	}

	yyin = file;

	do {
		yyparse(errors_list_root);
	} while(!feof(yyin));

	fclose(file);

	/******* Zmienne do translacji ******/
	new_address_counter = 0;
	new_label_counter = 0;
	address_counter = 0;
	/***********************************/

	translate(AST_root);

	printf("-----Tablica symboli-------\n");
	print_symbols();

	printf("-----Kod trójadresowy------\n");
	print_fours();

	printf("---------Błędy-------------\n");
	print_errors();

}