#define  _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include<malloc.h>
#include<string.h>
#include<math.h>

#define MAX_LEN_EXP 1100
#define ERROR_SYNTAX_ERROR -9999
#define ERROR_DIVISION_BY_ZERO -9998
 
#define ALF_OPERATION "\n *-/+()"
#define ALF_OPERAND "0123456789"

enum Syntax_type
{
	OPERATION,
	OPERAND,
	END
};
struct Symbol_data
{
	char* data;
	int value;
	enum Syntax_type type;
};
typedef struct Symbol_data Symbol_data;

Symbol_data* free_array_symbol(Symbol_data* arr);
Symbol_data* parsing_str_to_array_symbol(char* str);
Symbol_data* build_rpn(Symbol_data* arr);

double calculate_by_rpn(Symbol_data* arr);
double calculate_by_string(char* string);
int get_priority(char* operator);

Symbol_data* parsing_str_to_array_symbol(char* str) {
	unsigned int i = -1;
	unsigned int count = 0;
	unsigned int check_num = 0;

	Symbol_data* arr_exp = malloc(sizeof(Symbol_data) * MAX_LEN_EXP);
	char buffer[20] = { 0 };
	arr_exp[count].type = END;
	do {
		i++;
		if (strchr(ALF_OPERATION, str[i]) != NULL) {
			if (check_num > 0) {
				if (sscanf(buffer, "%d", &arr_exp[count].value))
				count++;
				check_num = 0;
			}
			if (str[i] != ' ' && str[i] != '\n') {
				arr_exp[count].data = (char*)malloc(sizeof(char) * 20);
				arr_exp[count].data[0] = str[i];
				arr_exp[count].data[1] = '\0';
				arr_exp[count].type = OPERATION;
				count++;
			}
			arr_exp[count].type = END;
		}
		else
			if (strchr(ALF_OPERAND, str[i]) != NULL) {
				if (check_num == 0) {
					 
					arr_exp[count].type = OPERAND;
					buffer[0] = (char)str[i];
					buffer[1] = '\0';
					check_num = 1;
				}
				else {
					buffer[check_num] = str[i];
					buffer[check_num + 1] = '\0';

					check_num++;
				}
			}
			else {
				if (check_num > 0)
					arr_exp[count + 1].type = END;
				free_array_symbol(arr_exp);
				return NULL;
			}
	} while (str[i] != '\n');

	return arr_exp;
}
Symbol_data* free_array_symbol(Symbol_data* arr) {
	if (arr != NULL) {
		int i = 0;
		while (arr[i].type != END) {
			if (arr[i].type == OPERATION)
				free(arr[i].data);
			i++;
		}
		free(arr);
		return NULL;
	}
	return NULL;
}
int get_priority(char* operator) {
	switch (operator[0])
	{
	case'+':case'-':return 1; break;
	case'*':case'/':return 2; break;
	case'(':return 0; break;
	case')':return -1; break;
	default:
		return -2;
		break;
	}
	return -2;
}
Symbol_data* build_rpn(Symbol_data* arr) {
	int i = 0;
	int situation_result = 0;
	int situation_stack = -1;
	int flag_operand = 0;

	Symbol_data* result = malloc(sizeof(Symbol_data) * MAX_LEN_EXP);
	Symbol_data* stack = malloc(sizeof(Symbol_data) * MAX_LEN_EXP);

	while (arr[i].type != END) {
		if (arr[i].type == OPERAND) {
			result[situation_result++] = arr[i];
			flag_operand = 1;
		}
		else {
			if (get_priority(arr[i].data) != get_priority("(")) {
				while (situation_stack >= 0 &&
					get_priority(arr[i].data) <= get_priority(stack[situation_stack].data)
					&& get_priority(stack[situation_stack].data) != get_priority("(")) {
					result[situation_result++] = stack[situation_stack];
					situation_stack--;
				}
				if (situation_stack >= 0 && get_priority(stack[situation_stack].data) == get_priority("(")
					&& get_priority(arr[i].data)==get_priority(")")) {
					if (flag_operand != 1) {
						break;
					}
					else {
						i++;
						situation_stack--;
						continue;
					}
				}
			}
			stack[++situation_stack] = arr[i];
			flag_operand = 0;
		}
		i++;
	}
	if (arr[i].type == END) {
		while (situation_stack >= 0)
		{
			result[situation_result++] = stack[situation_stack];
			situation_stack--;
		}
		result[situation_result++] = arr[i];
		free(stack);
		return result;
	}
	free(stack);
	free(result);
	return NULL;
}
double calculate_by_rpn(Symbol_data* arr) {
	double stack[MAX_LEN_EXP];
	int situation_stack = -1;
	int i = 0;
	while (arr[i].type != END) {
		if (arr[i].type == OPERAND){
			situation_stack++;
			stack[situation_stack] = arr[i].value;
		}
		else if (arr[i].type == OPERATION) {
			if (situation_stack < 1)
			{
				return ERROR_SYNTAX_ERROR;
			}
			else {
				switch (arr[i].data[0])
				{
				case '*':
					stack[situation_stack - 1] *= stack[situation_stack];
					break;
				case '/':
					if (stack[situation_stack] == 0) {
						return ERROR_DIVISION_BY_ZERO;
					}
					else {
						stack[situation_stack - 1] /= stack[situation_stack];
					}
					break;
				case '-':
					stack[situation_stack - 1] -= stack[situation_stack];
					break;
				case '+':
					stack[situation_stack - 1] += stack[situation_stack];
					break;
				}
				situation_stack--;
			}
		}
		i++;
	}
	if (situation_stack == 0) {
		return  stack[situation_stack];
	}
	return ERROR_SYNTAX_ERROR;
}
double calculate_by_string(char* string) {
	Symbol_data* symbol_arr = parsing_str_to_array_symbol(string);
	Symbol_data* symbol_arr_rpn = NULL;
	double result = ERROR_SYNTAX_ERROR;
	if (symbol_arr != NULL) {
		symbol_arr_rpn = build_rpn(symbol_arr);
		if (symbol_arr_rpn != NULL) {
			result = calculate_by_rpn(symbol_arr_rpn);
		}
	}
	free(symbol_arr_rpn);
	free_array_symbol(symbol_arr);
	return result;
}
int main() {
	char* str_expression = (char*)malloc(sizeof(char) * MAX_LEN_EXP);
	if (fgets(str_expression, MAX_LEN_EXP, stdin)) {
		double result = calculate_by_string(str_expression);
		if (result == ERROR_SYNTAX_ERROR) {
			printf("syntax error");
		}
		else if (result == ERROR_DIVISION_BY_ZERO) {
			printf("division by zero");
		}
		else{
			printf("%d", (int)result);
		}
	}
	free(str_expression);
	return 0;
}
