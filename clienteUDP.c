#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>


/*==========clienteUDP=========*/
int main(int argc, char** argv) {
    int socket_cliente;   // Identificador del socket del cliente
    struct sockaddr_in ipportserv,ipportcli;    // Estructuras para almacenar la dirección del emisor y receptor
    socklen_t size = sizeof(struct sockaddr_in);
    
    int puertocli,puertoserv; // Puerto del cliente y el servidor al que se conecta
    char ipserv[INET_ADDRSTRLEN];
    
    char name_file[50];   // Nombre del archivo y la dirección IP, con tamaño INET_ADDRSTRLEN
    char msg[1024];   // Buffer para el mensaje a enviar y recibir
    
    // Comprobamos que se introduzca el numero de argumentos correcto en línea de comandos
    if(argc == 5) {
        strcpy(name_file, argv[1]);
        puertocli = atoi(argv[2]);   // Primer argumento: puerto (convertido de string a entero)
        strcpy(ipserv,argv[3]);
        puertoserv=atoi(argv[4]);
        
    } else {
        // Valores por defecto si no se proporcionan argumentos
        printf("No se proporcionaron argumentos correctos, trabajaremos con valores por defecto:  file.txt, 6666, 127.0.0.1, 6665\nSi se quisieran introducir argumentos, hacer: ./ejecutable puertoPropio IPservidor puertoServidor\n");
        
        strcpy(name_file, "file.txt");
        puertocli = 6666;    // Puerto por defecto
        strcpy(ipserv, "127.0.0.1");
        puertoserv = 6665;    // Puerto por defecto
    }
    
    /*Creación del socket del cliente
        domain: AF_INET para IPv4
        style: SOCK_DGRAM para no orientado a conexion (UDP)
        protocol: por defecto, 0
    */
    if ((socket_cliente = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("No se pudo crear el socket");
        return(EXIT_FAILURE);
    }
    
    //Inicializamos estructura ipport del cliente
    ipportcli.sin_addr.s_addr=htonl(INADDR_ANY);
    ipportcli.sin_family=AF_INET;
    ipportcli.sin_port=htons(puertocli);
    
    //Inicializamos estructura ipport del servidor
    
    /*Convertir la dirección IP de texto a formato binario
        af: AF_INET para IPv4
        src: puntero a cadena de texto con la IP
        dst: puntero a struct in_addr donde se almacenará el resultado
    */
    if(inet_pton(AF_INET,ipserv,(void *) &ipportserv.sin_addr)!=1)
    {
        perror("Formato de dirección incorrecto\n");
        exit(EXIT_FAILURE);
      
    }
    ipportserv.sin_family = AF_INET;
    ipportserv.sin_port = htons(puertoserv);
    
    
     /*Asignamos una direccion al socket con bind
        socket: identificador del socket
        addr: puntero a struct sockaddr con la direccion a asignar (ipportserv)
        length: tamaño de la estructura addr
    */
    if (bind(socket_cliente, (struct sockaddr *) &ipportcli, size) < 0) {
        perror("No se pudo asignar la dirección");
        close(socket_cliente);
        return(EXIT_FAILURE);
    }
    
    
    // Abrimos el archivo de entrada, solo lectura, y el de salida, solo escritura
    FILE *entrada;
    FILE *salida;
    entrada=fopen(name_file,"r");
    for (int i = 0; name_file[i] != '\0'; i++) {
        name_file[i] = toupper(name_file[i]);
    }
    salida=fopen(name_file,"w");
   
    /*Convertir la dirección IP de binario a texto
            af: AF_INET para IPv4
            src: puntero a una struct in_addr (para IPv4)
            dst: puntero a cadena donde se guarda el resultado
            size: tamaño en bytes de la cadena destino
    */
    if (inet_ntop(AF_INET, (const void*) &(ipportserv.sin_addr), ipserv,INET_ADDRSTRLEN) != NULL){
            printf("IP: %s\nPuerto: %d\n", ipserv, ntohs(ipportserv.sin_port));
    }
    
    // Leemos cada linea del archivo de entrada
    size_t len = 0;
    
    while(fgets(msg,sizeof(msg), entrada)!=NULL)  // Mientras no llega al final del archivo
    {
        len = strlen(msg);    // Obtenemos longitud de la linea leida
        sleep(3);
        
        
        /*Enviamos la línea, en minúsculas, al servidor
                socket: identificador del socket
                buffer: puntero a mensaje a enviar
                size: numero maximo de bytes a recibir
                flags: opciones de recepcion, por defecto 0
                addr: puntero a un struct sockaddr con la dirección a la que se quiere enviar
                length: tamaño de la estructura addr
        */
        if (sendto(socket_cliente,msg,len+1,0,(struct sockaddr*) &ipportserv,size) < 0) {
		perror("No se ha podido enviar el mensaje\n");
		close(socket_cliente);
		return(EXIT_FAILURE);
	}
	
        /*Recibimos el tamaño del mensaje del servidor con recvfrom
                socket: identificador del socket
                buffer: puntero a donde se va a guardar el mensaje
                size: numero maximo de bytes a recibir
                flags: opciones de recepcion, por defecto 0
                addr: puntero a un struct sockaddr con la dirección de la que se quiere enviar
                length: tamaño de la estructura addr
        */
        int N=0;
        if((N=recvfrom(socket_cliente,msg,sizeof(msg),0,(struct sockaddr *) &ipportcli,&size)) < 0)
        {
            perror("No se pudo recibir el mensaje correctamente\n");
            close(socket_cliente);
            return(EXIT_FAILURE);
        }
        printf("%d bytes recibidos\n",N);
        
        // Escribimos la respuesta en el archivo de salida
        fputs(msg, salida);
   }        
    
    // Cerramos el socket y los archivos abiertos, al acabar el intercambio
    close(socket_cliente);
    fclose(entrada);
    fclose(salida);
    return(EXIT_SUCCESS);

}
