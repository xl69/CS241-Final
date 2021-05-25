// author: xl69

#include <stdbool.h> 
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>


bool file_exists(char *filename) {
  struct stat buffer;   
  return (stat(filename, &buffer) == 0);
}

int main(int argc, char** argv) {
	if (argc == 4) {
		if (!strcmp(argv[0], "./encrypt")) {
			//TODO: implement encrypt
			//argv[1]: input file; argv[2], argv[3]: 1.bin, 2.bin
			
			//Verify input files exist before processing
			if (!file_exists(argv[1])) {
				printf("Hello World\n");
				exit(EXIT_SUCCESS);
			}
			//If 1.bin already exists, print and exit
			if (file_exists(argv[2])) {
				printf("Hello World\n");
				exit(EXIT_SUCCESS);
			}
			//If 2.bin already exists, print and exit
			if (file_exists(argv[3])) {
				printf("Hello World\n");
				exit(EXIT_SUCCESS);
			}

			//Create 1.bin 
			FILE* firstbin = fopen(argv[2], "w");
		    if (firstbin == NULL) {
		        perror("Cannot open random bytes file");
		        exit(EXIT_FAILURE);
		    }
		    //Create 2.bin
		    FILE* secondbin = fopen(argv[3], "w");
		    if (secondbin == NULL) {
		        perror("Cannot open result bytes file");
		        exit(EXIT_FAILURE);
		    }
		    //Create /dev/urandom
		    FILE* urandom = fopen("/dev/urandom", "r");
		    if (urandom == NULL) {
		        perror("Cannot open /dev/urandom");
		        exit(EXIT_FAILURE);
		    }

		    //Create mystuff.txt
		    int inputfile = open(argv[1], O_RDWR);
		    if (inputfile < 0) {
		        perror("Cannot open the secret file");
		        exit(EXIT_FAILURE);
		    }

		    struct stat buffer;
		    size_t buffersize = sizeof(buffer); 
		    memset(&buffer, 0, buffersize);
		    stat(argv[1], &buffer);
		    ssize_t inputsize = buffer.st_size;
		    char* buf = mmap(NULL, inputsize, PROT_READ | PROT_WRITE, MAP_SHARED, inputfile, 0);
		    //printf("%s\n", buf);
		    if (buf == MAP_FAILED) {
		        perror("Cannot use mmap to read the secret file");
		        exit(EXIT_FAILURE);
		    }

		    ssize_t inputindex = 0;
		    while(inputindex < inputsize) {
		    	// 1.bin is a sequence of random bytes
		    	// read from urandom
		    	char random = (char) fgetc(urandom);
		    	fputc(random, firstbin);
		    	// 2.bin: original file xor 1.bin
		    	fputc((buf[inputindex])^random, secondbin);
		    	//zero bytes the original file
		    	buf[inputindex] = 0;
		    	inputindex++;
		    }

		    fclose(firstbin);
		    fclose(secondbin);
		    fclose(urandom);
		    close(inputfile);

		} else if (!strcmp(argv[0], "./decrypt")) {
			//TODO: implement decrypt
			//If 1.bin does not exist, print and exit
			if (!file_exists(argv[2])) {
				printf("Hello World\n");
				exit(EXIT_SUCCESS);
			}
			//If 2.bin does not exists, print and exit
			if (!file_exists(argv[3])) {
				printf("Hello World\n");
				exit(EXIT_SUCCESS);
			}
			struct stat firstbin, secondbin;
			size_t firstsize = sizeof(firstbin);
			size_t secondsize = sizeof(secondbin);  
		    memset(&firstbin, 0, firstsize);
		    memset(&secondbin, 0, secondsize);
		    stat(argv[2], &firstbin);
		    stat(argv[3], &secondbin);
		    // Verify input file sizes are equal before processing
		    ssize_t firstfilesize = firstbin.st_size;
		    ssize_t secondfilesize = secondbin.st_size;
		    if (firstfilesize != secondfilesize) {
		    	printf("Hello World\n");
				exit(EXIT_SUCCESS);
		    }


		    int firbin = open(argv[2], O_RDONLY);
		    if (firbin < 0) {
		    	perror("Cannot open random file");
		    	exit(EXIT_FAILURE);
		    }

		    char* firbuf = mmap(NULL, firstfilesize, PROT_READ, MAP_PRIVATE, firbin, 0);
		    if (firbuf == MAP_FAILED) {
		    	perror("Cannot use mmap to read the random file");
		        exit(EXIT_FAILURE);
		    }

		    int secbin = open(argv[3], O_RDONLY);
		    if (secbin < 0) {
		        perror("Cannot open encrypted file");
		        exit(EXIT_FAILURE);
		    }

		    char* secbuf = mmap(NULL, secondfilesize, PROT_READ, MAP_PRIVATE, secbin, 0);
		    if (secbuf == MAP_FAILED) {
		    	perror("Cannot use mmap to read the encrypted file");
		        exit(EXIT_FAILURE);
		    }

		    FILE* outputfile = fopen(argv[1], "w");
		    if (outputfile == NULL) {
		        perror("Cannot open the output file");
		        exit(EXIT_FAILURE);
		    }

		    for (ssize_t i = 0; i < firstfilesize; i++) {
		    	char put = firbuf[i] ^ secbuf[i];
		        fputc(put, outputfile);
		    }

		    munmap(firbuf, firstfilesize);
		    munmap(secbuf, secondfilesize);
		    close(firbin);
		    close(secbin);
		    fclose(outputfile);
		    //Delete files
		    unlink(argv[2]);
		    unlink(argv[3]);
		}
	} else {
		printf("Hello World\n");
		exit(EXIT_SUCCESS);
	}
	exit(EXIT_SUCCESS);
}