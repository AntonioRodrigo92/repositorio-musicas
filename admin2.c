#include "includes.h"

int id_mem_mus;
int id_mem_pl;
int id_mem_ouv;
int id_fila;

int id_sem;
int status;

void inicializa_mem_musicas(Tmusica *mus);
void inicializa_mem_playlists(Tplaylist *pl);
void inicializa_mem_ouvintes(Touvinte *ouv);
void enviar_mensagem_espera();

Touvinte *ouv;
Tplaylist *pl;
Tmusica *mus;

void down_sem_ouvinte() {
    int res = semop(id_sem, &DOWNouvinte, 1);
    exit_on_error(res, "ERRO NO DOWN ouvinte");
}

void up_sem_ouvinte() {
    int res = semop(id_sem, &UPouvinte, 1);
    exit_on_error(res, "ERRO NO UP ouvinte");
}

void down_sem_musica() {
    int res = semop(id_sem, &DOWNmusica, 1);
    exit_on_error(res, "ERRO NO DOWN musica");
}

void up_sem_musica() {
    int res = semop(id_sem, &UPmusica, 1);
    exit_on_error(res, "ERRO NO UP musica");
}

void down_sem_playlist() {
    int res = semop(id_sem, &DOWNplaylist, 1);
    exit_on_error(res, "ERRO NO DOWN playlist");
}

void up_sem_playlist() {
    int res = semop(id_sem, &UPplaylist, 1);
    exit_on_error(res, "ERRO UP playlist");
}

void criar_mem_musicas() {
    id_mem_mus = shmget(0xa187969, MAX_MUS * sizeof(Touvinte) , IPC_CREAT |IPC_EXCL| 0666 );
    if (id_mem_mus < 0) {
        id_mem_mus = shmget(0xa187969, MAX_MUS * sizeof(Touvinte) , 0);
        exit_on_error(id_mem_mus, "shmget musicas1");
        mus = (Tmusica *)shmat(id_mem_mus, 0 , 0);
        exit_on_null(mus, "erro no attach");
    }
    else {
        mus = (Tmusica *)shmat(id_mem_mus, 0 , 0);
        exit_on_null(mus, "erro no attach");
		inicializa_mem_musicas(mus);
    }
}

void criar_mem_playlists() {
    id_mem_pl = shmget(0xa287969, MAX_PL * sizeof(Tplaylist) , IPC_CREAT |IPC_EXCL| 0666 );
    if (id_mem_pl < 0) {
        id_mem_pl = shmget(0xa287969, MAX_PL * sizeof(Tplaylist) , 0);
        exit_on_error(id_mem_pl, "shmget playlists1");
        pl = (Tplaylist *)shmat(id_mem_pl, 0 , 0);
        exit_on_null(pl, "erro no attach");
    }
    else {
        pl = (Tplaylist *)shmat(id_mem_pl, 0 , 0);
        exit_on_null(pl, "erro no attach");
		inicializa_mem_playlists(pl);
    }
}

void criar_mem_ouvintes() {
    id_mem_ouv = shmget(0xa387969, MAX_OUV * sizeof(Touvinte) , IPC_CREAT |IPC_EXCL| 0666 );
	if (id_mem_ouv < 0) {
		id_mem_ouv = shmget(0xa387969, MAX_OUV * sizeof(Touvinte) , 0);
		exit_on_error(id_mem_ouv, "shmget ouvintes1");
		ouv = (Touvinte *)shmat(id_mem_ouv, 0 , 0);
		exit_on_null(ouv, "erro no attach");
	}
	else {
		ouv = (Touvinte *)shmat(id_mem_ouv, 0 , 0);
        exit_on_null(ouv, "erro no attach");
		inicializa_mem_ouvintes(ouv);
	}
}

void obter_substring(char linha[], char resultado[], char separador, int indice) {
   int i, j=0, meu_indice = 0;
   for (i=0; linha[i] != '\0'; i++) {
     if ( linha[i] == separador ) { 
       meu_indice++;
     } else if (meu_indice == indice) {
       resultado[j++]=linha[i];
     }
   }
   resultado[j]='\0';
}

void carrega_mem_playlists (Tplaylist *pl){
    int n=0;
    char linha[250];
    char tmp[100];
    FILE *fplaylists = fopen ("playlists.txt", "r");
    int i = 0;
    int pos=0;
    while ( fgets(linha, 250, fplaylists ) != NULL ){
        linha[strlen(linha)-1]='\0';
        if ( pos < MAX_PL ){
            strcpy(pl[pos].musicas, "");
            obter_substring(linha,pl[pos].ID_PL,':',0);
            obter_substring(linha,pl[pos].nome,':',1);

            for(int j=0;j<strlen(linha);j++){
                if(linha[j] == ':'){
                    n++;
                }
            }
            for(int k=2; k<n+1; k++){
                obter_substring(linha,tmp, ':',k);
                strcat(tmp,":");
                strcat(pl[pos].musicas, tmp);
                strcat(tmp,"");
            }
            pl[pos].musicas[strlen(pl[pos].musicas)-1]='\0';
            n=0;
            pos++;
        }else {
            printf("Impossivel carregar toda a memoria. Espaço insuficiente\n");
        }
        i++;
    }
    fclose(fplaylists);
}

void carrega_mem_ouvintes(Touvinte *ouv){
    int n=0;
    char linha[250];
    char tmp[100];
    FILE *fouvinte = fopen ("ouvintes.txt", "r");
    int i = 0;
    int pos=0;
    while ( fgets(linha, 250, fouvinte ) != NULL ){
        linha[strlen(linha)-1]='\0';
        if ( pos < MAX_OUV ){
            strcpy(ouv[pos].playlists, "");
            obter_substring(linha,ouv[pos].nick,':',0);
            obter_substring(linha,ouv[pos].pass,':',1);
            obter_substring(linha,tmp,':',2);
            ouv[pos].num=atoi(tmp);
			obter_substring(linha,ouv[pos].nome,':',3);
			obter_substring(linha,ouv[pos].email,':',4);
			obter_substring(linha,ouv[pos].curso,':',5);			

            for(int j=0;j<strlen(linha);j++){
                if(linha[j] == ':'){
                    n++;
                }
            }
            for(int k=6; k<n+1; k++){
                obter_substring(linha,tmp, ':',k);
                strcat(tmp,":");
                strcat(ouv[pos].playlists, tmp);
                strcat(tmp,"");
            }
            ouv[pos].playlists[strlen(ouv[pos].playlists)-1]='\0';
            n=0;
            pos++;
        }else {
            printf("Impossivel carregar toda a memoria. Espaço insuficiente\n");
        }
        i++;
    }
    fclose(fouvinte);
}

void carrega_mem_musicas(Tmusica *mus){
    char linha[250];
    char tmp[100];
    FILE *fmusicas = fopen ("musicas.txt", "r");
    int pos=0;
    while ( fgets(linha, 250, fmusicas ) != NULL ){
        linha[strlen(linha)-1]='\0';
        if ( pos < MAX_MUS ){         
			obter_substring(linha,mus[pos].ID_MUS,':',0);
			obter_substring(linha,mus[pos].titulo,':',1);
            obter_substring(linha,mus[pos].artista,':',2);
			char duracao[5];
			obter_substring(linha, duracao, ':', 3);
			mus[pos].duracao = atoi(duracao);
			char ano[5];
			obter_substring(linha, ano, ':', 4);
			mus[pos].ano = atoi(ano);
			obter_substring(linha,mus[pos].genero,':',5);
			char top[5];
			obter_substring(linha,top,':',6);
			mus[pos].top = atoi(top);
            pos++;
        }else {
            printf("Impossivel carregar toda a memoria. Espaço insuficiente\n");
        }
    }
    fclose(fmusicas);
}


void inicializa_mem_ouvintes (Touvinte *ouv) {
    for(int i = 0; i < MAX_OUV; i++){
//      printf("cheguei aqui\n");
        strcpy(ouv[i].nick, "vazio");
        strcpy(ouv[i].pass, "vazio");
        ouv[i].num=0;
        strcpy(ouv[i].playlists, "vazio");
    }
}

void inicializa_mem_playlists (Tplaylist *pl) {
    for(int i = 0; i < MAX_PL; i++){
//	    printf("cheguei aqui\n");
        strcpy(pl[i].ID_PL, "vazio");
        strcpy(pl[i].nome, "vazio");
        strcpy(pl[i].musicas, "vazio");
    }
}

void inicializa_mem_musicas (Tmusica *mus) {
    for(int i = 0; i < MAX_MUS; i++){
//      printf("cheguei aqui\n");
        strcpy(mus[i].ID_MUS, "vazio");
        strcpy(mus[i].titulo, "vazio");
		strcpy(mus[i].artista, "vazio");
		mus[i].duracao = -1;
		mus[i].ano = -1;
        strcpy(mus[i].genero, "vazio");
		mus[i].top = -1;
    }
}

void descarregar_mem_ouvintes(Touvinte* ouv) {
	FILE *f = fopen("ouvintes.txt", "w");
	if (f == NULL) {
		perror("Erro de escrita: ");
		exit(3);
	}
	int i;
	for (i = 0; i < MAX_OUV && ouv[i].num != 0; i++) {
		fprintf(f, "%s:%s:%d:%s:%s:%s:%s\n", ouv[i].nick, ouv[i].pass, ouv[i].num, ouv[i].nome, ouv[i].email, ouv[i].curso, ouv[i].playlists);
	}
	fclose(f);
}

void descarregar_mem_musicas(Tmusica* mus) {
    FILE *f = fopen("musicasTESTE.txt", "w");
    if (f == NULL) {
        perror("Erro de escrita: ");
        exit(3);
    }
    int i;
    for (i = 0; i < MAX_MUS && mus[i].ano != -1; i++) {
        fprintf(f, ":%s:%s:%s:%d:%d:%s:%d\n", mus[i].ID_MUS, mus[i].titulo, mus[i].artista, mus[i].duracao, mus[i].ano, mus[i].genero, mus[i].top);
    }
    fclose(f);
}

void descarregar_mem_playlists(Tplaylist* pl) {
    FILE *f = fopen("playlistsTESTE.txt", "w");
    if (f == NULL) {
        perror("Erro de escrita: ");
        exit(3);
    }
    int i;
    for (i = 0; i < MAX_PL && strcmp(pl[i].nome, "vazio") != 0; i++) {
        fprintf(f, ":%s:%s:%s\n", pl[i].ID_PL, pl[i].nome, pl[i].musicas);
    }
    fclose(f);
}

void em_manutencao() {
	down_sem_ouvinte();
	down_sem_musica();
	down_sem_playlist();
	enviar_mensagem_espera();
	printf("*************************************\n");
	printf("	EM MANUTENÇÃO	\n");
	printf("*************************************\n");
	sleep(10);
	up_sem_ouvinte();
	up_sem_musica();
	up_sem_playlist();
	printf("A aplicação está novamente ativa\n");
}

void mostra_mem_ouvintes(Touvinte *ouv){
    for (int u = 0; u < MAX_OUV; u++ ) {
        if(ouv[u].num != 0){
            printf ("Ouvinte %d: %s %s %d %s\n",u, ouv[u].nick, ouv[u].pass, ouv[u].num, ouv[u].playlists );
        }
    }
}

void mostra_mem_musicas(Tmusica *mus){
    for (int u = 0; u < MAX_MUS; u++ ) {
        if(mus[u].duracao != -1){
            printf ("Musica %d: %s %s %s %d %d %s %d\n",u, mus[u].ID_MUS, mus[u].titulo, mus[u].artista, mus[u].duracao, mus[u].ano, mus[u].genero, mus[u].top);
        }
    }
}

void mostra_mem_playlists(Tplaylist *pl){
    for (int u = 0; u < MAX_PL; u++ ) {
        if(strcmp(pl[u].nome, "vazio") != 0){
            printf ("Playlist %d: %s %s %s\n",u, pl[u].ID_PL, pl[u].nome, pl[u].musicas);
        }
    }
}

void inicializa_semaforos(int i) {
    status = semctl(id_sem, i, SETVAL, 1);
    exit_on_error(status, "erro na inicializacao dos semaforos");
}

void estado_todos_semaforos(int id_sem){
    for (int i=0; i<NUM_SEMAFOROS; i++){
        int valor_sem=semctl(id_sem, i, GETVAL, 0);
        printf("valor do semaforo %d: %d\n", i, valor_sem);
    }
}


void cria_semaforos(){
    id_sem = semget(0xa487969, 3, 0600 | IPC_CREAT | IPC_EXCL );
    if (id_sem < 0){
        id_sem = semget(0xa487969, 3, 0600 | IPC_CREAT);
        exit_on_error(id_sem, "algo correu mal com o semget");
    }
	else {
        exit_on_error(id_sem, "algo correu mal com o semget");
        for (int i=0; i<NUM_SEMAFOROS; i++){
            inicializa_semaforos(i); 
        }
    }
//  estado_todos_semaforos(id_sem);
}

void ligar_fila_mensagens() {
	id_fila = msgget(0xa587969, 0);
	exit_on_error(id_fila, "Erro no msgget");
}

void enviar_mensagem_espera() {
	mensagem m;
	m.tipo = 27;
	strcpy(m.texto, "servidor em manutencao. Aguarde 10 segundos");
	int status = msgsnd(id_fila, &m, sizeof(mensagem) - sizeof(m.tipo), 0);
	exit_on_error(status, "erro ao enviar mensagem");
}

void MENU() {
	int s;
	printf("----------MENU----------\n");
	printf("1. Carregar Ficheiros\n");
	printf("2. Descarregar memória\n");
	printf("3. Manutenção da aplicação\n");
	printf("4. Mostrar memória\n");
	printf("0. Sair\n");

	while(1) {
		scanf("%d", &s);
		if (s == 1) {
			down_sem_ouvinte();	
			inicializa_mem_ouvintes(ouv);
			carrega_mem_ouvintes(ouv);
			up_sem_ouvinte();

			down_sem_musica();
			inicializa_mem_musicas(mus);
			carrega_mem_musicas(mus);
			up_sem_musica();

			down_sem_playlist();
			inicializa_mem_playlists(pl);
			carrega_mem_playlists(pl);
			up_sem_playlist();
		}
		if (s == 2) {
			down_sem_ouvinte();
            descarregar_mem_ouvintes(ouv);
			up_sem_ouvinte();
			
			down_sem_musica();
			descarregar_mem_musicas(mus);
			up_sem_musica();

			down_sem_playlist();
			descarregar_mem_playlists(pl);
			up_sem_playlist();
        }
		if (s == 3) {
			em_manutencao();        
        }
		if (s == 4) {
			down_sem_ouvinte();
            mostra_mem_ouvintes(ouv);
			up_sem_ouvinte();
			
			down_sem_musica();
			mostra_mem_musicas(mus);
			up_sem_musica();

			down_sem_playlist();
			mostra_mem_playlists(pl);
			up_sem_playlist();
        }
		if (s == 0) {
            exit(1);
        }
	}	
}

int main() {
	criar_mem_musicas();
	criar_mem_playlists();
	criar_mem_ouvintes();

	ligar_fila_mensagens();

	cria_semaforos();

	MENU();
}
