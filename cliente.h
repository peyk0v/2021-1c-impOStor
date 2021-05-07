#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include "../server_utils/utils.h"
#include <commons/config.h>

/*
 Debera recibir conexion de i Mongo Store para administrar los sabotajes.
 Debera conectarse a Mi Ram HQ

 iniciar_patota(int cantTripulantes, string path_to_file_tasks, Posicion posiciones[])

  si no se especifica la posicion del tripulante se lo inicializa en (0,0)


 listar_tripulantes()


 expulsar_tripulante(int idTripulante)

  Conecta con Mi Ram HQ y le manda el tripulante para eyectarlo (eliminar segmento de tareas)


 iniciar_planificacion()

 pausar_planificacion()

 obtener_bitacora(int idTripulante)

 Conecta con i Mongo Store y le pide la bitacora del tripulante.


 */

typedef struct Posicion{
   int x;
   int y;
} Posicion;


typedef enum EstadoTripulante{
    LLEGADA, LISTO, TRABAJANDO, FINALIZADO, BLOQUEADO_IO, BLOQUEADO_EMERGENCIA
}EstadoTripulante;


typedef struct Tripulante{
	int idTripulante;
    char* nombre;
    //t_list* tareasPendientes;
    Posicion* posicion;
    EstadoTripulante estado;
    int cantCiclosCPUTotales;
} Tripulante;


typedef struct Patota{
    t_list* tripulantes;
    int idPatota;
    t_list* procesos;
    t_list* procesosDeIntercambio;
} Patota;

/*
 * Ṕuede servir para los procesos

typedef struct Proceso{
	Entrenador* entrenador;
	Pokemon* pokemon;
	int rafagaAnterior;
	float estimadoAnterior;
	float estimadoActual;
}Proceso;

typedef struct ProcesoIntercambio{
	Entrenador* entrenador1;
	Entrenador* entrenador2;
	char* pokemonQueNecesitaE1;
	char* pokemonQueNecesitaE2;
	int rafagaAnterior;
	float estimadoAnterior;
	float estimadoActual;
	bool favorableParaUnLado;
}ProcesoIntercambio;

*/

#endif

