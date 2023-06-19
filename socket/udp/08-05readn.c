/* Read ¡°n¡± bytes from a descriptor.
use in place of read() when fd is a stream socket. */

int readn(fd,ptr,nbytes)
register int fd;
register char *ptr;
register int nbytes;
{
  int 	nleft,nread;
  
  nleft=nbytes;
  while(nleft>0)
{
  nread=read(fd,ptr,nleft);
  if ( nread < 0)
     return(nread);		/* error,return < 0 */
    else if (nread==0)
           break;		/* EOF */
  nleft - = nread;
      ptr  + = nread;
}
  return(nbytes-nleft);		/* return >= 0 */
}
   The following function writes to a stream socket.

/* Write ¡°n¡± bytes to a descriptor.
use in place of write() when fd is a stream socket. */

int  writen(fd,ptr,nbytes)
register int fd;
register char *ptr;
register int nbytes;
{
  int 	nleft,nwritten;
  
  nleft=nbytes;
  while(nleft>0)
{
  nwritten=write(fd,ptr,nleft);
  if ( nwritten <= 0)
     return(nwritten);		/* error,return <= 0 */
    
  nleft - = nwritten;
      ptr  + = nwritten;
}
  return(nbytes-nleft);		/* return >= 0 */
}