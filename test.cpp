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

int main(int argC, char *args[]) {
	int i = 0;
	book_data library[NUM];
	
	FILE *fd = fopen("book_data.dat", "rb");
	fread(library, NUM, sizeof(book_data), fd);
	
	for (i = 0; i < NUM; i++) {
		//printf("%d\t%d - %s by %s (%f)\n", i, library[i].chave, library[i].nome, library[i].autor, library[i].preco); 
		printf("%d", i+1);
		printf("\t%d - %s by %s\n", library[i].chave, library[i].nome, library[i].autor);
		printf("\t\t%s - %s\n", library[i].categ, library[i].isbn);
	}
}
