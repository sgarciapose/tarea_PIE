#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "filtros.h"

/* estructuras auxiliares para crear un arreglo de las medias de cada 
 * region en el filtro caricatura */
typedef struct colores{
	int R;
	int G;
	int B;
} Colores;

typedef struct promedio {
	Colores suma;
	int cant;
	Colores media;
} Promedio;

/****************************************************/
/****************************************************/

void copia(Imagen *entrada, Imagen *salida){
	
	int cant_pixeles = entrada->ancho * entrada->alto;
	int i;
	
	for(i = 0; i < cant_pixeles; i++)
		salida->pixels[i] = entrada->pixels[i];
}

void negativo(Imagen *entrada, Imagen *salida){
	
	int cant_pixeles = entrada->ancho * entrada->alto;
	int i;
	
	if (entrada->tipo == COLOR){
		
		Pixel R,G,B;
		R = G = B = 0;
		for(i = 0; i < cant_pixeles; i++){
			
			IntToRGB(entrada->pixels[i], &R, &G, &B);
			R = entrada->valor_maximo - R;
			G = entrada->valor_maximo - G;
			B = entrada->valor_maximo - B;
			salida->pixels[i] = RGBtoInt(R,G,B);
		}
	}
	else
		for(i = 0; i < cant_pixeles; i++)
			salida->pixels[i] = entrada->valor_maximo - entrada->pixels[i];
}

void reflejo(Imagen *entrada, int parametro, Imagen *salida){
	int i, j, k;
	int ancho = entrada->ancho;
	int alto = entrada->alto;
	
	switch (parametro) {
	case 0: /* reflejo horizontal */
		j = ancho - 1;
		i = 0;
		while (i < ancho*alto)
		{
			for (k = 0; k < ancho; ++k)
				salida->pixels[j--] = entrada->pixels[i++];
			j += 2*ancho;
		}
		break;
	case 1: /* reflejo vertical */
		j = (alto - 1)*ancho;
			k = i = 0;
			
			while (k < ancho)
			{
				for (;i <= (alto-1)*ancho + k; i += ancho, j -= ancho)
					salida->pixels[j] = entrada->pixels[i];
				/* Me muevo a la siguiente columna de la foto */
				++k;
				i = k;
				j = (alto-1)*ancho + k;
			}
		break;
	case 2: /* reflejo central */
		for (i = 0, j = alto*ancho - 1; i < alto*ancho; ++i, --j)
			salida->pixels[j] = entrada->pixels[i];
		break;
	}
}

static int clipp(Imagen *entrada, int i, int j, int m, int n){
	if (i < 0)
		if (j < 0)
			return entrada->pixels[0];
		else if (j >= 0 && j < m)
			return entrada->pixels[j];
		else
			return entrada->pixels[m-1];
	else if (i >= 0 && i < n)
		if (j < 0)
			return entrada->pixels[i*m];
		else if (j >= 0 && j < m)
			return entrada->pixels[i*m+j];
		else
			return entrada->pixels[i*m+m-1];
	else
		if (j < 0)
			return entrada->pixels[(n-1)*m];
		else if (j >= 0 && j < m)
			return entrada->pixels[(n-1)*m+j];
		else
			return entrada->pixels[n*m-1];
}

/********* PASAR DE COLOR A GRISES *********/
static void color_a_grises(Imagen *entrada, Imagen *salida){
	int cant_pixel = entrada->ancho * entrada->alto;
	int i;
	Pixel R,G,B;
	
	for (i = 0; i < cant_pixel; i++){
		IntToRGB(entrada->pixels[i], &R, &G, &B);
		salida->pixels[i] = round(0.2*R + 0.7*G + 0.1*B);		
	}
}


void bordes(Imagen *entrada, int parametro, Imagen *salida){
		
	int i, j;
	int alto = entrada->alto;
	int ancho = entrada->ancho;
	int cant_pixel = alto * ancho;
	
	Imagen *img_aux = malloc(sizeof(Imagen));
	inicializar_imagen(ancho, alto, GRISES, img_aux);
	
	/* imagen auxiliar para sacar los bordes */
	if (entrada->tipo == COLOR)
		color_a_grises(entrada, img_aux);
	else
		copia(entrada, img_aux);
	
	switch (parametro) {
		case 0: 
			for (i = 0; i < alto; i++)
				for (j = 0; j < ancho; j++)
					salida->pixels[i*ancho + j] = abs(clipp(img_aux, i, j+1, ancho, alto) - clipp(img_aux, i, j-1, ancho, alto));
			break;
		case 1: 
			for (i = 0; i < alto; i++)
				for (j = 0; j < ancho; j++)
					salida->pixels[i*ancho + j] = abs(clipp(img_aux, i+1, j, ancho, alto) - clipp(img_aux, i-1, j, ancho, alto));
			break;
		case 2: 
			for (i = 0; i < alto; i++)
				for (j = 0; j < ancho; j++)
					salida->pixels[i*ancho + j] = round(sqrt( (float)pow( (float)clipp(img_aux, i, j+1, ancho, alto) - (float)clipp(img_aux, i, j-1, ancho, alto) ,2) 
								+ ((float)pow((float)clipp(img_aux, i+1, j, ancho, alto) - (float)clipp(img_aux, i-1, j, ancho, alto) ,2) )));
			break;
		};
	for (i = 0; i < cant_pixel; i++) {
		if (salida->pixels[i] < 0)
			salida->pixels[i] = 0;
		if (salida->pixels[i] > entrada->valor_maximo)
			salida->pixels[i] = entrada->valor_maximo;				
	}
	destruir_imagen(img_aux);
}

/************************************************************/
/*                   FILTRO DE CARICATURA                   */
/************************************************************/

/* Devuelve el valor de etiqueta del lugar (i,j) en la matriz etiquetas.
 * Si el lugar (i,j) esta fuera de rango devuelve 0 */
static int etiqueta (int *etiquetas, int i, int j, int ancho, int alto){
	if (i < 0)							/* si i esta fuera del rango */
		return 0;						/* es borde */
	else {
		if(j < 0)						/* si j esta fuera del rango */
			return 0;					/* es borde	*/
		else
			return etiquetas[i*ancho+j]; /* valor de su etiqueta */
	}
}

/* Recorre la matriz etiqueta desde primero hasta ultimo reemplazando
 * etiqueta1 por etiqueta2 */
static void pegar_regiones(int *etiquetas, int etiqueta1, int etiqueta2, int primero, int ultimo){
	int k;
	for (k = primero; k < ultimo; k++)
		if (etiquetas[k] == etiqueta1)
			etiquetas[k] = etiqueta2;
}

/* Dada una matriz en grises devuelve una matriz de etiquetas, donde
 * la etiqueta 0 indica que el en ese lugar hay un borde.
 * Precondicion: la matriz etiquetas esta inicializada en 0 */
static int etiquetado(Imagen *entrada, int umbral, int *etiquetas, int *indices){
	int k = 1;	/* contador de regiones */
	int i, j;	/* iteradores */
	int n, w;	/* para guardar los adyacentes */
	int ancho = entrada->ancho;
	int alto = entrada->alto;
	
	for (i = 0; i < alto; i++)
		for (j = 0; j < ancho; j++)
			if (entrada->pixels[i*ancho+j] <= umbral){	/* si no es borde */
				n = etiqueta(etiquetas, i-1, j, ancho, alto); /* adyacente superior */
				w = etiqueta(etiquetas, i, j-1, ancho, alto); /* adyacente izquierdo */
				if (w == 0){						/* si es borde w */
					if (n == 0)	{					/* si es borde n */
						etiquetas[i*ancho+j] = k;	/* etiqueta nueva */
						indices[k] = i*ancho+j;
						k++;
					}else
						etiquetas[i*ancho+j] = n;
				}else{
					if (n == 0)						/* si es borde n */
						etiquetas[i*ancho+j] = w;
					else{
						if (n == w)		/* si tienen la misma etiqueta */
							etiquetas[i*ancho+j] = n;
						else{
							if (indices[w] > indices[n]){
								etiquetas[i*ancho+j] = w;
								indices[w] = indices[n];
								pegar_regiones(etiquetas, n, w, indices[n], i*ancho+j);
							}else{
								etiquetas[i*ancho+j] = n;
								indices[n] = indices[w];
								pegar_regiones(etiquetas, w, n, indices[w], i*ancho+j);
							}
						}	
					}
				}
			}
	return k;
}

/* Guarda en salida la iamgen con los promedios de cada region de la 
 * imagen entrada, estas regiones estan dadas por la matriz etiquetas.
 * el parametro regiones es = (cantidad de regiones)+1, de entrada 
 * los valores de etiqueta estan en el intervalo [0, regiones-1] */
static void colorear(Imagen *entrada, int *etiquetas, int regiones, Imagen *salida){
	Pixel R, G, B, i;
	int cantpixel = entrada->ancho * entrada->alto;

	/* inicializar p arreglo de promedios donde cada lugar corresponde */
	/* a la region i */
	Promedio *p = calloc(regiones, sizeof(Promedio));
	
	/* calculo de los promedios de cada region */
	/* sumas y cantidades */
	for (i = 0; i < cantpixel; i++)
		if (etiquetas[i] != 0){			/* si no es borde */
			/* sumar valores de R, G y B de ese pixel de la region i */
			IntToRGB(entrada->pixels[i], &R, &G, &B);
			p[etiquetas[i]].suma.R = p[etiquetas[i]].suma.R + R;
			p[etiquetas[i]].suma.G = p[etiquetas[i]].suma.G + G;
			p[etiquetas[i]].suma.B = p[etiquetas[i]].suma.B + B;
			
			/* sumar una a la cantidad de pixeles de la region i */
			p[etiquetas[i]].cant++;
		}
	
	/* promedios */
	for (i = 0; i < regiones; i++){
		if (p[i].cant != 0){
			p[i].media.R = (Pixel)round((float)p[i].suma.R / (float)p[i].cant);
			p[i].media.G = (Pixel)round((float)p[i].suma.G / (float)p[i].cant);
			p[i].media.B = (Pixel)round((float)p[i].suma.B / (float)p[i].cant);
		}
	}
	
	/* Coloreo */
	for (i = 0; i < cantpixel; i++){
		if (etiquetas[i] == 0)			/* si es borde */
			salida->pixels[i] = 0;	/* color negro */
		else{ /* se colorea con el promedio de la region de ese pixel */
			R = p[etiquetas[i]].media.R;
			G = p[etiquetas[i]].media.G;
			B = p[etiquetas[i]].media.B;
			salida->pixels[i] = RGBtoInt(R,G,B);
		}
	}
}

/*--------------------------------------------------------------------*/

void caricatura(Imagen *entrada, int umbral, Imagen *salida){
	int ancho = entrada->ancho;
	int alto = entrada->alto;
	int cantpixel = ancho*alto;
	int num_regiones;
	
	int *indices = malloc(sizeof(int)*cantpixel);
	Imagen *borde = malloc(sizeof(Imagen));
	int *etiquetas = calloc(cantpixel, sizeof(int));
	
	/* sacar los bordes de la imagen */
	inicializar_imagen(ancho, alto, GRISES, borde);
	bordes(entrada, 2, borde);
	
	/* etiquetar las regiones */
	num_regiones = etiquetado(borde, umbral, etiquetas, indices);
	destruir_imagen(borde);
	
	/* poner los colores de la etiqueta */
	colorear(entrada, etiquetas, num_regiones, salida);
}

