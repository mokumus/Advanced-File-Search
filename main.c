#include <stdio.h>
#include <stdlib.h>  
#include <unistd.h>  
#include <sys/types.h>
#include <dirent.h>
#include <string.h>


#define MAX_PATH 255   // Input file path
#define PERMS_LEN 10	// # of file permissions

#define errExit(msg) do{ perror(msg); exit(EXIT_FAILURE); } while(0)


// Function Prototypes

// Input validation and informational prints
void print_usage(void);
void print_inputs(void);
int input_exists();

// Main functionality functions
void traverse_dir(char *name, char* target, int indent);

// Helper & Misc. functions
void to_lower_case(char* str);
int str_cmp(char* str1, char* str2);



// Globals
char _W[MAX_PATH],
	 _F[MAX_PATH],	// File name regex
     _P[PERMS_LEN],	// File permissions
     _T = 'x';		// File type


int _B = -1,		// File size in bytes
    _L = -1;		// File links(#)

int opt_B = 0, opt_T = 0, opt_P = 0, opt_L = 0, opt_F = 0, opt_W;

int main(int argc, char* argv[])  {
	int option;
    
    while((option = getopt(argc, argv, "f:b:t:p:l:w:")) != -1){ //get option from the getopt() method
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
            case 'w':
            	opt_W = 1;
                snprintf(_W,MAX_PATH,"%s",optarg);
                break;            
            default:
                print_usage(); exit(EXIT_FAILURE);
        }
    }
      
    if(opt_W == 0){
    	printf("No search directory provided\n");
    	print_usage();
    	exit(EXIT_FAILURE);

    }

    for(; optind < argc; optind++) //when some extra arguments are passed
        fprintf(stderr,"Given extra arguments: %s\n", argv[optind]);
    
    if(!input_exists()){
    	printf("No valid inputs given. Please read the usage information...\n");
    	print_usage();
    	exit(EXIT_FAILURE);
    }

    print_inputs();
    char str1[] = "mrAz";
    char str2[] = "Mraz";
    printf("result: %d\n", str_cmp(str1, str2));

    traverse_dir(_W,"file+lost", 0);

    return 0;
}

void print_usage(void){
	char buffer[1024] = "========================================\n"
						"Usage:\n"
						"./myFind [-f filename] [-b file size(bytes)] [-t file type] [-p permissions] [-l # of _L] [-w search directory(required)]\n\n"
						"-f : supporting the following regular expression(s): +\n"
						"-t :     d: directory\n\t s: socket\n\t b: block device\n\t c: character device\n\t f: regular file\n\t p: pipe\n\t l: symbolic link\n"
						"-p : 9 characters (e.g. ‘rwxr-xr--’)\n"
						"========================================\n";
	write(STDOUT_FILENO, buffer, 1024 );
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
}

int input_exists(){
	return opt_F || opt_B || opt_T || opt_P || opt_L;
}

int str_cmp(char* str1, char* str2){
	to_lower_case(str1);
	to_lower_case(str2);

	while(*str1){
		if(*str1 != *str2)
			break;
		str1++;
		str2++;
	}
	// ASCII difference
    return *str1 - *str2;
}

void to_lower_case(char* str){
	while(*str != '\0'){
		if(*str >= 65 && *str<= 90)
            *str = *str + 32;
        str++;
	}
}

void traverse_dir(char *name, char* target, int indent)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;

    if(indent == 0){
    	printf("target name: %s\n", target);
    	printf("target folder: %s\n",name);
    }


    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024];

            // Ignore special name-inode maps(Hard-links)
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            printf("|");
            for(int i = 0; i < indent+2; i++){
        		printf("-");
        	}
            printf("%s\n",entry->d_name);
            traverse_dir(path, target, indent + 2);
        } else {
        	printf("|");
        	for(int i = 0; i < indent+2; i++){
        		printf("-");
        	}
            printf("%s\n", entry->d_name);
        }
    }
    closedir(dir);
}




























