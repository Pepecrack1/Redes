#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

int main(int argc,char** argv) {
        
	int socket_emisor;  // Identificador del socket del emisor
	struct sockaddr_in ipport_emisor,ipport_receptor; // Estructuras para almacenar la dirección del emisor y receptor
	socklen_t N;
	char mensaje[1024]; // Buffer para el mensaje a enviar y recibir
	socklen_t size = sizeof(struct sockaddr_in);
        u_int16_t puerto_propio;  // Puerto propio del servidor
        
        // Comprobamos que se introduzca el numero de argumentos correcto en línea de comandos
	if (argc == 2)
	{
	    puerto_propio = (u_int16_t) atoi(argv[1]);  // Primer argumento: puerto propio del servidor
	}
	else
	{
	    // Valores por defecto si no se proporcionan argumentos
            printf("No se proporcionaron argumentos correctos, trabajaremos con valores por defecto:  6665\nSi se quisieran introducir argumentos, hacer: ./ejecutable puertoPropio\n");
            puerto_propio = (u_int16_t) 6665;
	}

        /*Creación del socket del cliente
            domain: AF_INET para IPv4
            style: SOCK_DGRAM para no orientado a conexion (UDP)
            protocol: por defecto, 0
        */
	if ((socket_emisor = socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		perror("No se pudo crear el socket\n");
		exit(EXIT_FAILURE);
	}
	
	//Inicializamos la estructura sockaddr del servidor
	ipport_emisor.sin_family = AF_INET; //Familia de direcciones: IPv4
	ipport_emisor.sin_addr.s_addr = htonl(INADDR_ANY);  //Cualquier direccion IP
	ipport_emisor.sin_port = htons(puerto_propio);  //Convertir el puerto de orden de host a orden de red, short

        /*Asignamos una direccion al socket con bind
            socket: identificador del socket
            addr: puntero a struct sockaddr con la direccion a asignar (ipportserv)
            length: tamaño de la estructura addr
        */
	if (bind(socket_emisor,(struct sockaddr*) &ipport_emisor,sizeof(struct sockaddr_in)) < 0) {
		perror("No se pudo asignar direccion\n");
		exit(EXIT_FAILURE);
	}

	while(1) { // utilizamos un bucle para recibir tantos mensajes como envíe el cliente

		N = 0;

		/*Recibimos el mensaje del cliente en minusculas con recvfrom
                    socket: identificador del socket del cliente
                    buffer: puntero a donde se va a guardar el mensaje
                    size: numero maximo de bytes a recibir
                    flags: opciones de recepcion, por defecto 0
                    addr: salida que es un puntero a un struct sockaddr con la dirección de la procedencia del paquete
        .           length_ptr: puntero que es parámetro de entrada indicando el tamaño de la estructura addr y de salida con el espacio real consumido
                */
		if((N = recvfrom(socket_emisor,mensaje,sizeof(mensaje),0,(struct sockaddr *) &ipport_receptor,&size)) < 0)
    	        {
	            perror("No se pudo recibir el mensaje correctamente\n");
            	    continue;
	  	}
	        
	  	size_t len = strlen(mensaje);

		printf("%d bytes recibidos\n",N);


		//Pasamos el mensaje a mayusculas, hasta llegar al fin de cadena
		for (int i = 0; mensaje[i]!='\0'; i++) {
			mensaje[i] = toupper(mensaje[i]);
		}
	        N = 0;

	        /*Enviamos la línea, en minúsculas, al servidor
                    socket: identificador del socket
                    buffer: puntero a mensaje a enviar
                    size: numero maximo de bytes a recibir
                    flags: opciones de recepcion, por defecto 0
                    addr: puntero a un struct sockaddr con la dirección a la que se quiere enviar
                    length: tamaño de la estructura addr
                */
		if ((N = sendto(socket_emisor,mensaje,len+1,0,(struct sockaddr*) &ipport_receptor,size)) < 0) {
		    perror("No se ha podido enviar el mensaje\n");
		    continue;
		}
		
                char ip_str[INET_ADDRSTRLEN];
                /*Convertir la dirección IP de binario a texto
                    af: AF_INET para IPv4
                    src: puntero a una struct in_addr (para IPv4)
                    dst: puntero a cadena donde se guarda el resultado
                    size: tamaño en bytes de la cadena destino
                */
	        inet_ntop(AF_INET,&(ipport_receptor.sin_addr),ip_str,INET_ADDRSTRLEN),
		printf("%d bytes enviados a IP: %s, Puerto: %d\n",N,ip_str,ntohs(ipport_receptor.sin_port));

	}

        //Cerramos el socket del emisor
	close(socket_emisor);

	return 0;
}
