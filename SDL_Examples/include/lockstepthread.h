
/* Public Domain / CC0 3d Lock-Step Threading Implementation


Written by Gek (DMHSW) in 2020


*/

#ifndef LOCKSTEPTHREAD_H
#define LOCKSTEPTHREAD_H
#include <pthread.h>
#include <stdio.h>
typedef struct {
	pthread_mutex_t myMutex;
	pthread_barrier_t myBarrier;
	pthread_t myThread;
	int isThreadLive;
	int shouldKillThread;
	int state;
	void (*execute)();
} lsthread;
void init_lsthread(lsthread* t);
void start_lsthread(lsthread* t);
void kill_lsthread(lsthread* t);
void destroy_lsthread(lsthread* t);
void lock(lsthread* t);
void step(lsthread* t);
void* lsthread_func(void* me_void);
#ifdef LOCKSTEPTHREAD_IMPL
//function declarations

void init_lsthread(lsthread* t){
	t->myMutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	pthread_barrier_init(&t->myBarrier, NULL, 2);
	t->isThreadLive = 0;
	t->shouldKillThread = 0;
	t->state = 0;
	t->execute = NULL;
}
void destroy_lsthread(lsthread* t){
	pthread_mutex_destroy(&t->myMutex);
	pthread_barrier_destroy(&t->myBarrier);
}
void lock(lsthread* t){
	if(t->state == 1)return;//if already locked, nono
	if(!t->isThreadLive)return;
	//puts("\nLocking! <lock>");
	pthread_barrier_wait(&t->myBarrier);
	//puts("\nPast Wait!");
	if(pthread_mutex_lock(&t->myMutex))
		puts("\nError locking mutex.");
	t->state = 1;
	//puts("\nPast Lock </lock>");
}

void step(lsthread* t){
	if(t->state == -1)return; //if already stepping, nono
	if(!t->isThreadLive)return;
	//puts("\nStepping! <step>");
	if(pthread_mutex_unlock(&(t->myMutex)))
		puts("\nError unlocking mutex");
	//puts("\nDone Unlocking!");
	pthread_barrier_wait(&t->myBarrier);
	t->state = -1;
	//puts("\nPast Wait </step>");
}
void kill_lsthread(lsthread* t){
	if(!t->isThreadLive)return;
	//puts("\nTime for the thread to die...");
	if(t->state != 1){
		lock(t);
		//puts("\nPast lock!");
	}
	t->shouldKillThread = 1;
	
	step(t);
	//puts("\nPast step!");
	pthread_join(t->myThread,NULL);
	//if(pthread_kill(t->myThread)){
	//	puts("\nError killing thread.");
	//}
	t->isThreadLive = 0;
	t->shouldKillThread = 0;
}
void* lsthread_func(void* me_void){
	lsthread* me = (lsthread*) me_void;
	int ret = 0;
	if (!me)pthread_exit(NULL);
	if(!me->execute)pthread_exit(NULL);
	while (1) {
		//ret = pthread_cond_wait(&(me->myCond), &(me->myMutex));
		pthread_barrier_wait(&me->myBarrier);
		//puts("\nTHREAD ACTIVATING...");
		pthread_mutex_lock(&me->myMutex);
		//puts("\nTHREAD ACTIVATED");
		//if(ret)pthread_exit(NULL);
		if (!(me->shouldKillThread) && me->execute)
			me->execute();
		else if(me->shouldKillThread){
			pthread_mutex_unlock(&me->myMutex);
			//puts("\nTHREAD DYING...");
			//pthread_barrier_wait(&me->myBarrier);
			//puts("\nTHREAD DED!");
			pthread_exit(NULL);
		}
		//puts("\nTHREAD DEACTIVATING...");
		pthread_mutex_unlock(&me->myMutex);
		//puts("\nTHREAD DEACTIVATED");
		pthread_barrier_wait(&me->myBarrier);
		//puts("\nTIME FOR A NEW CYCLE...");
	}
	pthread_exit(NULL);
}
void start_lsthread(lsthread* t){
	if(t->isThreadLive)return;
	t->isThreadLive = 1;
	t->shouldKillThread = 0;
	if(pthread_mutex_lock(&t->myMutex))
		puts("\nError locking mutex.");
	t->state = 1; //LOCKED
	pthread_create(
		&t->myThread,
		NULL,
		lsthread_func,
		(void*)t
	);
}
#endif
//end of implementation

#endif
//end of header
