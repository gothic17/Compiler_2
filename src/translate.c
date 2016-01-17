#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "main.h"


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
		add_four(":=", p->children[0]->string, "", p->children[1]->string, generate_new_address());
	}

	if (strcmp(p->string, "GET") == 0) {
		add_four("GET", p->children[0]->string, "", "", p->children[0]->string);
	}

	if (strcmp(p->string, "PUT") == 0) {
		add_four("PUT", p->children[0]->string, "", "", p->children[0]->string);
	}
}

/* translate jednoczesnie dodaje symbole do tablicy symboli i generuje kod trójadresowy */
void translate(node *p) {
	int i = 0;
	// IF, WHILE ...
	while (i < p->number_of_children) {
		translate(p->children[i]);
		i++;
	}
	interpret(p);
}
