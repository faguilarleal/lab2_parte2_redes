#ifndef CODIGO_HAMMING_H
#define CODIGO_HAMMING_H

#include <stdint.h>

typedef struct {
    uint64_t mensaje_codificado;
    int bits_datos;
} MensajeTransmision;

// Declaraciones de funciones
int calcular_r(int m);
void validacion(const char* codigo);
void stringToBinary(const char* input, char* output);
uint64_t string_to_uint64(const char* cadena_binaria);
void binario_a_texto(uint64_t datos, int m, char* salida);

uint64_t hamming_encode_general(uint64_t data, int m);
uint64_t hamming_decode_general(uint64_t hamming, int m, int* hubo_error, int* pos_error);

MensajeTransmision emisor(const char* cadena);
uint64_t receptor(uint64_t recibido, int m);

#endif
