	/*
 * funcionesYAMA.h
 *
 *  Created on: 20/9/2017
 *      Author: utnso
 */

#ifndef FUNCIONESYAMA_H_
#define FUNCIONESYAMA_H_



#include <stdio.h>
#include <stdlib.h>
#include <utils.h>
#include <signal.h>
#include <commons/temporal.h>
#include <dirent.h>

typedef struct{
		uint32_t largoArchivo;
		char* nombreArchivo;
		uint32_t largoArchivo2;
		char* nombreArchivoGuardadoFinal;
	}t_pedidoTransformacion;

struct sockaddr_in direccionFS;
struct sockaddr_in direccionYama;
int socketMasters, socketFS;
char* ip,*socketFSNodos;
extern uint32_t ultimoMaster,reconfiguracion;
uint32_t job;
t_config *config;
char* FS_IP;
int FS_PUERTO,PUERTO_FS_NODOS,RETARDO_PLANIFICACION,PUERTO_MASTERS;
t_log *logger;

typedef struct{
	uint32_t idNodo;
	uint32_t puerto;
	uint32_t largoIp;
	char *IP;
}t_infoNodos;

/*  FUNCIONES YAMA */

/* Carga del archivo de configuracion */
void cargarArchivoDeConfiguracion();

/* instancia la conexion al FileSystem */
void conectarseAFS();

/* Arma el select para los masters */
void yamaEscuchando();
void escucharMasters();

/* Arma el Select para las actualizaciones de nodos */
void escuchaActualizacionesNodos();

t_infoNodos deserializarActualizacion(void*);

/* recibe el  nombre del archivo a procesar */
int recibirRutaDeArchivoAProcesar(int,t_pedidoTransformacion**);
t_rutaArchivo* deserializarRutaArchivo(void* buffer);
t_pedidoTransformacion* deserializarRutasArchivos(void* buffer,int*);
char* deserializarNombreTMP(void *,int*);
t_pedidoTransformacion* deserializarTresRutasArchivos(void *buffer,char *nombreTMP);
void* obtenerBloquesDelArchivo(t_rutaArchivo*);

/* Crea la tabla de estados */
void crearTablaDeEstados();

void conseguirIdNodo(char*,t_header*);

/* log YAMA */
void encargadoInterrupciones(int);

void crearYAMALogger();
#endif /* FUNCIONESYAMA_H_ */
