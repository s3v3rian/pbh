
#include "threads.h"

#include "common/globals.h"
#include "gps/gps_sim.h"
#include "cam/cam_mngr.h"
#include "denm/denm_mngr.h"

// ------------------------------------------------------------
// ----------------- External variables -----------------------
// ------------------------------------------------------------

extern poti_service_t *g_psPotiHandler;

/**
 * activetion_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */
int32_t main(int argc, char **argv) {

    // -------------------------------------------------
    // ----------- Initialize GPS Simulator ------------
    // -------------------------------------------------

    int32_t n32Result = 0;

#ifdef __GPS_SIMULATOR_ENABLED__

    if(2 <= argc) {

        n32Result = gps_sim_init(argv[1], argv[2]);

        if(0 > n32Result) {

            printf("Cannot initialize gps simulator\n");
            return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
        }
    }

#endif

    // -------------------------------------------------
    // ------------ Initialize POTI Service ------------
    // -------------------------------------------------

    n32Result = poti_create_service(&g_psPotiHandler, NULL);

    if(0 > IS_SUCCESS(n32Result)) {

        printf("Cannot create POTI service: %s\n", ERROR_MSG(n32Result));
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    // -------------------------------------------------
    // ----------- Initialize Tx/Rx Threads ------------
    // -------------------------------------------------

    //  Activate tx tasks.
    pthread_create(&g_asThreads[0], NULL, main_sender_active, NULL);

    // Activate rx tasks.
    pthread_create(&g_asThreads[1], NULL, cam_receiver_active, NULL);
    pthread_create(&g_asThreads[2], NULL, denm_receiver_active, NULL);

    /* Wait till cam_sender_active is completed. */
    pthread_join(g_asThreads[0], NULL);

    // -------------------------------------------------
    // --------------- Release Resources ---------------
    // -------------------------------------------------

    printf("Releasing all resources...\n");

    /* Release POTI handler. */
    poti_release_service(g_psPotiHandler);

    return PROCEDURE_SUCCESSFULL;
}
