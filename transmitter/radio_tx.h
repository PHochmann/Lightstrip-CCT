#include <stddef.h>
#include <stdint.h>

void radio_init_tx();
void radio_send(uint8_t *buffer, size_t bytes);
void radio_socket_on();
void radio_socket_off();
