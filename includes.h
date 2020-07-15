#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <unistd.h>
#define exit_on_error(s,m) if (s<0) {perror(m); exit(1);}
#define exit_on_null(s,m) if (s==NULL) {perror(m); exit(1);}
#define MAX_OUV 20
#define MAX_PL 20
#define MAX_MUS 20
#define NUM_SEMAFOROS 3

struct sembuf DOWNouvinte = {0, -1, 0}; 
struct sembuf UPouvinte = {0, 1, 0};
struct sembuf DOWNmusica = {1, -1, 0};
struct sembuf UPmusica = {1, 1, 0};
struct sembuf DOWNplaylist = {2, -1, 0};
struct sembuf UPplaylist = {2, 1, 0};

typedef struct {
	char ID_MUS[10];
	char titulo[60];
	char artista[50];
	int duracao;
	int ano;
	char genero[20];
	int top;
} Tmusica;

typedef struct {
	char ID_PL[10];
	char nome[60];
	char musicas[250];
} Tplaylist;

typedef struct {
	char nick[50];
	char pass[50];
	int num;
	char nome[50];
	char email[50];
	char curso[50];
	char playlists[250];
} Touvinte;


typedef struct {
	long tipo;
	struct {
		char operacao[20];
		char info1[250];
		char info2[250];
		int myid;
	} dados;
} MsgOuvinte2Server;

typedef struct {
	long tipo;
	struct {
		Tmusica musica;
		char info1[250];
		int valor1;
		int caralhinho;
	} dados;
} MsgServer2Ouvinte;

typedef struct {
	long tipo;
	char texto[80];
} mensagem;

void estado_semaforo(int id_sem, unsigned short num_sem){
int status = semctl(id_sem, num_sem, GETVAL, 0);
printf("estado do semaforo: %d\n", status);
}

int valor_semaforo(int id_sem, unsigned short num_sem){
int status = semctl(id_sem, num_sem, GETVAL, 0);
return status;
}
