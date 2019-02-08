#include "stdlib.h"
#include "threading.h"

int wait_for_source();

bool wait_for_pulse(int source); // true means the sensor has been removed.

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
    bool done = false;
    while (!done)
    {
        done = wait_for_pulse(source);
        if (!done) {
            mutex_acquire(mutex);
            counter->count++;
            mutex_release(mutex);
        }
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

void print_count(struct counter *counter)
{
    struct mutex *mutex = counter->mutex;
    while (true)
    {
        sleep(1000);
        mutex_acquire(mutex);
        print_int(counter->count);
        mutex_release(mutex);
    }
}

int main()
{
    struct counter *counter = malloc(sizeof(struct counter));
    if (counter == 0) abort();
    counter->count = 0;
    struct mutex *mutex = create_mutex();
    counter->mutex = mutex;
    
    thread_start(print_count, counter);
    
    while (true)
    {
        int source = wait_for_source();
        count_pulses_async(counter, source);
    }
}