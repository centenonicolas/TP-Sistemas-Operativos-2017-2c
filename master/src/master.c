/*
 ============================================================================
 Name        : Master.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include "funcionesMaster.h"
#include <signal.h>

//void signal_handler(int);
//PARA EJECUTAR ESTE PROGRAMA RECORDAR QUE LOS PARAMETROS TIENEN QUE SER RUTAS VALIDAS
int main(int argsc,char **args) {
	//char * transformador;
	//char *reductor;
	//char *archivoAprocesar;
	//char *direccionDeResultado;
	if(argsc==5){
		transformador=args[1];
		reductor=args[2];
		archivoAprocesar=args[3];
		direccionDeResultado=args[4];

		if(!chequearParametros(transformador,reductor,archivoAprocesar,direccionDeResultado)){

			//signal(SIGINT, signal_handler);
			return 0;
		} else{
			printf("Esta todo ok. Se envia por socket la info al yamafs\n");
		}

	} else{
		printf("Cantidad de parametros invalida. Se esperan 4 pero se detectaron: %d \n",argsc-1);
		//pause();
		//signal(SIGINT, signal_handler);
		return 0;

	}

	iniciarMaster(transformador,reductor,archivoAprocesar,direccionDeResultado);
	return EXIT_SUCCESS;
}
/*
void signal_handler(int numero){
	printf("aborte\n");
	signal(SIGINT, SIG_DFL);
}*/
