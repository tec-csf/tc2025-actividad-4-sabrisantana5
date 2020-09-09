//
//  main.c
//  Actividad 4 procesos
//  Reforzar los conocimientos relacionados con la creación y manipulación de procesos en el lenguaje C.
//  Created by Sabrina Santana Lazos on 9/6/20.
//  Copyright © 2020 Sabrina Santana Lazos. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <math.h>

//Estructura de histograma
typedef struct{
	int pidHijo;
	int promedio;
} histograma;

//Relacion para imprimir asteriscos
float relacionHistograma(int maximo){
    return ceil(maximo/10);
}

//Función para imprimir histograma
void printHistograma(histograma * hist, int maximo, int cantidad){
    histograma * final = hist + cantidad;
    histograma * aux = hist;
    int i;
    float relacion;

    printf("PID Hijo \tPromedio \tHistograma\n");
    while(aux<final){
        i = 0;
        printf("%d \t\t %d \t\t", aux->pidHijo,aux->promedio);
        relacion = relacionHistograma(maximo);
				//Imprimir asteriscos de acuerdo a la relacion del maximo
        while(i<(aux->promedio)/relacion){
            printf("*");
            i++;
        }
        printf("\n");
        aux++;
				//Imprimir hasta el último hijo que se pudo crear
        if(aux->pidHijo==0){
          aux = final;
        }
    }
}

int main(int argc, char **argv) {

    int estado, c, index, cant2;
    int total = 0, i = 0, maximo = 0;
    char *cvalue = NULL;
    opterr = 0;


    //Para recibir como argumento en la línea de comandos el número de procesos hijos a crear (opción -n).
    while ((c = getopt (argc, argv, "n:")) != -1){
			printf("Actividad 4: Procesos. Ingrese un valor por consola con la opcion -n ENTERO, si ingresa un valor FLOAT se tomará como entero redondeado hacia abajo\n\n");
        switch (c)
    {
        case 'n':
            cvalue = optarg;
            //Validar que el valor especificado sea un número entero
            if(isdigit(*cvalue)!=0){
              cant2 = atoi(optarg);
            } else{
              printf("El argumento no es un numero entero \n");
            }
            break;
        case '?':
            if (optopt == 'n')
                fprintf (stderr, "Opción -%c requiere un argumento.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Opción desconocida '-%c'.\n", optopt);
            else
                fprintf (stderr,
                         "Opción desconocida '\\x%x'.\n",
                         optopt);
            return 1;
        default:
            abort ();
    }
	}

		//Cuando la opcion no es -n
    for (index = optind; index < argc; index++)
        printf ("El argumento no es una opción válida %s, Opción válida: -n entero\n", argv[index]);

		//Variables para los pids y el histograma
    pid_t * pids = (pid_t *)malloc(sizeof(pid_t)*cant2);
    pid_t * inicio = pids;
    pid_t * fin = pids + cant2;
    histograma * hist = (histograma *) malloc(sizeof(histograma)*cant2);
    histograma * iniciohist = hist;
    histograma * finhist = hist + cant2;

    //Crear procesos hijos
    while(inicio<fin){
        * inicio = fork();

        //Error al generar proceso hijo
        if (*inicio == -1) {
						//Esperar a que los hijos anteriores al error acaben de imprimir
            sleep(1);
            printf("\nError al crear hijo \n");
            printf("\nHijos creados hasta el momento: %d \n", i);
            break; //Para no crear mas procesos hijos
        }
        //Calcular promedio si el hijo se creó correctamente
        else if (*inicio == 0) {
            int pidactual = getpid();
            int pidpadre = getppid();
            int promedio;
            promedio = (pidactual + pidpadre) / 2;
            printf("\nSoy el proceso con PID = %d y mi promedio es = %d \n", \
                   pidactual,promedio);

            return promedio;
        }
        inicio++;
        i++;
    }

		//Esperar a que se impriman todos los hijos
    sleep(3);

    inicio = pids;
    while(inicio<fin){

      //Padre espera a que terminen todos los procesos hijos en el mismo orden en que se crearon.
       if (waitpid(*inicio, &estado, 0) != -1)
       {
           if (WIFEXITED(estado)){
             //Padre guarda el valor de retorno de cada proceso hijo
             iniciohist->pidHijo = *inicio;
             iniciohist->promedio = WEXITSTATUS(estado);
             //Calcular maximo para crear relacion
             if(iniciohist->promedio>maximo){
                 maximo = iniciohist->promedio;
             }
           }
       }
       inicio++;
       iniciohist++;
    }

    printf("\n\n");
    //Padre muestra tabla de histograma
    printHistograma(hist,maximo,cant2);

		//Liberar memoria
    free(pids);
    free(hist);
    return 0;
}
