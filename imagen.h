typedef unsigned int Pixel;

typedef enum codigo_error
{
	PNM_OK = 0,						
	PNM_ARCHIVO_INEXISTENTE = 1,	
	PNM_ERROR_LECTURA = 2,			
	PNM_ENCABEZADO_INVALIDO = 3,	
	PNM_DATOS_INVALIDOS = 4,		
	PNM_ERROR_ESCRITURA = 5			
} CodigoError;

typedef enum tipo_imagen
{
	GRISES = 0,
	COLOR = 1
} TipoImagen;

typedef enum canal
{
	ROJO = 0,
	VERDE = 1,
	AZUL = 2
} Canal;

typedef struct imagen
{
	TipoImagen tipo;
	int ancho;
	int alto;
	int valor_maximo;
	Pixel *pixels;
} Imagen;

int inicializar_imagen(int ancho, int alto, TipoImagen tipo, Imagen* pnew);

void destruir_imagen(Imagen* pimg);

void duplicar_imagen(const Imagen* pin, Imagen* pout);

CodigoError leer_imagen(const char* ruta, Imagen* pimg);

CodigoError escribir_imagen(const Imagen* pimg, const char* ruta);


/***********************************************************/
/*      RUTINAS AUXILIARES PARA PASAR DE RGB <--> INT      */
/***********************************************************/

void IntToRGB(Pixel numero, Pixel *r, Pixel *g, Pixel *b);

Pixel RGBtoInt(Pixel R, Pixel G, Pixel B);


