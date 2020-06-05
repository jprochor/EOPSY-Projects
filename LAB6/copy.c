#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main (int argc, char **argv)
{
  int index;
  int map=0;  //variable to indicate if read/write or mmap should be used
  int a;
  int test;
  int ifhelp=0;
  char data[128]; //buffer 
  int dscin;  //input file descriptor
  int dscout; //output file descriptor
  int fileread;  //stores output of read function (size of what was read)
  size_t filesize;
   void *source, *destination;  //pointers for usage of mmap to memory

  opterr = 0;

  while ((a = getopt (argc, argv, "hmc:")) != -1)    //using getopt to detect options
    switch (a)
      {
      case 'h': //help
	fprintf(stdout,"\nSyntax: copy [-m] <file_name> <new_file_name>\ncopy [-h]\nOptionless uses read() and write(), -m option indicates usage of mmap, -h option displays help\n");
	ifhelp=1;
        break;
      case 'm': //indicates usage of mmap
	map=1;
        break;
      case '?':
   
         if (isprint (optopt)) //detected not specified option
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        return 1;
      default:
        abort ();
      }


	
	if(ifhelp==1) //help option was called so can end here
	{
		return 1;
	}
		

test=optind;



if((dscin = open(argv[test], O_RDONLY)) == -1) //getting input file descriptor and displaying error in case of failure
  fprintf(stderr, ": Error: opening input file: %s\n", argv[test]), exit(1);




if((dscout = open(argv[test+1], O_CREAT | O_APPEND | O_RDWR, 0666))==-1)  //getting output file descriptor and displaying error in case of failure
  fprintf(stderr, ": Error: opening/creating output file: %s\n", argv[test]), exit(1);  



if(map==0) //no -m option so using read/write
{
	if(dscin>0)
	{ //nonempty input file
    	fileread = read(dscin, data, sizeof(data)); //read data from input file to buffer and store size read
    	write(dscout, data, fileread);  //write just read data to output file
    	close(dscin); //close input file
	}
close(dscout); //close output file

}

if(map==1) //-m option detected so using mmap
{

filesize = lseek(dscin, 0, SEEK_END); //checking filesize
ftruncate(dscout, filesize); //preemptively truncating output file to just established size
lseek(dscout, filesize - 1, SEEK_SET);
write(dscout, '\0', 1);

if((source = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, dscin, 0)) == (void *) -1) //map input file and display error in case of failure
  fprintf(stderr, "Error mapping input file: %s\n", argv[test]), exit(1);

if((destination = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, dscout, 0)) == (void *) -1) //map output file and display error in case of failure
  fprintf(stderr, "Error mapping ouput file: %s\n", argv[test+1]), exit(1);

memcpy(destination, source, filesize); //copy contents of input to output files using mmap
munmap(source, filesize); //delete the input file mapping
munmap(destination, filesize); //delete the output file mapping
close(dscin); //close input file
close(dscout); //close output file
}

  return 0;
}
