#include <stdio.h>
#include "strfn.h"
#include "string.h"

int getSubstring(const char *entrada, char *subcadena, int posicionInicial, size_t size_input) {
    if (entrada == NULL || subcadena == NULL || posicionInicial < 0) {
        // Verificar si los argumentos son válidos
        printf("Error1\n");
        return -1; // Error: Argumentos inválidos
    }

    
    if (posicionInicial >= size_input) {
        // La posición inicial está más allá del final de la cadena
        printf("Error2\n");
        return 0; // Tamaño del substring es cero
    }

    // Calcular el tamaño del substring
    size_t tamañoSubstring = size_input - posicionInicial;

    // Copiar el substring a la memoria asignada
    memcpy(subcadena, entrada + posicionInicial, tamañoSubstring);

    // Agregar el carácter nulo al final del substring
    subcadena[tamañoSubstring] = '\0';
    printf("Ok\n");
    //printf("Valor de substring: %s\n", subcadena);
    // Devolver el tamaño del substring
    return (int)tamañoSubstring;
}
