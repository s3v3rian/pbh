#include "serial_output.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int32_t serial_output_write(char *pchSentence, int32_t n32SentenceSize, const char *pchID) {

    int32_t n32Fd = open("/dev/ttyAMA2", O_RDWR);

    if(0 > n32Fd) {

        printf("Failed to open file descriptor for file /dev/ttyAMA2\n");
        return -1;
    }

    int8_t n32Result = 0;

    char achNewSentence[n32SentenceSize + 10];
    n32SentenceSize = sprintf(achNewSentence, "%s,%s", pchID, pchSentence);

    if(n32SentenceSize != write(n32Fd, achNewSentence, n32SentenceSize)) {

        n32Result = -2;
    }

    close(n32Fd);

    return n32Result;
}
