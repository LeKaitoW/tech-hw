#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_MSG "[error]\n"
#define ERR_CODE -1

#define RATE 6
#define BASE 1000000

typedef struct Stack {
	unsigned capacity;
	unsigned top_index;
	int* data;
} Stack;

typedef struct Value {
	unsigned length;
	unsigned* data;
	int sign;
} Value;

typedef struct ValueStack {
	unsigned capacity;
	unsigned top_index;
	Value** data;
} ValueStack;

Stack* stack_init(int len) {
	Stack* stack = (Stack*) malloc(sizeof(Stack));
	if (!stack) {
		return NULL;
	}
	stack->capacity = len;
	stack->top_index = -1;
	stack->data = (int*) malloc(sizeof(int) * len);
	if (!stack->data) {
		free(stack);
		return NULL;
	}
	return stack;
}

void stack_free(Stack* stack) {
	if (!stack) {
		printf(ERR_MSG);
		return;
	}
	free(stack->data);
	free(stack);
	return;
}

int is_empty(Stack* stack) {
	return stack->top_index == -1;
}

int push(Stack* stack, int value) {
	if (stack->capacity == stack->top_index) {
		stack->capacity = stack->capacity * 2;
		stack->data = (int*) realloc(stack->data, sizeof(int) * stack->capacity);
		if (!stack->data) {
			stack_free(stack);
			return ERR_CODE;
		}
	}
	stack->data[++stack->top_index] = value;
	return 0;
}

int peek(Stack* stack) {
	return stack->data[stack->top_index];
}

int pop(Stack* stack) {
	if (stack->top_index == -1) {
		return 0;
	}
	return stack->data[stack->top_index--];
}

ValueStack* value_stack_init(int len) {
	ValueStack* stack = (ValueStack*) malloc(sizeof(ValueStack));
	if (!stack) {
		return NULL;
	}
	stack->capacity = len;
	stack->top_index = -1;
	stack->data = (Value**) malloc(sizeof(Value*) * len);
	if (!stack->data) {
		free(stack);
		return NULL;
	}
	return stack;
}

void print_value(Value* value) {
	if (value->sign)
		printf("-");
	for (int i = value->length-1; i >= 0; i--) {
		if (value->data[i] == 0) {
			continue;
		}
		printf("%u", value->data[i]);
	}
	printf("\n");
}

void value_free(Value* value) {
	free(value->data);
	free(value);
}

void value_stack_free(ValueStack* stack) {
	if (!stack) {
		return;
	}
	for (int i = 0; i < stack->top_index; i++) {
		value_free(stack->data[i]);
	}
	free(stack->data);
	free(stack);
	return;
}

int values_is_empty(ValueStack* stack) {
	return stack->top_index == -1;
}

int value_push(ValueStack* stack, Value* value) {
	if (stack->capacity == stack->top_index) {
		stack->capacity = stack->capacity * 2;
		stack->data = (Value**) realloc(stack->data, sizeof(Value*) * stack->capacity);
		if (!stack->data) {
			value_stack_free(stack);
			return ERR_CODE;
		}
	}
	stack->data[++stack->top_index] = value;
	return 0;
}

Value* value_peek(ValueStack* stack) {
	return stack->data[stack->top_index];
}

Value* value_pop(ValueStack* stack) {
	if (stack->top_index == -1) {
		return NULL;
	}
	return stack->data[stack->top_index--];
}

int is_decimal(char ch) {
	return (ch >= '0' && ch <= '9');
}

int is_space(char ch) {
	return ch == ' ';
}

int precedence(char ch) {
	switch (ch) {
		case '+':
		case '-':
			return 1;

		case '*':
		case '/':
			return 2;
	}
	return -1;
}

char* infix_to_postfix(char* str, int len) {
	int i = 0, k = 0;
	char* new_str = (char*) calloc(len, sizeof(char));
	if (!new_str) {
		return NULL;
	}
	Stack* stack = stack_init(len);
	if (!stack) {
		free(new_str);
		return NULL;
	}
	while (str[i] != '\0') {
		if (!is_decimal(str[i]) && !is_space(str[i])) {
			new_str[k++] = ' ';
		}
		new_str[k] = str[i];
		k++;
		i++;	
	}
	i = 0, k = -1;
	while (new_str[i] != '\0') {
		if (is_decimal(new_str[i]) || is_space(new_str[i])) {
			new_str[++k] = new_str[i];
		}
		else if (new_str[i] == '(') {
			if (push(stack, new_str[i]) == -1) {
				stack_free(stack);
				free(new_str);
				return NULL;
			}
		}
		else if (new_str[i] == ')') {
			while (!is_empty(stack) && peek(stack) != '(') {
				new_str[++k] = pop(stack);
			}
			if (!is_empty(stack) && peek(stack) != '(') {
				return NULL;
			} else {
				pop(stack);
			}
		}
		else {
			while (!is_empty(stack) && precedence(new_str[i]) <= precedence(peek(stack))) {
				if (peek(stack) == '(') {
					stack_free(stack);
					free(new_str);
					return NULL;
				}
				new_str[++k] = pop(stack);
			}
			if (push(stack, new_str[i]) == -1) {
				stack_free(stack);
				free(new_str);
				return NULL;
			}
		}
		i++;
	}
	while (!is_empty(stack)) {
		if (peek(stack) == '(') {
			stack_free(stack);
			free(new_str);
			return NULL;
		}
		new_str[++k] = pop(stack);
	}
	new_str[++k] = '\0';
	stack_free(stack);
	return new_str;
}

Value* init_empty_value(int len) {
	Value* value = (Value*) malloc(sizeof(Value));
	if (!value) {
		return NULL;
	}
	value->sign = 0;
	value->length = len;
	value->data = (unsigned*) calloc(len, sizeof(unsigned));
	if (!value->data) {
		free(value);
		return NULL;
	}
	return value;
}

Value* init_value(char* str, int* length) {
	int i = 0, len = 0;
	while (is_decimal(str[i])){
		len++;
		i++;
	}
	*length = len;
	Value* value = (Value*) malloc(sizeof(Value));
	if (!value) {
		return NULL;
	}
	value->sign = 0;
	value->length = (len % RATE) ? len / RATE + 1 : len / RATE;
	value->data = (unsigned*) calloc(value->length, sizeof(unsigned));
	if (!value->data) {
		free(value);
		return NULL;
	}
	int k = len;
	char buff[RATE+1];
	for (i = 0; i < value->length; i++) {
		if (k < RATE) {
			memcpy(buff, str, k);
			buff[k] = '\0';
			value->data[i] = (unsigned)atoi(buff);
		} else {
			memcpy(buff, str+k-RATE, RATE);
			buff[RATE] = '\0';
			value->data[i] = (unsigned)atoi(buff);
		}
		k -= RATE;
	}
	return value;
}

Value* addition(Value* a, Value* b) {
	int carry = 0;
	int len = (a->length > b->length) ? a->length : b->length;
	Value* res = init_empty_value(len);
	if (!res) {
		return NULL;
	}
	for (int i = 0; i < len || carry; i++) {
		res->data[i] = a->data[i] + carry + (i < b->length ? b->data[i] : 0);
		carry = res->data[i] >= BASE;
		if (carry) res->data[i] -= BASE;
	}
	return res;
}

Value* subtraction(Value* a, Value* b) {
	int carry = 0;
	int len = a->length;
	Value* res = init_empty_value(len);
	if (!res) {
		return NULL;
	}
	for (int i = 0; i < len; i++) {
		res->data[i] = a->data[i] - (carry + (i < b->length ? b->data[i] : 0));
		carry = res->data[i] < 0;
		if (carry) res->data[i] += BASE;
	}
	return res;
}

Value* multiplication(Value* a, Value* b) {
	int len = a->length + b->length;
	Value* res = init_empty_value(len);
	if (!res) {
		return NULL;
	}
	for (int i = 0; i < a->length; i++) {
		for (int j = 0, carry = 0; j < b->length || carry; j++) {
			long long curr = res->data[i+j] + a->data[i] * 1ll * (j < b->length ? b->data[j] : 0) + carry;
			res->data[i+j] = (unsigned) curr%BASE;
			carry = (long long)curr/BASE;
		}
	}
	return res;
}

Value* division(Value* a, Value* b) {
	long long carry = 0;
	for (int i = a->length-1; i>=0; i--) {
		long long curr = a->data[i] + carry * 1ll * BASE;
		a->data[i] = (unsigned) curr/b->data[0];
		carry = curr%b->data[0];
	}
	return a;
}

int polish_calculator(char* str, int len) {
	int i = 0;
	ValueStack* values = value_stack_init(len);
	if (!values) {
		return ERR_CODE;
	}
	Value* c = NULL;
	Value* b = NULL;
	Value* a = NULL;
	int length = 0;
	int swap = 0;
	while (str[i] != '\0') {
		switch (str[i]) {
			case ' ':
			case '\n':
				break;
			case '+':
				a = value_pop(values);
				b = value_pop(values);
				c = addition(a,b);
				if (!a || !b || !c || value_push(values, c) == -1) {
					value_free(b);
					value_free(a);
					value_stack_free(values);
					return ERR_CODE;
				}
				value_free(b);
				value_free(a);
				break;
			case '-':
				b = value_pop(values);
				a = value_pop(values);
				swap = (b->length > a->length) ? 1 : 0;
				if (!swap){
					c = subtraction(a, b);
				} else {
					c = subtraction(b, a);
					c->sign = 1;
				}
				if (!a || !b || !c || value_push(values, c) == -1) {
					value_free(b);
					value_free(a);
					value_stack_free(values);
					return ERR_CODE;
				}
				value_free(b);
				value_free(a);
				break;
			case '*':
				a = value_pop(values);
				b = value_pop(values);
				c = multiplication(a, b);
				if (!a || !b || !c || value_push(values, c) == -1) {
					value_free(b);
					value_free(a);
					value_stack_free(values);
					return ERR_CODE;
				}
				value_free(a);
				value_free(b);
				break;
			case '/':
				b = value_pop(values);
				a = value_pop(values);
				if (!a || !b || value_push(values, division(a, b)) == -1) {
					value_free(b);
					value_free(a);
					value_stack_free(values);
					return ERR_CODE;
				}
				value_free(b);
				break;
			default:
				value_push(values, init_value(str+i, &length));
				break;
		}
		if (length != 0) {
			i += length;
			length = 0;
		} else {
			i++;
		}
	}
	print_value(value_peek(values));
	value_stack_free(values);
	if (c) value_free(c);
	return 0;
}

int main() {
	char* input = NULL;
	size_t len = 0;
	if (getline(&input, &len, stdin) == -1) {
		printf(ERR_MSG);
		return 0;
	}
	char* new_str = infix_to_postfix(input, len);
	if (!new_str) {
		free(input);
		printf(ERR_MSG);
		return 0;
	}
	if (polish_calculator(new_str, len) == -1) {
		printf(ERR_MSG);
	}
	free(input);
	free(new_str);
	return 0;
}