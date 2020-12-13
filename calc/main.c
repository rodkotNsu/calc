#define  _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include<malloc.h>
#include<string.h>
#include<math.h>

#define MAX_LEN_EXP 1100
#define ERROR_SYNTAX_ERROR -9999
#define ERROR_DIVISION_BY_ZERO -9998

enum Syntax_type
{
	OPERATION,
	OPERAND,
	END
};
struct Symbol_data
{
	char* data;
	enum Syntax_type type;
};
typedef struct Symbol_data Symbol_data;

Symbol_data* free_array_symbol(Symbol_data* arr);
Symbol_data* parsing_str_to_array_symbol(char* str);
Symbol_data* build_rpn(Symbol_data* arr);

double parsing_str_to_float(char* str);
double calculate_by_rpn(Symbol_data* arr);
double calculate_by_string(char* string);

int check_few_point(char* str) {
	int count = 0;
	for (int i = 0; i < strlen(str); i++) {
		if (str[i] == '.')
			count++;
		if (count > 1)
			return 1;
	}
	return 0;
}
Symbol_data* parsing_str_to_array_symbol(char* str) {
	unsigned int i = -1;
	unsigned int count = 0;
	unsigned int check_num = 0;

	Symbol_data* arr_exp = malloc(sizeof(Symbol_data) * MAX_LEN_EXP);

	char alf_operation[] = "\n *-/+()";
	char alf_operand[] = "0123456789";
	arr_exp[count].type = END;
	do {
		i++;
		if (strchr(alf_operation, str[i]) != NULL) {
			if (check_num > 0) {
				//Exception: Float haven`t float part 
				if (arr_exp[count].data[check_num - 1] == '.' ||
					arr_exp[count].data[0] == '.' ||
					check_few_point(arr_exp[count].data) == 1) {
					arr_exp[count + 1].type = END;
					free_array_symbol(arr_exp);

					return NULL;
				}
				count++;
				check_num = 0;
			}
			if (str[i] != ' ') {
				//buffer = (char*)malloc(sizeof(char) * 20);
				arr_exp[count].data = (char*)malloc(sizeof(char) * 20);
				arr_exp[count].data[0] = str[i];
				arr_exp[count].data[1] = '\0';
				arr_exp[count].type = OPERATION;
				count++;
			}
			arr_exp[count].type = END;
		}
		else
			if (strchr(alf_operand, str[i]) != NULL) {
				if (check_num == 0) {
					arr_exp[count].data = (char*)malloc(sizeof(char) * 20);
					arr_exp[count].type = OPERAND;
					arr_exp[count].data[0] = (char)str[i];
					arr_exp[count].data[1] = '\0';
					check_num = 1;
				}
				else {
					arr_exp[count].data[check_num] = str[i];
					arr_exp[count].data[check_num + 1] = '\0';

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
			free(arr[i].data);
			i++;
		}
		free(arr);
		return NULL;
	}
	return NULL;
}

Symbol_data* build_rpn(Symbol_data* arr) {
	int i = -1;
	int situation_result = 0;
	int situation_stack = -1;
	int flag_operand = 0;
	Symbol_data* result = malloc(sizeof(Symbol_data) * MAX_LEN_EXP);
	Symbol_data* stack = malloc(sizeof(Symbol_data) * MAX_LEN_EXP);
	if (result == NULL) {
		return NULL;
	}
	else if (stack == NULL) {
		free(result);
		return NULL;
	}
	else {
		do
		{
			i++;
			if (arr[i].type == OPERAND) {
				result[situation_result++] = arr[i];
				flag_operand = 1;
			}
			else
				if (arr[i].type == OPERATION) {

					if (arr[i].data[0] == '(') {
						stack[++situation_stack] = arr[i];
						flag_operand = 0;
					}

					if (arr[i].data[0] == ')') {
						int flag = 0;
						while (situation_stack >= 0 && stack[situation_stack].data[0] != '(') {
							if (stack[situation_stack].data[0] != '(')
							{
								result[situation_result++] = stack[situation_stack];
								flag = 1;
							}
							situation_stack--;
						}
						if (situation_stack == -1 || (flag == 0 && flag_operand == 0)) {
							free(stack);
							free(result);
							return NULL;
						}
						situation_stack--;
					}

					if (arr[i].data[0] == '*' || arr[i].data[0] == '/') {
						while (situation_stack >= 0 && stack[situation_stack].data[0] != '(') {
							if (stack[situation_stack].data[0] == '*' || stack[situation_stack].data[0] == '/') {
								result[situation_result++] = stack[situation_stack];
								situation_stack--;
							}
							else {
								break;
							}
						}
						stack[++situation_stack] = arr[i];
					}

					if (arr[i].data[0] == '+' || arr[i].data[0] == '-') {
						while (situation_stack >= 0 && stack[situation_stack].data[0] != '(') {
							result[situation_result++] = stack[situation_stack];
							situation_stack--;
						}
						stack[++situation_stack] = arr[i];
					}

				}
			if (arr[i].type == END) {
				while (situation_stack >= 0)
				{
					if (stack[situation_stack].data[0] == '(')
					{
						free(result);
						free(stack);
						return NULL;
					}
					result[situation_result++] = stack[situation_stack];
					situation_stack--;
				}
				result[situation_result++] = arr[i];
				free(stack);
				return result;
			}

		} while (arr[i].type != END);
	}
	return NULL;
}



double parsing_str_to_float(char* str) {
	double result = 0;
	double pow_10 = 1;
	char* point = strchr(str, '.');
	if (point != NULL)
		pow_10 = pow(10, (double)(point - &str[0] - 1));
	else
		pow_10 = pow(10, (double)(strlen(str) - 1));
	int i = 0;
	while (str[i] != '\0') {
		if (str[i] != '.')
		{
			result += (str[i] - '0') * pow_10;
			pow_10 /= 10.0;
		}
		i++;
	}
	return result;
}
double calculate_by_rpn(Symbol_data* arr) {
	double stack[MAX_LEN_EXP];
	int situation_stack = -1;
	int i = 0;
	while (arr[i].type != END) {
		if (arr[i].type == OPERAND)
		{
			situation_stack++;
			stack[situation_stack] = parsing_str_to_float(arr[i].data);
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
		else
		{
			printf("%d", (int)result);
		}

	}
	free(str_expression);
	//_CrtDumpMemoryLeaks();
	return 0;
}
