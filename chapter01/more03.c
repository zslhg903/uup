/*  more02.c  - version 0.2 of more
 *   *	read and print 24 lines then pause for a few special commands
 *    *      feature of version 0.2: reads from /dev/tty for commands
 *     */
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#include<termios.h>

int	PAGELEN;
int	LINELEN;
int 	FILELEN;
int     FILEBYTE;
void do_more(FILE *);
int see_more(FILE *,int readedBytes);

int main( int ac , char *av[] )
{
	FILE	*fp;

	if ( ac == 1 )
		do_more( stdin );
	else
		while ( --ac )
		{
			if ( (fp = fopen( *++av , "r" )) != NULL )
			{
				FILELEN=lseek(fileno(fp),0L,SEEK_END);
				lseek(fileno(fp),0L,SEEK_SET);	
				do_more( fp ) ; 
				fclose( fp );
			}
			else
				exit(1);
		}
	return 0;
}

void do_more( FILE *fp )
/*
 *  *  read PAGELEN lines, then call see_more() for further instructions
 *   */
{

	FILE	*fp_tty;
	fp_tty = fopen( "/dev/tty", "r" );	   /* NEW: cmd stream   */
	if ( fp_tty == NULL )			   /* if open fails     */
		exit(1);                           /* no use in running */
	struct winsize size;
	ioctl(fileno(fp_tty),TIOCGWINSZ,&size);
	PAGELEN=size.ws_row;
	LINELEN=size.ws_col;
	char	line[LINELEN];
	int	num_of_lines = 0;
	int	see_more(FILE *,int readedBytes), reply;

	int readedBytes=0;

	while ( fgets( line, LINELEN, fp ) ){		/* more input	*/
		readedBytes+=strlen(line);
		if ( num_of_lines == PAGELEN ) {	/* full screen?	*/
			reply = see_more(fp_tty,readedBytes);  /* NEW: pass FILE *  */
			if ( reply == 0 )		/*    n: done   */
				break;
			num_of_lines -= reply;		/* reset count	*/
		}
		if ( fputs( line, stdout )  == EOF )	/* show line	*/
		{
			printf("\n");
			exit(1);	
		}/* or die	*/
		num_of_lines++;				/* count it	*/
	}
}

int see_more(FILE *cmd,int readedBytes)				   /* NEW: accepts arg  */
/*
 *  *	print message, wait for response, return # of lines to advance
 *   *	q means no, space means yes, CR means one line
 *    */
{
	int	c;
	if(FILELEN==0)
		return 0;
	int percent=readedBytes*100/FILELEN; 
	printf("\033[7m more %d% \033[m",percent);		/* reverse on a vt100	*/
	struct termios oldt,newt;
	tcgetattr(fileno(cmd),&oldt);
	newt=oldt;
	newt.c_lflag&=~(ECHO|ICANON);
	tcsetattr(fileno(cmd),TCSANOW,&newt);
	while( (c=getc(cmd)) != EOF )		/* NEW: reads from tty  */
	{
		int result=0;
		if ( c == 'q' )			/* q -> N		*/
			result= 0;
		if ( c == ' ' )			/* ' ' => next page	*/
			result= PAGELEN;		/* how many to show	*/
		if ( c == '\n' )		/* Enter key => 1 line	*/
			result= 1;
		tcsetattr(fileno(cmd),TCSANOW,&oldt);		
		return result;
		
	}
	return 0;
}


