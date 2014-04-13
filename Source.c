#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

typedef enum {
	VAL,
	ADD,
	SUB,
	MUL,
	DIV,
	PAREN_OPEN,
	PAREN_CLOSE,
	FUNCTION,// +-/* are also functions a.k.a unary operators
	END // used in template as terminating symbol
} NODE_TYPE;

const char * TOKEN_NAMES[] = {"VAL", "+", "-", "*", "/", "(", ")", "FUNCTION"};

struct token {	
	NODE_TYPE node_type;	
	double value;	
	struct token* next;	
	struct token* left;
	struct token* right;
	bool reduced;
} token_head;

NODE_TYPE add_template[] = { VAL, ADD, VAL, END };
NODE_TYPE sub_template[] = { VAL, SUB, VAL, END };
NODE_TYPE mul_template[] = { VAL, MUL, VAL, END };
NODE_TYPE div_template[] = { VAL, DIV, VAL, END };
NODE_TYPE paren_template[] = { PAREN_OPEN, VAL, PAREN_CLOSE, END };
NODE_TYPE unary_template[] = { FUNCTION, VAL, END };

// Whitelist of possible unary operators. If not in this list then it's not available as unary operator
NODE_TYPE unary_whitelist[] = { ADD, SUB, END };

struct token * token_add(NODE_TYPE token) {
	struct token * curr = &token_head;

	while (curr->next != NULL) {
		curr = curr->next;
	}

	struct token * n = malloc(sizeof(struct token));
	n->node_type = token;
	n->left = NULL;
	n->right = NULL;
	n->next = NULL;
	n->reduced = false;
	curr->next = n;
	return n;
}

struct token * token_add_val(double val) {
	struct token * constructed = token_add(VAL);
	constructed->value = val;
	return constructed;
}

void print_tokens(struct token * head) {
	printf("Printing tokens\n");
	
	head = head->next;

	while (head != NULL) {
		if (head->node_type == VAL) {
			printf("Token: %s(%f)\n", TOKEN_NAMES[head->node_type], head->value);
		} else {
			printf("Token: %s\n", TOKEN_NAMES[head->node_type]);
		}		
		head = head->next;
	}
}

bool matches_template(struct token * head, NODE_TYPE * tmpl) {		
	printf("Trying to match a template...\n");
	int i = 0;
	NODE_TYPE curr_token = tmpl[i];

	while (curr_token != END) {
		if (head == NULL) return false;
		printf("curr_token = %s value=%f\n", TOKEN_NAMES[curr_token], head->value);

		NODE_TYPE head_type = head->node_type;

		// Reduced node is considered a value but we can't override it's operator
		if (head->reduced) {
			head_type = VAL;
		}

		bool in_unary_whitelist = false;
		int white_list_iterator = 0;

		while (unary_whitelist[white_list_iterator] != END) {
			if (unary_whitelist[white_list_iterator] == head_type) {
				in_unary_whitelist = true;
				break;
			}
			white_list_iterator++;
		}

		if (curr_token == FUNCTION && head_type != VAL && in_unary_whitelist) curr_token = head_type;

		if (head == NULL || head_type != curr_token) return false;

		i++;
		head = head->next;		
		curr_token = tmpl[i];
	}

	return true;
}

// Reduces expressions. Like Head -> 1 -> + -> 2 into head->reduced(left=1, right=2, node_type = +)
struct token * reduce(struct token * head) {
	struct token * combined = head->next->next;
	combined->reduced = true;
	combined->left = head->next;
	combined->right = combined->next;

	if (head->next->next->next->next != NULL) {
		combined->next = head->next->next->next->next;
	} else {
		combined->next = NULL;
	}
	head->next = combined;
	return head;
}

// Unfolds (VAL) into VAL
struct token * unwrap(struct token * head) {
	struct token * combined = head->next->next;
	combined->reduced = true;

	if (head->next->next->next->next != NULL) {
		combined->next = head->next->next->next->next;
	} else {
		combined->next = NULL;
	}
	head->next = combined;
	return head;
}

// Unfolds OP VAL into VAL based on OP. For example - VAL simply returns negative value of VAL
struct token * unwrap_unary(struct token * head) {

	struct token * combined = head->next->next;
	NODE_TYPE op = head->next->node_type;

	if (op == SUB) combined->value = 0 - combined->value;

	combined->reduced = true;

	if (head->next->next->next != NULL) {
		combined->next = head->next->next->next;
	} else {
		combined->next = NULL;
	}
	head->next = combined;
	return head;
}


// Solves a branch of AST
double solve(struct token * head) {

	NODE_TYPE op = head->node_type;

	if (head->left == NULL && head->right == NULL) {
		return head->value;
	}

	if (op == ADD) {
		return solve(head->left) + solve(head->right);
	} else if (op == SUB) {
		return solve(head->left) - solve(head->right);
	} else if (op == MUL) {
		return solve(head->left) * solve(head->right);
	} else if (op == DIV) {
		return solve(head->left) / solve(head->right);
	} else {
		printf("Unsupported operation! %d\n", op);
	}
}

void build_ast(struct token * head) {
	printf("Building AST\n");

	struct token * curr = head;

	while (head->next->next != NULL) {
		// Parenthesis
		curr = head;
		while (curr->next != NULL) {
			if (matches_template(curr->next, paren_template)) {
				printf("Matched template (/)\n");
				curr = unwrap(curr);
			} else {
				curr = curr->next;
			}
		}

		// Multiplication and division
		curr = head;
		while (curr->next != NULL) {
			if (matches_template(curr->next, mul_template) || matches_template(curr->next, div_template)) {
				printf("Matched template mul/div\n");
				curr = reduce(curr);
			} else {
				curr = curr->next;
			}
		}

		// Addition and subtraction
		curr = head;
		while (curr->next != NULL) {
			if (matches_template(curr->next, add_template) || matches_template(curr->next, sub_template)) {
				printf("Matched template add/sub\n");
				curr = reduce(curr);
			} else {
				curr = curr->next;
			}
		}

		// Unary
		curr = head;
		while (curr->next != NULL) {
			if (matches_template(curr->next, unary_template)) {
				printf("Matched template UNARY \n");
				curr = unwrap_unary(curr);
			} else {
				curr = curr->next;
			}
		}

	}
	printf("AST has been built\n");
}

bool parse(char * code) {
	printf("Parsing: %s\n", code);
	int last_numeric_idx = -1;

	for (int i = 0, len = strlen(code); i <= len; i++) {
		char c = code[i];
		if (c == ' ') continue;

		if (c >= '0' && c <= '9') {
			if (last_numeric_idx == -1)	last_numeric_idx = i;
		}
		else {
			if (last_numeric_idx != -1) {
				int size = i - last_numeric_idx;				

				char * buffer = malloc(size);
				strncpy(buffer, (code + last_numeric_idx), size);
				buffer[size] = '\0';

				token_add_val(atof(buffer));
				printf("Got a number: %f\n", atof(buffer));
				last_numeric_idx = -1;
			}

			if (c == '+') {
				token_add(ADD);
			} else if (c == '-') {
				token_add(SUB);
			} else if (c == '*') {
				token_add(MUL);
			} else if (c == '/') {
				token_add(DIV);
			} else if (c == '(') {
				token_add(PAREN_OPEN);
			} else if (c == ')') {
				token_add(PAREN_CLOSE);
			}
		}
	}

	return true;
}

/*
  Note that the first token is empty and first->next links to the first useful node. This is because when matching templates we must have the previous node so on first node there's no previous node
  aaaand so we have one
*/
int main(int argc, char** argv)
{	
	parse("1+-2");
	print_tokens(&token_head);
	build_ast(&token_head);

	printf("Result: %f\n", solve(token_head.next));
	return 0;
}
