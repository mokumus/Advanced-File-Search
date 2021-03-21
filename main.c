#include <stdio.h>
#include <stdlib.h>  
#include <unistd.h>  


#define errExit(msg) do{ perror(msg); exit(EXIT_FAILURE); } while(0)

// Function Prototypes
void print_usage(void);


int main(int argc, char* argv[])  {

    char filename_regex[255],
         perms[10],
         fie_type = 'x';
    
    int option,
        size_bytes = -1,
        links = -1;
    
    while((option = getopt(argc, argv, "f:b:t:p:l:")) != -1){ //get option from the getopt() method
        switch(option){
            case 'b':
                size_bytes = atoi(optarg);
                break;
            case 't':
                fie_type = optarg[0];
                break;
            case 'p':
                snprintf(perms,10,"%s",optarg);
                break;
            case 'l':
                links = atoi(optarg);
                break;
            case 'f':
                snprintf(filename_regex,255,"%s",optarg);
                break;
            default:
                print_usage(); exit(EXIT_FAILURE);
        }
    }
      
    for(; optind < argc; optind++) //when some extra arguments are passed
        fprintf(stderr,"Given extra arguments: %s\n", argv[optind]);
    
    
    printf("Inputs:\n"
           "File name(f):   %s\n"
           "File size(b):   %d %s\n"
           "File type(t):   %c\n"
           "Permissions(p): %s\n"
           "links(l):       %d %s\n"
           ,filename_regex, size_bytes,size_bytes == -1 ? "(Not specified)" : "", fie_type, perms,links, links == -1 ? "(Not specified)" : "");
    
    return 0;
}

void print_usage(void){
    printf("========================================\n");
    printf("Usage:\n"
           "./myFind [-f filename] [-b file size(bytes)] [-t file type] [-p permissions] [-l # of links]\n\n");
    printf("-f : supporting the following regular expression(s): +\n"
           "-t :     d: directory\n\t s: socket\n\t b: block device\n\t c: character device\n\t f: regular file\n\t p: pipe\n\t l: symbolic link\n"
           "-p : 9 characters (e.g. ‘rwxr-xr--’)\n");
  
    printf("========================================\n");
}

































