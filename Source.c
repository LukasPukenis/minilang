#include <stdbool.h>
#include <math.h>

#define NULL 0

typedef enum {
	VALUE,
	OPERATOR
} NODE_TYPE;

typedef enum {
	ADD,
	SUB,
	MUL,
	DIV
} OPERATOR_TYPE;

struct ast {
	NODE_TYPE node_type;
	OPERATOR_TYPE operator;
	short precedence;
	double value;
	struct ast* next;
	bool reduced;
	struct ast * left;
	struct ast * right;
	struct ast * parent;
} ast_head, syntax_tree_head;

void print_ast(struct ast * head) {
	printf("\nPrinting AST: ");

	struct ast * curr = head;
	if (curr->next == NULL) return;

	curr = curr->next;

	while (curr != NULL) {
		if (curr->node_type == VALUE) {
			printf("Node INT %f", curr->value);
		}
		else {
			printf("Node OPERATOR %f", curr->value);
		}
		printf(" | ");
		curr = curr->next;
	}
}

void syntax_tree_print(struct ast * head) {

	printf("\n\nPrinting Syntax Tree: ");

	printf("\nDone printing Syntax Tree\n");
}

void syntax_tree_insert(struct ast * instead, struct ast * combined) {

}

void syntax_tree_build(struct ast * head) {
	printf("\n\nBuilding syntax tree will\n");

	printf("\nDone building\n");
}

void ast_add_value(double value) {
	struct ast * new_node = (struct ast*)malloc(sizeof(struct ast));
	new_node->node_type = VALUE;
	new_node->next = NULL;
	new_node->reduced = false;

	new_node->value = value;
	struct ast * curr = &ast_head;

	// rewing head to the last element
	while (curr->next != NULL) {
		curr = curr->next;
	}

	curr->next = new_node;
}

void ast_add_operator(OPERATOR_TYPE type) {
	struct ast * new_node = (struct ast*)malloc(sizeof(struct ast));
	new_node->node_type = OPERATOR;
	new_node->next = NULL;
	new_node->reduced = false;

	new_node->operator = type;
	struct ast * curr = &ast_head;

	// rewing head to the last element
	while (curr->next != NULL) {
		curr = curr->next;
	}

	curr->next = new_node;
}

char input[] = "100-123+456.78";

int main(int argc, char** argv)
{
	printf("Lets process this: %s\n", input);

	ast_head.next = NULL;

	// This will be set once numeric is detected. Then if dot is detected it will be known it's float
	int numeric_start_pos = -1;
	bool last_was_numeric = false;
	bool dot_was_detected = false;

	for (int i = 0, len = sizeof(input)-1; i < len; i++) {
		printf("\n*************\nProcessing %c\n", input[i]);
		char val = input[i];

		// check for numeric token end
		if (i + 1 == len || ((val < '0' || val > '9') && val != '.' && numeric_start_pos != -1 && last_was_numeric)) {
			printf("Numeric ended at %d\n", i);

			char * buffer = malloc((i - numeric_start_pos) *sizeof(char));
			strncpy(buffer, (input + numeric_start_pos), i - numeric_start_pos + 1);

			ast_add_value(atof(buffer));

			printf("Numeric translated: %f", atof(buffer));

			dot_was_detected = false;
			numeric_start_pos = -1;
			last_was_numeric = false;
		}

		if (val >= '0' && val <= '9') {
			printf("Numeric detected at %d. Not sure if int or float yet\n", i);
			if (numeric_start_pos == -1 && !last_was_numeric) {
				numeric_start_pos = i;
				last_was_numeric = true;
			}

		}
		else if (val == '+') {
			ast_add_operator(ADD);
		}
		else if (val == '-') {
			ast_add_operator(SUB);
		}
		else if (val == '*') {
			ast_add_operator(MUL);
		}
		else if (val == '/') {
			ast_add_operator(DIV);
		}

		else if (val == '.') {
			dot_was_detected = true;
		}
		else {
			printf("Unsupported operator\n");

		}

	}

	print_ast(&ast_head);

	syntax_tree_build(&ast_head);

	syntax_tree_print(&syntax_tree_head);

	getch();
	return 0;
}