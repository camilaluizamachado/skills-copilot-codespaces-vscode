#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

typedef struct {
    char placa[8];
    char motivo[100];
    char dataApreensao[11];
    char local[100];
} CarroApreendido;

typedef struct {
    CarroApreendido *data;
    int size;
    int capacity;
} CarrosApreendidos;

void initCarrosApreendidos(CarrosApreendidos *carros) {
    carros->capacity = 10;
    carros->size = 0;
    carros->data = malloc(carros->capacity * sizeof(CarroApreendido));
    if (carros->data == NULL) {
        perror("Falha ao alocar memória inicial");
        exit(EXIT_FAILURE);
    }
}

void expandirCarros(CarrosApreendidos *carros) {
    int novaCapacidade = carros->capacity * 2;
    CarroApreendido *temp = realloc(carros->data, novaCapacidade * sizeof(CarroApreendido));
    if (temp == NULL) {
        perror("Falha ao realocar memória");
        free(carros->data);
        exit(EXIT_FAILURE);
    }
    carros->data = temp;
    carros->capacity = novaCapacidade;
}

void adicionarCarro(CarrosApreendidos *carros, CarroApreendido novo) {
    if (carros->size == carros->capacity) {
        expandirCarros(carros);
    }
    carros->data[carros->size] = novo;
    carros->size++;
}

int validarPlaca(const char *placa) {
    if (strlen(placa) != 7) return 0;
    for (int i = 0; i < 7; i++) {
        if (!isalnum(placa[i])) return 0;
    }
    return 1;
}

int validarData(const char *data) {
    if (strlen(data) != 10 || data[2] != '/' || data[5] != '/') return 0;

    int dia, mes, ano;
    if (sscanf(data, "%2d/%2d/%4d", &dia, &mes, &ano) != 3) return 0;

    if (ano < 1900 || ano > 2100) return 0;
    if (mes < 1 || mes > 12) return 0;

    int diasPorMes[] = { 31, (ano % 4 == 0 && (ano % 100 != 0 || ano % 400 == 0)) ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (dia < 1 || dia > diasPorMes[mes - 1]) return 0;

    return 1;
}

CarroApreendido criarCarroApreendido() {
    CarroApreendido carro;
    do {
        printf("Digite a placa (7 caracteres): ");
        fgets(carro.placa, sizeof(carro.placa), stdin);
        strtok(carro.placa, "\n");
    } while (!validarPlaca(carro.placa));
    printf("Digite o motivo: ");
    fgets(carro.motivo, sizeof(carro.motivo), stdin);
    strtok(carro.motivo, "\n");
    do {
        printf("Digite a data (DD/MM/AAAA): ");
        fgets(carro.dataApreensao, sizeof(carro.dataApreensao), stdin);
        strtok(carro.dataApreensao, "\n");
    } while (!validarData(carro.dataApreensao));
    printf("Digite o local: ");
    fgets(carro.local, sizeof(carro.local), stdin);
    strtok(carro.local, "\n");
    return carro;
}

void imprimirCarro(const CarroApreendido *carro) {
    printf("Placa: %s\n", carro->placa);
    printf("Motivo: %s\n", carro->motivo);
    printf("Data Apreensão: %s\n", carro->dataApreensao);
    printf("Local: %s\n", carro->local);
}

void buscarPorPlaca(CarrosApreendidos *carros) {
    char placa[8];
    printf("Digite a placa para buscar: ");
    fgets(placa, sizeof(placa), stdin);
    strtok(placa, "\n");
    int encontrado = 0;
    for (int i = 0; i < carros->size; i++) {
        if (strcmp(carros->data[i].placa, placa) == 0) {
            printf("\nCarro encontrado:\n");
            imprimirCarro(&carros->data[i]);
            encontrado = 1;
            break;
        }
    }
    if (!encontrado) {
        printf("Carro com placa %s não encontrado.\n", placa);
    }
}

void removerCarro(CarrosApreendidos *carros) {
    char placa[8];
    printf("Digite a placa do carro a ser removido: ");
    fgets(placa, sizeof(placa), stdin);
    strtok(placa, "\n");
    int encontrado = -1;
    for (int i = 0; i < carros->size; i++) {
        if (strcmp(carros->data[i].placa, placa) == 0) {
            encontrado = i;
            break;
        }
    }
    if (encontrado == -1) {
        printf("Carro com placa %s não encontrado.\n", placa);
        return;
    }
    for (int i = encontrado; i < carros->size - 1; i++) {
        carros->data[i] = carros->data[i + 1];
    }
    carros->size--;
    printf("Carro removido com sucesso.\n");
}

void salvarEmArquivo(CarrosApreendidos *carros, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "wb");
    if (arquivo == NULL) {
        perror("Erro ao abrir arquivo para salvar");
        return;
    }
    fwrite(carros->data, sizeof(CarroApreendido), carros->size, arquivo);
    fclose(arquivo);
    printf("Dados salvos no arquivo %s.\n", nomeArquivo);
}

void carregarDeArquivo(CarrosApreendidos *carros, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL) {
        printf("Nenhum arquivo encontrado. Iniciando com lista vazia.\n");
        return;
    }
    while (1) {
        CarroApreendido carro;
        size_t lido = fread(&carro, sizeof(CarroApreendido), 1, arquivo);
        if (lido == 0) break;
        adicionarCarro(carros, carro);
    }
    fclose(arquivo);
    printf("Dados carregados do arquivo %s.\n", nomeArquivo);
}

int main() {
    CarrosApreendidos carros;
    initCarrosApreendidos(&carros);
    carregarDeArquivo(&carros, "carros.dat");
    char opcao;
    do {
        printf("\n1. Adicionar carro\n2. Buscar por placa\n3. Remover carro\n4. Exibir todos\n5. Salvar e sair\nEscolha: ");
        opcao = getchar();
        while (getchar() != '\n'); // Limpa buffer
        switch (opcao) {
            case '1': adicionarCarro(&carros, criarCarroApreendido()); break;
            case '2': buscarPorPlaca(&carros); break;
            case '3': removerCarro(&carros); break;
            case '4': for (int i = 0; i < carros.size; i++) {
                printf("\nCarro %d:\n", i + 1);
                imprimirCarro(&carros.data[i]);
            } break;
            case '5': salvarEmArquivo(&carros, "carros.dat"); break;
            default: printf("Opção inválida.\n");
        }
    } while (opcao != '5');
    free(carros.data);
    return 0;
}
