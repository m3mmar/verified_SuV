#include "stdlib.h"

struct node {
    struct node *next;
    void *value;
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

void stack_push(struct stack *stack, void *value)
{
    struct node *n = malloc(sizeof(struct node));
    if (n == 0) { abort(); }
    n->next = stack->head;
    n->value = value;
    stack->head = n;
}

void stack_reverse(struct stack *stack)
{
    struct node *n = stack->head;
    struct node *m = 0;
    while (n != 0)
    {
        struct node *next = n->next;
        n->next = m;
        m = n;
        n = next;
    }
    stack->head = m;
}

void stack_dispose(struct stack *stack)
{
    free(stack);
}

struct point {
    int x;
    int y;
};

struct point *create_point(int x, int y)
{
    struct point *result = malloc(sizeof(struct point));
    if (result == 0) abort();
    result->x = x;
    result->y = y;
    return result;
}

int main()
{
    struct stack *s = create_stack();
    struct point *p1 = create_point(10, 0);
    struct point *p2 = create_point(0, 10);
    stack_push(s, p1);
    stack_push(s, p2);
    stack_reverse(s);
    abort();
    return 0;
}