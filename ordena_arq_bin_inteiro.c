#include <stdio.h>
#include <stdlib.h>

#define TAM_DE_NOME 30

int selecaoNatural(char *path, int tam, int tam_reservatorio, int *registros_totais);
int selecaoComSubstituicao(char *path, int tam, int *registros_totais);
int intercalacaoOtima(int total_particoes, int F, int registros_totais);

void printReservatorio(FILE *r);

int main(int argc, char *argv[]) {
	int total_particoes = 0, total_registros = 0;
	//total_particoes = selecaoComSubstituicao(argv[1], 10, &total_registros);
	total_particoes = selecaoNatural(argv[1], 10, 10, &total_registros);

	printf("Total de particoes == %d\n", total_particoes);

	intercalacaoOtima(total_particoes, 10, total_registros);

	return 0;
}

int selecaoNatural(char *path, int tam, int tam_reservatorio, int *registros_totais) {
	int total_particoes = 0, registros_lidos = 0, registros_escritos = 0;
	int M[tam];
	int i;
	int tam_reservatorio_atual;
	int tam_virtual;

	char nome_particao[TAM_DE_NOME];

	FILE *arq = fopen(path, "rb");
	FILE *part = NULL;
	FILE *reservatorio = fopen("reservatorio.dat", "wb+");

	fseek(arq, 0L, SEEK_END);
	*registros_totais = ftell(arq)/sizeof(int);
	fseek(arq, 0L, SEEK_SET);

	for(i = 0; i < tam; i++) {
		fread(&M[i], sizeof(int), 1, arq);
		registros_lidos++;
	}

	while(registros_lidos != *registros_totais) {
		total_particoes++;
		sprintf(nome_particao, "part%d.dat", total_particoes);
		part = fopen(nome_particao, "wb");

		tam_virtual = tam;

		int menor;
		int pos;

		tam_reservatorio_atual = 0;

		while(tam_reservatorio_atual != tam_reservatorio && registros_lidos != *registros_totais) {
			menor = M[0];
			pos = 0;

			for(i = 1; i < tam; i++) {
				if(M[i] < menor) {
					menor = M[i];
					pos = i;
				}
			}

			printf("\nMenor = %d\n", menor);

			fwrite(&M[pos], 1, sizeof(int), part);
			registros_escritos++;
			fread(&M[pos], sizeof(int), 1, arq);
			registros_lidos++;

			printf("Novo = %d\n", M[pos]);

			if(M[pos] < menor) {
				while(M[pos] < menor && registros_lidos != *registros_totais) {
					fwrite(&M[pos], 1, sizeof(int), reservatorio);
					tam_reservatorio_atual++;
					printf("\n%d vai pro reservatorio. Tam atual == %d\n", M[pos], tam_reservatorio_atual);
					if(tam_reservatorio_atual == tam_reservatorio) {
						printf("\n\n");
						break;
					}
					fread(&M[pos], sizeof(int), 1, arq);
					printf("Novo = %d\n", M[pos]);
					registros_lidos++;
				}
				if(registros_lidos == *registros_totais) {
					tam_reservatorio_atual++;
					fwrite(&M[pos], 1, sizeof(int), reservatorio);
				}
			}
		}

		if(pos != tam-1) {
			int temp = M[pos];
			M[pos] = M[tam-1];
			M[tam-1] = temp;
		}

		int tam_virtual = tam-1;

		int j;

		for(i = 0; i < tam-1; i++) {
			menor = M[0];
			pos = 0;
			
			for(j = 0; j < tam_virtual; j++) {
				if(M[j] < menor) {
					menor = M[j];
					pos = j;
				}
			}

			printf("\nMenor = %d\n", menor);

			fwrite(&M[pos], 1, sizeof(int), part);
			registros_escritos++;

			if(pos != tam_virtual-1) {
				int temp = M[pos];
				M[pos] = M[tam_virtual-1];
				M[tam_virtual-1] = temp;
			}
			tam_virtual--;

		}	

		fflush(reservatorio);
		fseek(reservatorio, 0L, SEEK_SET);
		printf("\n\n");

		for(i = 0; i < tam_reservatorio_atual; i++) {
			fread(&M[i], sizeof(int), 1, reservatorio);
			printf("%d voltou do reservatorio.\n", M[i]);
		}

		printf("\n");

		fseek(reservatorio, 0L, SEEK_SET);


		fclose(part);
	}

	fclose(reservatorio);

	if(registros_escritos != *registros_totais) {
		total_particoes++;
		sprintf(nome_particao, "part%d.dat", total_particoes);
		part = fopen(nome_particao, "wb");

		int menor;
		int pos;

		while(registros_escritos != *registros_totais) {
			menor = M[0];
			pos = 0;

			for(i = 0; i < tam_reservatorio_atual; i++) {
				if(M[i] < menor) {
					menor = M[i];
					pos = i;
				}
			}

			fwrite(&M[pos], 1, sizeof(int), part);
			registros_escritos++;

			if(pos != tam_reservatorio_atual-1) {
				int temp = M[tam_reservatorio_atual-1];
				M[tam_reservatorio_atual-1] = M[pos];
				M[pos] = temp;
			}
			tam_reservatorio_atual--;
		}
		fclose(part);
	}

	fclose(arq);
	return total_particoes;
}

int selecaoComSubstituicao(char *path, int tam, int *registros_totais) {
	int total_particoes = 0, registros_lidos = 0, registros_escritos = 0;
	int M[tam];
	int i;
	int tam_virtual, congelados;

	char nome_particao[TAM_DE_NOME];

	FILE *arq = fopen(path, "rb");
	FILE *part = NULL;

	fseek(arq, 0L, SEEK_END);
	*registros_totais = ftell(arq)/sizeof(int);
	fseek(arq, 0L, SEEK_SET);

	for(i = 0; i < tam; i++) {
		fread(&M[i], sizeof(int), 1, arq);
		registros_lidos++;
	}

	while(registros_lidos != *registros_totais) {
		total_particoes++;
		sprintf(nome_particao, "part%d.dat", total_particoes);
		part = fopen(nome_particao, "wb");

		tam_virtual = tam;
		congelados = 0;

		int menor;
		int pos;

		while(congelados < tam && registros_lidos != *registros_totais) {
			menor = M[0];
			pos = 0;

			for(i = 1; i < tam_virtual; i++) {
				if(M[i] < menor) {
					menor = M[i];
					pos = i;
				}
			}

			fwrite(&M[pos], 1, sizeof(int), part);
			registros_escritos++;
			fread(&M[pos], sizeof(int), 1, arq);
			registros_lidos++;

			if(M[pos] < menor) {
				if(pos != tam_virtual-1) {
					int temp = M[pos];
					M[pos] = M[tam_virtual-1];
					M[tam_virtual-1] = temp;
				}
				congelados++;
				tam_virtual--;
			}

		}

		if(congelados < tam) {
			int tam_virtual_temp = tam_virtual;
			int j;

			for(i = 0; i < tam_virtual; i++) {
				menor = M[0];
				pos = 0;

				for(j = 1; j < tam_virtual_temp; j++) {
					if(M[j] < menor) {
						menor = M[j];
						pos = j;
					}
				}

				fwrite(&M[pos], 1, sizeof(int), part);
				registros_escritos++;

				if(pos != tam_virtual_temp-1) {
					int temp = M[pos];
					M[pos] = M[tam_virtual_temp-1];
					M[tam_virtual_temp-1] = temp;
				}
				congelados++;
				tam_virtual_temp--;
			}
		}

		fclose(part);
	}


	if(registros_escritos != *registros_totais) {
		total_particoes++;
		sprintf(nome_particao, "part%d.dat", total_particoes);
		part = fopen(nome_particao, "wb");

		int menor;
		int pos;

		while(registros_escritos != *registros_totais) {
			menor = M[tam_virtual];
			pos = tam_virtual;

			for(i = tam_virtual+1; i < tam; i++) {
				if(M[i] < menor) {
					menor = M[i];
					pos = i;
				}
			}

			fwrite(&M[pos], 1, sizeof(int), part);
			registros_escritos++;

			if(pos != tam-1) {
				int temp = M[pos];
				M[pos] = M[tam-1];
				M[tam-1] = temp;
			}
			tam--;

		}
		fclose(part);
	}

	fclose(arq);
	return total_particoes;
}

int intercalacaoOtima(int total_particoes, int F, int registros_totais) {
	FILE **entrada = NULL;
	FILE *saida = NULL;

	int total_particoes_atual = total_particoes;
	int valores_de_entrada[F-1];
	int i, particao_atual = 0;
	int F_virtual;
	int registros_escritos;

	int run = 1;

	if(F < 3) {
		printf("O valor de F não é válido. Deve ser maior do que 2!\n");
		return -1;
	}

	entrada = (FILE **) malloc(sizeof(FILE*) * (F-1));

	while(run) {
		registros_escritos = 0;
		char nome_particao[TAM_DE_NOME];
		for(i = 0; i < F-1; i++) {
			particao_atual++;
			sprintf(nome_particao, "part%d.dat", particao_atual);
			entrada[i] = fopen(nome_particao, "rb");
			if(entrada[i] == NULL) {
				F = i+1;
				break;
			}
			fread(&valores_de_entrada[i], sizeof(int), 1, entrada[i]);
		}

		F_virtual = F;

		int continua = 1;
		int pos;
		int menor;
		int registros_escritos = 0;

		total_particoes_atual++;
		sprintf(nome_particao, "part%d.dat", total_particoes_atual);

		saida = fopen(nome_particao, "wb");

		while(continua) {
			menor = valores_de_entrada[0];
			pos = 0;
			for(i = 0; i < F_virtual-1; i++) {
				if(valores_de_entrada[i] < menor) {
					menor = valores_de_entrada[i];
					pos = i;

				}
			}

			fwrite(&valores_de_entrada[pos], 1, sizeof(int), saida);
			registros_escritos++;
			if(registros_escritos == registros_totais) {
				run = 0;
			}
			fread(&valores_de_entrada[pos], sizeof(int), 1, entrada[pos]);

			if(feof(entrada[pos])) {
				if(pos != F_virtual-2 && F_virtual-2 >= 0) {
					FILE *temp_file = entrada[pos];
					entrada[pos] = entrada[F_virtual-2];
					entrada[F_virtual-2] = temp_file;

					int temp_int = valores_de_entrada[pos];
					valores_de_entrada[pos] = valores_de_entrada[F_virtual-2];
					valores_de_entrada[F_virtual-2] = temp_int;
				}
				F_virtual--;
			}

			for(i = 0; i < F-1; i++) {
				if(!feof(entrada[i])) {
					continua = 1;
					break;
				}
				else {
					continua = 0;
				}
			}

		}

		for(i = 0; i < F-1; i++) {
			fclose(entrada[i]);
		}

		fclose(saida);

	}
	free(entrada);
	

	return 1;
}
