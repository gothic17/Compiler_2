#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "main.h"

/* Jeśli prawda, to zwraca wartośc niezerową. Jeśli fałsz, to zwraca 0 */
int is_numeric (const char * s)
{
    if (s == NULL || *s == '\0' || isspace(*s))
      return 0;
    char * p;
    strtod (s, &p);
    return *p == '\0';
}


char *generate_new_address() {
	// Count the number of digits
	int temp = new_address_counter;
	int count = 0;
	while (temp > 0) {
	    count++;
	    temp /= 10;
	}

	char *string = calloc(count + 1, sizeof(char));
	char *number_buffer = malloc(count * sizeof(char) + 1);
	sprintf(number_buffer, "%d", new_address_counter);
	strcat(string, "t");
	strcat(string, number_buffer);
	new_address_counter++;

	return string;
}

char *generate_new_label() {
	// Count the number of digits
	int temp = new_label_counter;
	int count = 0;
	while (temp > 0) {
	    count++;
	    temp /= 10;
	}

	char *string = calloc(count + 1, sizeof(char));
	char *number_buffer = malloc(count * sizeof(char) + 1);
	sprintf(number_buffer, "%d", new_label_counter);
	strcat(string, "L");
	strcat(string, number_buffer);
	new_label_counter++;

	return string;
}

void interpret(node *p) {

	if (strcmp(p->string, ":=") == 0) {
		if(is_numeric(p->children[0]->string) == 0) {
			if(search_for_symbol(p->children[0]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[0]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[0]->string);
				add_error(create_error(string, p->children[0]->first_line, p->children[0]->first_column,
						p->children[0]->last_line, p->children[0]->last_column));
			}
		}
		if(is_numeric(p->children[1]->string) == 0) {
			if(search_for_symbol(p->children[1]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[1]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[1]->string);
				add_error(create_error(string, p->children[1]->first_line, p->children[1]->first_column,
										p->children[1]->last_line, p->children[1]->last_column));
			}
		}

		add_four(":=", p->children[0]->string, "", p->children[1]->string, p->children[0]->string);
	}

	if (strcmp(p->string, "GET") == 0) {
		add_four("GET", p->children[0]->string, "", "", p->children[0]->string);
	}

	if (strcmp(p->string, "PUT") == 0) {
		add_four("PUT", p->children[0]->string, "", "", p->children[0]->string);
	}

	if (strcmp(p->string, "+") == 0) {
		// Sprawdzenie, czy symbole, które nie są liczbami zostały wczesniej zadeklarowane
		if(is_numeric(p->children[0]->string) == 0) {
			if(search_for_symbol(p->children[0]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[0]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[0]->string);
				add_error(create_error(string, p->children[0]->first_line, p->children[0]->first_column,
						p->children[0]->last_line, p->children[0]->last_column));
			}
		}
		if(is_numeric(p->children[1]->string) == 0) {
			if(search_for_symbol(p->children[1]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[1]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[1]->string);
				add_error(create_error(string, p->children[1]->first_line, p->children[1]->first_column,
										p->children[1]->last_line, p->children[1]->last_column));
			}
		}


		char *symbol_name = generate_new_address();
		symbol *new_symbol = create_symbol(symbol_name, "-", address_counter, 1,
				p->first_line, p->first_column, p->last_line, p->last_column);
		address_counter++;
		add_symbol(new_symbol);
		add_four("+", p->children[0]->string, "", p->children[1]->string, symbol_name);

		// Bez dodania poniższego w tablicy czwórek pojawiałyby się np. takie wiersze ":= a - \ a"
		p->string = symbol_name;
	}

	if (strcmp(p->string, "-") == 0) {
		if(is_numeric(p->children[0]->string) == 0) {
			if(search_for_symbol(p->children[0]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[0]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[0]->string);
				add_error(create_error(string, p->children[0]->first_line, p->children[0]->first_column,
						p->children[0]->last_line, p->children[0]->last_column));
			}
		}
		if(is_numeric(p->children[1]->string) == 0) {
			if(search_for_symbol(p->children[1]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[1]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[1]->string);
				add_error(create_error(string, p->children[1]->first_line, p->children[1]->first_column,
										p->children[1]->last_line, p->children[1]->last_column));
			}
		}


		char *symbol_name = generate_new_address();
		symbol *new_symbol = create_symbol(symbol_name, "-", address_counter, 1,
				p->first_line, p->first_column, p->last_line, p->last_column);
		address_counter++;
		add_symbol(new_symbol);
		add_four("-", p->children[0]->string, "", p->children[1]->string, symbol_name);

		p->string = symbol_name;
	}

	if (strcmp(p->string, "*") == 0) {
		if(is_numeric(p->children[0]->string) == 0) {
			if(search_for_symbol(p->children[0]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[0]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[0]->string);
				add_error(create_error(string, p->children[0]->first_line, p->children[0]->first_column,
						p->children[0]->last_line, p->children[0]->last_column));
			}
		}
		if(is_numeric(p->children[1]->string) == 0) {
			if(search_for_symbol(p->children[1]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[1]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[1]->string);
				add_error(create_error(string, p->children[1]->first_line, p->children[1]->first_column,
										p->children[1]->last_line, p->children[1]->last_column));
			}
		}


		char *symbol_name = generate_new_address();
		symbol *new_symbol = create_symbol(symbol_name, "-", address_counter, 1,
				p->first_line, p->first_column, p->last_line, p->last_column);
		address_counter++;
		add_symbol(new_symbol);
		add_four("*", p->children[0]->string, "", p->children[1]->string, symbol_name);

		p->string = symbol_name;
	}

	if (strcmp(p->string, "/") == 0) {
		if(is_numeric(p->children[0]->string) == 0) {
			if(search_for_symbol(p->children[0]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[0]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[0]->string);
				add_error(create_error(string, p->children[0]->first_line, p->children[0]->first_column,
						p->children[0]->last_line, p->children[0]->last_column));
			}
		}
		if(is_numeric(p->children[1]->string) == 0) {
			if(search_for_symbol(p->children[1]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[1]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[1]->string);
				add_error(create_error(string, p->children[1]->first_line, p->children[1]->first_column,
										p->children[1]->last_line, p->children[1]->last_column));
			}
		}


		char *symbol_name = generate_new_address();
		symbol *new_symbol = create_symbol(symbol_name, "-", address_counter, 1,
				p->first_line, p->first_column, p->last_line, p->last_column);
		address_counter++;
		add_symbol(new_symbol);
		add_four("/", p->children[0]->string, "", p->children[1]->string, symbol_name);

		p->string = symbol_name;
	}

	if (strcmp(p->string, "%") == 0) {
		if(is_numeric(p->children[0]->string) == 0) {
			if(search_for_symbol(p->children[0]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[0]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[0]->string);
				add_error(create_error(string, p->children[0]->first_line, p->children[0]->first_column,
						p->children[0]->last_line, p->children[0]->last_column));
			}
		}
		if(is_numeric(p->children[1]->string) == 0) {
			if(search_for_symbol(p->children[1]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[1]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[1]->string);
				add_error(create_error(string, p->children[1]->first_line, p->children[1]->first_column,
										p->children[1]->last_line, p->children[1]->last_column));
			}
		}


		char *symbol_name = generate_new_address();
		symbol *new_symbol = create_symbol(symbol_name, "-", address_counter, 1,
				p->first_line, p->first_column, p->last_line, p->last_column);
		address_counter++;
		add_symbol(new_symbol);
		add_four("%", p->children[0]->string, "", p->children[1]->string, symbol_name);

		p->string = symbol_name;
	}

	if (strcmp(p->string, "=") == 0) {
		if(is_numeric(p->children[0]->string) == 0) {
			if(search_for_symbol(p->children[0]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[0]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[0]->string);
				add_error(create_error(string, p->children[0]->first_line, p->children[0]->first_column,
						p->children[0]->last_line, p->children[0]->last_column));
			}
		}
		if(is_numeric(p->children[1]->string) == 0) {
			if(search_for_symbol(p->children[1]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[1]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[1]->string);
				add_error(create_error(string, p->children[1]->first_line, p->children[1]->first_column,
										p->children[1]->last_line, p->children[1]->last_column));
			}
		}


		char *symbol_name = generate_new_address();
		symbol *new_symbol = create_symbol(symbol_name, "-", address_counter, 1,
				p->first_line, p->first_column, p->last_line, p->last_column);
		address_counter++;
		add_symbol(new_symbol);
		add_four("=", p->children[0]->string, "", p->children[1]->string, symbol_name);

		p->string = symbol_name;
	}

	if (strcmp(p->string, "!=") == 0) {
		if(is_numeric(p->children[0]->string) == 0) {
			if(search_for_symbol(p->children[0]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[0]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[0]->string);
				add_error(create_error(string, p->children[0]->first_line, p->children[0]->first_column,
						p->children[0]->last_line, p->children[0]->last_column));
			}
		}
		if(is_numeric(p->children[1]->string) == 0) {
			if(search_for_symbol(p->children[1]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[1]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[1]->string);
				add_error(create_error(string, p->children[1]->first_line, p->children[1]->first_column,
										p->children[1]->last_line, p->children[1]->last_column));
			}
		}


		char *symbol_name = generate_new_address();
		symbol *new_symbol = create_symbol(symbol_name, "-", address_counter, 1,
				p->first_line, p->first_column, p->last_line, p->last_column);
		address_counter++;
		add_symbol(new_symbol);
		add_four("!=", p->children[0]->string, "", p->children[1]->string, symbol_name);

		p->string = symbol_name;
	}

	if (strcmp(p->string, "<") == 0) {
		if(is_numeric(p->children[0]->string) == 0) {
			if(search_for_symbol(p->children[0]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[0]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[0]->string);
				add_error(create_error(string, p->children[0]->first_line, p->children[0]->first_column,
						p->children[0]->last_line, p->children[0]->last_column));
			}
		}
		if(is_numeric(p->children[1]->string) == 0) {
			if(search_for_symbol(p->children[1]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[1]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[1]->string);
				add_error(create_error(string, p->children[1]->first_line, p->children[1]->first_column,
										p->children[1]->last_line, p->children[1]->last_column));
			}
		}


		char *symbol_name = generate_new_address();
		symbol *new_symbol = create_symbol(symbol_name, "-", address_counter, 1,
				p->first_line, p->first_column, p->last_line, p->last_column);
		address_counter++;
		add_symbol(new_symbol);
		add_four("<", p->children[0]->string, "", p->children[1]->string, symbol_name);

		p->string = symbol_name;
	}

	if (strcmp(p->string, ">") == 0) {
		if(is_numeric(p->children[0]->string) == 0) {
			if(search_for_symbol(p->children[0]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[0]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[0]->string);
				add_error(create_error(string, p->children[0]->first_line, p->children[0]->first_column,
						p->children[0]->last_line, p->children[0]->last_column));
			}
		}
		if(is_numeric(p->children[1]->string) == 0) {
			if(search_for_symbol(p->children[1]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[1]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[1]->string);
				add_error(create_error(string, p->children[1]->first_line, p->children[1]->first_column,
										p->children[1]->last_line, p->children[1]->last_column));
			}
		}


		char *symbol_name = generate_new_address();
		symbol *new_symbol = create_symbol(symbol_name, "-", address_counter, 1,
				p->first_line, p->first_column, p->last_line, p->last_column);
		address_counter++;
		add_symbol(new_symbol);
		add_four(">", p->children[0]->string, "", p->children[1]->string, symbol_name);

		p->string = symbol_name;
	}

	if (strcmp(p->string, "<=") == 0) {
		if(is_numeric(p->children[0]->string) == 0) {
			if(search_for_symbol(p->children[0]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[0]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[0]->string);
				add_error(create_error(string, p->children[0]->first_line, p->children[0]->first_column,
						p->children[0]->last_line, p->children[0]->last_column));
			}
		}
		if(is_numeric(p->children[1]->string) == 0) {
			if(search_for_symbol(p->children[1]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[1]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[1]->string);
				add_error(create_error(string, p->children[1]->first_line, p->children[1]->first_column,
										p->children[1]->last_line, p->children[1]->last_column));
			}
		}


		char *symbol_name = generate_new_address();
		symbol *new_symbol = create_symbol(symbol_name, "-", address_counter, 1,
				p->first_line, p->first_column, p->last_line, p->last_column);
		address_counter++;
		add_symbol(new_symbol);
		add_four("<=", p->children[0]->string, "", p->children[1]->string, symbol_name);

		p->string = symbol_name;
	}

	if (strcmp(p->string, ">=") == 0) {
		if(is_numeric(p->children[0]->string) == 0) {
			if(search_for_symbol(p->children[0]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[0]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[0]->string);
				add_error(create_error(string, p->children[0]->first_line, p->children[0]->first_column,
						p->children[0]->last_line, p->children[0]->last_column));
			}
		}
		if(is_numeric(p->children[1]->string) == 0) {
			if(search_for_symbol(p->children[1]->string) == 0) {
				char *string = calloc(strlen("Nie zadeklarowano zmiennej: ") + strlen(p->children[1]->string), sizeof(char));
				strcat(string, "Nie zadeklarowano zmiennej: ");
				strcat(string, p->children[1]->string);
				add_error(create_error(string, p->children[1]->first_line, p->children[1]->first_column,
										p->children[1]->last_line, p->children[1]->last_column));
			}
		}


		char *symbol_name = generate_new_address();
		symbol *new_symbol = create_symbol(symbol_name, "-", address_counter, 1,
				p->first_line, p->first_column, p->last_line, p->last_column);
		address_counter++;
		add_symbol(new_symbol);
		add_four(">=", p->children[0]->string, "", p->children[1]->string, symbol_name);

		p->string = symbol_name;
	}

}

/* translate jednoczesnie dodaje symbole do tablicy symboli i generuje kod trójadresowy */
void translate(node *p) {
	int i = 0;
	// IF, WHILE ...
	if(strcmp(p->string, "VDEC") == 0) {
		add_symbols_sublist();
		while (i < p->number_of_children) {
			if(strcmp(p->children[i]->string, "VDEC_TABLE") == 0) {
				symbol *new_symbol = create_symbol(p->children[i]->children[0]->string, "TABLE",
						address_counter, atoi(p->children[i]->children[1]->string),
						p->children[i]->first_line, p->children[i]->first_column,
						p->children[i]->last_line, p->children[i]->last_column);
				address_counter+= atoi(p->children[i]->children[1]->string);
				add_symbol(new_symbol);
				i++;
			}
			else {
				symbol *new_symbol = create_symbol(p->children[i]->string, "ID", address_counter, 1,
						p->children[i]->first_line, p->children[i]->first_column,
						p->children[i]->last_line, p->children[i]->last_column);
				address_counter++;
				add_symbol(new_symbol);
				i++;
			}
		}
	}

	else if (strcmp(p->string, "IF") == 0) {
		// Najpier dodaj warunek z wewnątrz IF
		interpret(p->children[0]);

		// Utwórz nowy symbol (t2) z przykładu "IFZ t1 GOTO L1 t2"
		char *symbol_name = generate_new_address();
		symbol *new_symbol = create_symbol(symbol_name, "-", address_counter, 1,
				p->first_line, p->first_column, p->last_line, p->last_column);
		address_counter++;
		add_symbol(new_symbol);

		// Utwórz etykietę L1 z powyzszego przykładu
		char *new_label = generate_new_label();

		add_four("IFZ", p->children[0]->string, "GOTO", new_label, symbol_name);

		translate(p->children[1]);

		add_four("", "", "", "", new_label);

		p->string = symbol_name;
	}

	else if (strcmp(p->string, "IF_ELSE") == 0) {
		interpret(p->children[0]);

		// Utwórz nowy symbol (t2) z przykładu "IFZ t1 GOTO L1 t2"
		char *symbol_name_1 = generate_new_address();
		symbol *new_symbol = create_symbol(symbol_name_1, "-", address_counter, 1,
				p->first_line, p->first_column, p->last_line, p->last_column);
		address_counter++;
		add_symbol(new_symbol);

		// Utwórz etykietę L1 z powyzszego przykładu
		char *new_label_1 = generate_new_label();

		add_four("IFZ", p->children[0]->string, "GOTO", new_label_1, symbol_name_1);

		p->string = symbol_name_1;

		// Komendy dla prawdziwego IF
		translate(p->children[1]);

		char *symbol_name_2 = generate_new_address();
		new_symbol = create_symbol(symbol_name_2, "-", address_counter, 1,
				p->first_line, p->first_column, p->last_line, p->last_column);
		address_counter++;
		add_symbol(new_symbol);

		char *new_label_2 = generate_new_label();

		p->string = symbol_name_2;

		add_four("GOTO", new_label_2, "", "", symbol_name_2);


		add_four("","","","", new_label_1);

		// Komendy dla fałszywego IF (za ELSE)
		translate(p->children[2]);

		add_four("", "", "", "", new_label_2);
	}

	else {
		while (i < p->number_of_children) {
			translate(p->children[i]);
			i++;
		}
		interpret(p);
	}
}
