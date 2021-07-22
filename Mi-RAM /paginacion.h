#ifndef PAGINACION_H_
#define PAGINACION_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <math.h>

#include "mi-ram.h"

int TAM_PAG;
int TAM_MEM;
int cantidadDeFrames;

typedef enum {
	OCUPADO, LIBRE
} estado_frame;

typedef enum {
	MEM_PPAL, MEM_VIRT
} tipo_memoria;

typedef struct {
	estado_frame estado;
	uint32_t memoria;
	int nro_frame;
} t_frame;

typedef struct {
	int nro_pagina;
	int nro_frame_mpal;
	//int nro_frame_swap;
	bool bit_uso;
	t_list* estructuras_alojadas;
	int tam_disponible;
} t_pagina;

typedef struct {
	int nro_estructura;
	int base;
	int tamanio;
	tipo_estructura tipo;
	int flagid;
} t_alojado;

t_bitarray* frames_ocupados_ppal;
int cant_marcos_memoria;

pthread_mutex_t mutexEscribiendoMemoria;
pthread_mutex_t mutexBitArray;
pthread_mutex_t mutexTablaPatota;
pthread_mutex_t mutexAlojados;
pthread_mutex_t mutexTablaProcesos;

/* Verifica que un frame existe en memoria y es válido */

bool traer_marco_valido(int, int);

/* Setea el frame como bit en uso */

void asignar_marco_en_uso(int, int);

/* Libera el marco que recibe por parametro */

void liberar_marco(int, int);

/* Verifica si el marco se encuentra utilizado por alguna pagina */

int marco_vacio(int );

/* Busca en la memoria un marco disponible */

uint32_t buscar_marco_disponible(int );

/* Devuelve si el frame indicado esta completamente libre */

int frame_libre(int );

/* Crea la estructura administrativa para manejar los tipos en la pagina
 * y la agrega a la pagina correspondiente */

void agregarEstructAdminTipoPAG(t_pagina*, int ,int , int , int);

/* Lee la pagina que esta en el frame indicado. */

void* leer_memoria_pag(int, int);

/* Inserta en el malloc de memoria la pagina. */

int insertar_en_memoria_pag(t_pagina*, void*, int, int*, int , int*, int);

/* Busca la pagina pasada por parametro en el malloc de memoria */

void* buscar_pagina(t_pagina* );

/* Crea la pagina en la estructura administrativa de la patota pasada por parametro */

t_pagina* crear_pagina_en_tabla(t_proceso* ,int);

/* Asigna la pagina en la tabla de la patota y la inserta en el malloc de memoria */

int asignar_paginas_en_tabla(void* , t_proceso* , int);

/* Verifica que la patota/proceso exista */

void existencia_patota(t_proceso*);

/* Busca la patota por el pid que se pasa por parametro */

t_proceso* buscar_patota(int );

/* Busca la ultima pagina del proceso enviado por parametro */

t_pagina* buscar_ultima_pagina_disponible(t_proceso* );

/* Guarda el TCB en el malloc de memoria */

int guardar_TCB_pag(TripuCB*, int);

/* Guarda la patota(PCB) y las tareas en el malloc de memoria */

int guardar_PCB_pag(PatotaCB*, char*);

/* Busca la siguiente tarea en el malloc de memoria */

char* obtener_siguiente_tarea_pag(t_proceso* , TripuCB* );//TODO:REVISAR

/* Busca la direccion logica donde empiezan las paginas de tareas de un proceso/patota */

uint32_t buscar_inicio_tareas(t_proceso* );

/*  Esta funcion hay que revisarla */

uint32_t calcuar_DL_tareas_pag();

/* Verifica si una pagina tiene la estructura alojada que se pasa por parametro */

bool tiene_pagina_estructura_alojadas(t_list* , int);

/* Verifica si una pagina tiene un tripulante como estructura alojada */

bool pagina_tripu_alojado(t_list* , int);

/* Retorna la estructura administrativa de la pagina de un tripulante */

t_alojado* obtener_tripulante_pagina(t_list* , int);

/* Sirve para actualizar el estado del tripulante en el malloc memoria */

int actualizar_tripulante_EnMem_pag(t_proceso* , TripuCB*);

/* Trae todas la paginas asociadas a un tripulante */

t_list* lista_paginas_tripulantes(t_list*, uint32_t );

/* Actualiza la estructura administrativa del tripulante */

int sobreescribir_tripulante(t_list* , TripuCB* );

/* Actualiza en memoria al tripulante */

int actualizar_tripulante_pag(TripuCB* , int);

/* Trae la estructura administrativa del tripulante  */

TripuCB* obtener_tripulante(t_proceso* ,int );

/* Asigna la proxima tarea al tripulante de la patota pasado por parametro  */

char* asignar_prox_tarea_pag(int , int);

/* Esta funcion cambiar por un list_iterate */

t_list_iterator* iterarHastaIndice(t_list*, int);

/* Verifica si el tripulante que se elimino no fue el ultimo del proceso enviado por parametro  */

void chequear_ultimo_tripulante(t_proceso*);

/* Recibe patota id y tripu id, elimina tanto estructuras administrativas como en memoria */

void expulsar_tripulante_pag(int ,int);

/* Verifica que un proceso le queden tripulantes */

bool proceso_tiene_tripulantes_pag(t_proceso*);

/* Retorna una patota si hay en el frame pasado por parametro */

t_proceso* frame_con_patota(int);

/* Retorna una pagina si el proceso enviado por parametro se encuentra en el frame enviado por parametro*/

t_pagina* frame_con_pagina(int ,t_proceso*);

/* Realiza el dump de memoria para paginacion */

void dump_pag();

/*Crea un buffer y da los valores a flag y el tamaño que se buscara guardar*/

void* meterEnBuffer(void* , int , int* , int* );

/*Busca el numero del primer frame vacio */

int buscar_frame_disponible(int);

/*Confirma si la pagina tiene la estructura indicada */

bool tieneEstructuraAlojada(t_list*, int);

/* Crea una tarea */

char* armarTarea(char*);

bool tieneTripulanteAlojado(t_list*, uint32_t); //TODO

t_list* obtenerAlojadoPagina(t_list*, uint32_t);//TODO

/* Actualiza un tripulante en memoria */

void sobreescribir_memoria(int, void*, int, int, int);//TODO

void cargarDLTripulante(void*, TripuCB*);//TODO

TripuCB* transformarEnTripulante(void*);


#endif
