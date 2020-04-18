#include "serial_boundary.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int32_t serial_boundary_write(char *pchSentence, int32_t n32SentenceSize, uint32_t un32StationId) {

    int32_t n32Fd = open("/dev/ttyAMA2", O_RDWR);

    if(0 > n32Fd) {

        printf("Failed to open file descriptor for file /dev/ttyAMA2\n");
        return -1;
    }

    int8_t n32Result = 0;

    char achNewSentence[n32SentenceSize + 10];
    n32SentenceSize = sprintf(achNewSentence, "T%d,%s", un32StationId, pchSentence);

    if(n32SentenceSize != write(n32Fd, achNewSentence, n32SentenceSize)) {

        n32Result = -2;
    }

    close(n32Fd);

    return n32Result;
}
