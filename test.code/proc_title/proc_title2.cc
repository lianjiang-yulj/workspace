#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include "set_proctitle.h"

std::string timestr() {
  struct timeval t;
  gettimeofday(&t, NULL);
  //time_t tNow =time(NULL);
  struct tm ptm = { 0 };  
  ::localtime_r(&t.tv_sec, &ptm); 
  char szTmp[50] = {0};  
  strftime(szTmp,50,"%Y-%m-%d %H:%M:%S",&ptm);
  return std::string(szTmp);
}

#define STDOUT_LOG(fmt, args...) \
{ \
  fprintf(stdout, "[%s][INFO] [%s:%d:%s()] [STD] (%d/%ld) : "fmt".\n", timestr().data(), __FILE__, __LINE__, __FUNCTION__, ::getpid(), (int64_t)syscall(SYS_gettid), ##args); \
  ::fflush(stdout); \
}

#define STDERR_LOG(fmt, args...) \
{ \
  fprintf(stderr, "[%s][ERROR] [%s:%d:%s()] [STD] (%d/%ld) : "fmt".\n", timestr().data(), __FILE__, __LINE__, __FUNCTION__, ::getpid(), (int64_t)syscall(SYS_gettid), ##args); \
  ::fflush(stderr); \
}


extern char **environ;
char **save_argv;
char* last = basic_proc::g_proc_last;
pid_t g_pid = 0;
sigset_t g_sig_set;

bool stop_ = false;

static int32_t g_argc = 0;
char** g_argv = NULL;
void SaveArgv(int argc, char** argv) {
  save_argv = new char* [argc];
  for (int i = 0; i < argc; ++i) {
    save_argv[i] = strdup(argv[i]);
    printf("save argv:%s\n", save_argv[i]);
  }

}

int InitSigSet() {

  sigset_t oldmask;
  /*
  sigemptyset(&g_sig_set);
  sigaddset(&g_sig_set, SIGINT);
  sigaddset(&g_sig_set, SIGTERM);
//  sigaddset(&g_sig_set, SIGILL);
//  sigaddset(&g_sig_set, SIGQUIT);
//  sigaddset(&g_sig_set, SIGABRT);
 // sigaddset(&g_sig_set, SIGSEGV);
  sigaddset(&g_sig_set, SIGCHLD);
  //sigaddset(&g_sig_set, SIGCONT);
  sigaddset(&g_sig_set, SIGUSR1);
  sigaddset(&g_sig_set, SIGUSR2);
  sigaddset(&g_sig_set, SIGPIPE);
  */
  sigfillset(&g_sig_set);
  if (0 != pthread_sigmask(SIG_BLOCK, &g_sig_set, &oldmask)) {
    fprintf(stderr, "master process(%d) set sigset fail.\n", ::getpid());
    return 1;
  }

  return 0;
}

int WaitWorkerSig() {
  int err, signo;
  for(;;)
  {
    err = sigwait(&g_sig_set, &signo);
    if (err != 0)
      continue;
    switch (signo)
    {
      case SIGUSR1:
      case SIGUSR2:
      case SIGINT:
      case SIGTERM:
        {
          printf("worker %d wait sig: %d\n", ::getpid(), signo);
          stop_ = true;
          return 0;
        }
      default:
        printf("worker %d wait other sig: %d\n", ::getpid(), signo);
        continue;
    }
  }
  return 1;
}


int WaitMasterSig() {
  int err, signo;
  for(;;)
  {
    err = sigwait(&g_sig_set, &signo);
    if (err != 0)
      continue;
    switch (signo)
    {
      case SIGUSR1:
      case SIGUSR2:
      case SIGINT:
      case SIGTERM:
        {
          printf("master %d wait sig: %d, errno=%d\n", ::getpid(), signo, errno);

          if(kill(g_pid, 0) == 0) {
            kill(g_pid, SIGUSR1);
          }
          return 0;
        }
      case SIGCHLD:
        {
          printf("master %d wait other sig: %d, errno=%d, child exits.\n", ::getpid(), signo, errno);
          return 0;
        }
      default:
        printf("master %d wait other sig: %d, errno=%d\n", ::getpid(), signo, errno);
        continue;
    }
  }
  return 1;
}

void handlerm(int sig) {
  if (sig != SIGUSR2 && sig != SIGUSR1 && sig != SIGINT && sig != SIGTERM) {
    printf("master recv err sig = %d, pid = %d\n", sig, ::getpid());
    return ;
  }
//  stop_ = true;
  printf("master recv sig = %d\n", sig);
  if(kill(g_pid, 0) == 0) {
    kill(g_pid, SIGUSR1);
  }
}

void handlerc(int sig) {

  if (sig != SIGUSR2 && sig != SIGUSR1 && sig != SIGINT && sig != SIGTERM) {
    printf("child recv err sig = %d\n", sig);
    return ;
  }
  stop_ = true;
  printf("child recv sig = %d\n", sig);
}

void initProcTitle(int argc, char **argv)
{
  size_t size = 0;
  for (int i = 0; environ[i]; ++i) {
    size += strlen(environ[i])+1; 
  }   

  char *raw = new char[size];
  for (int i = 0; environ[i]; ++i) {
    memcpy(raw, environ[i], strlen(environ[i]) + 1); 
    environ[i] = raw;
    raw += strlen(environ[i]) + 1;
  }   

  last = argv[0];
  for (int i = 0; i < argc; ++i) {
    last += strlen(argv[i]) + 1;   
  }   
  for (int i = 0; environ[i]; ++i) {
    last += strlen(environ[i]) + 1;
  }   
}

void setProcTitle(int argc, char **argv, const char *title)
{
  argv[1] = 0;
  char *p = argv[0];
  memset(p, 0x00, last - p); 
  strncpy(p, title, last - p); 
}

int main(int argc, char *argv[])
{
  g_argc = argc;
  g_argv = argv;
  SaveArgv(argc, argv);
  daemon(1,1);

  InitSigSet();

  /*
  signal(SIGPIPE, SIG_IGN);
  signal(SIGHUP, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);
  signal(SIGINT, handlerm);
  signal(SIGTERM, handlerm);
  signal(SIGUSR1, handlerm);
  signal(SIGUSR2, handlerm);
  */
  basic_proc::InitSetProcTitle(g_argc, g_argv);
  // initProcTitle(argc, argv);
  pid_t pid = fork();
  if (pid > 0) {
    g_pid = pid;
    // setProcTitle(argc, argv, "master so much argument can you believe              sdfsdfd sdfsfsdfsdfsd");
    std::string master_name = "master process: ";
    for (int i = 0; i < argc; ++i) master_name.append(" ").append(argv[i]);
    basic_proc::SetProcTitle(g_argv, master_name);
    //wait(&status);
    printf("option: %d\n", WNOHANG);
    //pid_t child_pid = wait(&status);
    WaitMasterSig();
    int status = 0;
    pid_t child_pid = waitpid(0, &status,0);
    printf("master status = %d, pid = %d\n", status, getpid());
    printf("master status = %d\n", WIFEXITED(status));
    printf("master status = %d\n", WIFSIGNALED(status));
    if (WIFSIGNALED(status)) {
      printf("kill: %d\n", WTERMSIG(status));
    }

    printf("master status = %d\n", WIFSTOPPED(status));
    printf("size of pid_t(%d)\n", sizeof(pid_t));

  } else {

      
    //setProcTitle(argc, argv, "worker so much argument can you believe");
    basic_proc::SetProcTitle(g_argv, "worker so much argument can you believe");
    WaitWorkerSig();
    /*
    signal(SIGINT, handlerc);
    signal(SIGTERM, handlerc);
    signal(SIGUSR1, handlerc);
    signal(SIGUSR2, handlerc);
    */
    int a = 0;
    while (++a < 5) {
      //sleep(1);
    }
    char* p = NULL;
    //*p = 'h';
    while(!stop_) sleep(1);  // you can ps now
    printf("child exist.\n");
    exit(0);
  }

  STDOUT_LOG("test stdout log");

  printf("%s\n", timestr().data());
  printf("%d\n", getppid());
  printf("%d\n", sizeof(pthread_t));
  return 0;
}
