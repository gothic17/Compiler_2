%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "main.h"

typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;

#define YYLTYPE_IS_DECLARED 1

extern int yylex();
extern int yyparse();
extern FILE* yyin;

void yyerror(error *errors_list_root, char *s, ...);
void lyyerror(YYLTYPE t, error *errors_list_root, int n, char *s, ...);

%}

%parse-param {error *errors_list_root}


%union {
	struct node *node_pointer;
};


/********* Bison declarations. *********/
%locations

%start program

%type <node_pointer> program vdeclarations
%type <node_pointer> commands command expression condition
%type <node_pointer> num value identifier pidentifier

%token '\n' QUIT '(' ')'

%token DECLARE IN END

%token ';'
%token IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE
%token FOR DOWN FROM TO ENDFOR
%token GET PUT

%token num pidentifier

%left '+' '-'
%left '*' '/' '%'

%left '=' '<' '>'
%left ASSIGN DIFFERENT SMALLEREQUAL BIGGEREQUAL

%nonassoc IF_ELSE
%nonassoc FOR_DOWN

%% /**********The grammar follows. ****************/ 

program:
	DECLARE vdeclarations IN commands END { struct node *children[2] = {$2, $4};
											$$ = add_node(2, "DECLARE", children);
										    post_order($$); 
										    //free_node($$); 
										    }	
|	%empty				{}
;

vdeclarations:
	vdeclarations pidentifier			{ add_child("VDEC", $1, $2); }
| 	vdeclarations pidentifier'('num')'	{ struct node *children[2] = {$2, $4};
										  add_child("VDEC",$1, add_node(2, "VDEC_TABLE", children));
										}
| 	%empty								{ struct node *children[0];
										  $$ = add_node(0, "VDEC", children); 				    }
;

commands:
	commands command	{ add_child("COMMANDS",$1, $2); }
|	%empty				{ struct node *children[0];
						  $$ = add_node(0, "COMMANDS", children);	}
;

command:
	identifier ASSIGN expression ';'								{ struct node *children[2] = {$1, $3};
																	  $$ = add_node(2, ":=", children);	}	  
																	  //$$ = add_node_2(":=", $1, $3);}
|	IF condition THEN commands ENDIF								{ struct node *children[2] = {$2, $4};
																	  $$ = add_node(2, "IF", children); 		  }
| 	IF condition THEN commands ELSE commands ENDIF %prec IF_ELSE	{ struct node *children[3] = {$2, $4, $6};
																	  $$ = add_node(3, "IF_ELSE", children); }
| 	WHILE condition DO commands ENDWHILE							{ struct node *children[2] = {$2, $4};
																	  $$ = add_node(2, "WHILE", children); 	  }
| 	FOR pidentifier FROM value TO value DO commands ENDFOR			{ struct node *children[4] = {$2, $4, $6, $8};
																	  $$ = add_node(4, "FOR", children); }
| 	FOR pidentifier DOWN FROM value TO value DO commands ENDFOR %prec FOR_DOWN { struct node *children[4] = {$2, $5, $7, $9};
																				 $$ = add_node(4, "FOR_DOWN", children); }
| 	GET identifier ';'												{ struct node *children[1] = {$2};
																	  $$ = add_node(1, "GET", children); }
| 	PUT value ';'													{ struct node *children[1] = {$2};
																	  $$ = add_node(1, "PUT", children); }
;

expression:
	value				{ $$ = $1; 						}
| 	value '+' value		{ struct node *children[2] = {$1, $3};
						  $$ = add_node(2, "+", children); }
| 	value '-' value		{ struct node *children[2] = {$1, $3};
						  $$ = add_node(2, "-", children); }
| 	value '*' value		{ struct node *children[2] = {$1, $3};
						  $$ = add_node(2, "*", children); }
| 	value '/' value		{ struct node *children[2] = {$1, $3};
						  $$ = add_node(2, "/", children); }
| 	value '%' value		{ struct node *children[2] = {$1, $3};
						  $$ = add_node(2, "%", children); }
;

condition:
	value '=' value				{ struct node *children[2] = {$1, $3};
								  $$ = add_node(2, "=", children);  }
| 	value DIFFERENT value		{ struct node *children[2] = {$1, $3};
								  $$ = add_node(2, "!=", children); }
| 	value '<' value				{ struct node *children[2] = {$1, $3};
								  $$ = add_node(2, "<", children);  }
| 	value '>' value				{ struct node *children[2] = {$1, $3};
								  $$ = add_node(2, ">", children);  }
| 	value SMALLEREQUAL value	{ struct node *children[2] = {$1, $3};
								  $$ = add_node(2, "<=", children); }
| 	value BIGGEREQUAL value		{ struct node *children[2] = {$1, $3};
								  $$ = add_node(2, ">=", children); }
;

value:
	num			{$$ = $1;}
| 	identifier	{$$ = $1;}
|	command		{lyyerror(@1, errors_list_root, 3, "Znaleziono command: '", $1->string, "' w miejscu value");}
;

identifier:
	pidentifier					 { $$ = $1;}
| 	pidentifier'('pidentifier')' { struct node *children[2] = {$1, $3};
								   $$ = add_node(2, "TABLE", children); }
| 	pidentifier'('num')'		 { struct node *children[2] = {$1, $3};
								   $$ = add_node(2, "TABLE", children); }
;

%%


void yyerror(error *errors_list_root, char *s, ...) {
  va_list ap;
  va_start(ap, s);

  if(yylloc.first_line)
    fprintf(stderr, "%d.%d-%d.%d: error: ", yylloc.first_line, yylloc.first_column,
	    yylloc.last_line, yylloc.last_column);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

/* n - liczba char* do przekazania. */
void lyyerror(YYLTYPE t, error *errors_list_root, int n, char *s, ...) {
  va_list ap;
  va_start(ap, s);
  int buffer = n;
  char *temp;
  char *string = strdup(s);
  int temp_length = 0;
  while(n > 0) {
	temp_length += strlen(string);
	string = va_arg(ap, char*);
	n--;
  }
  temp = malloc(temp_length * sizeof(char) + 1);
  va_start(ap, s);
  string = strdup(s);
  while(buffer > 0) {
	strcat(temp, string);
	string = va_arg(ap, char*);
	buffer--;
  }
  
  while (errors_list_root->next != NULL) errors_list_root = errors_list_root->next;
  add_error(create_error(temp, t.first_line, t.first_column, t.last_line, t.last_column));

}