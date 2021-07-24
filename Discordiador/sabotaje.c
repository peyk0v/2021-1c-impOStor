#include "sabotaje.h"


void esperar_sabotaje(void){ //este es un hilo
	int socket_mongo;
	int codigo_recibido;
	t_list *lista;
	int sabotaje_posX;
	int sabotaje_posY;
	char *mensaje;



	while(1){
		socket_mongo = iniciar_conexion(I_MONGO_STORE, config);

		if(socket_mongo == -1) {
			log_info(logs_discordiador, "ERROR AL CONECTARSE CON MONGO");
			pthread_exit(NULL);
		}

		enviar_mensaje(ESPERANDO_SABOTAJE," ",socket_mongo);
		codigo_recibido = recibir_operacion(socket_mongo);

		printf("SABOTAJEEEE\n");

		if(codigo_recibido == INICIO_SABOTAJE){
			pthread_mutex_lock(&sabotaje_lock);
			g_hay_sabotaje = true; //activo para q todos detengan su ejecucion
			pthread_mutex_unlock(&sabotaje_lock);

			lista = recibir_paquete(socket_mongo);
			sabotaje_posX = atoi(list_get(lista, 0));
			sabotaje_posY = atoi(list_get(lista, 1));
			log_info(logs_discordiador, "SABOTAJE EN (%d, %d)!", sabotaje_posX, sabotaje_posY);
			atender_sabotaje(sabotaje_posX, sabotaje_posY);
		}

		else {
			log_info(logs_discordiador, "RECIBI UNA OPERACION DIFERENTE DE SABOTAJE!");
			lista = recibir_paquete(socket_mongo); //para vaciar el buffer
			return;
		}
		liberar_cliente(socket_mongo);
	}

}

//TODO: rehacer este metodo con un atender_sabotaje(x, y)
void atender_sabotaje(int x, int y){
	t_list *lista_mergeada = (t_list *)malloc(sizeof(t_list));
    Tripulante_Planificando *tripulante_cercano = (Tripulante_Planificando*)malloc(sizeof(Tripulante_Planificando));

    pthread_mutex_lock(&sabotaje_lock);
	g_hay_sabotaje = true; //activo para q todos detengan su ejecucion
	pthread_mutex_unlock(&sabotaje_lock);

    bool sigue_activo(void *data){
    	Tripulante_Planificando *tripulante = (Tripulante_Planificando*) data;

    	if(tripulante == NULL)
    		return false;

    	return tripulante->sigo_planificando;
    }

    //si no hay tripulantes en ninguna de las listas
	if (list_size(lista_trabajando) == 0 && list_size(lista_listo->elements) == 0) {
		log_info(logs_discordiador, "No hay tripulantes disponibles para resolver el sabotaje");

		pthread_mutex_lock(&sabotaje_lock);
		g_hay_sabotaje = false;
		pthread_mutex_unlock(&sabotaje_lock);

		sem_post(&termino_sabotaje_planificador);
	}

	log_info(logs_discordiador, "Esperando que los procesos terminen su ciclo actual CPU");

    while(list_any_satisfy(lista_trabajando, sigue_activo) ||
    		list_any_satisfy(lista_bloqueado_IO, sigue_activo))
			; //espera a que todos los tripulantes terminen el su 'sleep'



    bool sorter(void *data1, void *data2){
    	int resta_patotas;
    	int resta_tripulantes;
    	bool resultado;
    	Tripulante_Planificando *tripulante1 = (Tripulante_Planificando *) data1;
    	Tripulante_Planificando *tripulante2 = (Tripulante_Planificando *) data2;

    	resta_patotas     = tripulante1->tripulante->patota - tripulante2->tripulante->patota;
    	resta_tripulantes = tripulante1->tripulante->id     - tripulante2->tripulante->id;

    	if(resta_patotas > 0)
    		return false;
    	if(resta_patotas < 0)
    		return true;

    	else
    		if(resta_tripulantes > 0)
    			return false;
    		else
    			return true;
    }


	pthread_mutex_lock(&lock_lista_exec);
	list_sort(lista_trabajando, sorter);
	pthread_mutex_unlock(&lock_lista_exec);

	pthread_mutex_lock(&lock_lista_listo);
	list_sort(lista_listo->elements, sorter);
	pthread_mutex_unlock(&lock_lista_listo);

	tripulante_cercano = (Tripulante_Planificando*) mas_cercano_al_sabotaje(x, y);

	void destroyer(void *data){
		Tripulante_Planificando *tripulante = (Tripulante_Planificando*) data;
		free(tripulante->tripulante);
		sem_destroy(&tripulante->termino_sabotaje);
		sem_destroy(&tripulante->ir_exec);
		sem_destroy(&tripulante->salir_pausa);
		free(tripulante->tarea->nombre);
	}

	if (list_size(lista_trabajando) > 0) {
		pthread_mutex_lock(&lock_lista_exec);
		list_add_all(lista_bloqueado_EM, lista_trabajando);
		list_clean(lista_trabajando); //sigue con referencias
		pthread_mutex_unlock(&lock_lista_exec);
	}

	if (list_size(lista_listo->elements) > 0) {
		pthread_mutex_lock(&lock_lista_listo);
		list_add_all(lista_bloqueado_EM, lista_listo->elements);
		queue_clean(lista_listo); // no se elimininan
		pthread_mutex_unlock(&lock_lista_listo);
	}

	bool soy_yo(void *data) { //funcion para buscar un tripulante
		Tripulante_Planificando *un_tripulante =
				(Tripulante_Planificando *) data;
		return tripulante_cercano->tripulante->id == un_tripulante->tripulante->id
				&& tripulante_cercano->tripulante->patota == un_tripulante->tripulante->patota;
	}

	pthread_mutex_lock(&lock_lista_bloq_em);
	list_remove_by_condition(lista_bloqueado_EM, soy_yo);
	list_add(lista_bloqueado_EM, tripulante_cercano); //seria para agregarlo a lo ultimo
	pthread_mutex_unlock(&lock_lista_bloq_em);


	void cambiar_estado_a_bloqueado_emergencia(void *data) {
		Tripulante_Planificando *tripulante = (Tripulante_Planificando*) data;
		tripulante->tripulante->estado = BLOQUEADO_EMERGENCIA;
	}

	list_iterate(lista_bloqueado_EM, cambiar_estado_a_bloqueado_emergencia);

	resolver_sabotaje(tripulante_cercano, x, y);

	sem_wait(&resolvi_sabotaje);

    void reanudar_tripulantes(void *data) {
		Tripulante_Planificando *tripulante = (Tripulante_Planificando*) data;
		sem_post(&tripulante->termino_sabotaje);
	}

	void cambiar_estado_a_ready(void *data) {
		Tripulante_Planificando *tripulante = (Tripulante_Planificando*) data;
		tripulante->tripulante->estado = LISTO;
	}

	sleep(10);
	printf("-------------ETAPA 1: TODOS BLOQ-------------");
	listar_discordiador();

	list_iterate(lista_bloqueado_EM, cambiar_estado_a_ready);

	pthread_mutex_lock(&lock_lista_bloq_em);
	pthread_mutex_lock(&lock_lista_listo);
	list_add_all(lista_listo->elements, lista_bloqueado_EM);
	pthread_mutex_unlock(&lock_lista_listo);
	list_clean(lista_bloqueado_EM);
	pthread_mutex_unlock(&lock_lista_bloq_em);

	pthread_mutex_lock(&sabotaje_lock);
	g_hay_sabotaje = false;
	pthread_mutex_unlock(&sabotaje_lock);

	sleep(10);
	printf("-------------ETAPA 3: TODOS LISTOS-------------");
	listar_discordiador();

	list_iterate(lista_listo->elements, reanudar_tripulantes);
	sem_post(&termino_sabotaje_planificador);
}

void *mas_cercano_al_sabotaje(int x, int y){
  t_list *tripulantes_mergeados = list_create();
  void *tripulante;

  if(list_size(lista_trabajando) > 0){
	  list_add_all(tripulantes_mergeados, lista_trabajando);
  }
  if(queue_size(lista_listo) > 0){
	  list_add_all(tripulantes_mergeados, lista_listo->elements);
  }
  if(queue_size(lista_listo) <= 0 && list_size(lista_trabajando) <= 0) {
	  return NULL;
  }


  void *maximo(void *data1, void *data2){
    float distancia1;
    float distancia2;

    Tripulante_Planificando *t1 = (Tripulante_Planificando *)data1;
    Tripulante_Planificando *t2 = (Tripulante_Planificando *)data2;

    distancia1 = (t1->tripulante->posicionX - x) * (t1->tripulante->posicionX - x)
      + (t1->tripulante->posicionY - y) * (t1->tripulante->posicionY - y);
    distancia2 = (t2->tripulante->posicionX - x) * (t2->tripulante->posicionX - x)
      + (t2->tripulante->posicionY - y) * (t2->tripulante->posicionY - y);

    if(distancia1 < distancia2) {
      return t1;
    }
    
    else return t2;
  }

  tripulante = list_get_maximum(tripulantes_mergeados, maximo);

  free(tripulantes_mergeados);

  return tripulante;
}

bool llegue_al_sabotaje(Tripulante_Planificando *tripulante, int x, int y){
	int sourceX = tripulante->tripulante->posicionX;
	int sourceY = tripulante->tripulante->posicionY;

	log_info(logs_discordiador, "Me movi en SABOTAJE de (%d, %d) a (%d, %d)",
			sourceX, sourceY, x, y);

	return estoy_en_mismo_punto(sourceX, sourceY, x, y);
}

void moverse_al_sabotaje(Tripulante_Planificando *tripulante, int targetX, int targetY){
	int sourceX = tripulante->tripulante->posicionX;
	int sourceY = tripulante->tripulante->posicionY;
	static bool last_move_x = false;

	if (sourceX == targetX && last_move_x == false)
			last_move_x = true;

		if (sourceY == targetY && last_move_x == true)
			last_move_x = false;


		//mucho codigo repetido
		if (sourceX < targetX && last_move_x == false) {
			tripulante->tripulante->posicionX += 1;
			last_move_x = true;
			return;
		}

		if (sourceX > targetX && last_move_x == false) {
			tripulante->tripulante->posicionX -= 1;
			last_move_x = true;
			return;
		}

		if (sourceY < targetY && last_move_x == true) {
			tripulante->tripulante->posicionY += 1;
			last_move_x = false;
			return;
		}

		if (sourceY > targetY && last_move_x == true) {
			tripulante->tripulante->posicionY -= 1;
			last_move_x = false;
			return;
		}
}

void resolver_sabotaje(Tripulante_Planificando *tripulante, int x, int y){
	while(!llegue_al_sabotaje(tripulante, x, y)){
		moverse_al_sabotaje(tripulante, x, y);
	}

	sleep(duracion_sabotaje);

	sem_post(&resolvi_sabotaje);
	log_info(logs_discordiador, "El tripulante N: %d resolvio el sabotaje!", tripulante->tripulante->id);
}