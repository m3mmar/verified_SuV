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

bool stack_is_empty(struct stack *stack)
{
    struct node *head = stack->head;
    bool result = stack->head == 0;
    return result;
}

int nodes_get_sum(struct node *nodes)
{
    int result = 0;
    if (nodes != 0) {
        result = nodes_get_sum(nodes->next);
        result += nodes->value;
    }
    return result;
}

int stack_get_sum(struct stack *stack)
{
    int result = nodes_get_sum(stack->head);
    return result;
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

void stack_dispose(struct stack *stack)
{
    struct node *n = stack->head;
    while (n != 0)
    {
        struct node *next = n->next;
        free(n);
        n = next;
    }
    free(stack);
}

int main()
{
    struct stack *s = create_stack();
    stack_push(s, 10);
    stack_push(s, 20);
    stack_pop(s);
    stack_pop(s);
    stack_dispose(s);
    return 0;
}