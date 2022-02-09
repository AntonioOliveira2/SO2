#include "client/tecnicofs_client_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char **argv) {

    if (argc < 3) {
        printf("Must provide 2 arguments: client and server pipe names");
        exit(EXIT_FAILURE);
    }

    assert(tfs_mount(argv[1], argv[2]) == 0);

    int f = tfs_open("Teste", TFS_O_CREAT);

    //assert(f != -1);

    assert(tfs_unmount() == 0);

    return 0;
}