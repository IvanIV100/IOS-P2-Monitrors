# IOS-P2-Monitrors

IOS second project focusing on synchronization of proceses with use of semaphores and shared memory. \
Uses one structure with needed data for shared memory that is protected by a semaphore on read and write to assure data integrity. \
\
Due to the safer approach with data manipulation this might not be the fastest/most effective solution. \
The semaphores locks the critical sections for longer periods of code that requiers to be run whole at once.
