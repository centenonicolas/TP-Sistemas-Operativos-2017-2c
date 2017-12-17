#include "funcionesDataNode.h"

void cargarArchivoConfiguracionDatanode(char *nombreArchivo) {
	char cwd[1024]; // Variable donde voy a guardar el path absoluto
	char *pathArchConfig = string_from_format("%s/%s", getcwd(cwd, sizeof(cwd)),
			nombreArchivo);
	t_config *config = config_create(pathArchConfig);

	if (!config) {
		fprintf(stderr,
				"[ERROR]: No se pudo cargar el archivo de configuracion.\n");
		exit(EXIT_FAILURE);
	}

	if (config_has_property(config, "PUERTO_FILESYSTEM")) {
		PUERTO_FILESYSTEM = config_get_int_value(config, "PUERTO_FILESYSTEM");
	} else {
		fprintf(stderr,
				"No existe la clave 'PUERTO_FILESYSTEM' en el archivo de configuracion.\n");
		exit(EXIT_FAILURE);
	}

	if (config_has_property(config, "IP_FILESYSTEM")) {
		IP_FILESYSTEM = config_get_string_value(config, "IP_FILESYSTEM");
	} else {
		fprintf(stderr,
				"No existe la clave 'IP_FILESYSTEM' en el archivo de configuracion.\n");
		exit(EXIT_FAILURE);
	}

	if (config_has_property(config, "ID_NODO")) {
		ID_NODO = config_get_string_value(config, "ID_NODO");
	} else {
		fprintf(stderr,
				"No existe la clave 'ID_NODO' en el archivo de configuracion.\n");
		exit(EXIT_FAILURE);
	}

	if (config_has_property(config, "PUERTO_WORKER")) {
		PUERTO_WORKER = config_get_int_value(config, "PUERTO_WORKER");
	} else {
		fprintf(stderr,
				"No existe la clave 'PUERTO_WORKER' en el archivo de configuracion.\n");
		exit(EXIT_FAILURE);
	}

	if (config_has_property(config, "RUTA_DATABIN")) {
		RUTA_DATABIN = config_get_string_value(config, "RUTA_DATABIN");
	} else {
		fprintf(stderr,
				"No existe la clave 'RUTA_DATABIN' en el archivo de configuracion.\n");
		exit(EXIT_FAILURE);
	}

	if (config_has_property(config, "BLOQUES_TOTALES")) {
		BLOQUES_TOTALES = config_get_int_value(config, "BLOQUES_TOTALES");
	} else {
		fprintf(stderr,
				"No existe la clave 'BLOQUES_TOTALES' en el archivo de configuracion.\n");
		exit(EXIT_FAILURE);
	}
}

void* serializarInfoNodo(t_infoNodo* infoNodo, t_header* header) {
	uint32_t bytesACopiar = 0, desplazamiento = 0, largoIp;

	void *payload = malloc(sizeof(uint32_t) * 5); // reservo 4 uint32_t para los primeros 4 campos del struct + 1 para el largo del string ip.
	/* Serializamos el payload */
	bytesACopiar = sizeof(uint32_t);
	memcpy(payload + desplazamiento, &infoNodo->sdNodo, bytesACopiar);
	desplazamiento += bytesACopiar;

	bytesACopiar = sizeof(uint32_t);
	memcpy(payload + desplazamiento, &infoNodo->idNodo, bytesACopiar);
	desplazamiento += bytesACopiar;

	bytesACopiar = sizeof(uint32_t);
	memcpy(payload + desplazamiento, &infoNodo->cantidadBloques, bytesACopiar);
	desplazamiento += bytesACopiar;

	bytesACopiar = sizeof(uint32_t);
	memcpy(payload + desplazamiento, &infoNodo->puerto, bytesACopiar);
	desplazamiento += bytesACopiar;

	bytesACopiar = sizeof(uint32_t);
	largoIp = strlen(infoNodo->ip);
	memcpy(payload + desplazamiento, &largoIp, bytesACopiar); // le agrego el largo de la cadena ip como parte del mensaje
	desplazamiento += sizeof(uint32_t);

	payload = realloc(payload, desplazamiento + largoIp); // Hacemos apuntar al nuevo espacio de memoria redimensionado.
	memcpy(payload + desplazamiento, infoNodo->ip, largoIp);
	desplazamiento += largoIp;

	header->tamanioPayload = desplazamiento; // Modificamos por referencia al argumento header.

	/* Serializamos y anteponemos el header */
	void *paquete = malloc(sizeof(uint32_t) * 2 + header->tamanioPayload);
	desplazamiento = 0; // volvemos a empezar..

	bytesACopiar = sizeof(uint32_t);
	memcpy(paquete + desplazamiento, &header->id, bytesACopiar);
	desplazamiento += bytesACopiar;

	bytesACopiar = sizeof(uint32_t);
	memcpy(paquete + desplazamiento, &header->tamanioPayload, bytesACopiar);
	desplazamiento += bytesACopiar;

	memcpy(paquete + desplazamiento, payload, header->tamanioPayload);
	free(payload);
	return paquete;
}

int conectarAfilesystem(char *IP_FILESYSTEM, int PUERTO_FILESYSTEM) {
	void *paquete;
	int socketPrograma = socket(AF_INET, SOCK_STREAM, 0);
	t_header *header = malloc(sizeof(t_header));
	header->id = 1;
	if (socketPrograma <= 0) {
		puts(
			"No se ha podido obtener un número de socket. Reintente iniciar el proceso.");
		exit(1);
	}
	printf("Intentando conectar a fileSystem con Ip:  %s \n", IP_FILESYSTEM);
        if (conectarSocket(socketPrograma, IP_FILESYSTEM, PUERTO_FILESYSTEM) != FAIL) {
		printf("Error intentar conexion a fileSystem. Verificar ip y puerto. \n");
		puts("");
		exit(1);
		return 0;
	}

	//Armo struct, lo serializo y lo envio x socket. Del lado del fs lo recibe para agregarlo a la lista de nodos.
	t_infoNodo *infoNodo = malloc(sizeof(t_infoNodo));
	infoNodo->sdNodo = 0;
	infoNodo->idNodo = atoi(ID_NODO);
	infoNodo->cantidadBloques = BLOQUES_TOTALES;
	int largoIp = strlen(IP_FILESYSTEM);
	infoNodo->ip = malloc(largoIp + 1);
	strcpy(infoNodo->ip, IP_FILESYSTEM);

	infoNodo->puerto = PUERTO_WORKER;
	infoNodo->sdNodo = 0;
	paquete = serializarInfoNodo(infoNodo, header);
	if (enviarPorSocket(socketPrograma, paquete, header->tamanioPayload)
			== (header->tamanioPayload + sizeof(uint32_t) * 2)) {
		printf("Informacion del nodo enviada correctamente al FileSystem.\n");
	} else {
		perror("Error en la cantidad de bytes enviados al filesystem.");
	}
	return socketPrograma;
}

void setBloque(int numero, char* datos) {
	size_t bytesAEscribir = UN_BLOQUE, tamanioArchivo;
	off_t desplazamiento = numero * UN_BLOQUE;
	char *regionDeMapeo;

	// Estructura que contiene informacion del estado de archivos y dispositivos.
	struct stat st;
	stat(RUTA_DATABIN, &st);
	tamanioArchivo = st.st_size;

	// Valida que se disponga del tamaño suficiente para realizar la escritura.
	if (tamanioArchivo < bytesAEscribir) {
		perror("No se dispone de tamaño suficiente en la base de datos.");
		exit(EXIT_FAILURE);
	}

	if (desplazamiento >= tamanioArchivo) {
		perror("El desplazamiento sobrepaso el fin de archivo (EOF).");
		exit(EXIT_FAILURE);
	}

	if (desplazamiento + bytesAEscribir > tamanioArchivo)
		bytesAEscribir = tamanioArchivo - desplazamiento;
	/* No puedo mostrar bytes pasando el EOF */

	regionDeMapeo = mmap(NULL, bytesAEscribir,
	PROT_READ | PROT_WRITE,
	MAP_SHARED, fileDescriptor, desplazamiento);

	if (regionDeMapeo == MAP_FAILED) {
		perror("No se pudo reservar la region de mapeo.");
		exit(EXIT_FAILURE);
	}

	memcpy(regionDeMapeo, datos, bytesAEscribir);
	// Libero la region de mapeo solicitada.
	munmap(regionDeMapeo, bytesAEscribir);
}

char* getBloque(int numero) {
	size_t bytesALeer = UN_BLOQUE, tamanioArchivo;
	off_t desplazamiento = numero * UN_BLOQUE;
	char *regionDeMapeo, *data = malloc(bytesALeer);

	// Estructura que contiene informacion del estado de archivos y dispositivos.
	struct stat st;
	stat(RUTA_DATABIN, &st);
	tamanioArchivo = st.st_size;

	if (desplazamiento >= tamanioArchivo) {
		perror("El desplazamiento sobrepaso el fin de archivo (EOF).");
		exit(EXIT_FAILURE);
	}

	if (desplazamiento + bytesALeer > tamanioArchivo)
		bytesALeer = tamanioArchivo - desplazamiento;
	/* No puedo mostrar bytes pasando el EOF */

	regionDeMapeo = mmap(NULL, bytesALeer,
	PROT_READ, MAP_SHARED, fileDescriptor, desplazamiento);

	if (regionDeMapeo == MAP_FAILED) {
		perror("No se pudo reservar la region de mapeo.");
		exit(EXIT_FAILURE);
	}

	memcpy(data, regionDeMapeo, bytesALeer);
	munmap(regionDeMapeo, bytesALeer); // Libero la region de mapeo solicitada.
	return data;
	free(regionDeMapeo);
}

void abrirDatabin() {
	filePointer = fopen(RUTA_DATABIN, "r+");
	// Valida que el archivo exista. Caso contrario lanza error.
	if (!filePointer) {
		perror("El archivo 'data.bin' no existe en la ruta especificada.");
		exit(EXIT_FAILURE);
	}
	fileDescriptor = fileno(filePointer);
}

void cerrarDatabin() {
	fclose(filePointer);
}

void escucharFileSystem(int socketFs) {
	int status = 1;
	t_header headerFs;
	while (status) {

		status = recibirHeader(socketFs, &headerFs);
		//status = recibirPorSocket(socketFs, buffer, 3);
		if ((status > 0) && headerFs.id > 0) {
			//printf("Recibido header : %d  payload: %d \n", headerFs.id,
			//	headerFs.tamanioPayload);
			void *numeroDeBloqueRecibido = malloc(sizeof(uint32_t));
			void* bloqueRecibido;
			char *bloque;
			switch (headerFs.id) {
			case 3: //Peticion de lectura de bloque
				bloque = getBloque(headerFs.tamanioPayload);
				printf("Peticion de lectura bloque Nº: %d \n",
						headerFs.tamanioPayload);
				send(socketFs, (void*) bloque, UN_BLOQUE, 0);
				free(bloque);
				break;
			case 4: //Peticion de escritura
				//Hago dos recv. uno para el numero de bloque y otro para el bloque entero.
				recv(socketFs, numeroDeBloqueRecibido, sizeof(uint32_t),
				MSG_WAITALL);
				bloqueRecibido = malloc(UN_BLOQUE);
				recibirPorSocket(socketFs, bloqueRecibido, UN_BLOQUE);//Recibo directamente ya que solo es un numero
				setBloque(*(int*) numeroDeBloqueRecibido,
						(char*) bloqueRecibido);
				int *respuesta = malloc(sizeof(uint32_t));
				printf("Bloque guardado Nº: %d",
						*(int*) numeroDeBloqueRecibido);
				puts("");
				*respuesta = 1;	//set bloque deberia devolver un valor si estuvo todo ok
				send(socketFs, (void*) respuesta, sizeof(uint32_t), 0);
				free(respuesta);
				free(bloqueRecibido);
				break;
			default: {
				printf("ERROR ID header invalido: %d", headerFs.id);
				break;
			}

			}
			free(numeroDeBloqueRecibido);

		}
	}
}
