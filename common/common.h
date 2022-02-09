#ifndef COMMON_H
#define COMMON_H

#define BUFFER_SIZE 2048
#define THREAD_WORKERS 10
#define RETURN_SIZE 1
/* tfs_open flags */
enum {
    TFS_O_CREAT = 0b001,
    TFS_O_TRUNC = 0b010,
    TFS_O_APPEND = 0b100,
};

/* operation codes (for client-server requests) */
enum {
    TFS_OP_CODE_MOUNT = 1,
    TFS_OP_CODE_UNMOUNT = 2,
    TFS_OP_CODE_OPEN = 3,
    TFS_OP_CODE_CLOSE = 4,
    TFS_OP_CODE_WRITE = 5,
    TFS_OP_CODE_READ = 6,
    TFS_OP_CODE_SHUTDOWN_AFTER_ALL_CLOSED = 7
};

enum {
    TFS_OP_CODE_MOUNT_STR = '1',
    TFS_OP_CODE_UNMOUNT_STR = '2',
    TFS_OP_CODE_OPEN_STR = '3',
    TFS_OP_CODE_CLOSE_STR = '4',
    TFS_OP_CODE_WRITE_STR = '5',
    TFS_OP_CODE_READ_STR = '6',
    TFS_OP_CODE_SHUTDOWN_AFTER_ALL_CLOSED_STR = '7'
};


#endif /* COMMON_H */