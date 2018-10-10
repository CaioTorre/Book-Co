#include <stdlib.h>
#include <stdio.h>

#define NUM 50
#define STR_LENGTH 45
#define ISBN_LENGTH 15

#define RAND_MOD 9000
#define RAND_BAS 1000

struct tdata {
	int chave;
	char nome[STR_LENGTH];
	char categ[STR_LENGTH];
	float preco;
	char isbn[ISBN_LENGTH];
	char autor[STR_LENGTH];
};

typedef struct tdata book_data;
void get(char* str, int length, FILE *fd);

int main (int argC, char *argv[]) {
	int i, j, temp, ok;
	int chaves[NUM];
	for (i = 0; i < NUM; i++) {
		do {
			temp = RAND_BAS + rand() % RAND_MOD;
			ok = 1;
			for (j = 0; j < i; j++) {
				if (chaves[j] == temp) ok = 0;
			}
		} while (!ok);
		chaves[i] = temp;
		printf("%d\n", temp);
	}
	
	book_data library[NUM];
	
	FILE *fd = fopen("livros.txt", "r");
	
	printf("Starting read...\n");
	
	for (i = 0; i < NUM; i++) {
		library[i].chave = chaves[i];
		get(library[i].nome, STR_LENGTH, fd);
		get(library[i].categ, STR_LENGTH, fd);
		library[i].preco = (float)(rand() % 10000) / 100.0;
		get(library[i].isbn, ISBN_LENGTH, fd);
		get(library[i].autor, STR_LENGTH, fd);
		printf("%.1f%%\n", 100.0*i/NUM); 
	}
	printf("Done!\n");
	
	for (i = 0; i < NUM; i++) {
		//printf("%d\t%d - %s by %s (%f)\n", i, library[i].chave, library[i].nome, library[i].autor, library[i].preco); 
		printf("%d", i);
		printf("\t%d - %s by %s\n", library[i].chave, library[i].nome, library[i].autor);
		printf("\t\t%s - %s\n", library[i].categ, library[i].isbn);
	}
	
	fclose(fd);
	
	fd = fopen("book_data.dat", "wb");
	
	fwrite(library, NUM, sizeof(book_data), fd);
	
	fclose(fd);
}

void get(char* str, int length, FILE *fd) {
	fgets(str, length, fd);
	int i = 0;
	while (str[i] != '\n') i++;
	str[i] = '\0';
}
