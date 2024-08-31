#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* thread_print_index(void* thread_id){
    long index = (long)thread_id;
    printf("Индекс потока - %ld\n", index);
    pthread_exit(NULL);

}
int main(){
    pthread_t threads[5];
    int  rc;
    long index;
    for ( index = 0; index < 5; index++)
    {
        rc = pthread_create(&threads[index], NULL, thread_print_index, (void*)index);
        if(rc)
        {
            printf("Ошибка при создании потока %d\n", rc);
            exit(-1);
        }
        
    }
    for (index = 0; index < 5; index++) {
        pthread_join(threads[index], NULL);
    }
    printf("Все потоки успешно завершили работу\n");
    pthread_exit(NULL);    

}