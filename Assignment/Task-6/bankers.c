#include <stdio.h>
#include <stdbool.h>

#define P 5
#define R 3
static int alloc[P][R]={{0,1,0},{2,0,0},{3,0,2},{2,1,1},{0,0,2}};
static int maximum[P][R]={{7,5,3},{3,2,2},{9,0,2},{2,2,2},{4,3,3}};
static int available[R]={3,3,2};

static bool safety(int seq[P]){
    int work[R];bool finish[P]={false};
    for(int r=0;r<R;r++)work[r]=available[r];
    int count=0;
    while(count<P){
        bool found=false;
        for(int p=0;p<P;p++)if(!finish[p]){
            int r;for(r=0;r<R;r++)if(maximum[p][r]-alloc[p][r]>work[r])break;
            if(r==R){for(r=0;r<R;r++)work[r]+=alloc[p][r];finish[p]=true;seq[count++]=p;found=true;}
        }
        if(!found)return false;
    }
    return true;
}
static void printseq(int seq[P]){
    for(int i=0;i<P;i++)printf("P%d%s",seq[i],i==P-1?"\n":" -> ");
}
int main(void){
    int seq[P];
    puts("OPERATING SYSTEMS - ASSIGNMENT 2");
    puts("Student Name: V. Sriram | Register No: 192524037 | AI & DS");
    if(safety(seq)){puts("System is in a SAFE state.");printf("Safe sequence: ");printseq(seq);}
    else puts("System is UNSAFE.");
    return 0;
}
