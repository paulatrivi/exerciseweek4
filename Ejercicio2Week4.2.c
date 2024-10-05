#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

// Funcion para convertir Celsius a Fahrenheit
float celsius_a_fahrenheit(float celsius) {
	return (celsius * 9.0 / 5.0) + 32.0;
}

// Funcion para convertir Fahrenheit a Celsius
float fahrenheit_a_celsius(float fahrenheit) {
	return (fahrenheit - 32.0) * 5.0 / 9.0;
}

int main(int argc, char *argv[]) {
	
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char peticion[512];
	char respuesta[512];
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// escucharemos en el port 9050
	serv_adr.sin_port = htons(9050);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	//La cola de peticiones pendientes no podr? ser superior a 4
	if (listen(sock_listen, 4) < 0)
		printf("Error en el Listen");
	int i;
	// Atenderemos solo 10 peticione
	for(i=0;i<10;i++){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexi?n\n");
		//sock_conn es el socket que usaremos para este cliente
		
		// Ahora recibimos su peticion
		ret=read(sock_conn,peticion, sizeof(peticion));
		printf ("Recibida una petición\n");
		// Tenemos que a?adirle la marca de fin de string 
		// para que no escriba lo que hay despues en el buffer
		peticion[ret]='\0';
		
		printf ("La petición es: %s\n",peticion);
		char *codigo = strtok(peticion, "/");
		char *valor_str = strtok (NULL,"/");
		float valor = atof(valor_str);
		float resultado;
		int codigo_valido = 1;
		
		if (strcasecmp(codigo, "CtoF") == 0) {
			resultado = celsius_a_fahrenheit(valor);
			sprintf(respuesta, "%.2f", resultado);
		}
		else if (strcasecmp(codigo, "FtoC") == 0) {
			resultado = fahrenheit_a_celsius(valor);
			sprintf(respuesta, "%.2f", resultado);
		}
		else {
			strcpy(respuesta, "Error: Codigo de conversion no valido. Use CtoF o FtoC.");
			codigo_valido = 0;
		}
		// Enviar la respuesta al cliente si el cÃ³digo es vÃ¡lido
		if (codigo_valido) {
			write(sock_conn, respuesta, strlen(respuesta));
			printf("Respuesta enviada: %s\n", respuesta);
		}
		else {
			write(sock_conn, respuesta, strlen(respuesta));
			printf("Respuesta de error enviada: %s\n", respuesta);
		}
		
		// Cerrar la conexiÃ³n con el cliente
		close(sock_conn);
		printf("Conexion cerrada\n");
	}
	// Cerrar el socket de escucha
	close(sock_listen);
	printf("Servidor terminado\n");
	
	return 0;
	
}

