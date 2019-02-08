#include "stdlib.h"

struct node {
    struct node *next;
    int value;
};

struct stack {
    struct node *head;
};

struct stack *create_stack()
{
    struct stack *stack = malloc(sizeof(struct stack));
    if (stack == 0) { abort(); }
    stack->head = 0;
    return stack;
}

void stack_push(struct stack *stack, int value)
{
    struct node *n = malloc(sizeof(struct node));
    if (n == 0) { abort(); }
    n->next = stack->head;
    n->value = value;
    stack->head = n;
}

int stack_pop(struct stack *stack)
{
    struct node *head = stack->head;
    int result = head->value;
    stack->head = head->next;
    free(head);
    return result;
}


typedef int int_func(void *data, int x);

void nodes_map(struct node *n, int_func *f, void *data)
{
    if (n != 0) {
        int y = f(data, n->value);
        n->value = y;
        nodes_map(n->next, f, data);
    }
}

void stack_map(struct stack *stack, int_func *f, void *data)
{
    nodes_map(stack->head, f, data);
}

void nodes_dispose(struct node *n)
{
    if (n != 0) {
        nodes_dispose(n->next);
        free(n);
    }
}

void stack_dispose(struct stack *stack)
{
    nodes_dispose(stack->head);
    free(stack);
}

struct plus_a_data {
    int a;
};

int plus_a(struct plus_a_data *data, int x)
{
    int result = x + data->a;
    return result;
}

int read_int();

int main()
{
    struct stack *s = create_stack();
    stack_push(s, 10);
    stack_push(s, 20);
    stack_push(s, 30);
    int a = read_int();
    struct plus_a_data *data = malloc(sizeof(struct plus_a_data));
    if (data == 0) abort();
    data->a = a;
    stack_map(s, plus_a, data);
    free(data);
    stack_dispose(s);
    return 0;
}
