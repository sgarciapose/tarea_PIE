#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "filtros.h"

int main(int argc, char* argv[]){	
	
	/* DEFINO VARIABLES CON LOS ARGUMENTOS */
	char *comando = argv[1];
	int parametro = atoi(argv[2]); 
	char *ruta_entrada = argv[3];
	char *ruta_salida = argv[4];
	
	/* CREO PUNTEROS DE ENTRADA Y SALIDA A ESTRUCTURA Imagen PARA GUARDAR VALORES */
	Imagen* img_entrada = malloc(sizeof(Imagen));
	Imagen* img_salida = malloc(sizeof(Imagen));
	
	/******** LEO LA IMAGEN Y DEVUELVO CODIGO DE ERROR ********/
	CodigoError codigo = leer_imagen(ruta_entrada,img_entrada);
	
	if (codigo == 0)
	{
		int ancho = img_entrada->ancho;
		int alto = img_entrada->alto;
		
		/*** COPIO ATRIBUTOS DE ENTRADA A SALIDA ***/
		duplicar_imagen(img_entrada, img_salida); 
		
		
		if (strcmp(comando, "bordes") == 0)
		{
			/* PARA BORDES NECESITO TRABAJAR EN GRISES, CAMBIO EL TIPO DE IMAGEN */
			inicializar_imagen(ancho, alto, GRISES, img_salida);
			if (0 <= parametro <= 2)
				bordes(img_entrada, parametro, img_salida);
			else
				printf("Parámetro no válido\n");
		}
		else if (strcmp(comando, "copia") == 0)
			
			copia(img_entrada, img_salida);
		
		else if (strcmp(comando, "negativo") == 0)
			
			negativo(img_entrada, img_salida);
		
		else if (strcmp(comando, "reflejo") == 0)
			
			if (0 <= parametro <= 2)
				reflejo(img_entrada, parametro, img_salida);
			else
				printf("Parámetro no válido\n");
		
		else if (strcmp(comando, "caricatura") == 0)
			
			if (0 <= parametro <= 255)
				caricatura(img_entrada, parametro, img_salida);
			else
				printf("Parámetro no válido\n");		
		
		/******** ESCRIBO IMAGEN DE SALIDA ********/
		CodigoError codigo1 = escribir_imagen(img_salida,ruta_salida);
		
		if (codigo1 == 0)
		{
			/* LIBERO MEMORIA DEL PROGRAMA */
			destruir_imagen(img_entrada);
			destruir_imagen(img_salida);
			return 0;
		}
		else
		{
			printf("ERROR: no se pudo escribir el archivo.\n");
			/* LIBERO MEMORIA DEL PROGRAMA */
			destruir_imagen(img_entrada);
			destruir_imagen(img_salida);
			return 1;
		}
	} 
	
	/******** SI LA LECTURA FUE INCORRECTA ********/
	else
		if (codigo == 1)
			printf("ERROR: archivo no encontrado.\n");
		if (codigo == 2)
			printf("ERROR: no se pudieron leer los datos del archivo.\n");
		if (codigo == 3)
			printf("ERROR: formato del encabezado no válido.\n");
		if (codigo == 4)
			printf("ERROR: datos inválidos.\n");
		
	/* LIBERO MEMORIA DEL PROGRAMA */
	destruir_imagen(img_entrada);
	destruir_imagen(img_salida);
	return 1;
		
} /* END MAIN */
