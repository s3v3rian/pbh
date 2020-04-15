#include "csv_infra.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t read_csv_doubles(const char *pchCsvFilePath, double **p2dCsvArray) {

    //printf("Loading CSV file - %s\n", pchCsvFilePath);

    FILE *pfInput = NULL;
    uint32_t un32CurrentSizeInValues = 0;
    int32_t n32Char = 0;
    int32_t n32QuotationOnOff = 0; //0 - off, 1 - on

    pfInput = fopen(pchCsvFilePath, "r") ;

    // Read the number of values first.
    if(NULL == pfInput) {

        printf("Failed to read csv file - %s\n", pchCsvFilePath);
        perror("Error");
        return 0;
    }

    // Move the file pointer to the beginning.
    rewind(pfInput);

    while( (n32Char = fgetc(pfInput)) != EOF) {

        switch(n32Char) {

        case ',':

            un32CurrentSizeInValues++;
            break;

        case '\n':

            un32CurrentSizeInValues++;
            break;

        case '\"':

            if(!n32QuotationOnOff) {

                n32QuotationOnOff = 1;

            } else {

                n32QuotationOnOff = 0;
            }
            break;

        default:

            break;
        }
    }

    char *pchTempValHolder = NULL;
    uint32_t un32CsvArrayIndex = 0;
    uint32_t un32LengthLongestValue = 0;
    uint32_t un32VCPM = 0; //value character marker
    uint32_t un32CurrentLine = 0;

    if(0 == un32CurrentSizeInValues) {

        return 0;
    }

    // Allocate csv array.
    *p2dCsvArray = malloc(un32CurrentSizeInValues * sizeof(double));

    // Set longest value size in characters.
    un32LengthLongestValue = 25;

    // Allocate temp value holder.
    pchTempValHolder = (char*)malloc(un32LengthLongestValue);

    // Move the file pointer to the beginning.
    rewind(pfInput);

    un32CurrentLine = 1;
    un32VCPM = 0;
    n32QuotationOnOff = 0;

    while( (n32Char = fgetc(pfInput)) != EOF) {

        switch(n32Char) {

        case ',':

            pchTempValHolder[un32VCPM] = '\0';
            (*p2dCsvArray)[un32CsvArrayIndex] = atof(pchTempValHolder);
            //printf("Value: %f\n", (*p2dCsvArray)[un32CsvArrayIndex]);
            un32CsvArrayIndex++;
            un32VCPM = 0;
            break;

        case '\n':

            pchTempValHolder[un32VCPM] = '\0';
            (*p2dCsvArray)[un32CsvArrayIndex] = atof(pchTempValHolder);
            //printf("Value: %f\n", (*p2dCsvArray)[un32CsvArrayIndex]);
            un32CsvArrayIndex++;
            un32VCPM = 0;
            un32CurrentLine++;
            break;

        case '\"':

            if(!n32QuotationOnOff) {

                n32QuotationOnOff = 1;
                pchTempValHolder[un32VCPM] = n32Char;
                un32VCPM++;

            } else {

                n32QuotationOnOff = 0;
                pchTempValHolder[un32VCPM] = n32Char;
                un32VCPM++;
            }
            break;

        default:

            pchTempValHolder[un32VCPM] = n32Char;
            un32VCPM++;
            break;
        }
    }

    printf("Loaded from csv file %s - number of values: %d\n", pchCsvFilePath, un32CurrentSizeInValues);

    fclose(pfInput);
    free(pchTempValHolder);

    return un32CurrentSizeInValues;
}
