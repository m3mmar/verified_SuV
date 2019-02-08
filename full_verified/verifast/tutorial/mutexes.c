#include "stdlib.h"
#include "threading.h"

void wait_for_pulse(int source);

void sleep(int millis);

void print_int(int n);

struct counter {
    int count;
    struct mutex *mutex;
};


struct count_pulses_data {
    struct counter *counter;
    int source;
};

void count_pulses(struct count_pulses_data *data)
{
    struct counter *counter = data->counter;
    int source = data->source;
    free(data);
    
    struct mutex *mutex = counter->mutex;
    
    while (true)
    {
        wait_for_pulse(source);
        mutex_acquire(mutex);
        counter->count++;
        mutex_release(mutex);
    }
}

void count_pulses_async(struct counter *counter, int source)
{
    struct count_pulses_data *data = malloc(sizeof(struct count_pulses_data));
    if (data == 0) abort();
    data->counter = counter;
    data->source = source;
    thread_start(count_pulses, data);
}

int main()
{
    struct counter *counter = malloc(sizeof(struct counter));
    if (counter == 0) abort();
    counter->count = 0;
    struct mutex *mutex = create_mutex();
    counter->mutex = mutex;
    
    count_pulses_async(counter, 1);
    count_pulses_async(counter, 2);
    
    while (true)
    {
        sleep(1000);
        mutex_acquire(mutex);
        print_int(counter->count);
        mutex_release(mutex);
    }
}