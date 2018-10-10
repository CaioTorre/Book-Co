#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM 50
#define MAX_REGISTERS 80
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

struct t_indice_denso {
	int chaves[MAX_REGISTERS][2];
};
typedef struct t_indice_denso indice_denso;

struct t_indice_alfab {
	int pri;
	int chaves[MAX_REGISTERS];
};
typedef struct t_indice_alfab indice_alfab;

void imprimir_book_data_posicao(book_data *library, int pos);

void criar_indice_denso(book_data *lib, int n, indice_denso *indice);
void imprimir_indice_denso(int n, indice_denso *indice);

int busca_codigo_indice_denso(int codigo, int min, int max, indice_denso *indice);
void busca_indice_denso(int codigo, int n, indice_denso *indice, book_data *library);

void criar_indice_alfab(book_data *lib, int n, indice_alfab *indice);
void imprimir_indice_alfab(int n, indice_alfab *indice);
void imprimir_titulos_ordem_alfab(book_data *li, indice_alfab *indice);

int main(int argC, char *args[]) {
	int i = 0;
	book_data library[NUM];
	
	FILE *fd = fopen("book_data.dat", "rb");
	fread(library, NUM, sizeof(book_data), fd);
	
	indice_denso indice_d;
	
	criar_indice_denso(library, NUM, &indice_d);
	//imprimir_indice_denso(NUM, &indice_d);
	
	//busca_indice_denso(6648, NUM, &indice_d, library);
	busca_indice_denso(2915, NUM, &indice_d, library);
	busca_indice_denso(6649, NUM, &indice_d, library);
	//busca_indice_denso(6650, NUM, &indice_d, library);
	
	indice_alfab indice_a;
	criar_indice_alfab(library, NUM, &indice_a);
	//imprimir_indice_alfab(NUM, &indice_a);
	//imprimir_titulos_ordem_alfab(library, &indice_a);
}

void imprimir_book_data_posicao(book_data *library, int pos) {
	printf("\t%d - %s by %s\n", library[pos].chave, library[pos].nome, library[pos].autor);
	printf("\t\t%s - ", library[pos].categ);
	if (library[pos].isbn[0] == 'B')
		printf("ASIN");
	else 
		printf("ISBN13");
	printf(": %s\n", library[pos].isbn);
}

void criar_indice_denso(book_data *lib, int n, indice_denso *indice) {
	int i, j, temp_chave, temp_ind;
	for (i = 0; i < n; i++) {
		indice->chaves[i][0] = lib[i].chave;
		indice->chaves[i][1] = i;
	}
	
	for (i = 0; i < n; i++) {
		for (j = i; j < n; j++) {
			if (indice->chaves[i][0] > indice->chaves[j][0]) {
				temp_chave = indice->chaves[i][0];
				temp_ind = indice->chaves[i][1];
				indice->chaves[i][0] = indice->chaves[j][0];
				indice->chaves[i][1] = indice->chaves[j][1];
				indice->chaves[j][0] = temp_chave;
				indice->chaves[j][1] = temp_ind;
			}
		}
	}
}

void imprimir_indice_denso(int n, indice_denso *indice) {
	int i;
	printf("INDICE DENSO\n");
	for (i = 0; i < n; i++) {
		printf("\t%2d - [%d][%2d]\n", i, indice->chaves[i][0], indice->chaves[i][1]);
	}
}

int busca_codigo_indice_denso(int codigo, int min, int max, indice_denso *indice) {
	int meio;
	while (min <= max) {
		meio = (min + max)/2;
		if (codigo == indice->chaves[meio][0]) return indice->chaves[meio][1];
		if (codigo <  indice->chaves[meio][0]) {
			max = meio - 1;
		} else {
			min = meio + 1;
		}
	}
	return -1;
}

void busca_indice_denso(int codigo, int n, indice_denso *indice, book_data *library) {
	int i = busca_codigo_indice_denso(codigo, 0, n, indice);
	if (i == -1) {
		printf("Código %d não encontrado...\n", codigo);
	} else {
		imprimir_book_data_posicao(library, i);
	}
}

void criar_indice_alfab(book_data *lib, int n, indice_alfab *indice) {
	int i, j, k, temp;
	indice->pri = 0;
	for (i = 0; i < n; i++) {
		indice->chaves[i] = -1;
	}
	for (i = 1; i < n; i++) {
		if (strcmp(lib[i].nome, lib[indice->pri].nome) < 0) {
			indice->chaves[i] = indice->pri;
			indice->pri = i;
			//printf("Adding new first (%s)\n", lib[i].nome);
		} else {
			j = indice->pri;
			//indice->chaves[j] = -1;
			while (j > -1 && strcmp(lib[i].nome, lib[j].nome) >= 0) {
				//if (j != indice->pri) k = j;
				//printf("\tCurrent (%s) > (%s)\n", lib[i].nome, lib[j].nome);
				k = j;
				j = indice->chaves[j];
			}
			//printf("\t\tFound pos at (%s) [%s] (%s)\n", lib[k].nome, lib[i].nome, lib[j].nome);
			indice->chaves[i] = j;
			indice->chaves[k] = i;
		}
	}
}

void imprimir_indice_alfab(int n, indice_alfab *indice) {
	int i;
	printf("INDICE EM ORDEM ALFABETICA\n\tPRI=%d\n", indice->pri);
	for (i = 0; i < n; i++) {
		printf("\t%2d - [%2d]\n", i, indice->chaves[i]);
	}
}

void imprimir_titulos_ordem_alfab(book_data *li, indice_alfab *indice) {
	int temp = indice->pri;
	int i = 0;
	printf("TITULOS EM ORDEM ALFABETICA\n");
	while (temp > -1) {
		printf("\t%2d - %s\n", i++, li[temp].nome);
		temp = indice->chaves[temp];
	}
}
