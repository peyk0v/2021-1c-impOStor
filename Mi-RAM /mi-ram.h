#ifndef MiRAM_H_
#define MiRAM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include "../utils/utils.h"
#include <commons/string.h>
#include "segmentacion.h"
//#include "paginacion.h"
#include <commons/temporal.h>
#include <commons/txt.h>
#include <signal.h>

int servidor;
int socket_mi_ram;
t_config *config;
char* puerto;
int socket_cliente;
int numero_patota = 1;
char* tipoMemoria;
void* memoria;
int tamaniomemoria;
t_list* patotas;
t_log *logs_ram;

// Todos los int de 32bits hacen referencia a una direccion en la memoria
typedef enum {
	PCB, TCB, TAREAS
} tipo_estructura;

typedef struct{
	int pid;
	t_list *tabla;
} t_proceso;

#endif
