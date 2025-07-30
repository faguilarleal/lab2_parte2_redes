#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "codigo_hamming.h"

// -- Correcion de errores --

//////////// codigo de Hamming //////////////

// --- Calcular r (bits de paridad) ---
int calcular_r(int m) {
    int r = 0;
    while ((m + r + 1) > (1 << r)) r++;
    return r;
}

// --- Validar que la cadena solo tenga 0s y 1s---
void validacion(const char* codigo) {
    for (int i = 0; i < strlen(codigo); i++) {
        if (codigo[i] != '0' && codigo[i] != '1') {
            printf("Error: Solo se permiten 0s y 1s\n");
            exit(1);
        }
    }
}

// --- Convertir texto a binario ---
void stringToBinary(const char* input, char* output) {
    int k = 0;
    for (int i = 0; i < strlen(input); i++) {
        unsigned char c = input[i];
        for (int j = 7; j >= 0; j--) {
            output[k++] = (c & (1 << j)) ? '1' : '0';
        }
    }
    output[k] = '\0';
     printf("%s\n", output); 
}

// --- Convertir string binario a uint64_t ---
// uint64_t es unsigned integer 64-bit type, este siempre ocupa 64 bits
// para operaciones este es ideal, por eso se usa 
uint64_t string_to_uint64(const char* bin) {
    uint64_t val = 0;
    for (int i = 0; i < strlen(bin); i++) {
        val <<= 1;
        if (bin[i] == '1') val |= 1;
    }
    return val;
}

//capa de presentacion
// para cualquier codigo(n,m) que cumpla (m+r+1)<=2^r
// m = numero de bits de datos
// r= numero de bits de pariedad
// n= m + r, NO +1 (ese era el error principal)
// --- Codificación Hamming ---
uint64_t hamming_encode_general(uint64_t data, int m) {
    //determinar bits de pariedad
    int r = calcular_r(m);
    printf("Bits de pariedad necesarios: %d\n", r);

    //total de bits con pariedad 
    int n = m + r;
    printf("Total de bits en código Hamming: %d\n", n);
    
    //posicionar los bits de datos (No de paridad inicialmente)
    uint64_t hamming = 0;
    int data_index = 0;

    // Si la posicion no es potencia de 2, es para datos
    for (int i = 1; i <= n; i++) {
        //extraer bit de datos de derecha a izquierda
        if ((i & (i - 1)) != 0) { 
            if ((data >> (m - 1 - data_index)) & 1) {
                hamming |= (1ULL << (i - 1));
                printf("Posicion %d: Dato 1 (bit %d de datos)\n", i, data_index);
            } else {
                printf("Posicion %d: Dato 0 (bit %d de datos)\n", i, data_index);
            }
            data_index++;
        } else {
            printf("Posicion %d: Paridad (se calculara)\n", i);
        }
    }

    //calcular los bits de pariedad
    printf("\n---- Calculo de paridad: ----\n");
    for (int p = 0; p < r; p++) {
        int parity_pos = (1 << p);
        int count = 0;

        printf("P%d (posicion %d) cubre: ", parity_pos, parity_pos);
        for (int i = 1; i <= n; i++) {
            if (i & parity_pos) {
                if (hamming & (1ULL << (i - 1))) count++;
            }
        }
        // pariedad par (si count es impar, ponemos 1)
        if (count % 2 == 1) {
            hamming |= (1ULL << (parity_pos - 1));
            printf("-> %d unos, se pone P%d=1\n", count, parity_pos);
        } else {
            printf("-> %d unos, se pone P%d=0\n", count, parity_pos);
        }
    }

    // mostrar resultado final (posicion 1 a n, de izquierda a derecha)
    printf("\nCodigo Hamming final (%d bits): \n", n);
    for (int i = 0; i < n; i++) {
        printf("%d", (int)((hamming >> i) & 1));
    }
    printf("\n");

    //mostrar posicion por posicion
    printf("----Desglose por posiciones:----\n");
    printf("(leer de abajo para arriba) \n");
    for (int i = 1; i <= n; i++) {
        int bit_val = (hamming >> (i-1)) & 1;
        if ((i & (i - 1)) == 0) {
            printf("Pos %d: P%d = %d\n", i, i, bit_val);
        } else {
            printf("Pos %d: Dato = %d\n", i, bit_val);
        }
    }


    return hamming;
}

// --- Decodificación Hamming ---
uint64_t hamming_decode_general(uint64_t hamming, int m, int* hubo_error, int* pos_error) {
    int r = calcular_r(m);
    int n = m + r;
    int syndrome = 0;

    printf("\n--- DECODIFICACION ---\n");
    printf("Codigo recibido (%d bits): ", n);
    for (int i = 0; i < n; i++) {
        printf("%d", (int)((hamming >> i) & 1));
    }
    printf("\n");

    //calcular sindrome
    printf("\nCalculo de sindrome:\n");
    for (int p = 0; p < r; p++) {
        int parity_pos = (1 << p);
        int count = 0;

        printf("S%d (P%d): ", parity_pos, parity_pos);
        for (int i = 1; i <= n; i++) {
            if (i & parity_pos) {
                if (hamming & (1ULL << (i - 1))) count++;
            }
        }
        // if (count % 2 == 1) syndrome |= parity_pos;
        printf("-> %d unos", count);
        if (count % 2 == 1) {
            syndrome |= parity_pos;
            printf(" (impar) -> S%d=1\n", parity_pos);
        } else {
            printf(" (par) -> S%d=0\n", parity_pos);
        }
    }

    //capa de integridad
    printf("Sindrome: %d\n", syndrome);

    *hubo_error = 0;
    *pos_error = 0;

    if (syndrome != 0) {
        printf("Error detectado en la posición: %d. Corrigiendo...\n", syndrome);
        printf("Posicion indicada por sindrome: %d\n", syndrome);
        // Advertencia para multiples errores
        //en donde se encontro que solo puede encontrar errores de un bit y corregirlos es: 
        // https://es.wikipedia.org/wiki/C%C3%B3digo_Hamming
        //https://espanol.libretexts.org/Ingenieria/Ingenieria_Electrica_(Johnson)/06%3A_Comunicaci%C3%B3n_de_la_informaci%C3%B3n/6.29%3A_C%C3%B3digos_de_correcci%C3%B3n_de_errores_-_C%C3%B3digos_Hamming#:~:text=Si%20ocurre%20m%C3%A1s%20de%20un%20error%20%28aunque%20sea,cambian%20m%C3%A1s%20bits%20de%20lo%20que%20se%20transmiti%C3%B3
        printf("\n/// ADVERTENCIA ///\n");
        printf("\nEl codigo Hamming solo puede:\n");
        printf("1. Corregir 1 error de forma confiable\n");
        printf("2. Detectar (pero NO corregir) 2 errores\n");
        printf("3. Con 3+ errores: comportamiento impredecible y usualmente lo suele empeorar\n");
        

        hamming ^= (1ULL << (syndrome - 1));
        *hubo_error = 1;
        *pos_error = syndrome;
    } else {
        printf("No hay errores detectados\n");
    }

    // Extraer los bits de datos
    uint64_t datos = 0;
    int data_index = 0;
    for (int i = 1; i <= n; i++) {
        if ((i & (i - 1)) != 0) {
            if (hamming & (1ULL << (i - 1))) {
                datos |= (1ULL << (m - 1 - data_index));
            }
            data_index++;
        }
    }

    return datos;
}

// --- Convertir bits a texto ---
void binario_a_texto(uint64_t datos, int m, char* salida) {
    int num_chars = m / 8;
    for (int i = 0; i < num_chars; i++) {
        char c = 0;
        for (int j = 0; j < 8; j++) {
            int bit_index = m - (i * 8 + j) - 1;
            if ((datos >> bit_index) & 1) {
                c |= (1 << (7 - j));
            }
        }
        salida[i] = c;
    }
    salida[num_chars] = '\0'; 
}


//canal con ruido
uint64_t canal_transmision(uint64_t mensaje, int n) {

    char input[10];
    printf("\nAgregar ruido? (s/n): ");
    scanf("%s", input);
    
    if (input[0] == 's' || input[0] == 'S') {
        printf("Cuantos errores introducir? (1-3): ");
        int num_errores;
        scanf("%d", &num_errores);

        printf("Mensaje original: ");
        for (int i = 0; i < n; i++) {
            printf("%d", (int)((mensaje >> i) & 1));
        }
        printf("\n");
        
        for(int i = 0; i < num_errores; i++) {
            printf("Ingrese posicion del error %d (1-based): ", i+1);
            int pos_error;
            scanf("%d", &pos_error);
            //introducir error
            mensaje ^= (1ULL << (pos_error - 1)); 
            printf("error %d introducido en posicion %d\n", i+1, pos_error);
        }


    printf("Mensaje con ruido: ");
        for (int i = 0; i < n; i++) {
            printf("%d", (int)((mensaje >> i) & 1));
        }
        printf("\n");
    } else {
        printf("Transmision sin errores\n");
    }

    return mensaje;

}

// --- EMISOR ---
MensajeTransmision emisor(const char* cadena) {
    char binario[512];
    stringToBinary(cadena, binario);
    int m = strlen(binario);
    validacion(binario);
    uint64_t data = string_to_uint64(binario);
    uint64_t encoded = hamming_encode_general(data, m);

    // Canal con ruido
    uint64_t mensaje_transmitido = canal_transmision(encoded, m + calcular_r(m));

    // Devolver estructura con mensaje final (posiblemente con ruido) y bits de datos
    MensajeTransmision mensaje_final = {mensaje_transmitido, m};
    return mensaje_final;
}

// --- RECEPTOR ---
uint64_t receptor(uint64_t recibido, int m) {
    int hubo_error = 0;
    int pos_error = 0;

    printf("\n--- Receptor ---\n");
    uint64_t datos = hamming_decode_general(recibido, m, &hubo_error, &pos_error);

    printf("Bits de datos decodificados: ");
    for (int i = m - 1; i >= 0; i--) {
        printf("%d", (int)((datos >> i) & 1));
    }
    printf("\n");

    if (m % 8 != 0) {
        printf("Advertencia: no se puede reconstruir texto porque no es múltiplo de 8 bits.\n");
    } else {
        char texto[64];
        binario_a_texto(datos, m, texto);
        printf("Texto reconstruido: '%s'\n", texto);
    }

    if (hubo_error) {
        printf("Se corrigió 1 error en la posición: %d\n", pos_error);
    } else {
        printf("Mensaje sin errores\n");
    }

    return datos;
}
