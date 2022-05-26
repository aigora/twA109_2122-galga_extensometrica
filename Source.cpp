#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "SerialClass/SerialClass.h"



#define MAX_BUFFER 200
#define PAUSA_MS 200 
#define N 200

int menu(void)
{
	int opcion;
	printf("\n");
	printf("Menú Principal\n");
	printf("==============\n");
	printf("1 - Obtener en fuerza\n");
	printf("2 - Obtener peso\n");
	printf("3- Tabla de resultados\n");
	printf("4 - Salir de la aplicación\n");
	printf("Opción:");
	scanf_s("%d", &opcion);
	return opcion;
}
float obtener_fuerza(Serial* Arduino, char* port);
float obtener_peso(Serial* Arduino, char* port);
int Enviar_y_Recibir(Serial* Arduino, const char* mensaje_enviar, char* mensaje_recibir);
void crear_fichero_txt(char* mensaje);

void crear_fichero_txt(char* mensaje) {

	FILE* fichero;
	int i;
	errno_t e;


	e = fopen_s(&fichero, "Pesos.txt", "a");
	
	if (fichero == NULL)
		printf("No se ha podido guardar los datos\n");
	else {

		fprintf(fichero, "%s\n", mensaje);

		fclose(fichero);
	}
}



float obtener_fuerza(Serial* Arduino, char* port) {

	float peso = 0;

	int fuerza;
	
	int bytesRecibidos;
	
	char mensaje_recibido[MAX_BUFFER];
	float numero = 0;
	int i, divisor = 10, estado = 0;

	if (Arduino->IsConnected())
	{

		bytesRecibidos = Enviar_y_Recibir(Arduino, "GET_PESO\n", mensaje_recibido);

		


		if (bytesRecibidos <= 0)
		{
			printf("\nNo se ha recibido respuesta a la petición\n");

		}
		else
		{
			for (i = 0; mensaje_recibido[i] != '\0' && estado != 3 && i < MAX_BUFFER; i++)
				switch (estado)
				{
				case 0:// Antes del número
					if (mensaje_recibido[i] >= '0' && mensaje_recibido[i] <= '9')
					{
						numero = mensaje_recibido[i] - '0';
						estado = 1;
					}
					break;
				case 1:// Durante el número
					if (mensaje_recibido[i] >= '0' && mensaje_recibido[i] <= '9')
						numero = numero * 10 + mensaje_recibido[i] - '0';
					else
						if (mensaje_recibido[i] == '.' || mensaje_recibido[i] == ',')
							estado = 2;
						else
							estado = 3;
					break;
				case 2: // Parte decimal
					if (mensaje_recibido[i] >= '0' && mensaje_recibido[i] <= '9')
					{
						numero = numero + (float)(mensaje_recibido[i] - '0') / divisor;
						divisor *= 10;
					}
					else
						estado = 3;
					break;
				}

			printf("\n FUERZA = %.2f N\n",numero*9.81);
		}
	}
	else
	{
		printf("\nNo se ha podido conectar con Arduino.\n");
		printf("Revise la conexión, el puerto %s y desactive el monitor serie del IDE de Arduino.\n", port);
	}


	

	return peso;
}

float obtener_peso(Serial* Arduino, char* port)
{
	float peso = 0;
	int bytesRecibidos;
	char mensaje_recibido[MAX_BUFFER];

	if (Arduino->IsConnected())
	{

		bytesRecibidos = Enviar_y_Recibir(Arduino, "GET_PESO\n", mensaje_recibido);
		if (bytesRecibidos <= 0)
		{
			printf("\nNo se ha recibido respuesta a la petición\n");

		}
		else
		{
			printf("\nPESO = %s kg\n", mensaje_recibido);
			crear_fichero_txt(mensaje_recibido);
		}
	}
	else
	{
		printf("\nNo se ha podido conectar con Arduino.\n");
		printf("Revise la conexión, el puerto %s y desactive el monitor serie del IDE de Arduino.\n", port);
	}



	return peso;
}


int main(void)
{
	Serial* Arduino;
	char puerto[] = "COM3"; // Puerto serie al que está conectado Arduino
	int opcion_menu;
	float peso;
	float tara;
	setlocale(LC_ALL, "es-ES");
	FILE* fichero;
	errno_t e;
	int ctrl;
	Arduino = new Serial((char*)puerto);
	char mens[N];


	do
	{
		opcion_menu = menu();

		switch (opcion_menu)

		{
		case 1:

			peso = obtener_fuerza(Arduino, puerto);
			

			break;
		case 2:
			peso = obtener_peso(Arduino, puerto);

			break;
		case 3:

			e = fopen_s(&fichero, "Pesos.txt", "rt");

			if (fichero == NULL) {
				printf("Error al leer el fichero");
			}
			else {

				do {

					ctrl = fscanf_s(fichero, "%s", mens, N);

					if (ctrl == 1) {
						printf("Peso = %skg\n", mens);
					}

				} while (ctrl == 1);
				fclose(fichero);
			}

			break;
		case 4:
			break;
		default: printf("\nOpción incorrecta\n");
		}
	} while (opcion_menu != 4);


	return 0;
}




int Enviar_y_Recibir(Serial * Arduino, const char* mensaje_enviar, char* mensaje_recibir)
{
	int bytes_recibidos = 0, total = 0;
	int intentos = 0, fin_linea = 0;
	Arduino->WriteData((char*)mensaje_enviar, strlen(mensaje_enviar));
	Sleep(PAUSA_MS);
	bytes_recibidos = Arduino->ReadData(mensaje_recibir, sizeof(char) * MAX_BUFFER - 1);
	while ((bytes_recibidos > 0 || intentos < 5) && fin_linea == 0)
	{
		if (bytes_recibidos > 0)
		{
			total += bytes_recibidos;
			if (mensaje_recibir[total - 1] == 13 || mensaje_recibir[total - 1] == 10)
				fin_linea = 1;
		}
		else
			intentos++;
		Sleep(PAUSA_MS);
		bytes_recibidos = Arduino->ReadData(mensaje_recibir + total, sizeof(char) * MAX_BUFFER - 1);
	}
	if (total > 0)
		mensaje_recibir[total - 1] = '\0';
	return total;
}