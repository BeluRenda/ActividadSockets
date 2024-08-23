#include <winsock2.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib") // Para que linkee ws2_32.lib para los sockets

#define PUERTO 8080
#define BUFFER_TAM 1024

int main() {
    WSADATA wsaData;
    SOCKET socketCliente;
    struct sockaddr_in direccionServidor;
    char buffer[BUFFER_TAM];
    int bytesEnviados;
    int opciones;
    int longitud;

    // Inicializa Winsock
    // DESCRIP:
    // Si el valor que devuelve es distnito de 0, tira excepcion
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error al inicializar Winsock \n");
        return 1;
    }

    // Crea el socket del cliente
    // DESCRIP:
    // Creo el socket y verifico si se creo con exito, sino tira excepcion
    socketCliente = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketCliente == INVALID_SOCKET) {
        printf("Error al crear el socket del cliente \n");
        WSACleanup();
        return 1;
    }

    // Establece la dir del servidor
    // DESCRIP:
    // Configuro la dir del servidor, uso IPv4, el puerto 8080 (que lo estableci como PUERTO con el define) y la dir IP 127.0.0.1
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_port = htons(PUERTO);
    inet_pton(AF_INET, "127.0.0.1", &direccionServidor.sin_addr);

    // Conexion al servidor
    // DESCRIP:
    // Intenta establecer una conexion al servidor usando el socket cliente, si falla tira excepcion y limpia con WSACleanup para evitar problemas de memoria
    if (connect(socketCliente, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor)) == SOCKET_ERROR) {
        printf("Error al conectar al servidor\n");
        closesocket(socketCliente);
        WSACleanup();
        return 1;
    }
     // Bucle principal con el menu de opciones
     // DESCRIP:
     // Permite al cliente interactuar con el menu, generar usuario o contrasenia y salir voluntariamente
     // Si se selecciona una opcion por fuera del 1-3, tira excepcion
     // Nota: Se puede hacer tanto con un if como con un switch el menu
    while (1) {
        printf("Conectado al servidor con exito. \n");
        printf("1. Generar nombre de usuario\n");
        printf("2. Generar contrasenia\n");
        printf("3. Salir\n");
        scanf("%d", &opciones);

        if (opciones == 1) {
            printf("Ingrese la longitud del nombre de usuario: ");
            scanf("%d", &longitud);
            sprintf(buffer, "generar_usuario %d", longitud);
        } else if (opciones == 2) {
            printf("Ingrese la longitud de la contrasenia: ");
            scanf("%d", &longitud);
            sprintf(buffer, "generar_contrasenia %d", longitud);
        } else if (opciones == 3) {
            break; // Sale (y cierra conexion)
        } else {
            printf("Opcion invalida\n");
            continue; // Continua a lo siguiente en el while
        }

        // Envia la solicitud al servidor
        // DESCRIP:
        // Envia la solicitud al servidor con el send() mediante el socket y verifica si se mando bien, sino tira excepcion y limpia con WSACleanup()
        bytesEnviados = send(socketCliente, buffer, strlen(buffer), 0);
        if (bytesEnviados == SOCKET_ERROR) {
            printf("Error al enviar la solicitud al servidor\n");
            closesocket(socketCliente);
            WSACleanup();
            return 1;
        }

        // Recibe la respuesta del servidor
        // DESCRIP:
        // Recibe la respuesta del servidor, verifica si es valida y si lo es agrega un caracter nulo '\0' al final del buffer
        // (esto lo hago para que el buffer sea una cadena de caracteres valida)
        // Si NO recibe una respuesta valida, tira expcepcion
        bytesEnviados = recv(socketCliente, buffer, BUFFER_TAM - 1, 0);
        if (bytesEnviados > 0) {
            buffer[bytesEnviados] = '\0'; // Se agrega al final de la cadena
            printf("El servidor respondio: %s\n", buffer);
        } else {
            printf("Error al recibir respuesta del servidor\n");
        }
    }

    // Cierra el socket del cliente y limpia con WSACleanup()
    closesocket(socketCliente);
    WSACleanup();

    return 0;
}
