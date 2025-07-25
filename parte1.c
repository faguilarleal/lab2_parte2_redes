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

// calcular el sindrome y detecta errores
int calcular_sindrome(uint64_t hamming, int n, int r) {
    int syndrome = 0;
    
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
    
    return syndrome;
}

//receptor:
//recibe como un mensaje (la salida del emisor) que sigue un protocolo 
//realizar la deteccion/correccion de errores ( "el bit de parierdad es 0, pero vemos un numero impar de 1s, por lo que hubo error" )
//3.devolver la informacion en cada caso: 
//a. No se detectaron errores: mostrar la trama recibida 
//b. Se detectaron errores: indicar que la trama se descarta por detectar errores.
//c. Se detectaron y corrigieron errores: indicar que se corrigieron errores, indicar
//posicion de los bits que se corrigieron y mostrar la trama corregida
uint64_t receptor(uint64_t hamming, int syndrome) {
    // Advertencia para multiples errores
    //en donde se encontro que solo puede encontrar errores de un bit y corregirlos es: 
    // https://es.wikipedia.org/wiki/C%C3%B3digo_Hamming
    //https://espanol.libretexts.org/Ingenieria/Ingenieria_Electrica_(Johnson)/06%3A_Comunicaci%C3%B3n_de_la_informaci%C3%B3n/6.29%3A_C%C3%B3digos_de_correcci%C3%B3n_de_errores_-_C%C3%B3digos_Hamming#:~:text=Si%20ocurre%20m%C3%A1s%20de%20un%20error%20%28aunque%20sea,cambian%20m%C3%A1s%20bits%20de%20lo%20que%20se%20transmiti%C3%B3
    printf("\n/// ADVERTENCIA ///\n");
    printf("\nEl codigo Hamming solo puede:\n");
    printf("1. Corregir 1 error de forma confiable\n");
    printf("2. Detectar, pero no corregir, 2 errores\n");
    printf("3. Con 3+ errores: comportamiento impredecible y usualmente lo suele empeorar\n");
    
    char respuesta;
    printf("\nIntentar correccion? (s/n): ");
    scanf(" %c", &respuesta);
    
    if (respuesta == 's' || respuesta == 'S') {
        // Corregir error
        hamming ^= (1ULL << (syndrome - 1));
        printf("Correccion aplicada en posicion %d\n", syndrome);
        return hamming;
    } else {
        printf("Mensaje rechazado por errores detectados\n");
        return 0; 
    }
}

// para cualquier codigo(n,m) que cumpla (m+r+1)<=2^r
// m = numero de bits de datos
// r= numero de bits de pariedad
uint64_t hamming_decode_general(uint64_t hamming, int n) {
    // Calcular m (bits de datos) y r (bits de paridad) basado en n (total)
    int r = 0;
    int m = 0;
    
    //r contando potencias de 2 hasta n
    for (int i = 1; i <= n; i <<= 1) {
        if (i <= n) r++;
    }
    m = n - r;

    printf("\n--- DECODIFICACION ---\n");
    printf("Codigo recibido (%d bits): ", n);
    for (int i = 0; i < n; i++) {
        printf("%d", (int)((hamming >> i) & 1));
    }
    printf("\n");
    printf("Bits de datos: %d, Bits de paridad: %d\n", m, r);

    //calcular sindrome
    int syndrome = calcular_sindrome(hamming, n, r);
    printf("Sindrome: %d\n", syndrome);


    if (syndrome == 0) {
        printf("No hay errores detectados\n");
    } else {
        printf("Error detectado - Sindrome: %d\n", syndrome);
        printf("Posicion indicada por sindrome: %d\n", syndrome);
        
        printf("\n--- Receptor ---\n");
        hamming = receptor(hamming, syndrome);
        if (hamming == 0) {
            return 0; // Mensaje rechazado
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
    for (int i = 0; i < m; i++) {
        printf("%d", (int)((datos >> i) & 1));
    }
    printf("\n");

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

//emisor: 
uint64_t emisor(){
    //1. solicirar la trama en binario
    char cadena[65];

    printf("Ingrese el codigo Hamming : ");
    scanf("%64s", cadena);

    int n = strlen(cadena);
    printf("Longitud de la información: %d bits\n", n);

    validacion(cadena, n);

    uint64_t data = string_to_uint64(cadena);
    printf("En decimal: %llu\n", (unsigned long long)data);

    // Mostrar el código bit por bit
    printf("Codigo recibido en binario: ");
    for (int i = n - 1; i >= 0; i--) {
        printf("%c", ((data >> i) & 1) ? '1' : '0');
    }
    printf("\n");
    //2. ejecutar el algoritmo y obtener la infomacion  
    //que requiere para comprobar la integridad del mensaje
    return hamming_decode_general(data, n);
}



int main(){
    // printf("\n=== CORRECION DE ERRORES ===\n");
    printf("\n=== Bienvenido a mensajeria 3000 ===\n");
    printf("\n=== CORRECION DE ERRORES ===\n");

    printf("Nota: Ingrese un codigo Hamming que ya contenga un posible error, con su pariedad\n");
    printf("\n--- Emisor ---\n");
    emisor();

    return 0;
}