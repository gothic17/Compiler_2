#ifndef MAIN_H
#define MAIN_H

/****** POMOCNICZE ZMIENNE ********/
int new_address_counter; // Pozwala na tworzenie nowych adresów to tablicy symboli (np. t1, t2...)
int new_label_counter; // Pozwala na tworzenie nowych etykiet do kodu trójadresowego (np. L0 ...)

/****** DRZEWO AST ****************/

// Struktura węzła w drzewie AST
typedef struct node {
	char *string;
	char *type;
	int first_line;
	int last_line;
	int first_column;
	int last_column;

	int number_of_children;
	struct node *children[];
} node;

// Korzen drzewa AST
static node *AST_root;

/******* LISTA BŁĘDÓW *******/

// Struktura błędu
typedef struct error {
	char *string;
	int first_line;
	int last_line;
	int first_column;
	int last_column;
} error;

typedef struct errors_list_element{
   error *data;
   struct errors_list_element *next;

} errors_list;

/******* Korzen listy błędów *******/
static errors_list *errors_list_root;


/****** LISTA SYMBOLI *************/

/*	Struktura symbolu, który będzie przechowywany w liście symboli.
 * 	address - miejsce w pamięci, w którym znajduje się identyfikator,
 * 	string - nazwa identyfikatora.
 */
typedef struct symbol {
	char *string;
	char *type;
	int address;
	int size;

	int first_line;
	int first_column;
	int last_line;
	int last_column;

	struct symbol *next;
} symbol;


typedef struct symbols_list_element {
	symbol *data;
	struct symbols_list_element *next;

} symbols_list;

/******** Korzeń listy symboli *******/
static symbols_list *symbols_list_root;


/************* CZWÓRKI ***************/


/*	Struktura jednej czwórki, która zostanie użyta w tablicy czwórek.
 * 	operator - operator w działaniu,
 * 	arg1, arg2, result - nazwa zmiennej w tablicy symboli, w której będzie się znajdował argument/wynik.
*/
typedef struct four {
	char *operator1;
	char *arg1;
	char *operator2;
	char *arg2;
	char *result;

	struct four *next;
} four;

/******** Korzeń listy czwórek ********/
static four *fours_root;


node *add_node(int number_of_children, char *string, node *children[]);

void add_child(char *operation, node *parent, node *child);

void free_node(node *p);

void post_order(node *p);

error *create_error(char *string, int first_line, int first_column, int last_line, int last_column);

void add_error(errors_list *current, error *x);

void remove_error(errors_list *root, error *x);

errors_list *find_error(errors_list *root, error *x);

void add_four(char *operator1, char *arg1, char *operator2, char *arg2, char *result);

typedef struct assembler_instruction {
	int row;
	char order[6];
	char arg1[3];
	char arg2[3];

} assembler_instruction;

#endif
