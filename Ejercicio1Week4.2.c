#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

// Funcion para verificar si una palabra es palindromo
int es_palindromo(char *nombre) {
	int longitud = strlen(nombre);
	for (int i = 0; i < longitud / 2; i++) {
		if (tolower(nombre[i]) != tolower(nombre[longitud - i - 1])) { // Comparar ignorando mayusculas/minusculas
			return 0; // No es palindromo
		}
	}
	return 1; // Es palindromo
}

// Funcion para convertir una cadena a mayusculas
void convertir_a_mayusculas(char *nombre, char *resultado) {
	int i = 0;
	while (nombre[i]) {
		resultado[i] = toupper(nombre[i]);
		i++;
	}
	resultado[i] = '\0'; // A√±adir terminador nulo al final
}

int main(int argc, char *argv[])
{
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char peticion[512];
	char respuesta[512];
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
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
		printf ("Recibida una peticiÛn\n");
		// Tenemos que a?adirle la marca de fin de string 
		// para que no escriba lo que hay despues en el buffer
		peticion[ret]='\0';
		
		//Escribimos la peticion en la consola
		
		printf ("La peticiÛn es: %s\n",peticion);
		char *p = strtok(peticion, "/");
		int codigo =  atoi (p);
		p = strtok( NULL, "/");
		char nombre[20];
		strcpy (nombre, p);
		printf ("Codigo: %d, Nombre: %s\n", codigo, nombre);
		
		if (codigo ==1) //piden la longitd del nombre
		{
			sprintf (respuesta,"%d",strlen (nombre));
		}
		else if (codigo==2)
		{
			// quieren saber si el nombre es bonito
			if((nombre[0]=='M') || (nombre[0]=='S'))
				strcpy (respuesta,"SI");
			else
				strcpy (respuesta,"NO");
		}
		else if (codigo==3)//decir si es alto
		{
			p = strtok(peticion, "/");
			float altura =  atof (p);
			if (altura>1.7)
				sprintf (respuesta,"%s: eres alto",nombre);
			else
				sprintf(respuesta,"%s: eres bajo",nombre);
		}
		else if (codigo == 4) {
			// C√≥digo 4: verificar si el nombre es pal√≠ndromo
			if (es_palindromo(nombre)) {
				sprintf(respuesta, "%s es un pali≠ndromo", nombre);
			} else {
				sprintf(respuesta, "%s no es un palindromo", nombre);
			}
		}
		else if (codigo == 5) {
			// Codigo 5: devolver el nombre en mayusculas
			char nombre_mayusculas[100];
			convertir_a_mayusculas(nombre, nombre_mayusculas);
			sprintf(respuesta, "%s", nombre_mayusculas);
		}
		// Enviamos la respuesta
		write (sock_conn,respuesta, strlen(respuesta));
			
		// Se acabo el servicio para este cliente
		close(sock_conn); 
	}
}
