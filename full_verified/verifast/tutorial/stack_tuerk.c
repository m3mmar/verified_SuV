struct node {
    struct node *next;
    int value;
};

struct stack {
    struct node *head;
};

int stack_get_count(struct stack *stack)
{
    struct node *n = stack->head;
    int i = 0;
    for (;;)
    {
        if (n == 0) {
            break;
        }
        n = n->next;
        i++;
    }
    return i;
}
