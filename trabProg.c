#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>


#define N 20
int contPostIt = 0;

int entregasFeitas = 0;
int maximoDeEntregas = 5;

int NUM_usuarios = 5;
void *usuario();
void *pombo();

void dorme_aleatorio();
void ColaPostIt();
void leva_mochila_ate_B_e_volta();

sem_t sairEntrega;//Semáforo para deixar o pombo sair para entrega
sem_t suaVez;//Semáforo para garantir 1 usuário colando postit por vez
sem_t pomboVoltou;//Semáforo para deixar os usuários voltarem a colar quando o pombo voltar

int main(int argc, char *argv[])
{
	if (argv[1]!=NULL)
	{
		NUM_usuarios = atoi(argv[1]);
		
		if (argv[2]!=NULL)
			maximoDeEntregas = atoi(argv[2]);
	}
	
	srand(time(NULL));
	
	//Iniciando os semáforos
	sem_init(&sairEntrega, 0, 0); //De início o pombo tem que esperar para sair para a entrega, então começa em 0
	sem_init(&suaVez, 0, 1); //O primeiro usuário não tem que esperar nada para poder colar os postitis, então começa em 1
	sem_init(&pomboVoltou,0 ,0); //O usuário que acordar o pombo vai espera-lo voltar, então começa em 0
	
	//Criando a thread do pombo
	pthread_t pombo_t;
	pthread_create(&pombo_t,NULL,pombo,NULL);
	
	//Criando quantas threads de usuário são indicadas pelo argv[1]
	pthread_t pessoas[NUM_usuarios];
	int userID[NUM_usuarios];//Como a thread recebe os argumentos por referencia, cada ID teve que ser uma variável diferente
	
	for(int i=0;i<NUM_usuarios;i++)
	{
		userID[i] = i+1;
		pthread_create(&pessoas[i],NULL,usuario,(void*)&userID[i]);
	}
	
	
	//Esperando todas as threads terminarem antes de terminar o programa
	pthread_join(pombo_t,NULL);
	for(int i=0;i<NUM_usuarios;i++)
	{
		pthread_join(pessoas[i],NULL);
	}
		
	
	printf("\nFIM\n");

	return 0;
}

void dorme_aleatorio()
{
	sleep((rand()%3)+1);
}
void ColaPostIt()
{

}
void leva_mochila_ate_B_e_volta()
{
	sleep((rand()%3)+1);
}

void *usuario(void *arg)
{	
	int *tid = (int*)arg;//Thread ID: Número que identifica o usuário
	while(1)
	{	
		dorme_aleatorio();//Tempo do usuário escrever uma mensagem e leva-la ao pombo
		
		sem_wait(&suaVez);
		
		//Condição para encerrar a thread
		if(entregasFeitas >= maximoDeEntregas)
		{	
			sem_post(&suaVez);
			printf("Usuario %2d parou\n",*tid);
			break;
		}
		
		ColaPostIt();
		contPostIt++;
		
		printf("Usuario %2d colou um Post-it - Post-its colados: %2d\n", *tid, contPostIt);
		
		//Verificando se 20 postits foram colados
		if (contPostIt == N)
		{			
			printf("\nMochila cheia\n");		

			sem_post(&sairEntrega);
			
			sem_wait(&pomboVoltou);
		}
		
		sem_post(&suaVez);
	}
}

void *pombo(void *arg) 
{	
	while(1)
	{
		sem_wait(&sairEntrega);// O pombo deve esperar ser acordado por um usuário
		printf("Pombo saiu para entrega\n\n");
		
		leva_mochila_ate_B_e_volta(); //Esperar o tempo necessário para o pombo ir e voltar
		contPostIt = 0;
		
		for (int i=0; i<N; i++)
		{
			printf("Adic. msg %d\n", i);
		}
		
		
		entregasFeitas +=1;
		printf("\nPombo voltou da entrega\n");
		printf("entrgas feitas:%d\n\n",entregasFeitas);
		
		
		sem_post(&pomboVoltou);// O pombo avisa que voltou e os usuários podem voltar a colar postitis
		
		//Condição para encerrar a thread
		if(entregasFeitas >= maximoDeEntregas)
		{	
			printf("Pombo parou\n");
			break;
		}
	}	
}

