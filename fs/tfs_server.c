#include "operations.h"
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
    int id;
    int reserved;
    pthread_cond_t is_occupied;
    char buffer[BUFFER_SIZE];

} worker;

worker threads[THREAD_WORKERS];
int occup_worker_thrds;
pthread_mutex_t thread_lock;

int initialize_workers() {
    occup_worker_thrds = 0;
    for (int i = 0; i < THREAD_WORKERS; i++) {
        threads[i].id = i;
        threads[i].reserved = 0;
        if (pthread_cond_init(&threads[i].is_occupied, NULL) != 0)
            return -1;
    }
    return 0;
}

void *worker_routine(void *arg) {

    int id = *(int *)arg;
    int cl_tx = -1;

    while (1) {
        pthread_mutex_lock(&thread_lock);
        pthread_cond_wait(&threads[id].is_occupied, &thread_lock);
        pthread_mutex_unlock(&thread_lock);

        char *tok = strtok(threads[id].buffer, "|");
        int opcode = atoi(tok);

        switch (opcode) {
        case TFS_OP_CODE_MOUNT:
            char *client_named_pipe = strtok(NULL, "|");

            cl_tx = open(client_named_pipe, O_WRONLY);
            if (cl_tx < 0) {
                perror("Error");
                break;
            }

            break;
        case TFS_OP_CODE_UNMOUNT:
        
            if (close(cl_tx) < 0) {
                printf("Failed to close client pipe");
                write(cl_tx, "-2", strlen("-2"));
            }
            break;
        case TFS_OP_CODE_OPEN:

            break;
        case TFS_OP_CODE_CLOSE:
            break;
        case TFS_OP_CODE_WRITE:
            break;
        case TFS_OP_CODE_READ:
            break;
        case TFS_OP_CODE_SHUTDOWN_AFTER_ALL_CLOSED:
            break;
        default:
            printf("Error: unknown opcode %d", opcode);
            break;
        }
    }
}

void debug_thread_workers() {
    for (int i = 0; i < THREAD_WORKERS; i++) {
        if (threads[i].reserved == 1) {
            printf("[INFO]: Id: %d\n", threads[i].id);
            printf("[INFO]: Reserved: %d\n", threads[i].reserved);
            printf("[INFO]: Buffer: %s\n", threads[i].buffer);
        }
    }
}

int get_session_id(char *message) {
    char *session_id;

    if (strtok_r(message, "|", &session_id) == NULL) {
        return -1;
    }
    if (strtok_r(NULL, "|", &session_id) == NULL) {
        return -1;
    }

    return atoi(session_id);
}

/*
    Cliente -> API -> Server -> API -> Cliente
    O cliente ao chamar uma funcao, tendo em conta que a mesma tem
    um op code, envia para este buffer o formato especificado da api.

    Esse formato e enviado ate aqui, que depois descodifica e
    processa. Depois de acabado, retorna o valor
*/

int main(int argc, char **argv) {

    char msg_buffer[BUFFER_SIZE] = {'\0'};
    pthread_t thread_ids[THREAD_WORKERS];

    if (argc < 2) {
        printf("Please specify the pathname of the server's pipe.\n");
        return 1;
    }

    char *pipename = argv[1];
    printf("Starting TecnicoFS server with pipe called %s\n", pipename);

    if (unlink(pipename) != 0 && errno != ENOENT) {
        printf("Failed to unlink pipe %s", pipename);
        return -1;
    }

    if (mkfifo(pipename, 0640) != 0) {
        printf("Failed to create pipe %s", pipename);
        return -1;
    }

    int fd = open(pipename, O_RDONLY);
    if (fd < 0) {
        printf("Failed to open pipe %s", pipename);
        return -1;
    }

    initialize_workers();

    for (int i = 0; i < THREAD_WORKERS; i++) {
        pthread_create(&thread_ids[i], NULL, worker_routine, &threads[i].id);
    }

    struct timespec timesleep;

    timesleep.tv_nsec = 1000;
    while (1) {
        ssize_t rd = read(fd, msg_buffer, BUFFER_SIZE);
        nanosleep(&timesleep, NULL);
        if (rd > 0) {
            int opcode = msg_buffer[0] - '0';
            msg_buffer[rd] = '\0';

            switch (opcode) {
            case TFS_OP_CODE_MOUNT:
                for (int i = 0; i < THREAD_WORKERS; i++) {
                    threads[i].id = i;
                    threads[i].reserved = 1;
                    occup_worker_thrds++;
                    memcpy(threads[i].buffer, msg_buffer, strlen(msg_buffer));
                    pthread_cond_signal(&threads[i].is_occupied);

                    break;
                }

                break;
            case TFS_OP_CODE_UNMOUNT:
            case TFS_OP_CODE_OPEN:
            case TFS_OP_CODE_CLOSE:
            case TFS_OP_CODE_WRITE:
            case TFS_OP_CODE_READ:
            case TFS_OP_CODE_SHUTDOWN_AFTER_ALL_CLOSED:
                char *msg;
                strtok_r(msg_buffer, "|", &msg);
                strtok_r(NULL, "|", &msg);

                int sid = atoi(msg);
                memcpy(threads[sid].buffer, msg_buffer, strlen(msg_buffer));
                pthread_cond_signal(&threads[sid].is_occupied);
                break;
            default:
                return -1;
                break;
            }
            debug_thread_workers();

        } else if (rd == 0 && occup_worker_thrds == 0) {
            printf("Pipe closed.");
            break;
        }
    }

    for (int i = 0; i < occup_worker_thrds; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    return 0;
}