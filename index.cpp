#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cmath>

#define NUM 50
#define MAX_REGISTERS 80
#define STR_LENGTH 45
#define ISBN_LENGTH 15

#define RAND_MOD 9000
#define RAND_BAS 1000

#define MODULO 65536

#define HASH_LENGTH 16

#define NUM_CATEGORIAS 5

#define DONE_FILE

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

struct t_indice_denso_cat {
	int primeiros[NUM_CATEGORIAS];
	int chaves[MAX_REGISTERS];
};
typedef struct t_indice_denso_cat indice_denso_cat;

struct t_indice_alfab {
	int pri;
	int chaves[MAX_REGISTERS];
};
typedef struct t_indice_alfab indice_alfab;

struct t_indice_alfab_cat {
	int primeiros[NUM_CATEGORIAS];
	int chaves[MAX_REGISTERS];
};
typedef struct t_indice_alfab_cat indice_alfab_cat;

void purge_ln(char *txt);

void imprimir_formato();
int menu();
int submenu();
int submenu_dev();

void consulta(book_data *library, indice_denso *indice_d);
int atualizar_campos(book_data *library, int n, indice_denso *indice_d, char categorias[][STR_LENGTH]);
int insere_elemento(book_data *lib, int *n, indice_denso *indice, char categorias[][STR_LENGTH]);
int remove_elemento(book_data *lib, int *n, indice_denso *indice, book_data *excluido);

int encontra_codigo_categoria(char categorias[][STR_LENGTH], char categoria[]);
void imprimir_todos_desordenados(book_data *library, int n);
void imprimir_book_data_posicao(book_data *library, int pos);

void criar_indice_denso(book_data *lib, int n, indice_denso *indice);
void imprimir_indice_denso(int n, indice_denso *indice);
void imprimir_todos_indice_denso(book_data *lib, int n, indice_denso *indice);
void insere_indice_denso(int cod, int pos, indice_denso *indice);
void remove_indice_denso(int n, int cod, int pos, indice_denso *indice);

int busca_codigo_indice_denso(int codigo, int min, int max, indice_denso *indice);
void busca_indice_denso(int codigo, int n, indice_denso *indice, book_data *library);

void criar_indice_denso_categorias(book_data *lib, char categorias[][STR_LENGTH], int n, indice_denso *ordenado, indice_denso_cat *indice);
void imprimir_indice_denso_categorias(indice_denso_cat *indice, int n);
void imprimir_todos_indice_denso_categorias(book_data *lib, char categorias[][STR_LENGTH], indice_denso_cat *indice);
void insere_indice_denso_categorias(int cat, int pos, indice_denso *ordenado, indice_denso_cat *indice);
void remove_indice_denso_categorias(int cat, int pos, int n, indice_denso_cat *indice);

void criar_indice_alfab(book_data *lib, int n, indice_alfab *indice);
void imprimir_indice_alfab(int n, indice_alfab *indice);
void imprimir_titulos_ordem_alfab(book_data *li, indice_alfab *indice);
void imprimir_todos_indice_alfab(book_data *lib, indice_alfab *indice);
void insere_indice_alfab(char nome[], int pos, indice_alfab *indice, book_data *library);
void remove_indice_alfab(int pos, int n, indice_alfab *indice);

void criar_indice_alfab_categorias(book_data *lib, char categorias[][STR_LENGTH], int n, indice_alfab *ordenado, indice_alfab_cat *indice);
void imprimir_indice_alfab_categorias(indice_alfab_cat *indice, int n);
void imprimir_todos_indice_alfab_categorias(book_data *lib, char categorias[][STR_LENGTH], indice_alfab_cat *indice);
void insere_indice_alfab_categorias(char nome[], int cat, int pos, indice_alfab *ordenado, indice_alfab_cat *indice);
void remove_indice_alfab_categorias(int cat, int pos, int n, indice_alfab_cat *indice);

int hashing(book_data *library, int n);
int valor(char *txt);
int salvar_arquivo(book_data *library, int n, const char *arquivo);
int salvar_indices(indice_denso *d, indice_denso_cat *dc, indice_alfab *a, indice_alfab_cat *ac, int hash);
int carregar_indices(indice_denso *d, indice_denso_cat *dc, indice_alfab *a, indice_alfab_cat *ac, int hash);

int main(int argC, char *args[]) {
	int i = 0;
	int total_registros = NUM;
	book_data library[MAX_REGISTERS];
	book_data excluido;
	int hash_atual;
	int erros_carregamento;
	int modificacoes_nao_salvas = 0;
	
	indice_denso indice_d;
	indice_denso_cat indice_dc;
	indice_alfab indice_a;
	indice_alfab_cat indice_ac;

	char categorias[NUM_CATEGORIAS][STR_LENGTH] = {
		"Ficcao",
		"Misterio",
		"Nao-ficcao",
		"Fantasia",
		"Classicos"
	};
	
	FILE *fd = fopen("book_data_new.dat", "rb");

	fread(&total_registros, 1, sizeof(int), fd);
	
	printf("Carregando arquivo principal... ");
	fread(library, total_registros, sizeof(book_data), fd);
	printf("Feito! (%d registros)\n", total_registros);

	//printf("HASH CALCULADO := %d\n", hashing(library, total_registros - 1));

	fread(&hash_atual, 1, sizeof(int), fd);
	fclose(fd);
	//printf("HASH LIDO := %d\n", hash_atual);
	printf("Carregando indices... ");
	erros_carregamento = carregar_indices(&indice_d, &indice_dc, &indice_a, &indice_ac, hash_atual);
	if (erros_carregamento < 0) {
		printf("Erro ao ler arquivos (codigo %d)! Recriando todos os indices...\n", erros_carregamento);
		criar_indice_denso(library, NUM, &indice_d);
		criar_indice_denso_categorias(library, categorias, NUM, &indice_d, &indice_dc);
		criar_indice_alfab(library, NUM, &indice_a);
		criar_indice_alfab_categorias(library, categorias, NUM, &indice_a, &indice_ac);
	} else {
		printf("Feito! (codigo %d)\n", erros_carregamento);
		if (erros_carregamento % 2) criar_indice_denso(library, NUM, &indice_d);
		erros_carregamento /= 2;
		if (erros_carregamento % 2) criar_indice_denso_categorias(library, categorias, NUM, &indice_d, &indice_dc);
		erros_carregamento /= 2;
		if (erros_carregamento % 2) criar_indice_alfab(library, NUM, &indice_a);
		erros_carregamento /= 2;
		if (erros_carregamento % 2) criar_indice_alfab_categorias(library, categorias, NUM, &indice_a, &indice_ac);
	}
	
	int op1, op2;
	
	do {
		op1 = menu();
		switch (op1) {
			case 1:
				consulta(library, &indice_d);
				break;
			case 2:
				if (atualizar_campos(library, total_registros, &indice_d, categorias) != -1) { //ATUALIZAR INDICES
					printf("HASH := %d\n", hashing(library, total_registros-1));
					criar_indice_denso(library, total_registros, &indice_d);
					criar_indice_denso_categorias(library, categorias, total_registros, &indice_d, &indice_dc);
					criar_indice_alfab(library, total_registros, &indice_a);
					criar_indice_alfab_categorias(library, categorias, total_registros, &indice_a, &indice_ac);
					modificacoes_nao_salvas = 1;
					//REESCREVER ARQUIVO (possivelmente so editar o arquivo relevante (fseek))
				}
				break;
			case 3:
				int pos_removida;
				if ((pos_removida = remove_elemento(library, &total_registros, &indice_d, &excluido)) != -1) { //ATUALIZAR INDICES
					#ifdef BETA_FUNCTIONS
						int cat = encontra_codigo_categoria(categorias, excluido.categ);
						int n = total_registros - 1;
						remove_indice_denso(n, excluido.chave, pos_removida, &indice_d);
						printf("Removido do indice_denso\n");
						remove_indice_denso_categorias(cat, pos_removida, n, &indice_dc);
						printf("Removido do indice_denso_cat\n");
						remove_indice_alfab(pos_removida, total_registros, &indice_a);
						printf("Removido do indice_alfab\n");
						remove_indice_alfab_categorias(cat, pos_removida, total_registros, &indice_ac);
						printf("Removido do indice_alfab_cat\n");
					#else
						criar_indice_denso(library, total_registros, &indice_d);
						criar_indice_denso_categorias(library, categorias, total_registros, &indice_d, &indice_dc);
						criar_indice_alfab(library, total_registros, &indice_a);
						criar_indice_alfab_categorias(library, categorias, total_registros, &indice_a, &indice_ac);
					#endif
					modificacoes_nao_salvas = 1;
				}
				break;
			case 4:
				if (insere_elemento(library, &total_registros, &indice_d, categorias) == 1) { //ATUALIZAR INDICES
					//printf("HASH := %d\n", hashing(library, n));
					#ifdef BETA_FUNCTIONS
						int cat = encontra_codigo_categoria(categorias, library[total_registros-1].categ);
						int n = total_registros - 1;
						printf("Insercao realizada com sucesso, atualizando indices...\n");
						insere_indice_denso(library[n+1].chave, n+1, &indice_d);
						printf("Inserido em indice_denso\n");
						insere_indice_denso_categorias(cat, n, &indice_d, &indice_dc);
						printf("Inserido em indice_denso_cat\n");
						insere_indice_alfab(library[n].nome, n, &indice_a, library);
						printf("Inserido em indice_alfab\n");
						insere_indice_alfab_categorias(library[n].nome, cat, n, &indice_a, &indice_ac);
						printf("Inserido em indice_alfab_cat\n");
					#else
						criar_indice_denso(library, total_registros, &indice_d);
						criar_indice_denso_categorias(library, categorias, total_registros, &indice_d, &indice_dc);
						criar_indice_alfab(library, total_registros, &indice_a);
						criar_indice_alfab_categorias(library, categorias, total_registros, &indice_a, &indice_ac);
					#endif
					modificacoes_nao_salvas = 1;
					//REESCREVER ARQUIVO (possivelmente so editar o arquivo relevante (fseek))
				}
				break;
			case 5:
				op2 = submenu();
				switch (op2) {
					case 1:
						imprimir_todos_indice_denso(library, total_registros, &indice_d);
						break;
					case 2:
						imprimir_todos_indice_alfab(library, &indice_a);
						break;
					case 3:
						imprimir_todos_indice_denso_categorias(library, categorias, &indice_dc);
						break;
					case 4:
						imprimir_todos_indice_alfab_categorias(library, categorias, &indice_ac);
						break;
					case 5:
						imprimir_todos_desordenados(library, total_registros);
						break;
					case 6:
						break;
					default:
						printf("Erro na execucao\n");
						break;
				}
				break;
			case 6:
				printf("Salvando arquivo...\n");
				hash_atual = salvar_arquivo(library, total_registros, "book_data_new.dat");
				if (hash_atual == -1) {
					printf("Erro ao salvar arquivo...\n");
					break;
				} else {
					modificacoes_nao_salvas = 0;
					printf("Arquivo salvo com sucesso!\nSalvando indices...\n");
					int cod = salvar_indices(&indice_d, &indice_dc, &indice_a, &indice_ac, hash_atual);
					if (cod == 1) printf("Indices salvos com sucesso!\n");
					else printf("Erro ao salvar indices (codigo %d)\n", cod);
				}
				break;
			case 7:
				op2 = submenu_dev();
				switch (op2) {
					case 1:
						imprimir_indice_denso(total_registros, &indice_d);
						break;
					case 2:
						imprimir_indice_alfab(total_registros, &indice_a);
						break;
					case 3:
						imprimir_indice_denso_categorias(&indice_dc, total_registros);
						break;
					case 4:
						imprimir_indice_alfab_categorias(&indice_ac, total_registros);
						break;
					case 5:
						break;
					default:
						printf("Erro na execucao\n");
						break;
				}
				break;
			case 8:
				if (modificacoes_nao_salvas == 1) {
					printf("Voce tem modificacoes nao salvas, deseja sair mesmo assim? (0 para NAO, 1 para SIM)\n> ");
					scanf("%d", &op2);
					if (op2 == 0) op1 = 0;
				}
			default:
				break;
		}
	} while (op1 != 8);
	printf("Encerrando sistema...\n");
}

void imprimir_formato() {
	int line_wid = 8 + 4 + 3 + 45 + 3 + 25 + 10 + 23 + 3 + 8 + 6;
	int i;
	for (i = 0; i < line_wid; i++) printf("-");
	printf("\n");
	printf("POS\t%4s - %45s | %25s | %10s | %23s | %s\n", "COD", "TITULO", "AUTOR", "CATEGORIA", "ISBN/ASIN", "PRECO");
	for (i = 0; i < line_wid; i++) printf("-");
	printf("\n");
}

int menu() {
	int opcao;
	do {
		printf("\n-------Sistema de livraria Book-Co-------\n");
		printf("\t1. Consultar um livro\n");
		printf("\t2. Alterar dados de um livro\n");
		printf("\t3. Eliminar um livro do sistema\n");
		printf("\t4. Inserir um livro no sistema\n");
		printf("\t5. Impressoes (sub-menu)\n");
		printf("\t6. Salvar alteracoes no arquivo\n");
		printf("\t7. Impressoes-indices (dev)\n");
		printf("\t8. Sair\n");
		printf("-----------------------------------------\n");
		printf("\n> ");
		scanf("%d", &opcao);
		if (opcao < 1 || opcao > 8) printf("Opcao digitada invalida!\n");
	} while (opcao < 1 || opcao > 8);
	return opcao;
}

int submenu() {
	int opcao;
	do {
		printf("\t\t1. Imprimir todos ordenados pela chave primaria\n");
		printf("\t\t2. Imprimir todos ordenados pela chave secundaria\n");
		printf("\t\t3. Imprimir todos por categoria, ordenados pela chave primaria\n");
		printf("\t\t4. Imprimir todos por categoria, ordenados pela chave secundaria\n");
		printf("\t\t5. Imprimir arquivo (desordenado)\n");
		printf("\t\t6. Cancelar\n");
		printf("\n> ");
		scanf("%d", &opcao);
		if (opcao < 1 || opcao > 6) printf("Opcao digitada invalida!");
	} while (opcao < 1 || opcao > 6);
	return opcao;
}

int submenu_dev() {
	int opcao;
	do {
		printf("\t\t1. Imprimir indice denso      - chave primaria\n");
		printf("\t\t2. Imprimir tabela invertida  - chave secundaria\n");
		printf("\t\t3. Imprimir indice secundario - chave primaria   + categorias\n");
		printf("\t\t4. Imprimir indice secundario - chave secundaria + categorias\n");
		printf("\t\t5. Cancelar\n");
		printf("\n> ");
		scanf("%d", &opcao);
		if (opcao < 1 || opcao > 5) printf("Opcao digitada invalida!");
	} while (opcao < 1 || opcao > 5);
	return opcao;
}

int carregar_indices(indice_denso *d, indice_denso_cat *dc, indice_alfab *a, indice_alfab_cat *ac, int hash) {
	int chash;
	int ret = 0;
	FILE *fd = fopen("indice_denso.dat", "rb");
	if (!fd) return -1;
	fread(d, 1, sizeof(indice_denso), fd);
	fread(&chash, 1, sizeof(int), fd);
	fclose(fd);
	if (chash != hash) {
		printf("Indice denso esta desatualizado!\n");
		ret += 1;
	}
	
	fd = fopen("indice_denso_cat.dat", "rb");
	if (!fd) return -2;
	fread(dc, 1, sizeof(indice_denso_cat), fd);
	fread(&chash, 1, sizeof(int), fd);
	fclose(fd);
	if (chash != hash) {
		printf("Indice denso_categorias esta desatualizado!\n");
		ret += 2;
	}
	
	fd = fopen("indice_alfab.dat", "rb");
	if (!fd) return -3;
	fread(a, 1, sizeof(indice_alfab), fd);
	fread(&chash, 1, sizeof(int), fd);
	fclose(fd);
	if (chash != hash) {
		printf("Indice alfabetico esta desatualizado!\n");
		ret += 4;
	}
	
	fd = fopen("indice_alfab_cat.dat", "rb");
	if (!fd) return -4;
	fread(ac, 1, sizeof(indice_alfab_cat), fd);
	fread(&chash, 1, sizeof(int), fd);
	fclose(fd);
	if (chash != hash) {
		printf("Indice alfabetico_categorias esta desatualizado!\n");
		ret += 8;
	}
	
	return ret;
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

int salvar_arquivo(book_data *library, int n, const char *arquivo) {
	FILE *fd = fopen(arquivo, "wb");
	if (!fd) return -1;
	fwrite(&n, 1, sizeof(int), fd);
	fwrite(library, n, sizeof(book_data), fd);
	
	int hashval = hashing(library, n - 1);
	fwrite(&hashval, 1, sizeof(int), fd);
	
	fclose(fd);
	return hashval;
}

int salvar_indices(indice_denso *d, indice_denso_cat *dc, indice_alfab *a, indice_alfab_cat *ac, int hash) {
	FILE *fd = fopen("indice_denso.dat", "wb");
	if (!fd) return -1;
	fwrite(d, 1, sizeof(indice_denso), fd);
	fwrite(&hash, 1, sizeof(int), fd);
	fclose(fd);
	
	fd = fopen("indice_denso_cat.dat", "wb");
	if (!fd) return -2;
	fwrite(dc, 1, sizeof(indice_denso_cat), fd);
	fwrite(&hash, 1, sizeof(int), fd);
	fclose(fd);
	
	fd = fopen("indice_alfab.dat", "wb");
	if (!fd) return -3;
	fwrite(a, 1, sizeof(indice_alfab), fd);
	fwrite(&hash, 1, sizeof(int), fd);
	fclose(fd);
	
	fd = fopen("indice_alfab_cat.dat", "wb");
	if (!fd) return -4;
	fwrite(ac, 1, sizeof(indice_alfab_cat), fd);
	fwrite(&hash, 1, sizeof(int), fd);
	fclose(fd);
	
	return 1;
}

void consulta(book_data *library, indice_denso *indice_d) {
	int cod;
	printf("\t\tDigite o codigo a ser buscado: ");
	scanf("%d", &cod);
	busca_indice_denso(cod, NUM, indice_d, library);
}

int atualizar_campos(book_data *library, int n, indice_denso *indice_d, char categorias[][STR_LENGTH]) {
	int cod, editar, i;
	printf("\t\tDigite o codigo a ser buscado: ");
	scanf("%d", &cod);
	int pos = busca_codigo_indice_denso(cod, 0, n, indice_d);
	if (pos == -1) {
		printf("\tCodigo %d nao encontrado...\n", cod);
		return -1;
	} else {
		printf("\tLivro encontrado:\n");
		imprimir_book_data_posicao(library, pos);
		
		editar = 0;
		printf("Deseja editar o campo TITULO? (0 para NAO, 1 para SIM)\n> ");
		scanf("%d", &editar);
		if (editar) {
			fgetc(stdin);
			printf("\tDigite o novo TITULO\n> ");
			fgets(library[pos].nome, STR_LENGTH, stdin);
			purge_ln(library[pos].nome);
		}
		
		editar = 0;
		printf("Deseja editar o campo AUTOR? (0 para NAO, 1 para SIM)\n> ");
		scanf("%d", &editar);
		if (editar) {
			fgetc(stdin);
			printf("\tDigite o novo AUTOR\n> ");
			fgets(library[pos].autor, STR_LENGTH, stdin);
			purge_ln(library[pos].autor);
		}
		
		editar = 0;
		printf("Deseja editar o campo CATEGORIA? (0 para NAO, 1 para SIM)\n> ");
		scanf("%d", &editar);
		if (editar) {
			fgetc(stdin);
			printf("\tSelecione a nova CATEGORIA\n");
			for (i = 0; i < NUM_CATEGORIAS; i++) {
				printf("\t%d. %s\n", i+1, categorias[i]);
			}
			printf("> ");
			do {
				scanf("%d", &i);
				fgetc(stdin);
				if (i < 1 || i > NUM_CATEGORIAS) printf("Categoria invalida!\n");
			} while (i < 1 || i > NUM_CATEGORIAS);
			strcpy(library[pos].categ, categorias[i-1]);	
		}
		
		editar = 0;
		printf("Deseja editar o campo ISBN/ASIN? (0 para NAO, 1 para SIM)\n> ");
		scanf("%d", &editar);
		if (editar) {
			fgetc(stdin);
			printf("\tDigite o novo ISBN/ASIN\n> ");
			fgets(library[pos].isbn, ISBN_LENGTH, stdin);
			purge_ln(library[pos].isbn);
		}
		
		editar = 0;
		printf("Deseja editar o campo PRECO? (0 para NAO, 1 para SIM)\n> ");
		scanf("%d", &editar);
		if (editar) {
			fgetc(stdin);
			printf("\tDigite o novo PRECO\n> ");
			scanf("%f", &library[pos].preco);
		}
		
		printf("\tLivro apos edicoes:\n");
		imprimir_book_data_posicao(library, pos);
		return pos;
	}
}

int insere_elemento(book_data *lib, int *n, indice_denso *indice, char categorias[][STR_LENGTH]) {
	if (*n >= MAX_REGISTERS) {
		printf("Numero maximo de registros! Remova um livro antes...\n");
		return -1;
	}
	int ok;
	do {
		printf("Digite o codigo do novo livro (ou -1 para cancelar)\n> ");
		scanf("%d", &lib[*n].chave);
		fgetc(stdin);
		if (lib[*n].chave == -1) return -1;
		ok = (busca_codigo_indice_denso(lib[*n].chave, 0, *n-1, indice) == -1) ? 1 : 0;
		if (!ok) printf("Codigo ja encontrado, tente novamente...\n");
	} while (!ok);
	
	printf("Digite o TITULO do novo livro\n> ");
	fgets(lib[*n].nome, STR_LENGTH, stdin);
	purge_ln(lib[*n].nome);
	
	printf("Digite o AUTOR do novo livro\n> ");
	fgets(lib[*n].autor, STR_LENGTH, stdin);
	purge_ln(lib[*n].autor);
	
	printf("Selecione a CATEGORIA do novo livro\n");
	int i;
	for (i = 0; i < NUM_CATEGORIAS; i++) {
		printf("\t%d. %s\n", i+1, categorias[i]);
	}
	printf("> ");
	do {
		scanf("%d", &i);
		fgetc(stdin);
		if (i < 1 || i > NUM_CATEGORIAS) printf("Categoria invalida!\n");
	} while (i < 1 || i > NUM_CATEGORIAS);
	strcpy(lib[*n].categ, categorias[i-1]);
	purge_ln(lib[*n].categ);
	
	printf("Digite o ISBN/ASIN do novo livro\n> ");
	fgets(lib[*n].isbn, ISBN_LENGTH, stdin);
	purge_ln(lib[*n].isbn);
	
	printf("Digite o PRECO do novo livro\n> ");
	scanf("%f", &lib[*n].preco);
	
	printf("-------------------------------------------\nO novo livro é:\n");
	imprimir_book_data_posicao(lib, *n);
	printf("Confirma? (0 para NAO, 1 para SIM)\n> ");
	scanf("%d", &i);
	
	if (!i) return -1;
	(*n) = (*n) + 1;
	return 1;
}

int remove_elemento(book_data *lib, int *n, indice_denso *indice, book_data *excluido) {
	if (*n == 0) {
		printf("Nenhum elemento a ser removido!\n");
		return -1;
	}
	int cod;
	int ok;
	int pos;
	do {
		printf("Digite o codigo do livro a ser removido (ou -1 para cancelar)\n> ");
		scanf("%d", &cod);
		if (cod == -1) return -1;
		ok = ((pos = busca_codigo_indice_denso(cod, 0, *n-1, indice)) == -1) ? 0 : 1;
		if (!ok) printf("Codigo nao encontrado, tente novamente...\n");
	} while (!ok);
	printf("Livro encontrado:\n[%2d]", pos);
	imprimir_book_data_posicao(lib, pos);
	printf("\nTem certeza que deseja remover esse livro? (1 para SIM, 0 para NAO)\n> ");
	scanf("%d", &ok);
	if (ok == 1) {
		*excluido = lib[pos];
		int i;
		*n = *n - 1;
		for (i = pos + 1; i <= *n; i++) {
			lib[i - 1] = lib[i];
		}
		return pos;
	}
	return -1;
}

void insere_indice_denso(int cod, int pos, indice_denso *indice) {
	int i = pos;
	while (cod < indice->chaves[pos][0]) {
		indice->chaves[i+1][0] = indice->chaves[i][0];
		indice->chaves[i+1][1] = indice->chaves[i][1];
		i--;
	}
	indice->chaves[i+1][0] = cod;
	indice->chaves[i+1][1] = pos;
}

void remove_indice_denso(int n, int cod, int pos, indice_denso *indice) {
	int i;
	n = n + 1;
	for (i = 0; i <= n; i++) {
		if (indice->chaves[i][1] > pos) indice->chaves[i][1] = indice->chaves[i][1] - 1; //Todas as posicoes maiores que a removida
		if (indice->chaves[i][0] > cod) { //UP-SHIFTING valores com codigo maior (ficam depois dele no vetor)
			indice->chaves[i-1][0] = indice->chaves[i][0];
			indice->chaves[i-1][1] = indice->chaves[i][1];
		}
	}
}

void insere_indice_denso_categorias(int cat, int pos, indice_denso *ordenado, indice_denso_cat *indice) {
	int atual = indice->primeiros[cat];
	int em_ordem = 0; //Vai percorrer ordenado
	int anterior; //Ultimo da lista ligada
	while (ordenado->chaves[em_ordem][1] != pos && ordenado->chaves[em_ordem][1] != atual) em_ordem++;
	if (ordenado->chaves[em_ordem][1] == pos) { //Novo elemento é o primeiro da categoria
		indice->chaves[pos] = indice->primeiros[cat];
		indice->primeiros[cat] = pos;
	} else { //Existe algum elemento que vem antes do novo
		int ok = 0;
		anterior = indice->primeiros[cat];
		do {
			atual = indice->chaves[atual];
			while (ordenado->chaves[em_ordem][1] != pos && ordenado->chaves[em_ordem][1] != atual) em_ordem++;
			if (ordenado->chaves[em_ordem][1] == pos) { //Achou a posicao do novo elemento
				indice->chaves[pos] = atual; //= indice->chaves[anterior];
				indice->chaves[anterior] = pos;
				ok = 1;
			}
			anterior = atual;
		} while (!ok);
	}
}

void remove_indice_denso_categorias(int cat, int pos, int n, indice_denso_cat *indice) {
	int atual = indice->primeiros[cat];
	if (atual == pos) {
		indice->primeiros[cat] = indice->chaves[atual];
	} else {
		atual = indice->chaves[atual];
		while (indice->chaves[atual] != pos) atual = indice->chaves[atual];
		indice->chaves[atual] = indice->chaves[indice->chaves[atual]];
	}
	int i;
	for (i = 0; i < NUM_CATEGORIAS; i++) {
		if (indice->primeiros[i] > pos) indice->primeiros[i] = indice->primeiros[i] - 1; //Todas as posicoes maiores que a removida
	}
	for (i = pos + 1; i <= n + 1; i++) indice->chaves[i - 1] = indice->chaves[i]; //UP-SHIFTING valores com codigo maior (ficam depois dele no vetor)
	for (i = 0; i <= n + 1; i++) {
		if (indice->chaves[i] > pos) indice->chaves[i] = indice->chaves[i] - 1; //Todas as posicoes maiores que a removida
	}
}

void insere_indice_alfab(char nome[], int pos, indice_alfab *indice, book_data *library) {
	int atual = indice->pri;
	int anterior;
	if (strcmp(library[atual].nome, nome) > 0) {
		indice->chaves[pos] = atual;
		indice->pri = pos;
		return;
	}
	anterior = atual;
	atual = indice->chaves[atual];
	while (strcmp(library[atual].nome, nome) < 0 && atual != -1) {
		anterior = atual;
		atual = indice->chaves[atual];
	}
	
	indice->chaves[anterior] = pos;
	indice->chaves[pos] = atual;
}

void remove_indice_alfab(int pos, int n, indice_alfab *indice) {
	int atual = indice->pri;
	if (pos == atual) {
		indice->pri = indice->chaves[atual];
		if (indice->pri > pos) indice->pri = indice->pri - 1;
	} else {
		if (atual > pos) indice->pri = indice->pri - 1; //Diminui se for um ponteiro maior que o removido
		atual = indice->chaves[atual];
		while (indice->chaves[atual] != pos) atual = indice->chaves[atual];
		indice->chaves[atual] = indice->chaves[indice->chaves[atual]]; 
	}
	int i;
	for (i = pos + 1; i <= n; i++) indice->chaves[i - 1] = indice->chaves[i]; //UP-SHIFTING valores com codigo maior (ficam depois dele no vetor)
	for (i = 0; i <= n; i++) { 
		if (indice->chaves[i] > pos) indice->chaves[i] = indice->chaves[i] - 1; //Todas as posicoes maiores que a removida
	} 
}

void insere_indice_alfab_categorias(char nome[], int cat, int pos, indice_alfab *ordenado, indice_alfab_cat *indice) {
	int em_ordem = ordenado->pri;
	int atual = indice->primeiros[cat];
	int anterior;
	
	if (em_ordem == pos) {
		indice->chaves[pos] = em_ordem;
		indice->primeiros[cat] = pos;
		return;
	}
	
	anterior = atual;
	int ok = 0;
	do {
		while (em_ordem != pos && em_ordem != atual) em_ordem = ordenado->chaves[em_ordem];
		if (em_ordem == pos) {
			indice->chaves[anterior] = pos;
			indice->chaves[pos] = atual;
			ok = 1;
		} else {
			anterior = atual;
			atual = indice->chaves[atual];
		}
	} while (!ok);
}

void remove_indice_alfab_categorias(int cat, int pos, int n, indice_alfab_cat *indice) {
	int atual = indice->primeiros[cat];
	if (pos == atual) {
		indice->primeiros[cat] = indice->chaves[atual];
	} else {
		atual = indice->chaves[atual];
		while (indice->chaves[atual] != pos) atual = indice->chaves[atual];
		indice->chaves[atual] = indice->chaves[indice->chaves[atual]]; 
	}
	int i;
	for (i = 0; i < NUM_CATEGORIAS; i++) {
		if (indice->primeiros[i] > pos) indice->primeiros[i] = indice->primeiros[i] - 1; //Todas as posicoes maiores que a removida
	}
	for (i = pos + 1; i <= n + 1; i++) indice->chaves[i - 1] = indice->chaves[i]; //UP-SHIFTING valores com codigo maior (ficam depois dele no vetor)
	for (i = 0; i <= n + 1; i++) { 
		if (indice->chaves[i] > pos) indice->chaves[i] = indice->chaves[i] - 1; //Todas as posicoes maiores que a removida
	} 
}

void purge_ln(char *txt) {
	int i = 0;
	while (txt[i] != '\n' && txt[i] != '\0') i++;
	txt[i] = '\0';
}
int encontra_codigo_categoria(char categorias[][STR_LENGTH], char categoria[]) {
	int i = 0;
	while (strcmp(categorias[i], categoria)) i++;
	return i;
}

void imprimir_todos_desordenados(book_data *library, int n) {
	int i;
	printf("TODOS OS TITULOS DESORDENADOS\n");
	imprimir_formato();
	for (i = 0; i < n; i++) {
		printf("[%2d]", i);
		imprimir_book_data_posicao(library, i);
	}
}

void imprimir_book_data_posicao(book_data *library, int pos) {
	printf("\t%4d - %45s | %25s | ", library[pos].chave, library[pos].nome, library[pos].autor);
	printf("%10s | ", library[pos].categ);
	if (library[pos].isbn[0] == 'B')
		printf("  ASIN");
	else 
		printf("ISBN13");
	printf(": %15s | R$%.2f\n", library[pos].isbn, library[pos].preco);
}

void criar_indice_denso(book_data *lib, int n, indice_denso *indice) {
	int i, j, temp_chave, temp_ind;
	for (i = 0; i < n; i++) {
		indice->chaves[i][0] = lib[i].chave;
		indice->chaves[i][1] = i;
	}
	for (i = 0; i < n; i++) { //BUBBLESORT
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

int busca_codigo_indice_denso(int codigo, int min, int max, indice_denso *indice) { //BUSCA BINARIA
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
	int i = busca_codigo_indice_denso(codigo, 0, n-1, indice);
	if (i == -1) {
		printf("Código %d não encontrado...\n", codigo);
	} else {
		imprimir_book_data_posicao(library, i);
	}
}

void imprimir_todos_indice_denso(book_data *lib, int n, indice_denso *indice) {
	int i;
	printf("TODOS OS TITULOS COM ORDENACAO PRIMARIA\n");
	imprimir_formato();
	for (i = 0; i < n; i++) {
		printf("[%2d]", i);
		imprimir_book_data_posicao(lib, indice->chaves[i][1]);
	}
}

void criar_indice_denso_categorias(book_data *lib, char categorias[][STR_LENGTH], int n, indice_denso *ordenado, indice_denso_cat *indice) {
	int atual = 0;
	int categoria_atual;
	int i;
	
	for (i = 0; i < NUM_CATEGORIAS; i++) {
		indice->primeiros[i] = -1;
	}
	for (i = 0; i < n; i++) {
		indice->chaves[i] = -1;
	}
	while (atual < n) {
		categoria_atual = encontra_codigo_categoria(categorias, lib[ordenado->chaves[atual][1]].categ);
		
		if (indice->primeiros[categoria_atual] == -1) {
			indice->primeiros[categoria_atual] = ordenado->chaves[atual][1];
		} else {
			i = indice->primeiros[categoria_atual];
			while (indice->chaves[i] != -1) i = indice->chaves[i];
			indice->chaves[i] = ordenado->chaves[atual][1];
		}
		atual++;
	}
}

void imprimir_indice_denso_categorias(indice_denso_cat *indice, int n) {
	int i;
	printf("INDICE DENSO POR CATEGORIAS\n\tPRIs\n");
	for (i = 0; i < NUM_CATEGORIAS; i++) {
		printf("\t[%2d]\n", indice->primeiros[i]);
	}
	printf("\tCHAVES\n");
	for (i = 0; i < n; i++) {
		printf("\t%2d - [%2d]\n", i, indice->chaves[i]);
	}
}

void imprimir_todos_indice_denso_categorias(book_data *lib, char categorias[][STR_LENGTH], indice_denso_cat *indice) {
	int i, cat, p = 0;
	printf("TODOS OS TITULOS COM ORDENACAO PRIMARIA (POR CATEGORIAS)\n");
	for (cat = 0; cat < NUM_CATEGORIAS; cat++) {
		printf("\t\tCategoria: %s\n", categorias[cat]);
		imprimir_formato();
		i = indice->primeiros[cat];
		while (i != -1) {
			printf("[%2d]", p++);
			imprimir_book_data_posicao(lib, i);
			i = indice->chaves[i];
		}
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
		} else {
			j = indice->pri;
			while (j > -1 && strcmp(lib[i].nome, lib[j].nome) >= 0) {
				k = j;
				j = indice->chaves[j];
			}
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

void imprimir_todos_indice_alfab(book_data *lib, indice_alfab *indice) {
	int i = indice->pri;
	int p = 0;
	printf("TODOS OS TITULOS COM ORDENACAO SECUNDARIA\n");
	imprimir_formato();
	while (i != -1) {
		printf("[%2d]", p++);
		imprimir_book_data_posicao(lib, i);
		i = indice->chaves[i];
	}
}


void criar_indice_alfab_categorias(book_data *lib, char categorias[][STR_LENGTH], int n, indice_alfab *ordenado, indice_alfab_cat *indice) {
	int atual = ordenado->pri;
	int categoria_atual;
	
	int i;
	
	for (i = 0; i < NUM_CATEGORIAS; i++) {
		indice->primeiros[i] = -1;
	}
	
	for (i = 0; i < n; i++) {
		indice->chaves[i] = -1;
	}
	
	while (atual != -1) {
		categoria_atual = encontra_codigo_categoria(categorias, lib[atual].categ);
		
		if (indice->primeiros[categoria_atual] == -1) {
			indice->primeiros[categoria_atual] = atual;
		} else {
			i = indice->primeiros[categoria_atual];
			while (indice->chaves[i] != -1) i = indice->chaves[i];
			indice->chaves[i] = atual;
		}
		
		atual = ordenado->chaves[atual];
	}
}

void imprimir_indice_alfab_categorias(indice_alfab_cat *indice, int n) {
	int i;
	printf("INDICE ALFABETICO POR CATEGORIAS\n\tPRIs\n");
	for (i = 0; i < NUM_CATEGORIAS; i++) {
		printf("\t[%2d]\n", indice->primeiros[i]);
	}
	printf("\tCHAVES\n");
	for (i = 0; i < n; i++) {
		printf("\t%2d - [%2d]\n", i, indice->chaves[i]);
	}
}

void imprimir_todos_indice_alfab_categorias(book_data *lib, char categorias[][STR_LENGTH], indice_alfab_cat *indice) {
	int i, cat, pos = 0;
	printf("TODOS OS TITULOS COM ORDENACAO SECUNDARIA (POR CATEGORIAS)\n");
	for (cat = 0; cat < NUM_CATEGORIAS; cat++) {
		printf("\t\tCategoria: %s\n", categorias[cat]);
		imprimir_formato();
		i = indice->primeiros[cat];
		while (i != -1) {
			printf("[%2d]", pos++);
			imprimir_book_data_posicao(lib, i);
			i = indice->chaves[i];
		}
	}
}
