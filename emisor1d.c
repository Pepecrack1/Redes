#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc,char** argv) {	// args: puerto_propio ip puerto_destinatario 

        // detectamos argumentos y hacemos la conversion de tipos
	if (argc < 2) { perror("No detectaron argumentos\n"); exit(EXIT_FAILURE); }
	u_int16_t puerto_propio = (u_int16_t) atoi(argv[1]);
	u_int16_t puerto_destinatario = (u_int16_t) atoi(argv[3]);

        //declaraciones previas
	int socket_emisor;
	struct sockaddr_in ipport_emisor,ipport_receptor;
	socklen_t N;
	char mensaje[] = "Mensaje 1";

	float array[3];
	array[0] = 3.2;
	array[1] = 2.83;
	array[2] = 0.69;

        // creamos el socket del servidor
	if ((socket_emisor = socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		perror("No se pudo crear el socket\n");
		exit(EXIT_FAILURE);
	}
	
	// modificamos la estructura sockaddr_in para introducir el puerto y la ip
	ipport_emisor.sin_family = AF_INET;
	ipport_emisor.sin_addr.s_addr = htonl(INADDR_ANY);
	ipport_emisor.sin_port = htons(puerto_propio);

	ipport_receptor.sin_family = AF_INET;
	ipport_receptor.sin_port = htons(puerto_destinatario);

	if (inet_pton(AF_INET, argv[2], &ipport_receptor.sin_addr) <= 0) {
        perror("Error en inet_pton\n");
        exit(EXIT_FAILURE);
    }

        // asociamos al socket la direccion y el puerto al que nos queremos conectar
	if (bind(socket_emisor,(struct sockaddr*) &ipport_emisor,sizeof(struct sockaddr_in)) < 0) {
		perror("No se pudo asignar direccion\n");
		exit(EXIT_FAILURE);
	}

	socklen_t length_ptr = sizeof(struct sockaddr_in);

	char ip_str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET,&(ipport_receptor.sin_addr),ip_str,INET_ADDRSTRLEN),
	printf("IP: %s\nPuerto: %d\n",ip_str,ntohs(ipport_receptor.sin_port));
	size_t nBytes = strlen(mensaje);
	nBytes = sizeof(array);

	if ((N = sendto(socket_emisor,&nBytes,sizeof(int),0,(struct sockaddr*) &ipport_receptor,length_ptr)) < 0) {
		perror("No se ha podido enviar el mensaje\n");
		exit(EXIT_FAILURE);
	}

	printf("Tamaño del mensaje enviado\n");

    	// enviamos mensaje
	if ((N = sendto(socket_emisor,array,nBytes,0,(struct sockaddr*) &ipport_receptor,length_ptr)) < 0) {
		perror("No se ha podido enviar el mensaje\n");
		exit(EXIT_FAILURE);
	}
	
	printf("%d/%ld bytes enviados\n",N,nBytes);
	}
    // cerramos el socket del emisor
	close(socket_emisor);

	return 0;
}
