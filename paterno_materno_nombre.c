#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Cliente {
    char rut[12];
    char nombre[100];
    int entradas_solicitadas;
    int entradas_asignadas;
    struct Cliente *siguiente;
} Cliente;

Cliente *crear_cliente(char *rut, char *nombre, int entradas_solicitadas) {
    Cliente *cliente = (Cliente *)malloc(sizeof(Cliente));
    strcpy(cliente->rut, rut);
    strcpy(cliente->nombre, nombre);
    cliente->entradas_solicitadas = entradas_solicitadas;
    cliente->entradas_asignadas = 0;
    cliente->siguiente = NULL;
    return cliente;
}

Cliente *insertar_cliente(Cliente *lista, Cliente *nuevo) {
    nuevo->siguiente = lista;
    return nuevo;
}

Cliente *buscar_cliente(Cliente *lista, char *rut) {
    Cliente *actual = lista;
    while (actual != NULL) {
        if (strcmp(actual->rut, rut) == 0) {
            return actual;
        }
        actual = actual->siguiente;
    }
    return NULL;
}

void eliminar_lista(Cliente *lista) {
    Cliente *actual;
    while (lista != NULL) {
        actual = lista;
        lista = lista->siguiente;
        free(actual);
    }
}

void procesar_entrada(char *archivo_entrada, char *archivo_asignadas, char *archivo_espera) {
    FILE *entrada = fopen(archivo_entrada, "r");
    FILE *asg = fopen(archivo_asignadas, "w");
    FILE *esp = fopen(archivo_espera, "w");

    Cliente *clientes = NULL;
    Cliente *espera = NULL;
    int entradas_asignadas = 0;
    int entradas_pendientes = 50;

    char rut[12];
    char nombre[100];
    int entradas_solicitadas;

    while (fscanf(entrada, "%[^,],%[^,],%d%*c", rut, nombre, &entradas_solicitadas) == 3) {
        Cliente *cliente = buscar_cliente(clientes, rut);

        if (cliente == NULL) {
            cliente = crear_cliente(rut, nombre, entradas_solicitadas);
            clientes = insertar_cliente(clientes, cliente);
        } else {
            cliente->entradas_solicitadas += entradas_solicitadas;
        }

        if (entradas_pendientes > 0) {
            if (cliente->entradas_solicitadas <= 2) {
                int entradas_a_asignar = cliente->entradas_solicitadas - cliente->entradas_asignadas;
                if (entradas_pendientes >= entradas_a_asignar) {
                    cliente->entradas_asignadas += entradas_a_asignar;
                    entradas_asignadas += entradas_a_asignar;
                    entradas_pendientes -= entradas_a_asignar;
                } else {
                    cliente->entradas_asignadas += entradas_pendientes;
                    entradas_asignadas += entradas_pendientes;
                    entradas_pendientes = 0;
                }
                            } else if (cliente->entradas_solicitadas >= 3 && cliente->entradas_solicitadas <= 5) {
                cliente->entradas_asignadas = 2;
                entradas_asignadas += 2;
                entradas_pendientes -= 2;
            } else {
                cliente->entradas_asignadas = 1;
                entradas_asignadas += 1;
                entradas_pendientes -= 1;
            }
        }

        if (cliente->entradas_asignadas < cliente->entradas_solicitadas) {
            Cliente *cliente_espera = crear_cliente(rut, nombre, cliente->entradas_solicitadas - cliente->entradas_asignadas);
            espera = insertar_cliente(espera, cliente_espera);
        }
    }

    Cliente *actual = clientes;
    while (actual != NULL) {
        fprintf(asg, "%s,%s,%d\n", actual->rut, actual->nombre, actual->entradas_asignadas);
        actual = actual->siguiente;
    }

    actual = espera;
    while (actual != NULL) {
        fprintf(esp, "%s,%s,%d\n", actual->rut, actual->nombre, actual->entradas_solicitadas);
        actual = actual->siguiente;
    }

    eliminar_lista(clientes);
    eliminar_lista(espera);

    fclose(entrada);
    fclose(asg);
    fclose(esp);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Uso: %s <archivo_entrada> <archivo_asignadas> <archivo_espera>\n", argv[0]);
        return 1;
    }

    procesar_entrada(argv[1], argv[2], argv[3]);

    return 0;
}

