#include <stddef.h>
#include <stdint.h>

#ifdef RADIO_TX
void radio_init_tx();
void radio_send(uint8_t *buffer, size_t bytes);
#endif

#ifdef RADIO_RV
void radio_init_rv();
void radio_receive(uint8_t *buffer, size_t bytes);
#endif
