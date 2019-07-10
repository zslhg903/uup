#include<stdio.h>
#include<fcntl.h>
#include<sys/types.h>
#include<utmp.h>

#define NRECS 16
#define UTSIZE (sizeof(struct utmp))
#define NULLUT ((struct utmp *)NULL)

static char utmpbuf[NRECS*UTSIZE];
static int num_recs;
static int cur_rec;
static int fd_utmp=-1;

int utmp_open(char * filename)
{
    fd_utmp=open(filename,O_RDONLY);
    num_recs=cur_rec=0;
    return fd_utmp;
}

utmp_close()
{
    if(fd_utmp!=-1)
	close(fd_utmp);
}

int utmp_reload()
{
   int amt_read;//actually read bytes;
   amt_read=read(fd_utmp,utmpbuf,NRECS*UTSIZE);
   num_recs=amt_read/UTSIZE;
   cur_rec=0;
   return num_recs;
}
struct utmp *utmp_next()
{
    struct utmp * result;
    if(fd_utmp==-1)
	return NULLUT;
    if(cur_rec==num_recs&&utmp_reload()==0)
	return NULLUT;
    result=(struct utmp *)&utmpbuf[cur_rec++*UTSIZE];
    return result;
}
