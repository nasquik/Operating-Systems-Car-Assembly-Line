#ifndef KEYS_H
#define KEYS_H

#define SEMCP_KEY_FILE ("semcp.key")            //Semaphore used as mutex, full and empty for the shared memory segment between Construction and PaintShop
#define SEMPITYPE1_KEY_FILE ("sempitype1.key")      //Semaphore used as a mutex for Components of Type 1 in the shm segment between PaintShop and Inspection
#define SEMPITYPE2_KEY_FILE ("sempitype2.key")      //Semaphore used as a mutex for Component of Type 2 in the shm segment between PaintShop and Inspection
#define SEMPITYPE3_KEY_FILE ("sempitype3.key")      //Semaphore used as a mutex for Component of Type 3 in the shm segment between PaintShop and Inspection
#define SEMIA_KEY_FILE ("semia.key")            //Semaphore used as mutex, full, empty for the shm segment between Inspection and Assembly
#define SHMCP_KEY_FILE ("shmcp.key")            //Shared memory segment used between Construction and PaintShop
#define SHMPITYPE1_KEY_FILE ("shmpitype1.key")            //Shared memory segment used between PaintShop and Inspection
#define SHMPITYPE2_KEY_FILE ("shmpitype2.key")            //Shared memory segment used between PaintShop and Inspection
#define SHMPITYPE3_KEY_FILE ("shmpitype3.key")            //Shared memory segment used between PaintShop and Inspection
#define SHMIA_KEY_FILE ("shmia.key")            //Shared memory segment used between Inspection and Assembly

#endif