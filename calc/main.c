#define _CRT_SECURE_NO_WARNINGS 
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


Symbol_data* parsing_str_to_array_symbol(char* str) {


	Symbol_data* arr_exp = malloc(sizeof(Symbol_data) * MAX_LEN_EXP);

	if (arr_exp == NULL) {
		return NULL;
	}
	else {
		unsigned int i = -1;
		unsigned int count = 0;
		unsigned int check_num = 0;
		int flag_error = 0;

		char* buffer = NULL;

		char alf_operation[] = "\n *-/+()";
		char alf_operand[] = "0123456789";

		while (str[i] != '\n' && flag_error == 0) {
			i++;
			if (strchr(alf_operation, str[i]) != NULL) {
				if (check_num > 0) {
					//Exception: Float haven`t float part 
					if (buffer[check_num - 1] == '.') {
						flag_error = 1;
						break;
					}
					arr_exp[count].data = buffer;
					arr_exp[count].type = OPERAND;
					count++;

					check_num = 0;
				}
				if (str[i] != ' ') {
					if (str[i] == '\n')
						arr_exp[count].type = END;
					else {
						arr_exp[count].type = OPERATION;
						buffer = malloc(sizeof(char) * 20);

						buffer[0] = (char)str[i];
						buffer[1] = '\0';
						arr_exp[count].data = buffer;
					}count++;

				}
			}
			else
				if (strchr(alf_operand, str[i]) != NULL) {
					if (check_num == 0) {
						//Exception: Float haven`t integer part 
						if (str[i] == '.') {
							flag_error = 1;
							break;
						}
						buffer = malloc(sizeof(char) * 20);

						buffer[0] = (char)str[i];
						buffer[1] = '\0';

						check_num = 1;
					}
					else {
						//Exception: Float have more then 1 points 
						if (str[i] == '.' && strchr(buffer, alf_operand[0]) != NULL) {
							if (buffer != NULL) {
								flag_error = 1;
								break;
							}
						}
						buffer[check_num] = str[i];
						buffer[check_num + 1] = '\0';

						check_num++;
					}
				}
				else
				{
					//Exception: String have foreign symbol
					flag_error = 1;
					break;
				}
		}
		// exception handling 
		if (flag_error == 1) {
			if (buffer != NULL)
				free(buffer);
			arr_exp[count].type = END;
			free_array_symbol(arr_exp);
			return NULL;
		}
		return arr_exp;
	}
}
Symbol_data* free_array_symbol(Symbol_data* arr) {
	if (arr != NULL) {
		int i = 0;
		while (arr[i].type != END) {
			if (arr[i].data != NULL)
				free(arr[i].data);
			i++;
		}

		free(arr);
		return NULL;
	}
	return NULL;
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

			if (arr[i].data[0] == '(') {
				stack[++situation_stack] = arr[i];
				flag_operand = 0;
			}

			if (arr[i].data[0] == ')') {
				int flag = 0;
				while (situation_stack >= 0 && stack[situation_stack].data[0] != '(') {
					result[situation_result++] = stack[situation_stack];
					flag = 1;
					situation_stack--;
				}

				if (situation_stack == -1 || (flag == 0 && flag_operand == 0)) {
					break;
				}
				situation_stack--;
			}

			if (arr[i].data[0] == '*' || arr[i].data[0] == '/') {
				while (situation_stack >= 0 && stack[situation_stack].data[0] != '('
					&& (stack[situation_stack].data[0] == '*' || stack[situation_stack].data[0] == '/')) {
					result[situation_result++] = stack[situation_stack];
					situation_stack--;
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
	return 0;
}
