#include "funcionesWorker.h"

int main(void) {

	verificarExistenciaCarpetaLogs();
	crearLogger();
	NODOARCHCONFIG = "nodoConfig.cfg";
	cargarArchivoConfiguracion(NODOARCHCONFIG);

	abrirDatabin();

	direccionWorker.sin_family = AF_INET;
	direccionWorker.sin_port = htons(PUERTO_WORKER);
	direccionWorker.sin_addr.s_addr = INADDR_ANY;

	int activado = 1;
	int socketMaster;
	pid_t pid;

	socketMaster = socket(AF_INET, SOCK_STREAM, 0);
	// Permite reutilizar el socket sin que se bloquee por 2 minutos
	if (setsockopt(socketMaster, SOL_SOCKET, SO_REUSEADDR, &activado,
			sizeof(activado)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	// Se enlaza el socket al puerto
	if (bind(socketMaster, (struct sockaddr *) &direccionWorker,
			sizeof(struct sockaddr)) != 0) {
		perror("No se pudo conectar.");
		exit(1);
	}
	// Se pone a escuchar el servidor kernel
	if (listen(socketMaster, 100) == -1) {
		perror("listen");
		exit(1);
	}

	int tamanioDir = sizeof(direccionWorker);
	int bytesRecibidos, nuevoSocket;

	int i = 0;
	printf("Listo para recibir peticiones de jobs.\n");
	while (1) {
		i++;
		if ((nuevoSocket = accept(socketMaster,
				(struct soccaddr*) &direccionWorker, &tamanioDir)) <= 0)
			perror("accept");
		else {
			void* buffer;
			t_infoTransformacion* infoTransformacion;
			t_infoReduccionLocal* infoReduccionLocal;
			t_infoReduccionGlobal* infoReduccionGlobal;
			t_infoGuardadoFinal* infoGuardadoFinal;
			char* nombreArchTempPedido;
			t_header header;
			recibirHeader(nuevoSocket, &header);

			buffer = malloc(header.tamanioPayload);
			bytesRecibidos=recibirPorSocket(nuevoSocket, buffer, header.tamanioPayload);
			if(bytesRecibidos<=0){
				printf("Error conexion con master entrante.\n");
				continue;
			}
			pid = fork();
			if(pid<0){
				fprintf(stderr,"fallo el fork.\n");
			}
			if(pid == 0){

				switch (header.id) {

				case TRANSFORMACION:
					infoTransformacion = deserializarInfoTransformacion(buffer);
					realizarTransformacion(infoTransformacion, nuevoSocket);
					break;
				case REDUCCION_LOCAL:
					infoReduccionLocal = deserializarInfoReduccionLocal(buffer);
					realizarReduccionLocal(infoReduccionLocal, nuevoSocket);
					break;
				case REDUCCION_GLOBAL:
					infoReduccionGlobal = deserializarInfoReduccionGlobal(
							buffer);
					realizarReduccionGlobal(infoReduccionGlobal, nuevoSocket);
					printf("Termino reduccion global,job: %s\n",infoReduccionGlobal->rutaArchivoTemporalFinal);
					break;
				case ORDEN_GUARDADO_FINAL:
					infoGuardadoFinal = deserializarInfoGuardadoFinal(buffer);
					printf("inicio guardado final de archivo:  %s\n",infoGuardadoFinal->rutaArchFinal);
					guardadoFinalEnFilesystem(infoGuardadoFinal,nuevoSocket);
					printf("Finalizo guardado final de archivo: %s\n",infoGuardadoFinal->rutaArchFinal);
					break;
				case SOLICITUD_WORKER:
					printf("Solicitud de archivo temporal de encargado.\n");
					nombreArchTempPedido = deserializarSolicitudArchivo(buffer);
					responderSolicitudArchivoWorker(nombreArchTempPedido,
							nuevoSocket);
					printf("Finalizo solicitud de archivo de encargado.\n");
					break;
				}

				close(nuevoSocket);
				exit(0);
			}
			else{

				waitpid(pid,0,WNOHANG);
				free(buffer);
			}

		}

	}

}
