#include <stdio.h>
#include <stdlib.h>  
#include <unistd.h>  
#include <string.h>

#define MAX_PATH 1024   // Input file path
#define PERMS_LEN 10	// # of file permissions

#define errExit(msg) do{ perror(msg); exit(EXIT_FAILURE); } while(0)

// Globals
char _F[MAX_PATH],	// File name regex
     _P[PERMS_LEN],	// File permissions
     _T = 'x';		// File type


int _B = -1,		// File size in bytes
    _L = -1;		// File links(#)

int opt_B = 0, opt_T = 0, opt_P = 0, opt_L = 0, opt_F = 0;


// Function Prototypes

// Input validation and informational prints
void print_usage(void);
void print_inputs(void);
int input_exists();

// Helper & Misc. functions
int find_str(char* haystack, char* needle);

int main(int argc, char* argv[])  {
	int option;

    snprintf(_F,MAX_PATH,"%s","---");
    snprintf(_P,PERMS_LEN,"%s","---");
    
    while((option = getopt(argc, argv, "f:b:t:p:l:")) != -1){ //get option from the getopt() method
        switch(option){
            case 'b':
            	opt_B = 1;
                _B = atoi(optarg);
                break;
            case 't':
            	opt_T = 1;
                _T = optarg[0];
                break;
            case 'p':
            	opt_P = 1;
                snprintf(_P,PERMS_LEN,"%s",optarg);
                break;
            case 'l':
            	opt_L = 1;
                _L = atoi(optarg);
                break;
            case 'f':
            	opt_F = 1;
                snprintf(_F,MAX_PATH,"%s",optarg);
                break;
            default:
                print_usage(); exit(EXIT_FAILURE);
        }
    }
      
    for(; optind < argc; optind++) //when some extra arguments are passed
        fprintf(stderr,"Given extra arguments: %s\n", argv[optind]);
    
    if(!input_exists()){
    	printf("No valid inputs given. Please read the usage information...\n");
    	print_usage();
    	exit(EXIT_FAILURE);
    }

    print_inputs();

    
    find_str("char* haystack", _F);
    
    return 0;
}

void print_usage(void){
    printf("========================================\n");
    printf("Usage:\n"
           "./myFind [-f filename] [-b file size(bytes)] [-t file type] [-p permissions] [-l # of _L]\n\n");
    printf("-f : supporting the following regular expression(s): +\n"
           "-t :     d: directory\n\t s: socket\n\t b: block device\n\t c: character device\n\t f: regular file\n\t p: pipe\n\t l: symbolic link\n"
           "-p : 9 characters (e.g. ‘rwxr-xr--’)\n");
  
    printf("========================================\n");
}

void print_inputs(void){
	printf("Inputs:\n");
	if(opt_F)
		printf("File name(f):   %s\n", _F);
	if(opt_B)
		printf("File size(b):   %d\n", _B);
	if(opt_T)
		printf("File type(t):   %c\n", _T);
	if(opt_P)
		printf("Permissions(p): %s\n", _P);
	if(opt_L)
		printf("links(l):       %d\n", _L);

	return;
}

int input_exists(){
	return opt_F || opt_B || opt_T || opt_P || opt_L;
}

int find_str(char* haystack, char* needles){
	char *p = strtok(needles, "+");
	while(p != NULL) {
	    printf("%s\n", p);
	    p = strtok(NULL, "+");
	}
	return 0;
}


































