
#include <stdio.h>      //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define QUEUEKEY 777
#define ENDTYPE 19L
#define NODATA -1


///==============================
//don't mess with this variable//
int qid;
//===============================

struct processData {
    int ArrivalTime;
    int Priority;
    int RunningTime;
    int ID;
};

struct messagebuffer
{
 long mtype;
 struct processData data;
 
};

bool queueIsEmpty()
{
    struct messagebuffer msg;
    msg.mtype = 0L;
    int ret=msgrcv(qid,&msg,sizeof(msg)-sizeof(long),0,IPC_NOWAIT|MSG_COPY);
    if(ret==NODATA)
        return true;
    return false;

}
void initQueue(bool flag) {
  if(flag) {
    qid= msgget(QUEUEKEY,IPC_CREAT  |   0644);
    if(qid == -1)
    {
        perror("queue not created");
    }
  }

  else
  {
     qid = msgget(QUEUEKEY, 0444);
    if(qid == -1)
      {
            //Make sure that the Clock exists
          printf("Error, queue not initialized yet\n");
          kill(getpgrp(),SIGINT);
      }
  }
  
}



int Sendmsg(struct processData pData) {
  struct messagebuffer msg;
  msg.data = pData;
  msg.mtype = 1L;
  return msgsnd(qid, &msg, sizeof(msg)-sizeof(long), !IPC_NOWAIT);
}

int Recmsg( processData &pData) {
  struct messagebuffer msg;
  msg.mtype = 1L;
  int ret=msgrcv(qid,&msg,sizeof(msg)-sizeof(long),0,IPC_NOWAIT);
  pData=msg.data;
  if(ret == NODATA)
    return 0;
  if(msg.mtype == ENDTYPE)
      return -1;
  return ret;   //Return number of bytes read
  
}

void lastSend() {
  struct messagebuffer msg;
  msg.mtype = ENDTYPE;
  msgsnd(qid, &msg, sizeof(msg)-sizeof(long), !IPC_NOWAIT);
}

void destroyQueueAndExit(int)
{
    msgctl(qid, IPC_RMID, (struct msqid_ds*)0); 
    exit(0);
}


