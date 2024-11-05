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

int main(int argc,char** argv) {	// args: puerto_propio ip puerto_destinatario 

    // detectamos argumentos y hacemos la conversion de tipos
	if (argc != 2) { perror("No detectaron argumentos\n"); exit(EXIT_FAILURE); }
	u_int16_t puerto_propio = (u_int16_t) atoi(argv[1]);

    //declaraciones previas
	int socket_emisor;
	struct sockaddr_in ipport_emisor,ipport_receptor;
	socklen_t N;
	char mensaje[1024];

    // creamos el socket del servidor
	if ((socket_emisor = socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		perror("No se pudo crear el socket\n");
		exit(EXIT_FAILURE);
	}
	
	// modificamos la estructura sockaddr_in para introducir el puerto y la ip
	ipport_emisor.sin_family = AF_INET;
	ipport_emisor.sin_addr.s_addr = htonl(INADDR_ANY);
	ipport_emisor.sin_port = htons(puerto_propio);

    // asociamos al socket la direccion y el puerto al que nos queremos conectar
	if (bind(socket_emisor,(struct sockaddr*) &ipport_emisor,sizeof(struct sockaddr_in)) < 0) {
		perror("No se pudo asignar direccion\n");
		exit(EXIT_FAILURE);
	}

	socklen_t length_ptr = sizeof(struct sockaddr_in);

	size_t nBytes = 0;


	while(1) {

		N = 0;

		if((N = recvfrom(socket_emisor,mensaje,sizeof(mensaje),0,(struct sockaddr *) &ipport_receptor,&length_ptr)) < 0)
    	        {
	            perror("No se pudo recibir el mensaje correctamente\n");
            	    continue;
	  	}
	        
	  	nBytes = strlen(mensaje) + 1;

		printf("%d bytes recibidos\n",N);



		for (int i = 0; mensaje[i]!='\0'; i++) {
			mensaje[i] = toupper(mensaje[i]);
		}
	        N = 0;
		if ((N = sendto(socket_emisor,mensaje,nBytes,0,(struct sockaddr*) &ipport_receptor,length_ptr)) < 0) {
		    perror("No se ha podido enviar el mensaje\n");
		    continue;
		}
		
                char ip_str[INET_ADDRSTRLEN];
	        inet_ntop(AF_INET,&(ipport_receptor.sin_addr),ip_str,INET_ADDRSTRLEN),
		printf("%d bytes enviados a IP: %s, Puerto: %d\n",N,ip_str,ntohs(ipport_receptor.sin_port));

	}

    // cerramos el socket del emisor
	close(socket_emisor);

	return 0;
}
