#include "stdlib.h"

struct account {
    int balance;
};

struct account *create_account()
{
    struct account *myAccount = malloc(sizeof(struct account));
    if (myAccount == 0) { abort(); }
    myAccount->balance = 0;
    return myAccount;
}

int account_get_balance(struct account *myAccount)
{
    return myAccount->balance;
}

void account_set_balance(struct account *myAccount, int newBalance)
{
    myAccount->balance = newBalance;
}

void account_deposit(struct account *myAccount, int amount)
{
    myAccount->balance += amount;
}

void account_dispose(struct account *myAccount)
{
    free(myAccount);
}

int main()
{
    struct account *myAccount = create_account();
    account_set_balance(myAccount, 5);
    account_deposit(myAccount, 10);
    int b = account_get_balance(myAccount);
    assert(b == 15);
    account_dispose(myAccount);
    return 0;
}