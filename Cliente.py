import socket

PUERTO = 8080
BUFFER_TAM = 1024

def generar_usuario(longitud):
    # Envia solicitud al servidor
    mensaje = f"generar_usuario {longitud}"
    cliente.sendall(mensaje.encode())

    # Recibi respuesta del servidor
    respuesta = cliente.recv(BUFFER_TAM).decode()
    return respuesta

def generar_contrasenia(longitud):
    # Envia solicitud al servidor
    mensaje = f"generar_contrasenia {longitud}"
    cliente.sendall(mensaje.encode())

    # Recibe respuesta del servidor
    respuesta = cliente.recv(BUFFER_TAM).decode()
    return respuesta

def main():
    global cliente

    # Crea el socket del cliente
    cliente = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Conecta al servidor
    servidor_addr = ("127.0.0.1", PUERTO)
    cliente.connect(servidor_addr)

    print("Conectado al servidor con exito.")

    while True:
        print("1. Generar nombre de usuario")
        print("2. Generar contrasenia")
        print("3. Salir")
        opcion = input("Ingrese su opcion: ")

        if opcion == "1":
            longitud = int(input("Ingrese la longitud del nombre de usuario: "))
            respuesta = generar_usuario(longitud)
            print("Nombre de usuario generado:", respuesta)
        elif opcion == "2":
            longitud = int(input("Ingrese la longitud de la contrasenia: "))
            respuesta = generar_contrasenia(longitud)
            print("Contrasenia generada:", respuesta)
        elif opcion == "3":
            break
        else:
            print("Opcion invalida")
            continue

    # Cierra el socket del cliente
    cliente.close()

if __name__ == "__main__":
    main()