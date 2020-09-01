#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "imagen.h"

int inicializar_imagen(int ancho, int alto, TipoImagen tipo, Imagen* pnew){
	pnew->tipo = tipo;
	pnew->ancho = ancho;
	pnew->alto = alto;
	pnew->valor_maximo = 255;
	pnew->pixels = malloc(ancho*alto*sizeof(Pixel));
	if (pnew->pixels != NULL)
		return 0;
	else
		return 1;
}

void destruir_imagen(Imagen* pimg){
	pimg->tipo = 0;
	pimg->ancho = 0;
	pimg->alto = 0;
	pimg->valor_maximo = 0;
	if (pimg->pixels != NULL){
		free(pimg->pixels);
		pimg->pixels = NULL;
	}
}

void duplicar_imagen(const Imagen* pin, Imagen* pout){
	pout->tipo = pin->tipo;
	pout->ancho = pin->ancho;
	pout->alto = pin->alto;
	pout->valor_maximo = pin->valor_maximo;
	pout->pixels = malloc(pin->ancho*pin->alto*sizeof(Pixel));
}

CodigoError leer_imagen(const char* ruta, Imagen *pimg){
	
	
	char letra; 		
	int num;			
	int datos_leidos;	
	
	/* abro el archivo */
	FILE *archivo = fopen(ruta,"r");
	if (archivo == NULL){
		return 1;
	}
	else
	{
		datos_leidos = fscanf(archivo, "%c %d", &letra, &num);
		if (datos_leidos == 2){
			if (num == 6 && letra == *"P")
				pimg->tipo = COLOR;
			else if (num == 2 && letra == *"P")
				pimg->tipo = GRISES;
			else{
				fclose(archivo);
				return 2;
			}
		}
		else{
			fclose(archivo);
			return 2;
		}
		
		
		datos_leidos = fscanf(archivo, "%d %d %d ", &pimg->ancho, &pimg->alto, &pimg->valor_maximo);
		if (datos_leidos != 3){
			fclose(archivo);
			return 3;
		}
		
		if (pimg->ancho <= 0 || pimg->alto <= 0 || pimg->valor_maximo < 0 || pimg->valor_maximo > 255 ){
			fclose(archivo);
			return 2;
		}
		
		/* Si es color o gris */
		int cantidad_pixeles = pimg->ancho * pimg->alto;
		pimg->pixels = malloc(cantidad_pixeles * sizeof(Pixel));
		
		fscanf(archivo, "\n"); 
		Pixel x=0;
		if (pimg->tipo == COLOR)
		{
			int r=0, g=0, b=0, i;
			unsigned char byte_rd;
		
			for (i = 0; i < cantidad_pixeles; i++){
				datos_leidos = fread(&byte_rd, sizeof(unsigned char), 1, archivo);
				r = (Pixel)byte_rd;
				if (datos_leidos != 1){
					fclose(archivo);
					return 4;
				}
				
				datos_leidos = fread(&byte_rd, sizeof(unsigned char), 1, archivo);
				g = (Pixel)byte_rd;
				if (datos_leidos != 1){
					fclose(archivo);
					return 4;
				}
				
				datos_leidos = fread(&byte_rd, sizeof(unsigned char), 1, archivo);
				b = (Pixel)byte_rd;
				if (datos_leidos != 1){
					fclose(archivo);
					return 4;
				}
				
				pimg->pixels[i] = RGBtoInt(r,g,b);
			}
		}else{
			Pixel p, i;
			for (i = 0; i < cantidad_pixeles; i++){
				datos_leidos = fscanf(archivo,"%u", &p);
				if (datos_leidos != 1 || p < 0 || p > pimg->valor_maximo){
					fclose(archivo);
					return 4;
				}
				pimg->pixels[i] = p;
			}
		}
		fclose(archivo);
		return 0;
	}
}

CodigoError escribir_imagen(const Imagen* pimg, const char* ruta){
	
	/* en caso de imagen invalida */
	if (pimg == NULL || pimg->pixels == NULL)
		return 5;
	
	FILE *archivo = fopen(ruta, "w");
	if (archivo == NULL)
		return 5;
		
	int cantidad_pixeles = pimg->ancho * pimg->alto;
	int i;
	int datos_leidos;
	
	if (pimg->tipo == COLOR){
		Pixel r, g, b;
		unsigned char byte_w;
		/* escribir numero magico */
		datos_leidos = fprintf(archivo, "P6\n");
		if (datos_leidos <= 0)
			return 5;
			
		/* escribir ancho alto y valmax */
		datos_leidos = fprintf(archivo, "%d %d\n%d\n", pimg->ancho, pimg->alto, pimg->valor_maximo);
		if (datos_leidos <= 0)
			return 5;
		Pixel x=0;
		/* escribir pixeles */
		for (i = 0; i < cantidad_pixeles; i++) {
			
			x = pimg->pixels[i];
			r = x >> 16;
			g = x << 16;
			g = g >> 24;
			b = x & 255;
			byte_w = (unsigned char)r;
			datos_leidos = fwrite(&byte_w, sizeof(unsigned char), 1, archivo);
			if (datos_leidos == 0)
				return 5;
				
			byte_w = (unsigned char)g;
			datos_leidos = fwrite(&byte_w, sizeof(unsigned char), 1, archivo);
			if (datos_leidos == 0)
				return 5;
				
			byte_w = (unsigned char)b;
			datos_leidos = fwrite(&byte_w, sizeof(unsigned char), 1, archivo);
			if (datos_leidos == 0)
				return 5;
		}
		fprintf(archivo,"\n");
		//~ free (byte_w);
	}else {
		/* escribir numero magico */
		int cont = 0;
		datos_leidos = fprintf(archivo, "P2\n");
		if (datos_leidos <= 0)
			return 5;
		
		/* escribir ancho alto y valmax */
		datos_leidos = fprintf(archivo, "%d %d\n%d\n", pimg->ancho, pimg->alto, pimg->valor_maximo);
		if (datos_leidos <= 0)
			return 5;
		
		/* escribir pixeles */
		for (i = 0; i < cantidad_pixeles; i++){
				char* byte_w = malloc(4*sizeof(char));
				sprintf(byte_w, "%d", pimg->pixels[i]);
				cont = cont + strlen(byte_w);
				if (cont < 79){
					datos_leidos = fprintf(archivo, "%d ", pimg->pixels[i]);
					cont++;
				}
				else{
					datos_leidos = fprintf(archivo,"\n%d ", pimg->pixels[i]);
					cont = strlen(byte_w) + 1;
				}
				if (datos_leidos <= 0)
					return 5;
				free(byte_w);
		}
		fprintf(archivo,"\n");
	};
	fclose(archivo);
	return 0;
}



/***********************************************************/
/*      RUTINAS AUXILIARES PARA PASAR DE RGB <--> INT      */
/***********************************************************/

void IntToRGB(Pixel numero, Pixel *r, Pixel *g, Pixel *b){
	*b = numero & 255;
	numero = numero >> 8;
	*g = numero & 255;
	numero = numero >> 8;
	*r = numero & 255;
}

Pixel RGBtoInt(Pixel R, Pixel G, Pixel B){
	Pixel numero;
	
	numero = 0;
	numero = R;
	numero = numero << 8;
	numero = numero | G;
	numero = numero << 8;
	numero = numero | B;
	
	return numero;
}

