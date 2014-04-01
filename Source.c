#include <stdbool.h>
#include <math.h>

#define NULL 0

union node_value {
	int iValue;
	float fValue;
};

struct ast {
	enum { VALUE, OPERATOR } node_type;
	enum { INT, FLOAT} value_type;
	enum { ADD, SUB, MUL, DIV} operator_type;
	short precedence;
	union node_value value;
	struct ast* next;
};

void print_ast(struct ast * head) {
	printf("\nPrinting AST: ");

	struct ast * curr = head;
	if (curr->next == NULL) return;

	curr = curr->next;

	while (curr != NULL) {
		if (curr->value_type == INT) {
			printf("Node INT %d", curr->value.iValue);
		}
		else {
			printf("Node FLOAT %f", curr->value.fValue);
		}
		printf(" | ");

		curr = curr->next;
	}
}

char input[] = "123+456.78";

int main(int argc, char** argv)
{
	printf("Lets process this: %s\n", input);

	// head and last elements of ast to amortize the inserting
	struct ast * ast_head = (struct ast*)malloc(sizeof(struct ast));
	ast_head->next = NULL;

	// This will be set once numeric is detected. Then if dot is detected it will be known it's float
	int numeric_start_pos = -1;
	union node_value numeric;
	bool last_was_numeric = false;
	bool dot_was_detected = false;

	for (int i = 0, len = sizeof(input) - 1; i < len; i++) {
		printf("\nProcessing %c\n", input[i]);
		char val = input[i];		

		if (val >= '0' && val <= '9') {
			printf("Numeric detected at %d. Not sure if int or float yet\n", i);
			if (numeric_start_pos == -1 && !last_was_numeric) {
				numeric_start_pos = i;
				last_was_numeric = true;
			}			

		} else if (val == '+' || val == '-') {
			printf("+ or - detected\n");


		}
		else if (val == '*' && val == '/') {
			printf("* or / detected\n");

		}
		else if (val == '.') {
			dot_was_detected = true;
		}
		else {
			printf("Unsupported operator\n");

		}

		// check for numeric token end
		if ( i+1 == len || ((val < '0' || val > '9') && val != '.' && numeric_start_pos != -1 && last_was_numeric)) {
			printf("Numeric ended at %d\n", i);

			char buffer[100];
			memset(buffer, '\0', sizeof(buffer));
			strncpy(buffer, (input + numeric_start_pos), i - numeric_start_pos+1);
			struct ast * new_node = (struct ast*)malloc(sizeof(struct ast));
			new_node->node_type = VALUE;
			new_node->next = NULL;

			if (dot_was_detected) {
				new_node->value.fValue = atoi(buffer);
				new_node->value_type = FLOAT;
				struct ast * curr = ast_head;

				// rewing head to the last element
				while (curr->next != NULL) {
					curr = curr->next;
				}
				curr->next = new_node;

				printf("Float translated: %f", atof(buffer));
			}
			else {				
				new_node->value.iValue = atoi(buffer);
				new_node->value_type = INT;
				struct ast * curr = ast_head;
				
				// rewing head to the last element
				while (curr->next != NULL) {					
					curr = curr->next;
				}
				curr->next = new_node;
				
				printf("Int translated: %d", atoi(buffer));
			}			

			dot_was_detected = false;
			numeric_start_pos = -1;
			last_was_numeric = false;
		}
	}

	print_ast(ast_head);
	getch();
	return 0;
}
