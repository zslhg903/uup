#include<stdio.h>
#include<fcntl.h>
#include<sys/types.h>
#include<utmp.h>


#define ROWS 16
#define NULLUT ((struct utmp *)NULL)
#define UTSIZE (sizeof(struct utmp))
#define BUFFERSIZE (ROWS*UTSIZE)
static char utmpbuf[BUFFERSIZE];
static int cur_row;
static int buf_rows;
static int fd_utmp=-1;
int utmp_open(char * filename)
{
    fd_utmp=open(filename,O_RDONLY);
    cur_row=buf_rows=0;
    return fd_utmp;
}
utmp_close()
{
    if(fd_utmp!=-1)
	close(fd_utmp);
}
int utmp_reload()
{
    int read_bytes=0;
    read_bytes=read(fd_utmp,utmpbuf,BUFFERSIZE);
    buf_rows=read_bytes/UTSIZE;
    cur_row=0;
    return buf_rows;
}
struct utmp * utmp_next()
{
    struct utmp* result;
    if(fd_utmp==-1)
	return NULLUT;
    if(cur_row==buf_rows&&utmp_reload()==0)
	return NULLUT;
    result=(struct utmp *)&utmpbuf[cur_row*UTSIZE];
    cur_row++;
    return result;
}
