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
    float *msg;
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
    
    ipportreceptor.sin_addr.s_addr=htonl(INADDR_ANY);
    ipportreceptor.sin_family=AF_INET;
    ipportreceptor.sin_port=htons(puerto);
    
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
    
    int tamanoMAX=1000*sizeof(float);
    msg = malloc(tamanoMAX);  // Reservamos memoria para el mensaje, mucha memoria, para asegurarnos de que vaya a caber
    if (msg == NULL) {
        perror("ERROR en la reserva de memoria\n");
        free(msg); // Liberamos si falla
        close(socket_cliente);
        return (EXIT_FAILURE);
    }
    
    int N;
    if((N=recvfrom(socket_cliente,msg,tamanoMAX,0,(struct sockaddr *) &ipportemisor,&size)) < 0)
    {
        perror("No se pudo recibir el mensaje correctamente\n");
        close(socket_cliente);
        return(EXIT_FAILURE);
    }
    
    if (inet_ntop(AF_INET, (const void*) &(ipportemisor.sin_addr), ipconex,INET_ADDRSTRLEN) != NULL){
            printf("Se conectó cliente con IP: %s y puerto %d\n", ipconex, ntohs(ipportemisor.sin_port));
    }
    
    int num=N/sizeof(float);
    printf("El mensaje es: "); // Sacamos por pantalla el mensaje recibido
    for(int i=0;i<num;i++)
    {
        printf("%f, ",msg[i]);
    }
    printf("\n");
    
    printf("El numero de elementos recibidos es %d\n",num);
    
    // Cerramos el socket al acabar el intercambio
    close(socket_cliente);
    return(EXIT_SUCCESS);

}
