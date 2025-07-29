#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

// -- Correcion de errores --

//////////// codigo de Hamming //////////////

// ---- Calcular r (bits de paridad) ----
int calcular_r(int m) {
    int r = 0;
    while ((m + r + 1) > (1 << r)) {
        r++;
    }
    return r;
}

//capa de presentacion
// para cualquier codigo(n,m) que cumpla (m+r+1)<=2^r
// m = numero de bits de datos
// r= numero de bits de pariedad
// n= m + r, NO +1 (ese era el error principal)
uint64_t hamming_ecode_general(uint64_t data, int m){

    // // Mostrar el numero en binario 
    // printf("Datos originales en binario: ");
    // for (int i = m - 1; i >= 0; i--) {
    //     printf("%c", ((data >> i) & 1) ? '1' : '0');
    // }
    // printf("\n");

    //determinar bits de pariedad
    int r = calcular_r(m);
    printf("Bits de pariedad necesarios: %d\n", r);

    //total de bits con pariedad 
    int n = m + r;
    printf("Total de bits en código Hamming: %d\n", n);

    //posicionar los bits de datos (No de paridad inicialmente)
    uint64_t hamming = 0;
    int data_index = 0; 
    
    printf("\n---- Posicionamiento de bits: ----\n");
    printf("\n (leer de abajo para arriba) \n");

    for (int i = 1; i <= n; i++) {
        // Si la posicion no es potencia de 2, es para datos
        if ((i & (i - 1)) != 0) {
            //extraer bit de datos de derecha a izquierda
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
                printf("%d ", i);
                if (hamming & (1ULL << (i - 1))) {
                    count++;
                }
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

//capa de presentacion
uint64_t hamming_decode_general(uint64_t hamming, int m) {
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
                printf("%d ", i);
                if (hamming & (1ULL << (i - 1))) {
                    count++;
                }
            }
        }

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

    if (syndrome == 0) {
        printf("No hay errores detectados\n");
    } else {
        printf("Error detectado - Sindrome: %d\n", syndrome);
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
        
        char respuesta;
        printf("\nIntentar correccion? (s/n): ");
        scanf(" %c", &respuesta);
        
        if (respuesta == 's' || respuesta == 'S') {
            // Corregir error
            hamming ^= (1ULL << (syndrome - 1));
            printf("Correccion aplicada en posicion %d\n", syndrome);
        } else {
            printf("Mensaje rechazado por errores detectados\n");
            return 0; 
        }
    }

    //extraer bits de datos
    uint64_t datos = 0;
    int data_index = 0;
    
    for (int i = 1; i <= n; i++) {
        if ((i & (i - 1)) != 0) { // no es potencia de 2
            if (hamming & (1ULL << (i - 1))) {
                datos |= (1ULL << (m - 1 - data_index));
            }
            data_index++;
        }
    }

    printf("Trama corregida: ");
    for (int i = m - 1; i >= 0; i--) {
        printf("%d", (int)((datos >> i) & 1));
    }
    printf("\n");
    //for (int i = 0; i < m; i++) {
    //     printf("%d", (int)((datos >> i) & 1));
    // }
    // printf("\n");

    return datos;
}

// --- Conversion de cadena a numero ---
uint64_t string_to_uint64(const char *cadena_de_bits)
{
    uint64_t valor = 0;     //uint64_t es para numeros binarios

    for (int i = 0; i < strlen(cadena_de_bits); i++) {
        // Desplaza un bit a la izquierda, (como multiplicar 2 en binario), eso se hace para preparar el prox bit a aniadir
        valor <<= 1;
        //si el caracter es 1, entonces se activa el bit mas a la derecha del numero, usando or bitwise
        if (cadena_de_bits[i] == '1') {
            /// coloca un 1 en el bit menos significativo
            valor |= 1;                        
        }
    }
    return valor;
}

// Validar que el input solo contenga 0s y 1s
void validacion(char* codigo, int longitud) {
    for(int i = 0; i < strlen(codigo); i++) {
        if(codigo[i] != '0' && codigo[i] != '1') {
            printf("Error: Solo se permiten 0s y 1s\n");
            exit(1);
        }
    }
}


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


typedef struct {
    uint64_t mensaje_codificado;
    int bits_datos;
} MensajeTransmision;

//emisor: 
MensajeTransmision emisor(){

    //capa de aplicacion
    //0. ingresar mensaje
    char cadena[64];
    printf("Ingrese el mensaje (max 7 caracteres) : ");
    scanf("%s", cadena);

    printf("cadena ingresada: '%s'\n", cadena);


    printf("Representacion binaria: ");
    char binario[512];  // 64 * 8 = 512 bits máximo
    // charToBinary(cadena);
    stringToBinary(cadena, binario);   
    

    int m = strlen(binario);
    printf("Longitud de la información: %d bits\n", m);

    validacion(binario, m);

    uint64_t data = string_to_uint64(binario);
    printf("En decimal: %llu\n", (unsigned long long)data);

    //2. ejecutar el algoritmo y obtener la infomacion  
    //que requiere para comprobar la integridad del mensaje
    uint64_t encoded = hamming_ecode_general(data, m);
    //3. devolver el mensaje en binario concatenado con la informacion
    //adicional requerida para la correccion de errores
    MensajeTransmision resultado = {encoded, m};
    return resultado;
}

//receptor:
//recibe como un mensaje (la salida del emisor) que sigue un protocolo 
//realizar la deteccion/correccion de errores ( "el bit de parierdad es 0, pero vemos un numero impar de 1s, por lo que hubo error" )
//3.devolver la informacion en cada caso: 
//a. No se detectaron errores: mostrar la trama recibida 
//b. Se detectaron errores: indicar que la trama se descarta por detectar errores.
//c. Se detectaron y corrigieron errores: indicar que se corrigieron errores, indicar
//posicion de los bits que se corrigieron y mostrar la trama corregida
uint64_t receptor(uint64_t recibido, int m){ //, int m
    printf("\n--- Receptor  ---\n");
    return hamming_decode_general(recibido, m);
}


int main(){
    printf("\n=== Bienvenido a mensajeria 3000 ===\n");
    printf("\n--- Emisor ---\n");
    MensajeTransmision mensaje_original = emisor();

    int r = calcular_r(mensaje_original.bits_datos);
    int n = mensaje_original.bits_datos + r;
    uint64_t mensaje_transmitido = canal_transmision(mensaje_original.mensaje_codificado, n);

    receptor(mensaje_transmitido, mensaje_original.bits_datos);

    return 0;
}