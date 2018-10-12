#include <stdlib.h>
#include <stdio.h>

#define NUM 50
#define STR_LENGTH 45
#define ISBN_LENGTH 15

#define RAND_MOD 9000
#define RAND_BAS 1000

#define MODULO 65536

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
int hashing(book_data *library, int n);
int valor(char *txt);

int main (int argC, char *argv[]) {
	int i, j, temp, ok;
	int tam = NUM;
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
		//printf("%d\n", temp);
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
		//printf("%.1f%%\n", 100.0*i/NUM); 
	}
	printf("Done!\n");
	
//	for (i = 0; i < NUM; i++) {
//		//printf("%d\t%d - %s by %s (%f)\n", i, library[i].chave, library[i].nome, library[i].autor, library[i].preco); 
//		printf("%d", i);
//		printf("\t%d - %s by %s\n", library[i].chave, library[i].nome, library[i].autor);
//		printf("\t\t%s - %s\n", library[i].categ, library[i].isbn);
//	}
	
	fclose(fd);
	
	fd = fopen("book_data_new.dat", "wb");
	fwrite(&tam, 1, sizeof(int), fd);
	fwrite(library, NUM, sizeof(book_data), fd);
	int hash = hashing(library, NUM - 1);
	fwrite(&hash, 1, sizeof(int), fd);
	fclose(fd);
	printf("Written hash = %d\n", hash);
}

int hashing(book_data *library, int n) {
	int out = 0;
	while (n) {
		out += ((valor(library[n].nome) * library[n].chave) % MODULO); 
		out += valor(library[n].categ) % MODULO;
		out += valor(library[n].autor) % MODULO;
		out += ((valor(library[n].isbn) * (int)(library[n].preco * 100)) % MODULO);
		out = out % MODULO;
		n--;
	}
	out += ((valor(library[0].nome) << library[0].chave) % MODULO) + valor(library[0].categ) % MODULO + valor(library[0].autor) % MODULO + ((valor(library[0].isbn) << (int)(library[0].preco * 100)) % MODULO);
	out = out % MODULO;
	return out;
}

int valor(char *txt) {
	int i = 0;
	int val = 0;
	while (txt[i] != '\0') {
		val += (i+1) * (int)txt[i++];
		val = val % MODULO;
	}
	return val;
}

void get(char* str, int length, FILE *fd) {
	fgets(str, length, fd);
	int i = 0;
	while (str[i] != '\n') i++;
	str[i] = '\0';
}
