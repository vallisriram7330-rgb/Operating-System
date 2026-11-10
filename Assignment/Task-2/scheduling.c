#include <stdio.h>
#include <limits.h>

#define N 4
typedef struct { int id, at, bt, pr, ct, tat, wt, rt, rem, started; } Proc;

static void report(const char *name, Proc p[]) {
    double sw=0, st=0, sr=0;
    printf("\n%s\nPID\tAT\tBT\tCT\tTAT\tWT\tRT\n", name);
    for (int i=0;i<N;i++) {
        p[i].tat=p[i].ct-p[i].at;
        p[i].wt=p[i].tat-p[i].bt;
        sw+=p[i].wt; st+=p[i].tat; sr+=p[i].rt;
        printf("P%d\t%d\t%d\t%d\t%d\t%d\t%d\n",p[i].id,p[i].at,p[i].bt,p[i].ct,p[i].tat,p[i].wt,p[i].rt);
    }
    printf("Average TAT=%.2f, Average WT=%.2f, Average RT=%.2f\n",st/N,sw/N,sr/N);
}
static void init(Proc p[], const int at[], const int bt[]) {
    for(int i=0;i<N;i++) p[i]=(Proc){.id=i+1,.at=at[i],.bt=bt[i],.rem=bt[i],.rt=-1};
}
static void fcfs_sjf(int sjf) {
    const int at[N]={0,1,2,3}, bt[N]={8,4,9,5};
    Proc p[N]; init(p,at,bt); int done[N]={0},t=0;
    for(int k=0;k<N;k++){
        int pick=-1;
        for(int i=0;i<N;i++) if(!done[i] && p[i].at<=t &&
            (pick<0 || (sjf ? p[i].bt<p[pick].bt : p[i].at<p[pick].at))) pick=i;
        if(pick<0){t++;k--;continue;}
        p[pick].rt=t-p[pick].at; t+=p[pick].bt; p[pick].ct=t; done[pick]=1;
    }
    report(sjf?"Non-preemptive SJF":"FCFS",p);
}
static void srtf(void) {
    const int at[N]={0,1,2,3},bt[N]={8,4,9,5}; Proc p[N];init(p,at,bt);
    int finished=0,t=0;
    while(finished<N){
        int pick=-1;
        for(int i=0;i<N;i++) if(p[i].rem>0 && p[i].at<=t && (pick<0 || p[i].rem<p[pick].rem)) pick=i;
        if(pick<0){t++;continue;}
        if(p[pick].rt<0)p[pick].rt=t-p[pick].at;
        p[pick].rem--;t++;
        if(p[pick].rem==0){p[pick].ct=t;finished++;}
    }
    report("SRTF",p);
}
static void priority_np(void) {
    const int at[N]={0,1,2,3},bt[N]={8,4,9,5}; Proc p[N];init(p,at,bt);
    printf("\nPriority convention: smaller number = higher priority.\n");
    for(int i=0;i<N;i++){printf("Enter priority for P%d: ",i+1);if(scanf("%d",&p[i].pr)!=1)return;}
    int done[N]={0},t=0;
    for(int k=0;k<N;k++){
        int pick=-1;
        for(int i=0;i<N;i++)if(!done[i]&&p[i].at<=t&&(pick<0||p[i].pr<p[pick].pr))pick=i;
        if(pick<0){t++;k--;continue;}
        p[pick].rt=t-p[pick].at;t+=p[pick].bt;p[pick].ct=t;done[pick]=1;
    }
    report("Non-preemptive Priority",p);
}
static void rr(void) {
    const int at[N]={0,1,2,3},bt[N]={8,4,9,5}; Proc p[N];init(p,at,bt);
    int q;printf("Enter Round Robin quantum: ");if(scanf("%d",&q)!=1||q<=0){puts("Quantum must be positive.");return;}
    int queue[1000],head=0,tail=0,t=0,next=0,finished=0,inqueue[N]={0};
    while(finished<N){
        for(int i=0;i<N;i++)if(!inqueue[i]&&p[i].rem>0&&p[i].at<=t){queue[tail++]=i;inqueue[i]=1;}
        if(head==tail){t++;continue;}
        int x=queue[head++]; if(p[x].rt<0)p[x].rt=t-p[x].at;
        int run=p[x].rem<q?p[x].rem:q;
        printf("P%d(%d-%d) ",p[x].id,t,t+run);
        for(int s=0;s<run;s++){
            t++;p[x].rem--;
            for(int i=0;i<N;i++)if(!inqueue[i]&&p[i].rem>0&&p[i].at<=t){queue[tail++]=i;inqueue[i]=1;}
        }
        if(p[x].rem==0){p[x].ct=t;finished++;}
        else {queue[tail++]=x;}
        (void)next;
    }
    puts("");report("Round Robin",p);
}
int main(void){
    puts("OPERATING SYSTEMS - ASSIGNMENT 2");
    puts("Student Name: V. Sriram | Register No: 192524037 | AI & DS");
    puts("Processes: P1-P4; AT = 0,1,2,3; BT = 8,4,9,5");
    fcfs_sjf(0);fcfs_sjf(1);srtf();priority_np();rr();
    return 0;
}
