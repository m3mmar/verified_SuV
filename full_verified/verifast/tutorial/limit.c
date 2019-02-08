#include "stdlib.h"

struct account {
    int limit;
    int balance;
};

struct account *create_account(int limit)
{
    struct account *myAccount = malloc(sizeof(struct account));
    if (myAccount == 0) { abort(); }
    myAccount->limit = limit;
    myAccount->balance = 0;
    return myAccount;
}

int account_get_balance(struct account *myAccount)
{
    return myAccount->balance;
}

void account_deposit(struct account *myAccount, int amount)
{
    myAccount->balance += amount;
}

int account_withdraw(struct account *myAccount, int amount)
{
    int result = myAccount->balance - amount < myAccount->limit ? myAccount->balance - myAccount->limit : amount;
    myAccount->balance -= result;
    return result;
}

void account_dispose(struct account *myAccount)
{
    free(myAccount);
}

int main()
{
    struct account *myAccount = create_account(-100);
    account_deposit(myAccount, 200);
    int w1 = account_withdraw(myAccount, 50);
    assert(w1 == 50);
    int b1 = account_get_balance(myAccount);
    assert(b1 == 150);
    int w2 = account_withdraw(myAccount, 300);
    assert(w2 == 250);
    int b2 = account_get_balance(myAccount);
    assert(b2 == -100);
    account_dispose(myAccount);
    return 0;
}