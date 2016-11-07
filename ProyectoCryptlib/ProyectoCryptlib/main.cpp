#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h> 
#include "cryptlib.h"
#include <iostream>
#include <fstream>
char entradaArchivo[2000];
/* prototipos funciones */
void leerEntrada(char* cadena);
void leerArchivo();
char MENU();
int aplicarAES(char* mensaje, char* clave);


int main(){

	/* declaración devariables de trabajo	*/
	char entradaTexto[300];
	char contrasenia_ingreso[300];

	int salir = false;

	while (!salir){

		switch (MENU()){

		//Aplicar AES al contenido del archivo
		case '1':
			system("cls");
			leerArchivo();
			if (entradaArchivo != NULL){

				if (entradaArchivo != " "){
					system("cls");
					printf("Ingrese la contrasenia para cifrar: ");
					fflush(stdin);
					leerEntrada(contrasenia_ingreso);

					printf("\n");
					aplicarAES(entradaArchivo, contrasenia_ingreso);
				}
				else
					printf("%s\n","Archivo Vacio");
			}
			else{
				printf("%s\n", "No se pudo abrir el archivo");
			}
			break;

		//Aplicar AES a la entrada por terminal
		case '2':
			system("cls");
			printf("Ingrese un mensaje:\n"); 
			fflush(stdin);
			leerEntrada(entradaTexto);
			printf("Ingrese la contrasenia para cifrar: ");
			fflush(stdin);
			leerEntrada(contrasenia_ingreso);

			printf("\n");
			aplicarAES(entradaTexto, contrasenia_ingreso);
			break;
		//Salir
		case '3':
			salir = true;
			printf("\n\n\nFin del programa...\n");
			break;
		}

		printf("\n\n\n\nEnter para continuar...\t");
		getch();
	}

}





//Para leer una cadena con espacios ingresada por teclado y eliminando el salto de linea
void leerEntrada(char* cadena){
	int tamanio = 100; //leer hasta 100 chars
	char *p;
	fgets(cadena, tamanio, stdin);
	p = strchr(cadena, '\n'); //buscamos el salto de linea leido
	if (p)
		*p = '\0';  //lo reemplazamos con fin de cadena
}

void leerArchivo(){
	FILE *archivo;

	char caracteres[100]="";
	char palabra[2000] = "";
	char* nombreArchivo = "mensaje.txt";

	archivo = fopen(nombreArchivo, "r");

	if (archivo == NULL)
		exit(1);

	while (feof(archivo) == 0){
		strcat(entradaArchivo, caracteres);
		fgets(caracteres, 100, archivo);

	}

	fclose(archivo);
}

char MENU(){
	char op;
	system("cls");
	printf("----------------------------------------------------------\n"
		   "       A L G O R I T M O   D E   C I F R A D O   A E S    \n"
		   "                           M E N U                        \n"
		   "----------------------------------------------------------\n\n"
		"  1. Aplicar AES al contenido de un fichero.\n"
		"  2. Aplicar AES a un texto ingresado.\n"
		"  3. Salir\n"
		"  -> ");

	fflush(stdin);
	op = getchar();
	return op;
}


int aplicarAES(char* mensaje, char* clave){
	//Definicion de variables -------------------------------------------
	int dataSize = strlen(mensaje);
	unsigned char data[1024];
	const int claveSize = 16;
	unsigned char iv[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	char * ivStr = "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00";//Solo para mostrar
	CRYPT_CONTEXT cryptDecryptContext;//Contexto para encriptar y desencriptar
	int status;

	//Inicializacion de los buffers
	memset(data, ' ', 1024);//padding, la salida del bloque final depende del padding utilizado
	memcpy(data, mensaje, dataSize);

	//Mostrar datos iniciales-------------------------------------------
	int k;
	printf("\n\n****************DATOS INICIALES****************\n");
	printf("Texto de entrada:---->");
	for (k = 0; k < dataSize; k++)
		printf("%c", data[k]);
	printf("\nClave:---->%s\n", clave);
	printf("Vector de inicializacion:----->%s\n\n", ivStr);


	//Inicializacion de cryptlib ----------------------------------------
	status = cryptInit();
	if (cryptStatusError(status)){
		printf("ERROR: Error inicializando cryptlib...\n");
		return 1;
	}

	//Creacion del contexto de encriptacion/desencriptacion para BLOWFISH
	status = cryptCreateContext(&cryptDecryptContext, CRYPT_UNUSED, CRYPT_ALGO_AES);
	if (cryptStatusError(status)){
		printf("ERROR: Error al crear el contexto de enc/dec...\n");
		return 1;
	}

	//Selecionar el modo Cipher Block Chaining
	status = cryptSetAttribute(cryptDecryptContext, CRYPT_CTXINFO_MODE, CRYPT_MODE_CBC);
	if (cryptStatusError(status)){
		printf("ERROR: Error el seleccionar el modo de enc/dec...\n");
		return 1;
	}

	//configurar vector de inicializacion
	status = cryptSetAttributeString(cryptDecryptContext, CRYPT_CTXINFO_IV, iv, 16);
	if (cryptStatusError(status)){
		printf("ERROR: Error al configurar el vector de inicializacion...\n");
		return 1;
	}

	//Asignar la clave de configuracion al contexto
	status = cryptSetAttributeString(cryptDecryptContext, CRYPT_CTXINFO_KEY, clave, claveSize);
	if (cryptStatusError(status)){
		printf("ERROR: Error al configurar la clave...\n");
		return 1;
	}

	//Encriptar los datos
	status = cryptEncrypt(cryptDecryptContext, data, 1024);
	if (cryptStatusError(status)){
		printf("ERROR: Error al encriptar los datos...\n");
		return 1;
	}

	//Mostrar resultado encryptado
	printf("\n\n****************** ENCRIPTAR ******************\n");
	printf("Texto encriptado:----> \n");

	int i = 0;
	int fin;
	if (dataSize % 16 == 0)
		fin = dataSize;
	else
		fin = dataSize + (16 - dataSize % 16);


	for (i; i<fin; i++) {
		printf(" 0x%2x", data[i]);
		if ((i + 1) % 16 == 0)
			printf("\n");
	}
	printf("\n");


	//Desencriptar
	//Si se usa el mismo contexto para encriptar y desencriptar, se debe
	//configurar nuevamente el vector de inicializacion
	status = cryptSetAttributeString(cryptDecryptContext, CRYPT_CTXINFO_IV, iv, 16);
	if (cryptStatusError(status)){
		printf("ERROR: Error al configurar el vector de inicializacion...\n");
		return 1;
	}

	status = cryptDecrypt(cryptDecryptContext, data, 1024);
	if (cryptStatusError(status)){
		printf("ERROR: Falla al desencriptar los datos...\n");
		return 1;
	}

	//Mostrar texto desencriptado
	printf("\n\n****************** DESENCRIPTAR ******************\n");
	printf("Texto plano:---->");
	for (k = 0; k < dataSize; k++)
		printf("%c", data[k]);
	printf("\n");

	cryptEnd();//Finalizamos la rutina
	
}