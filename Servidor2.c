#include "includes.h"

Touvinte *ouv;
Tplaylist *pl;
Tmusica *mus;

int id_mem_ouv;
int id_mem_pl;
int id_mem_mus;
int id_sem;
int status_sem;
int pid_filho;

//void inicializa_mem_musicas(Tmusica *mus);
//void inicializa_mem_playlists(Tplaylist *pl);
//void inicializa_mem_ouvintes(Touvinte *ouv);

int total(char *str) {
    int cont = 0;
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] == ':') {
            ++cont;
        }
    }
    int total = cont + 1;
    return total;
}

void criar_filho() {
	pid_filho = fork();
	if (pid_filho < 0) {
		perror("erro no fork");
		exit(1);
	}
	if (pid_filho == 0) {
		int id_man_rec, id_man_env, status_man_rec, status_man_env;

		id_man_rec = msgget(0xa587969, 0666 | IPC_CREAT);
		exit_on_error(id_man_rec, "Erro no smgget");

		id_man_env = msgget(0xa687969, 0666 | IPC_CREAT);
		exit_on_error(id_man_env, "Erro no msgget");

		while(1) {	
			mensagem mensagem_rec;
			mensagem mensagem_env;

			printf("antes de receber manutencao\n");
			status_man_rec = msgrcv (id_man_rec, &mensagem_rec, sizeof(mensagem)-sizeof(mensagem_rec.tipo), 27, 0);
			exit_on_error(status_man_rec, "erro ao receber Admin");
			printf("mensagem do admin: %s\n", mensagem_rec.texto);

			mensagem_env.tipo = 27;
			strcpy(mensagem_env.texto, mensagem_rec.texto);
			printf("mensagem enviar: %s\n", mensagem_env.texto);

			printf("antes d enviar manutencao\n");
			status_man_env = msgsnd(id_man_env, &mensagem_env, sizeof(mensagem)-sizeof(mensagem_env.tipo), 0);
			exit_on_error(status_man_env, "erro ao enviar ao Ouvinte");			
		}
	}
}

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

void inicializa_semaforos(int i) {
    status_sem = semctl(id_sem, i, SETVAL, 1);
    exit_on_error(status_sem, "erro na inicializacao dos semaforos");
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

void criar_mem_ouvintes() {
	id_mem_ouv = shmget(0xa387969, MAX_OUV * sizeof(Touvinte) , IPC_CREAT |IPC_EXCL| 0666 );
    if (id_mem_ouv < 0) {
        id_mem_ouv = shmget(0xa387969, MAX_OUV * sizeof(Touvinte) , 0);
        exit_on_error(id_mem_ouv, "shmget ouvintes1");
        ouv = (Touvinte *)shmat(id_mem_ouv, 0 , 0);
        exit_on_null(ouv, "erro no attach");
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
}

void criar_mem_musicas() {
   id_mem_mus = shmget(0xa187969, MAX_MUS * sizeof(Touvinte) , IPC_CREAT |IPC_EXCL| 0666 );
    if (id_mem_mus < 0) {
        printf("dentro do if criar mem musicas\n");
        id_mem_mus = shmget(0xa187969, MAX_MUS * sizeof(Touvinte) , 0);
        exit_on_error(id_mem_mus, "shmget musicas1");
        mus = (Tmusica *)shmat(id_mem_mus, 0 , 0);
        exit_on_null(mus, "erro no attach");
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


void reslist(MsgOuvinte2Server m, int id){
	int temp=0, status;
	printf("vou comercar\n");
	for (int u = 0; u < MAX_PL; u++) {
		printf("entrei no for\n");
		if(mus[u].duracao != -1 && strcmp(pl[u].nome, m.dados.info2) == 0){
			char temporaria[100];
			printf("encontrei a play\n");
			for(int i = 0; i<total(pl[u].musicas);i++) {	//AQUI!!!
				obter_substring(pl[u].musicas, temporaria, ':', i);
				printf("a dividir\n");
				for (int a = 0; a < MAX_MUS; a++) {	//AQUI!!!
					if ( mus[a].duracao !=-1 && strcmp(temporaria, mus[a].ID_MUS) == 0) {
						MsgServer2Ouvinte s;
		                Tmusica musica;
				        strcpy(musica.ID_MUS, mus[a].ID_MUS);
						strcpy(musica.artista,mus[a].artista);
						strcpy(musica.titulo, mus[a].titulo);
						musica.top=mus[a].top;
						s.dados.musica=musica;
						temp=1;
						s.dados.caralhinho=temp;
						s.tipo=m.dados.myid;
						printf("criei a mensagem\n");
						status = msgsnd(id,&s,sizeof(s)-sizeof(s.tipo),0);
						printf(" enviei list?%d\n", status);
						exit_on_error(status, "ERRO AO ENVIAR LIST");			
					}
				}
			}	
		}
	}
	if(temp!=1){
		MsgServer2Ouvinte s;
		s.tipo=m.dados.myid;
        s.dados.caralhinho=0;
		status = msgsnd(id, &s, sizeof(s)-sizeof(s.tipo),0);
        exit_on_error(status,"ERRO AO ENVIAR LIST PROB");

	}else{
	MsgServer2Ouvinte so;
	so.tipo=m.dados.myid;
	so.dados.valor1=99;
	status= msgsnd(id,&so,sizeof(so)-sizeof(so.tipo), 0);
	exit_on_error(status, "ERRO AO ENVIAR FIM");
	}
	
}


void resouvir(MsgOuvinte2Server m, int id ){
	int temp=0;
	int status;
//	id = msgget(0xa873062, 0666 | IPC_CREAT | IPC_EXCL);
//	exit_on_error(id,"ERRO NO MSGGETOUVIF");
	
	down_sem_playlist();
	down_sem_musica();

    for (int u = 0; u < MAX_PL; u++) {
//	printf("entrei no for\n");
		printf("recebi%syei\n", m.dados.info1);

		
		printf("encontrei? da pl %s %d\n",pl[u].nome,strcmp(pl[u].nome, m.dados.info1));
        if( strcmp(pl[u].nome,m.dados.info1) == 0 ){
            char temporaria[100];
			printf("recebi %s\n", m.dados.info1);
			printf("a playlist %s\n", pl[u].nome);

		//	printf("ja sei a pl\n");
            for(int i = 0; i<total(pl[u].musicas);i++) {    //AQUI!!!
                obter_substring(pl[u].musicas, temporaria, ':', i);
			//	printf("a dividir\n");
                for (int a = 0; a < MAX_MUS; a++) { //AQUI!!!
				//	printf("vai mais um\n");
					printf("a comp %s com %s res %d\n", mus[a].ID_MUS, temporaria ,strcmp(temporaria, mus[a].ID_MUS));
                    if ( mus[a].duracao !=-1 && strcmp(temporaria, mus[a].ID_MUS) == 0 || strcmp(temporaria, mus[a].ID_MUS) == 10) {
						MsgServer2Ouvinte s;
                        Tmusica musica;
						mus[a].top++;
						printf("top da mus %s  %d\n", mus[a].ID_MUS, mus[a].top);
                        strcpy(musica.ID_MUS, mus[a].ID_MUS);
                        strcpy(musica.artista,mus[a].artista);
                        strcpy(musica.titulo, mus[a].titulo);
                        musica.top=mus[a].top;
                        s.dados.musica=musica;
                        temp=1;
                        s.dados.caralhinho=temp;
                        s.tipo=m.dados.myid;
						printf("criei a mensagem\n");
                        status = msgsnd(id,&s,sizeof(s)-sizeof(s.tipo),0);
						printf("status de ouvir %d\n", status);
                        exit_on_error(status, "ERRO AO ENVIAR OUVIR	");
					}
				}
			}
		}        
	}	

	up_sem_playlist();
	up_sem_musica();

    if(temp!=1){
		MsgServer2Ouvinte s;
        s.dados.caralhinho=0;
		s.tipo=m.dados.myid;
		status = msgsnd(id, &s, sizeof(s)-sizeof(s.tipo),0);
		exit_on_error(status,"ERRO AO ENVIAR OUVIR PROB");
    }
	else{
		MsgServer2Ouvinte so;
	    so.tipo=m.dados.myid;
		so.dados.valor1=99;
		status= msgsnd(id,&so,sizeof(so)-sizeof(so.tipo), 0);
	    exit_on_error(status, "ERRO AO ENVIAR FIM");
	}
}


void reslog(MsgOuvinte2Server m, int id){
	MsgServer2Ouvinte s;
	int temp;
//	id = msgget(0xa87306a, 0666 | IPC_CREAT | IPC_EXCL);
//	exit_on_error(id, "ERRO NO MSGGETLOG");
	s.tipo=m.dados.myid;
	
	printf("RECEBI, dentro do reslog\n");
	
	down_sem_ouvinte();

	for (int u = 0; u < MAX_OUV; u++) {
		if (ouv[u].num != 0) {
	

			if(strcmp(ouv[u].pass, m.dados.info2) == 0 && strcmp(ouv[u].nick, m.dados.info1) == 0){
				temp=1;
				printf("valor do temp %d\n", temp);
				s.dados.valor1=ouv[u].num;
				strcpy(s.dados.info1,"sucesso");
				s.dados.caralhinho=temp;;
				printf("valor do s.dados.status %d\n", s.dados.caralhinho);
				printf("num mem%d\n", s.dados.valor1);
				printf("a mensagem %s\n", s.dados.info1);
		}
		//incrementar ouvinte

	}
	}

	up_sem_ouvinte();

	if( temp != 1 ){
		printf("temp em if %d\n", temp);

		s.dados.caralhinho=0;
		printf("status em if %d\n", s.dados.caralhinho);
	}
	int status;
	status = msgsnd(id, &s, sizeof(s)-sizeof(s.tipo),0);
	printf("enviou?? %d\n" ,status); 
	exit_on_error(status, "ERRO AO ENVIAR LOG");

	


}

void resget(MsgOuvinte2Server m, int id){
	MsgServer2Ouvinte s;
	int temp=0, status;
	s.tipo=m.dados.myid;
	char temporaria[100];
//	id = msgget(0xa873063, 0666 | IPC_CREAT | IPC_EXCL);
//	exit_on_error(id, "ERRO NO MSGGETGET");

	down_sem_playlist();
	down_sem_ouvinte();
	
	for (int u = 0; u < MAX_PL; u++) {
        if(strcmp(pl[u].nome, "vazio") != 0 && strcmp(pl[u].nome, m.dados.info1) == 0){
			temp=1;
			s.dados.caralhinho=temp;
		
			for (int a = 0; a < MAX_OUV; a++) {	
				if(strcmp(ouv[a].nick, m.dados.info2) == 0){
					//buscar as playlists do ouvinte

					printf("o total %d\n", total(ouv[a].playlists));
					for(int i = 0; i<total(ouv[a].playlists);++i){//AQUI!!!!!!
						obter_substring(ouv[a].playlists, temporaria, ':', i);
						printf("a plu %s  a temp %s\n", pl[u].ID_PL, temporaria);
						if(strcmp(temporaria,pl[u].ID_PL )==0){
							strcpy(s.dados.info1,"ja esta associado");
						//	printf("a escrever\n");	
							break;	
						}
						else{
							if(i==total(ouv[a].playlists)-1){
							strcat(ouv[a].playlists, ":");
							strcat(ouv[a].playlists, pl[u].ID_PL);					
					//ATENCAO	
						//	printf(" a confirmaR\n");
				//associar na memoria a play ao ouvinte
							strcpy(s.dados.info1,"associacao concluida");

							break;
							}
				
						}
					}
				}
			}
		}
	}

	up_sem_playlist();
    up_sem_ouvinte();	

	if(temp!=1){
		s.dados.caralhinho=0;
	}
	status = msgsnd(id, &s, sizeof(s)-sizeof(s.tipo),0);
    exit_on_error(status, "ERRO AO ENVIAR GET");
}
void reslogout(MsgOuvinte2Server m, int id){
	MsgServer2Ouvinte s;
	int temp, status;
	s.tipo=m.dados.myid;
	s.dados.caralhinho=1;
	printf("a preparar mensagem\n");
	strcpy(s.dados.info1,"a desligar ouvinte");	
//	id = msgget(0xa873060, 0666 | IPC_CREAT | IPC_EXCL);
//	exit_on_error(id, "ERRO NO MSGGETOUT");
	status = msgsnd(id, &s, sizeof(s)-sizeof(s.tipo),0);
	exit_on_error(status, "ERRO AO ENVIAR OUT ");
}


void main() { 
	system("ipcrm -Q 0x0a687969");
	system("ipcrm -Q 0xa87306");
	int id, status;
//	id = msgget(0xa87306, 0666 |IPC_CREAT);
    id = msgget(0xa87306, 0666 |IPC_CREAT);
    exit_on_error(id,"Erro no smgget");

	criar_filho();
	
	cria_semaforos();

//	id_list = msgget(0xa873061, 0666 | IPC_CREAT );
 //   exit_on_error(id_list,"ERRO NO MSGGETLIST");

//	id_ouvir = msgget(0xa873062, 0666 | IPC_CREAT);
 //   exit_on_error(id_ouvir,"ERRO NO MSGGETOUVIF");

//	id_log = msgget(0xa87306a, 0666 | IPC_CREAT);
 //   exit_on_error(id_log, "ERRO NO MSGGETLOG");

//	id_get = msgget(0xa873063, 0666 | IPC_CREAT);
 //   exit_on_error(id_get, "ERRO NO MSGGETGET");

//	id_out = msgget(0xa873060, 0666 | IPC_CREAT);
//	exit_on_error(id, "ERRO NO MSGGETOUT");
while(1==1){
	printf("antes de receber do ouvinte\n");
	MsgOuvinte2Server m;
	status = msgrcv (id, &m, sizeof(m)-sizeof(m.tipo), 1, 0);
	exit_on_error(status, "erro ao receber");
	printf("o valor do status = %d\n", status);

//	printf("antes de receber manutencao\n");
//	mensagem mensagem;
//	status_man = msgrcv (id_man, &mensagem, sizeof(mensagem)-sizeof(mensagem.tipo), 27, 0);
//	exit_on_error(status_man, "erro ao receber Admin");
//	printf("mensagem do admin: %s\n", mensagem.texto);

	criar_mem_ouvintes();
	criar_mem_musicas();
	criar_mem_playlists();

	printf("valor do %s... \n", m.dados.operacao);

	if(strcmp(m.dados.operacao,"login")==0){
		printf("vou ligar\n");
		reslog(m, id);
	}
	if(strcmp(m.dados.operacao,"list_music")==0){
		printf("vou ler\n");
		reslist(m, id);

	}
	printf("está? %d\n",strcmp(m.dados.operacao,"listen_playlist"));
	if(strcmp(m.dados.operacao,"listen_playlist")==0 ){
		printf("vou ouvir\n");
		resouvir(m, id);	
	}
	if(strcmp(m.dados.operacao,"get_playlist")==0){
		printf("vou buscar\n");
		resget(m, id);
			
	}
	if(strcmp(m.dados.operacao,"logout")==0){
		printf("vou sair\n");
		reslogout(m, id);
	
	}
}
} 
