#ifndef SERVIDOR_H_
#define SERVIDOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include "../utils/utils.h"
char *puntoMontaje;
char *dirMetadata;
char *dirFiles;
char *dirBlocks;
t_config* mongoConfig;
void crearEstructuraFileSystem();

#endif
