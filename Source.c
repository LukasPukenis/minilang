#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

char input[] = "4 + 3 * 2 + 1";

typedef enum {
	VALUE,
	OPERATOR
} NODE_TYPE;

typedef enum {
	ADD,
	SUB,
	MUL,
	DIV,
	PAREN_LEFT,
	PAREN_RIGHT
} OPERATOR_TYPE;

const char * OPERATOR_TYPE_NAMES[] = {"+", "-", "*", "/", "(", ")"};

struct ast {
	NODE_TYPE node_type;
	OPERATOR_TYPE operator_type;
	double value;
	struct ast* next;

	bool reduced;
	struct ast* left;
	struct ast* right;
	struct ast* parent;
} ast_head;

/*
	template_arithmetic is matching "VALUE OPERATOR VALUE"
	template_parentheses is matching "( VALUE )"
	template_unary is matching "OPERATOR VALUE"
*/
struct syntax_template {
	NODE_TYPE node_type;
	OPERATOR_TYPE operator_type;
	struct syntax_template * next;
} template_arithmetic, template_parentheses, template_unary;

void build_syntax_templates() {
	// arithmetic template building "VAL OP VAL"
	template_arithmetic.node_type = VALUE;
	template_arithmetic.next = (struct syntax_template *)malloc(sizeof(struct syntax_template));
	template_arithmetic.next->node_type = OPERATOR;
	template_arithmetic.next->next = (struct syntax_template *)malloc(sizeof(struct syntax_template));
	template_arithmetic.next->next->node_type = VALUE;
	template_arithmetic.next->next->next = NULL;
}

// returns true if ast tree beginning with head matches the template beginning with template
bool matches_template(struct ast * head, struct syntax_template * check_template) {
	if (head->node_type != check_template->node_type) {
		return false;
	}
	else if (head->next != NULL && check_template->next != NULL) {
		return matches_template(head->next, check_template->next);
	}	
	return true;
}

void print_ast(struct ast * head) {
	printf("\nPrinting AST\n");

	struct ast * curr = head;
	if (curr->next == NULL) return;

	curr = curr->next;

	while (curr != NULL) {
		if (curr->node_type == VALUE) {
			printf("Node VALUE: %f", curr->value);
		}
		else {
			printf("Node OPERATOR: %s", OPERATOR_TYPE_NAMES[curr->operator_type]);
		}
		printf("\n");
		curr = curr->next;
	}
}

double solve(struct ast * node) {
	OPERATOR_TYPE op_type = node->operator_type;

	if (node->left == NULL && node->right == NULL) {
		printf("\nSolving: %f\n", node->value);
		return node->value;
	}

	// binary operators
	if (op_type == MUL) {
		return solve(node->left) * solve(node->right);
	} else if (op_type == DIV) {
		return solve(node->left) / solve(node->right);
	} else if (op_type == ADD) {
		return solve(node->left) + solve(node->right);
	} else if (op_type == SUB) {
		return solve(node->left) - solve(node->right);
	}	
}

void syntax_tree_evaluate(struct ast * head) {
	printf("\n\nEval... ");
	printf("Solved \"%s\" = %f", input, solve(head));
	printf("\nSolved!\n");
}

void syntax_tree_build(struct ast * head, struct ast ** ast_tree) {
	printf("\n\nBuilding syntax tree will\n");
	// head = head->next; // skip first empty head node

	struct ast * original_head = head;

	while (head->next != NULL && head->next->next != NULL && head->next->next->next != NULL) {
		if (!head->reduced && matches_template(head->next, &template_arithmetic) && (head->next->next->operator_type == MUL || head->next->next->operator_type == DIV)) {

			struct ast * combined = head->next->next;
			head->next->reduced = true;
			combined->reduced = true;
			combined->next->reduced = true;

			combined->left = head->next;
			combined->right = combined->next;
			if (head->next->next->next->next == NULL) {
				combined->next = NULL;
			}
			else {
				combined->next = head->next->next->next->next;
			}
			
			combined->node_type = VALUE;
			head->next = combined;
		}
		else {
			head = head->next;
		}
	}

	head = original_head;

	while (head->next != NULL && head->next->next != NULL && head->next->next->next != NULL) {
		if (!head->reduced && matches_template(head->next, &template_arithmetic) && (head->next->next->operator_type == ADD || head->next->next->operator_type == SUB)) {

			struct ast * combined = head->next->next;
			head->next->reduced = true;
			combined->reduced = true;
			combined->next->reduced = true;

			combined->left = head->next;
			combined->right = combined->next;
			if (head->next->next->next->next == NULL) {
				combined->next = NULL;
			}
			else {
				combined->next = head->next->next->next->next;
			}

			combined->node_type = VALUE;
			head->next = combined;
		}
		else {
			head = head->next;
		}
	}

	*ast_tree = head->next;
	printf("\nDone building\n");
}

void ast_add_value(double value) {
	struct ast * new_node = (struct ast*)malloc(sizeof(struct ast));
	new_node->node_type = VALUE;
	new_node->next = NULL;
	new_node->left = NULL;
	new_node->right = NULL;
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

	new_node->operator_type = type;
	struct ast * curr = &ast_head;

	// rewing head to the last element
	while (curr->next != NULL) {
		curr = curr->next;
	}

	curr->next = new_node;
}

int main(int argc, char** argv)
{
	printf("Lets process this: %s\n", input);

	build_syntax_templates();
	ast_head.next = NULL;

	// This will be set once numeric is detected. Then if dot is detected it will be known it's float
	int numeric_start_pos = -1;
	bool last_was_numeric = false;
	bool dot_was_detected = false;

	for (int i = 0, len = sizeof(input); i < len; i++) {
		printf("\n*************\nProcessing %c\n", input[i]);
		char val = input[i];

		// Ignore whitespace
		if (val == ' ') continue;

		// check for numeric token end
		if (i == len || ((val < '0' || val > '9') && val != '.' && numeric_start_pos != -1 && last_was_numeric)) {
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
		else if (val == '\0') {
			// ASCIIZ ending. so ignore
		}
		else {
			printf("Unsupported operator\n");

		}

	}

	print_ast(&ast_head);

	struct ast * new_ast_tree;
	syntax_tree_build(&ast_head, &new_ast_tree);

	syntax_tree_evaluate(new_ast_tree);

	return 0;
}