#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
//shared data
typedef struct {
    int value;
    bool has_new_data;
} shared_data_t;

shared_data_t shared_data = {0, false};
pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;
bool running = true;

void* writer_thread(void* arg) {
    int count = 0;
    
    while (running && count < 10) {
        pthread_mutex_lock(&data_mutex);
        shared_data.value = count;
        shared_data.has_new_data = true;
        printf("Writethread:Wrote value %d\n", count);
        pthread_mutex_unlock(&data_mutex);
        
        count++;
    }
    
    running = false;
    printf("Writer thread: Finished writing\n");
    return NULL;
}
void* reader_thread(void* arg) {
    while (running) {
        //locks
        pthread_mutex_lock(&data_mutex);
        if (shared_data.has_new_data) {
            printf("Reader thread: Read value %d from shared data\n", shared_data.value);
            shared_data.has_new_data = false;
        }
        //unlocks
        pthread_mutex_unlock(&data_mutex);
        }
    
    printf("Read thread: DONE\n");
    return NULL;
}

int main() {
    pthread_t thread_1, thread_2;
    if (pthread_create(&thread_1, NULL, writer_thread, NULL) != 0) {
        fprintf(stderr, "Error w/ writer thread\n");
        return 1;
    }
    if (pthread_create(&thread_2, NULL, reader_thread, NULL) != 0) {
        fprintf(stderr, "Error w/ reader thread\n");
        running = false;
        pthread_join(thread_1, NULL);
        return 1;
    }
    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);
    pthread_mutex_destroy(&data_mutex);
    
    printf("completed\n");
    return 0;
}