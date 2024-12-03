#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

/*==========CLIENTE==========*/
int main(int argc, char** argv) {
    int socket_cliente;   // Identificador del socket del cliente
    struct sockaddr_in ipportserv;    // Estructura para almacenar la dirección del servidor
    socklen_t size = sizeof(struct sockaddr_in);
    char name_file[50], IP_text[INET_ADDRSTRLEN];   // Nombre del archivo y la dirección IP, con tamaño INET_ADDRSTRLEN
    char msg[1024];   // Buffer para el mensaje a enviar y recibir
    int puerto;   // Puerto del servidor al que se conecta
    
    // Comprobamos que se introduzca el numero de argumentos correcto en línea de comandos
    if(argc == 4) {
        strcpy(name_file, argv[1]);   // Primer argumento: nombre del archivo
        strcpy(IP_text, argv[2]);   // Segundo argumento: dirección IP
        puerto = atoi(argv[3]);   // Tercer argumento: puerto (convertido de string a entero)
    } else {
        // Valores por defecto si no se proporcionan argumentos
        printf("No se proporcionaron argumentos correctos, trabajaremos con valores por defecto: file.txt, 127.0.0.1, 6666\nSi se quisieran introducir argumentos, hacer: ./ejecutable archivo IP puerto\n");
        strcpy(name_file, "file.txt");    // Nombre del archivo por defecto
        strcpy(IP_text, "127.0.0.1");   // Dirección IP por defecto (localhost)
        puerto = 6666;    // Puerto por defecto
    }
    
    
    /*Creación del socket del cliente
        domain: AF_INET para IPv4
        style: SOCK_STREAM para orientado a conexion (TCP)
        protocol: por defecto, 0
    */
    if ((socket_cliente = socket(AF_INET, SOCK_STREAM, 0)) < 0) {   //Comprobamos errores en la creacion
        perror("No se pudo crear el socket"); 
        exit(EXIT_FAILURE);
    }
    
    
    /*Convertir la dirección IP de texto a formato binario
        af: AF_INET para IPv4
        src: puntero a cadena de texto con la IP
        dst: puntero a struct in_addr donde se almacenará el resultado
    */
    if(inet_pton(AF_INET,IP_text,(void *) &ipportserv.sin_addr)!=1)
    {
        perror("Formato de dirección incorrecto\n");
        exit(EXIT_FAILURE);
      
    }
    
    
    // Inicializacion de la estructura sockaddr_in
    ipportserv.sin_family = AF_INET;    // Familia de direcciones: IPv4
    ipportserv.sin_port = htons(puerto);    // Convertir el puerto de orden de host a orden de red, short
    
    
    /*Solicitamos conexion al servidor
        socket: el socket del cliente
        addr: puntero a struct sockaddr con la IP y puerto del servidor, hay que convertirlo a tipo generico
        length: tamaño de la estructura sockaddr
    */
    if (connect(socket_cliente, (struct sockaddr *) &ipportserv, size) < 0) {
        perror("ERROR conectando al servidor\n");
        close(socket_cliente);
        exit(EXIT_FAILURE);
    }
    
    // Abrimos el archivo de entrada, solo lectura, y el de salida, solo escritura
    FILE *entrada;
    FILE *salida;
    entrada=fopen(name_file,"r");
    if (entrada == NULL) {
        perror("Error al abrir el archivo de entrada");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; name_file[i] != '\0'; i++) {
        name_file[i] = toupper(name_file[i]);
    }
    
    salida=fopen(name_file,"w");
    if (salida == NULL) {
        perror("Error al abrir el archivo de salida");
        exit(EXIT_FAILURE);
    }
    
    // Leemos cada linea del archivo de entrada
    size_t len = 0;
    int N=0;
    while(fgets(msg,sizeof(msg), entrada)!=NULL){ //Mientras no llega al final del archivo
        len = strlen(msg);    // Obtenemos longitud de la linea leida
        sleep(2);
        
        /*Enviamos la linea al servidor con send
            socket: identificador del socket del cliente
            buffer: puntero al mensaje
            size: numero de bytes a enviar (longitud de la cadena)
            flags: opciones de envio, por defecto 0
        */
        if((N=send(socket_cliente, msg, len+1, 0)) < 0)
        {
            perror("No se pudo enviar el mensaje");
            close(socket_cliente);
            fclose(entrada);
            fclose(salida);
            exit(EXIT_FAILURE);
        }
        printf("\nBytes enviados: %d\n",N);
      
      
      /*Recibimos la respuesta del servidor con recv
            socket: identificador del socket del cliente
            buffer: puntero a donde se va a guardar el mensaje
            size: numero maximo de bytes a recibir
            flags: opciones de recepcion, por defecto 0
        */
      if((N=recv(socket_cliente,msg,sizeof(msg),0))<0)    //Recibe línea en mayúsculas del servidor y la escribe en el archivo de salida
      {
          perror("ERROR recibiendo los datos\n");
          close(socket_cliente);
          fclose(entrada);
          fclose(salida);
          exit(EXIT_FAILURE);
      }
      
      printf("Bytes recibidos: %d\n",N);
      // Escribimos la respuesta en el archivo de salida
      fputs(msg, salida);
        
    }
    // Cerramos el socket y los archivos abiertos, al acabar el intercambio
    close(socket_cliente);
    fclose(entrada);
    fclose(salida);
    
    return(EXIT_SUCCESS);

}
