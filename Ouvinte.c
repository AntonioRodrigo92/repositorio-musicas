#include "includes.h"

Touvinte ov;
int pid_filho;

void criar_filho() {
	pid_filho = fork();
	if (pid_filho < 0) {
		perror("erro no fork");
		exit(1);
	}
	if (pid_filho == 0) {
		int id_fila, status;
		id_fila = msgget(0xa687969, 0);
		exit_on_error (id_fila, "Erro no msgget");
		while(1) {
			mensagem mensagem;
			
			status = msgrcv(id_fila, &mensagem, sizeof(mensagem)-sizeof(mensagem.tipo), 27, 0);
			exit_on_error(status, "erro ao receber do Server");
			printf("mensagem do server: %s\n", mensagem.texto);
		}
	}
}

MsgOuvinte2Server login(){
	printf("indique o nickname: \n");
    fgets(ov.nick, 50, stdin);
    ov.nick[strlen(ov.nick)-1]=0;

//	printf("o nick : %s\n", ov.nick);

    printf("indique a password: \n");
    fgets(ov.pass, 50, stdin);
    ov.pass[strlen(ov.nick)-1]=0;
	
//	printf("a pass: %s\n", ov.pass);
		
	MsgOuvinte2Server m;
	m.tipo = 1;
//	printf("o tipo: %d\n", m.tipo);

//	m.dados.operacao = ("login\0");
	strcpy(m.dados.operacao, "login");
//	m.dados.operacao[strlen(m.dados.operacao)-1]=0;
//	printf("a operação: %s\n", m.dados.operacao);

//	m.dados.info1 = ov.nick;
//	m.dados.info1[strlen(m.dados.info1)-1]=0;

	strcpy(m.dados.info1, ov.nick);
//	printf("nick na mensagem: %s\n", m.dados.info1);

//	m.dados.info2 = ov.pass;
//	m.dados.info2[strlen(m.dados.info2)-1]=0;

	strcpy(m.dados.info2, ov.pass);
//	printf("pass na mensagem: %s\n", m.dados.info2); 

	m.dados.myid=5555;
//	printf("o myid: %d\n", m.dados.myid);

	return m;
	
	
}

void mensagem1(){
	MsgOuvinte2Server m;
	strcpy(m.dados.operacao,"list_music");
	strcpy(m.dados.info1, ov.nick);
	printf("Indique a playlist: \n");
	fgets(m.dados.info2,250,stdin);
	m.dados.info2[strlen(m.dados.info2)-1]=0;
	m.dados.myid=ov.num;
	m.tipo=1;
	int id, status;
	id = msgget(0xa87306,0);
	exit_on_error(id,"ERRO NO MSGGET DE MENSAGEM 1 \n");
	status = msgsnd(id, &m, sizeof (m) - sizeof (m.tipo),0);
	exit_on_error(status, "ERRO AO ENVIAR DE MENSAGEM1\n");

	MsgServer2Ouvinte s;
	int di, status2;
	di = msgget(0xa87306,0);
	exit_on_error(di,"ERRO NO MSGET2 DE MENSAGEM1\n");
	while(1){
		status2 = msgrcv(di,&s,sizeof(s) - sizeof(s.tipo), m.dados.myid, 0);
	
		exit_on_error(status2, "ERRO AO RECEBER DE MENSAGEM1\n");
		if( s.dados.valor1==99){
			break;
		}
		if( s.dados.caralhinho == 1 ){
			printf("%s  %s\n",s.dados.musica.ID_MUS, s.dados.musica.titulo);
		}
		else{
			printf("ERRO:PLAYLIST NAO EXISTE\n");
		}
	}
}
void mensagem2(){
	MsgOuvinte2Server m;
	strcpy(m.dados.operacao,"listen_playlist");
	printf("Indique a playlist para ouvir: \n");
	fgets(m.dados.info1,250,stdin);
	m.dados.info1[strlen(m.dados.info1)-1]=0;

	m.dados.myid=ov.num;
	m.tipo=1;
	strcpy(m.dados.info2,ov.nick);
	
	int id, status;
	id = msgget(0xa87306,0);
	exit_on_error(id,"ERRO NO MSGGET DE MEN2\n");
	status = msgsnd(id, &m,sizeof(m)-sizeof(m.tipo),0);
	exit_on_error(status,"ERRO AO ENVIAR MEN2\n");

	MsgServer2Ouvinte s;
	int di,status2;
	di = msgget(0xa87306,0);
	exit_on_error(di, "ERRO NO MSGET2 DE MEN2\n");
	while(1){
		status2 = msgrcv(di,&s,sizeof(s)-sizeof(s.tipo), m.dados.myid,0);
		exit_on_error(status2, "ERRO AO RECEBER DE MEN2\n");
		if( s.dados.valor1==99){
            break;
        }
		printf("recebi ? %d\n", status2);
		
		if( s.dados.caralhinho == 1 ){
            printf("A ouvir: %s  %s\n",s.dados.musica.ID_MUS, s.dados.musica.titulo);
        }
        else{
            printf("ERRO:PLAYLIST NAO EXISTE\n");
        }
	}



}

void mensagem3(){
	MsgOuvinte2Server m;
	strcpy(m.dados.operacao,"get_playlist");
	printf("Indique a playlist a associar: \n");
	fgets(m.dados.info1,250,stdin);
	m.dados.info1[strlen(m.dados.info1)-1]=0;
	m.dados.myid=ov.num;
	m.tipo=1;
	strcpy(m.dados.info2,ov.nick);

	int id, status;
	id = msgget(0xa87306,0);
	exit_on_error(id,"ERRO NO MSGGET DE MEN3\n");
	status = msgsnd(id,&m,sizeof(m)-sizeof(m.tipo),0);
	exit_on_error(status,"ERRO AO ENVIAR MEN3\n");
	
	MsgServer2Ouvinte s;
	int di, status2;
	di = msgget(0xa87306,0);
	exit_on_error(di,"ERRO NO MSGET2 DE MEN3");
	status2 = msgrcv(di,&s,sizeof(s)-sizeof(s.tipo),m.dados.myid,0);
	exit_on_error(status2,"ERRO AO RECEBER DE MEN3");
	if(s.dados.caralhinho==1){
		printf("%s\n ", s.dados.info1);
	}
	else{
		printf("ERRO: PLAYLIST NAO EXISTE\n");
	}
}
void mensagem0(){
	MsgOuvinte2Server m;
	strcpy(m.dados.operacao,"logout");
	m.tipo=1;
	m.dados.myid=ov.num;
	strcpy(m.dados.info1,ov.nick);
	strcpy(m.dados.info2, ov.pass);
	
	int id,status;
	id = msgget(0xa87306,0);
//	printf("o id de men0 %d\n", id);
	exit_on_error(id,"ERRO NO MSGGET DE MEN0");
	status = msgsnd(id,&m,sizeof(m)-sizeof(m.tipo),0);
//	printf("a env0 ? %d\n", status);
	exit_on_error(status,"ERRO AO ENVIAR MEN0");
	
	MsgServer2Ouvinte s;
	int  status2;
	status2 = msgrcv(id,&s,sizeof(s)-sizeof(s.tipo), m.dados.myid,0);
//	printf("o status rec0%d\n", status2);
	exit_on_error(status2,"ERRO AO RECEBER DE MEN0");
	printf("%s \n", s.dados.info1);
	if(s.dados.caralhinho==1){
		exit(1);
	}
}

void receberlog( MsgServer2Ouvinte so){
  //  printf("a respostas de num %d\n", so.dados.valor1);

    if(so.dados.caralhinho==0){

        printf( "OUVINTE NAO IDENTIFICADO NA MEMORIA\n");
        exit(1);
    }
	 else{
        ov.num=so.dados.valor1;
        printf("%s \n", so.dados.info1);

    }
}

void main(){

	criar_filho();

	MsgServer2Ouvinte so;
	MsgOuvinte2Server m;	
	m=login();
	
	int id,status2, status;
	id = msgget(0xa87306,0);
	exit_on_error(id, "ERRO NO MSGGET\n");
//	printf("o valor do id = %d\n", id);	
	status = msgsnd(id, &m, sizeof (m) - sizeof(m.tipo) , 0);
	exit_on_error(status, "ERRO AO ENVIAR\n");
//	printf("o valor do status = %d\n", status);	
	
	status2 = msgrcv(id, &so, sizeof(so) - sizeof(so.tipo),m.dados.myid,0);
//	printf("tipo do so %d\n", so.tipo);
//	printf("tipo do m %d\n", m.tipo);
//	printf("status2 = %d\n", status2);
	exit_on_error(status2,"ERRO AO RECEBER\n");
//	printf("a respostas de status %d\n", so.dados.caralhinho);
	receberlog(so);




//	if(so.dados.caralhinho==0){

//		printf( "OUVINTE NAO IDENTIFICADO NA MEMORIA\n");
//		exit(1);
//	}
//	else{
//		ov.num=so.dados.valor1;
//		printf("%s \n", so.dados.info1);

//	}

	printf("--------MENU--------\n1. Listar musicas de playlist\n2. Ouvir playlist\n3. Associar a playlist\n0. Sair(Logout)\n");




	while(1==1){
		char  s [100] ;	
		fgets(s, 100, stdin);
		s[strlen(s)-1]=0;

//		printf("o s %s\n",s);	
		if(strcmp(s,"1")==0){
			mensagem1();
		}
		if(strcmp(s,"2")==0){
			mensagem2();
		}
		if(strcmp(s,"3")==0){
			mensagem3();
		}
	//	printf("strcmp %d\n", strcmp(s,"0"));
		if(strcmp(s,"0")==0) {
		//	printf("vou tentar");			
			mensagem0();
		}

	
	


	}		






		
	
	
	




}
