#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

/*==========RECEPTOR==========*/
int main(int argc, char** argv) {
    int socket_cliente;   // Identificador del socket del cliente
    struct sockaddr_in ipportreceptor,ipportemisor;    // Estructuras para almacenar la dirección del emisor y receptor
    socklen_t size = sizeof(struct sockaddr_in);
    socklen_t N;  //Para contar el numero de bytes recibidos
    char msg[50];  // Cliente para el mensaje a enviar y recibir
    int puerto; // Puerto del servidor al que se conecta
    char ipconex[INET_ADDRSTRLEN];
    
    // Comprobamos que se introduzca el numero de argumentos correcto en línea de comandos
    if(argc == 2) {
        puerto = atoi(argv[1]);   // Primer argumento: puerto (convertido de string a entero)
    } else {
        // Valores por defecto si no se proporcionan argumentos
        printf("No se proporcionaron argumentos correctos, trabajaremos con valores por defecto: 6666\nSi se quisieran introducir argumentos, hacer: ./ejecutable puerto\n");
        
        puerto = 6666;    // Puerto por defecto
    }
    
    /*Creación del socket del cliente
        domain: AF_INET para IPv4
        style: SOCK_DGRAM para no orientado a conexion (UDP)
        protocol: por defecto, 0
    */
    if ((socket_cliente = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("No se pudo crear el socket");
        exit(EXIT_FAILURE);
    }
    
    //Inicializamos la estructura sockaddr del receptor
    ipportreceptor.sin_addr.s_addr=htonl(INADDR_ANY);   //Cualquier direccion IP
    ipportreceptor.sin_family=AF_INET;    //Familia de direcciones: IPv4
    ipportreceptor.sin_port=htons(puerto);    //Convertir el puerto de orden de host a orden de red, short
    
    
    
     /*Asignamos una direccion al socket con bind
        socket: identificador del socket
        addr: puntero a struct sockaddr con la direccion a asignar (ipportserv)
        length: tamaño de la estructura addr
    */
    if (bind(socket_cliente, (struct sockaddr *) &ipportreceptor, size) < 0) {
        perror("No se pudo asignar la dirección");
        close(socket_cliente);
        exit(EXIT_FAILURE);
    }
    
    
    /*Recibimos el mensaje del emisor con recvfrom
            socket: identificador del socket del cliente
            buffer: puntero a donde se va a guardar el mensaje
            size: numero maximo de bytes a recibir
            flags: opciones de recepcion, por defecto 0
            addr: salida que es un puntero a un struct sockaddr con la dirección de la procedencia del paquete
.           length_ptr: puntero que es parámetro de entrada indicando el tamaño de la estructura addr y de salida con el espacio real consumido
        */
    if((N=recvfrom(socket_cliente,msg,sizeof(msg),0,(struct sockaddr *) &ipportemisor,&size)) < 0)
    {
        perror("No se pudo recibir el mensaje correctamente\n");
        close(socket_cliente);
        return(EXIT_FAILURE);
    }    
    
    /*Convertir la dirección IP de binario a texto
            af: AF_INET para IPv4
            src: puntero a una struct in_addr (para IPv4)
            dst: puntero a cadena donde se guarda el resultado
            size: tamaño en bytes de la cadena destino
        */
    if (inet_ntop(AF_INET, (const void*) &(ipportemisor.sin_addr), ipconex,INET_ADDRSTRLEN) != NULL){
            printf("Se conectó cliente con IP: %s y puerto %d\n", ipconex, ntohs(ipportemisor.sin_port));
    }
    

    msg[50]='\0';   //Terminamo el mensaje con caracter de fin de palabra, para evitar errores
    //Sacamos por pantalla el mensaje recibido
    printf("El mensaje es: %s, numero bytes recibidos %d\n",msg,N);
    
    //Cerramos el socket al acabar el intercambio
    close(socket_cliente);
    return(EXIT_SUCCESS);

}

/*
DF completa: determinante es una clave candidata entera
DF transitiva: no coincide con ninguna CC
DF parcial: es parte de una CC
*/
