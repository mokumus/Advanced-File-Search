#include <stdio.h>
#include <stdlib.h>  
#include <unistd.h>  
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>


#define MAX_PATH 256   // Input file path
#define PERMS_LEN 10	// # of file permissions

#define errExit(msg) do{ perror(msg); exit(EXIT_FAILURE); } while(0)


// Function Prototypes

// Input validation and informational prints
void print_usage(void);
void print_inputs(void);
int input_exists();

// Main functionality functions
void traverse_dir(char *name, int indent);
int regex_plus(const char* regex, const char* input);
int check_file(struct dirent * entry);

// Helper & Misc. functions
void to_lower_case(char* str);
int str_cmp(char* str1, char* str2);
int count_chars(const char* input, const char target);


// Globals
char _W[MAX_PATH],
	 _F[MAX_PATH],		// File name regex
     _P[PERMS_LEN], 	// File permissions
     curr_P[PERMS_LEN],	// Current files permissions
     _T = 'x';			// File type


int _B = -1,		// File size in bytes
    _L = -1;		// File links(#)

int opt_B = 0, opt_T = 0, opt_P = 0, opt_L = 0, opt_F = 0, opt_W;

int dfd = -1;
int matches = 0;

int main(int argc, char* argv[])  {
	// Local variables
	int option;


	// Input parsing & validation =======================
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
    // Input parsing & validation  END =======================

    //print_inputs();


    
	traverse_dir(_W, 0);
	if(matches == 1)
		printf("File found!\n");
	else
		printf("No file found\n");
	

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

void traverse_dir(char *name, int indent) {
	DIR *dir;
	struct dirent *entry;

	if (!(dir = opendir(name)))
		return;

	

	if (indent == 0)
		printf("%s\n", name);
	


	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type == DT_DIR) {
			char path[1024];

			// Ignore special name-inode maps(Hard-links)
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;
			snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
			printf("|");
			for (int i = 0; i < indent + 2; i++)
				printf("-");

			//printf("%s\n", entry->d_name);
			dfd = dirfd(dir);
			if (check_file(entry) == 1){
				matches = 1;
				return;
			}


			traverse_dir(path, indent + 2);
		} 

		else {
			printf("|");
			for (int i = 0; i < indent + 2; i++)
				printf("-");

			//printf("%s\n", entry->d_name);
			dfd = dirfd(dir);
			if (check_file(entry) == 1){
				matches = 1;
				return;
			}
		}
	}
	
	closedir(dir);
}

int count_chars(const char* input, const char target){
	int i = 0, sum = 0;

	while(input[i] != '\0'){
		if(input[i] == target)
			sum++;
		i++;
	}
	return sum;
}


/*
	@regex: file+name+.txt
	@input: FileName+.txt
	@return: 0 on success
*/
int regex_plus( const char* regex, const char* input){
	int n = 0;
	int n_token = count_chars(regex, '+') + 1;
	int current_token = 0;
   	char *token = strdup(regex);

   	//printf("input: %s\n", input);


   	token = strtok(token, "+");
   	for(int i = 0; i < n_token-1; i++){
   		int len = strlen(token);
   		//printf("%s %d : %s\n", token, len, &input[n]);
   		
   		if(strncmp(token, &input[n], len) == 0){
   			n += len;
   			
   			while(input[n] == token[len-1]){
   				//printf("input[n+1]: %c\n", input[n]);
   				n++;
   			}
   		}

   		token = strtok(NULL, "+");
   	}

   	//printf( "%s\n", token);
   	if(strcmp(token, &input[n]) == 0)
   		return 0;


	return -1;
}

int check_file(struct dirent * entry){
	struct stat sb;
	int current_sum = 0;
	int target_sum = opt_L + opt_P + opt_T + opt_F + opt_B;

	printf("%s ", entry->d_name);

	if (dfd != -1 && fstatat(dfd, entry->d_name, &sb, 0) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    dfd = -1;

	if(opt_T == 1){
		switch(entry->d_type){
			case DT_DIR:
				printf("{Directory} ");
				if(_T == 'd') current_sum++;
				break;
			case DT_REG:
				printf("{Regular file} ");
				if(_T == 'f') current_sum++;
				break;
			case DT_FIFO:
				printf("{Pipe/FIFO} ");
				if(_T == 'p') current_sum++;
				break;
			case DT_SOCK:
				printf("{Socket} ");
				if(_T == 's') current_sum++;
				break;
			case DT_CHR:
				printf("{Character device} ");
				if(_T == 'c') current_sum++;
				break;			
			case DT_BLK:
				printf("{Block device} ");
				if(_T == 'b') current_sum++;
				break;	
			case DT_LNK:
				printf("{Symbolic link} ");
				if(_T == 'l') current_sum++;
				break;	
			default:
				printf("{Unkown type} ");
				break;	
		}
	}

	if(opt_B == 1) {
		printf("{%lld bytes}",(long long) sb.st_size);
		if(_B == sb.st_size) current_sum++;
	}

	if(opt_P == 1) {
		mode_t perm = sb.st_mode;
        curr_P[0] = (perm & S_IRUSR) ? 'r' : '-';
        curr_P[1] = (perm & S_IWUSR) ? 'w' : '-';
        curr_P[2] = (perm & S_IXUSR) ? 'x' : '-';
        curr_P[3] = (perm & S_IRGRP) ? 'r' : '-';
        curr_P[4] = (perm & S_IWGRP) ? 'w' : '-';
        curr_P[5] = (perm & S_IXGRP) ? 'x' : '-';
        curr_P[6] = (perm & S_IROTH) ? 'r' : '-';
        curr_P[7] = (perm & S_IWOTH) ? 'w' : '-';
        curr_P[8] = (perm & S_IXOTH) ? 'x' : '-';
        curr_P[9] = '\0';

        printf("{%s} ", curr_P);
        if(str_cmp(curr_P, _P) == 0) current_sum++;
	}

	if(opt_L == 1) {
 		printf("{%ld} ", sb.st_nlink);
        
	}

	if(opt_F == 1) {
		printf("{regex: %s} ", _F);
	}

	printf("\n");
	return current_sum == target_sum;
}
