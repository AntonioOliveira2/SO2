#include "tecnicofs_client_api.h"
#include "fs/operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

typedef struct {
    int session_id;
    int cl_rx;
    int sv_tx;
} client;

client cl;

int tfs_mount(char const *client_pipe_path, char const *server_pipe_path) {

    char message[43] = { '\0' };

    // Checks if client input path is valid
    if (strlen(client_pipe_path) < 40) {
        // Builds message for the worker thread to mount writing end of pipe
        message[0] = TFS_OP_CODE_MOUNT_STR;
        message[1] = '|';
        strcat(message, client_pipe_path);
    } else {
        printf("Error: Client pipe name length exceeded.");
        return -1;
    }

    // Opens server pipe writing end, even activating server pipe
    cl.sv_tx = open(server_pipe_path, O_WRONLY);
    if (cl.sv_tx < 0) {
        printf("Error: Failed to open server pipe writing end.\n");
        return -1;
    }
    
    ssize_t written = write(cl.sv_tx, message, strlen(message));

    if ((size_t)written < strlen(message)) {
        printf("Failed to write whole message\n");
        return -1;
    }

    if (unlink(client_pipe_path) != 0 && errno != ENOENT) {
        printf("Failed to unlink client pipe receiver.\n");
        return -1;
    }

    if (mkfifo(client_pipe_path, 0640) != 0) {
        printf("Failed to create client pipe receiver.\n");
    }

    cl.cl_rx = open(client_pipe_path, O_RDONLY);    
    if (cl.cl_rx < 0) {
        perror("Error ");
        return -1;
    }
    
    return 0;
}

int tfs_unmount() {
    char message[20] = { '\0' };
    message[0] = TFS_OP_CODE_UNMOUNT_STR;
    message[1] = '|';
    char chr_res[10];
    
    char id_char[10];

    sprintf(id_char, "%d", cl.session_id);

    strcat(message, id_char);

    if (write(cl.sv_tx, message, strlen(message)) < strlen(message)) {
        printf("Error writing message to server on unmount\n");
        return -1;
    }

    ssize_t b_read = read(cl.cl_rx, chr_res, strlen(chr_res));
    if (read(cl.cl_rx, chr_res, strlen(chr_res))) {
        printf("Client pipe server side close failed.\n");
        return -1;
    } else {
        if (close(cl.cl_rx) != 0 && close(cl.sv_tx) != 0) {
            printf("Failed to close client read or server write pipe ends.");
            return -1;
        }
        return 0;
    }
    return -1;
}

int tfs_open(char const *name, int flags) {
    char message[100] = { '\0' };
    char resp[10] =  { '\0' };
    char id_char[10];
    char flags_char[10];
    message[0] = TFS_OP_CODE_OPEN_STR;
    message[1] = '|';
    sprintf(id_char, "%d", cl.session_id);
    sprintf(flags_char, "%d", flags);
    
    message[2] = cl.session_id;


    if (strlen(name) > MAX_FILE_NAME) {
        printf("File name length exceeds maximum allowed size.\n");
        return -1;
    }
    strcat(message, id_char);
    strcat(message, "|");
    strcat(message, name);
    strcat(message, "|");
    strcat(message, flags_char);
    strcat(message, "|");


    ssize_t written = write(cl.sv_tx, message, strlen(message));

    if ((size_t)written < strlen(message)) {
        printf("Written message is not the same as original\n");
        return -1;
    }

    while (1) {
        ssize_t b_read = read(cl.cl_rx, resp, strlen(resp));

        if (b_read < 0) {
            printf("Error while reading from response pipe.\n");
            return -1;
        } else if (b_read > 0) {
            int res = atoi(resp);
            return res;
        }
        break;
    }
    return -1;
}

int tfs_close(int fhandle) {
    /* TODO: Implement this */
    return -1;
}

ssize_t tfs_write(int fhandle, void const *buffer, size_t len) {
    /* TODO: Implement this */
    return -1;
}

ssize_t tfs_read(int fhandle, void *buffer, size_t len) {
    /* TODO: Implement this */
    return -1;
}

int tfs_shutdown_after_all_closed() {
    /* TODO: Implement this */
    return -1;
}
