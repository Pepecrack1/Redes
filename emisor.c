#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

/*==========EMISOR==========*/
int main(int argc,char** argv) {
      
        u_int16_t puerto_propio;  // Puerto del servidor y receptor
	u_int16_t puerto_receptor;
	char ip_receptor[INET_ADDRSTRLEN];    //Ip del receptor
	int socket_emisor;  //Identificador del socket emisor
	struct sockaddr_in ipport_emisor,ipport_receptor;   // Estructuras para almacenar la dirección del emisor y receptor
	socklen_t N;  //Para contar el numero de bytes enviados
	socklen_t size = sizeof(struct sockaddr_in);
	char mensaje[] = "Mensaje 1";

        // detectamos argumentos y hacemos la conversion de tipos
	if (argc!=4) 
	{
	    // Valores por defecto si no se proporcionan argumentos
            printf("No se proporcionaron argumentos correctos, trabajaremos con valores por defecto: 6665 127.0.0.1 6666\nSi se quisieran introducir argumentos, hacer: ./ejecutable puertoPropio ipReceptor puertoReceptor\n");
            puerto_propio = 6665;    // Puerto por defecto
            puerto_receptor= 6666;
            strcpy(ip_receptor,"127.0.0.1");
        }
        else
        {
            puerto_propio=(u_int16_t) atoi(argv[1]);
            puerto_receptor=(u_int16_t) atoi(argv[3]);
            strcpy(ip_receptor,argv[2]);
          
        }

        // creamos el socket del servidor
	if ((socket_emisor = socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		perror("No se pudo crear el socket\n");
		exit(EXIT_FAILURE);
	}
	
	// Inicializamos la estructura sockaddr del emisor
	ipport_emisor.sin_family = AF_INET;   //Familia de direcciones: IPv4
	ipport_emisor.sin_addr.s_addr = htonl(INADDR_ANY);    //Cualquier direccion IP
	ipport_emisor.sin_port = htons(puerto_propio);    //Convertir el puerto de orden de host a orden de red, short
        
        // Inicializamos la estructura sockaddr del receptor
	ipport_receptor.sin_family = AF_INET;   //Familia de direcciones: IPv4
	ipport_receptor.sin_port = htons(puerto_receptor);    //Convertir el puerto de orden de host a orden de red, short
        
        /*Convertir la dirección IP del receptor de texto a binario--------------------------CAMBIAR
            af: AF_INET para IPv4
            src: puntero a una struct in_addr (para IPv4)
            dst: puntero a cadena donde se guarda el resultado
        */
	if (inet_pton(AF_INET, ip_receptor, &ipport_receptor.sin_addr) < 0) {
            perror("Error en inet_pton\n");
            exit(EXIT_FAILURE);
 	}

        /*Asignamos una direccion al socket con bind
          socket: identificador del socket
          addr: puntero a struct sockaddr con la direccion a asignar (ipportserv)
          length: tamaño de la estructura addr
        */
	if (bind(socket_emisor,(struct sockaddr*) &ipport_emisor,size) < 0) {
		perror("No se pudo asignar direccion\n");
		exit(EXIT_FAILURE);
	}
        
	char ip_str[INET_ADDRSTRLEN];
	/*Convertir la dirección IP de binario a texto
            af: AF_INET para IPv4
            src: puntero a una struct in_addr (para IPv4)
            dst: puntero a cadena donde se guarda el resultado
            size: tamaño en bytes de la cadena destino
        */
	inet_ntop(AF_INET,&(ipport_receptor.sin_addr),ip_str,INET_ADDRSTRLEN);
	
	printf("IP: %s\nPuerto: %d\n",ip_str,ntohs(ipport_receptor.sin_port));
	
	size_t nBytes = strlen(mensaje);

   	/* enviamos mensaje con sendto
   	    socket: entero identificador del socket
            buffer: puntero a los datos a enviar
            size: número de bytes a enviar
            flags: opciones del envío. Por defecto 0
            addr: puntero a un struct sockaddr con la dirección a la que se quiere enviar
            length: tamaño de la estructura addr
   	*/
	if ((N = sendto(socket_emisor,mensaje,nBytes,0,(struct sockaddr*) &ipport_receptor,size)) < 0) {
		perror("No se ha podido enviar el mensaje\n");
		close(socket_emisor);
		exit(EXIT_FAILURE);
	}

	printf("%d/%ld bytes enviados\n",N,nBytes); //Imprimimos el numero de bytes enviados

    	// Cerramos el socket del emisor
	close(socket_emisor);

	return 0;
}
