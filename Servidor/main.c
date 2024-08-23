#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib") // Para que linkee ws2_32.lib para los sockets

#define PUERTO 8080
#define BUFFER_TAM 1024

void generar_usuario(char* buffer, int longitud) {
    char vocales[] = "aeiouAEIOU";
    char consonantes[] = "bcdfghjklmnpqrstvwxyzBCDFGHJKLMNPQRSTVWXYZ";
    int longitudVocales = strlen(vocales);
    int longitudConsonantes = strlen(consonantes);

    if (longitud < 5 || longitud > 16) {
        sprintf(buffer, "Error: La longitud del usuario debe ser entre 5 y 15 caracteres");
        return;
    }

    for (int i = 0; i < longitud; i++) {
        if (i % 2 == 0) { // Alterna entre vocales y consonantes
            buffer[i] = vocales[rand() % longitudVocales];
        } else {
            buffer[i] = consonantes[rand() % longitudConsonantes];
        }
    }
    // Agrega un caracter nulo '\0' al final del buffer
    // (esto lo hago para que el buffer sea una cadena de caracteres valida)
    buffer[longitud] = '\0'; // Agrega fin de cadena
}

void generar_contrasenia(char* buffer, int longitud) {
    if (longitud < 8 || longitud >= 50) {
        sprintf(buffer, "Error: La longitud de la contrasenia debe ser entre 8 y 50 caracteres");
        return;
    }

    char caracteres[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()_+-=";
    int longitudCaracteres = strlen(caracteres);

    for (int i = 0; i < longitud; i++) {
        buffer[i] = caracteres[rand() % longitudCaracteres];
    }
    // Agrega un caracter nulo '\0' al final del buffer
    // (esto lo hago para que el buffer sea una cadena de caracteres valida)
    buffer[longitud] = '\0'; // Se agrega al final de la cadena
}

int main() {
    WSADATA wsaData;
    SOCKET socketServidor, socketCliente;
    struct sockaddr_in direccionServidor, direccionCliente;
    char buffer[BUFFER_TAM];
    int bytesRecibidos;
    int longitudCliente = sizeof(direccionCliente);

    // Inicializa Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error al inicializar Winsock\n");
        return 1;
    }

    // Crea el socket del servidor
    // DESCRIP:
    // Creo el socket y verifico si se creo con exito, sino tira excepcion
    socketServidor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketServidor == INVALID_SOCKET) {
        printf("Error al crear socket del servidor\n");
        WSACleanup();
        return 1;
    }

    // Establece la dir del servidor
    // DESCRIP:
    // Configuro la dir del servidor, uso IPv4, el puerto 8080 (que lo estableci como PUERTO con el define) y la dir IP 127.0.0.1
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_port = htons(PUERTO);
    inet_pton(AF_INET, "127.0.0.1", &direccionServidor.sin_addr);

    // Enlaza socket del servidor con la dir
    // DESCRIP:
    // Enlaza el socket del servidor a la dir y puerto usando bind()
    // Si se produce un error, tira excepcion y limpia con WSACleanup()
    if (bind(socketServidor, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor)) == SOCKET_ERROR) {
        printf("Error al enlazar socket del servidor\n");
        closesocket(socketServidor);
        WSACleanup();
        return 1;
    }

    // Escucha conexiones entrantes
    // DESCRIP:
    // Configura el socket del sevidor para escuchar conexiones entrantes usando listen()
    // Si se produce un error, tira excepcion y limpia con WSACleanup()
    if (listen(socketServidor, 1) == SOCKET_ERROR) {
        printf("Error al escuchar conexiones entrantes\n");
        closesocket(socketServidor);
        WSACleanup();
        return 1;
    }

    // Avisa que inicio y que espera conexion
    printf("Servidor iniciado. Esperando conexiones...\n");

    // Acepta la conexion entrante
    // DESCRIP:
    // Acepta la conexion entrante al servidor usando accept()
    // Si se produce un error, tira excepcion y limpia con WSACleanup()
    socketCliente = accept(socketServidor, (struct sockaddr *)&direccionCliente, &longitudCliente);
    if (socketCliente == INVALID_SOCKET) {
        printf("Error al aceptar conexion entrante\n");
        closesocket(socketServidor);
        WSACleanup();
        return 1;
    }

    // Avisa que se pudo conectar exitosamente
    printf("Conexion establecida con el cliente\n");

    srand(time(NULL)); // Inicializa el generador de numeros aleatorios

    while (1) {
    // Recibe las solicitudes del cliente
    bytesRecibidos = recv(socketCliente, buffer, BUFFER_TAM - 1, 0);
        if (bytesRecibidos > 0) {
            // Agrega un caracter nulo '\0' al final del buffer
            // (esto lo hago para que el buffer sea una cadena de caracteres valida)
            buffer[bytesRecibidos] = '\0'; // Se agrega al final de la cadena
            printf("El cliente solicito: %s\n", buffer);

            // Procesa las solicitudes del cliente
            // DESCRIP:
            // Tiene las solicitudes de generar_usuario y generar_contrasenia
            // Recupera el siguiente token de la cedena con el strtok y lo convierte a un int con la funcion atoi()
            // Llama a generar_usuario o generar_contrasenia con el buffer y el valor int como parametros
            //Muestra por consola la respuesta
            char* token = strtok(buffer, " ");
            if (strcmp(token, "generar_usuario") == 0) {
                token = strtok(NULL, " ");
                int longitud = atoi(token);
                generar_usuario(buffer, longitud);
                // printf("El servidor respondio: %s\n", buffer);
            } else if (strcmp(token, "generar_contrasenia") == 0) {
                token = strtok(NULL, " ");
                int longitud = atoi(token);
                generar_contrasenia(buffer, longitud);
                // printf("El servidor respondio: %s\n", buffer);
            } else {
                sprintf(buffer, "ERROR: Solicitud invalida, pruebe de vuelta.");
                // printf("El servidor respondio: %s\n", buffer);
            }

            // Envia respuesta al cliente
            // DESCRIP:
            // Si bytesRecibidos es distinto de 0 y no se cumple lo anterior, hubo error recibiendo datos del cliente, tira expcecion y sale del bucle con el break
            send(socketCliente, buffer, strlen(buffer), 0);
        } else if (bytesRecibidos == 0) {
            printf("El cliente cerro la conexion\n");
            break;
        } else {
            printf("Error al recibir datos del cliente\n");
            break;
        }

        // Avisa que mantiene la conexión abierta
        printf("El servidor esta listo para recibir la proxima solicitud...\n");
    }

    // Cierra el socket del cliente
    closesocket(socketCliente);

    // Cierra el socket del servidor y limpia con WSACleanup()
    closesocket(socketServidor);
    WSACleanup();

    return 0;
}
