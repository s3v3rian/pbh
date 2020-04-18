#include "print_utils.h"

#include <stdio.h>
#include "lib/inc/itsmsg_cdd.h"

int32_t sprintf_denm_event_code(char *pchSentence, int32_t n32CauseCode) {

    int32_t n32SentenceSize = 0;

    switch(n32CauseCode) {

        case CauseCodeType_redLight:

            n32SentenceSize = sprintf(pchSentence, "EVENT,RED_LIGHT\n");
            break;

        default:

            n32SentenceSize = sprintf(pchSentence, "EVENT,%d\n", n32CauseCode);
            break;
    }

    return n32SentenceSize;
}
