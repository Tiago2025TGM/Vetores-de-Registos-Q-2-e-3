#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_CONTAS 100
#define TAM_NOME 100
#define TAM_CPF 16
#define TAM_TELEFONE 24

typedef struct {
    int numero;
    char nome[TAM_NOME];
    char cpf[TAM_CPF];
    char telefone[TAM_TELEFONE];
    double saldo;
} Conta;

static int lerLinha(const char *mensagem, char *linha, size_t tamanho)
{
    size_t comprimento;
    int caractere;

    printf("%s", mensagem);
    if (fgets(linha, (int)tamanho, stdin) == NULL) {
        return 0;
    }

    comprimento = strlen(linha);
    if (comprimento > 0 && linha[comprimento - 1] == '\n') {
        linha[comprimento - 1] = '\0';
    } else {
        while ((caractere = getchar()) != '\n' && caractere != EOF) {
        }
    }

    return 1;
}

static int lerInteiro(const char *mensagem, int *valor)
{
    char linha[128];
    char *fim;
    long numero;

    for (;;) {
        if (!lerLinha(mensagem, linha, sizeof(linha))) {
            return 0;
        }

        errno = 0;
        numero = strtol(linha, &fim, 10);
        while (isspace((unsigned char)*fim)) {
            fim++;
        }

        if (fim != linha && *fim == '\0' && errno != ERANGE &&
            numero >= -2147483647L - 1 && numero <= 2147483647L) {
            *valor = (int)numero;
            return 1;
        }

        printf("Entrada invalida. Digite um numero inteiro.\n");
    }
}

static int lerValor(const char *mensagem, double *valor)
{
    char linha[128];
    char *fim;
    double numero;

    for (;;) {
        if (!lerLinha(mensagem, linha, sizeof(linha))) {
            return 0;
        }

        errno = 0;
        numero = strtod(linha, &fim);
        while (isspace((unsigned char)*fim)) {
            fim++;
        }

        if (fim != linha && *fim == '\0' && errno != ERANGE &&
            numero >= 0.0 && numero <= 1.0e100) {
            *valor = numero;
            return 1;
        }

        printf("Valor invalido. Digite um numero maior ou igual a zero.\n");
    }
}

static int buscarConta(const Conta cadastro[], int quantidade, int numero)
{
    int i;

    if (quantidade == 0) {
        return -1;
    }

    for (i = 0; i < quantidade; i++) {
        if (cadastro[i].numero == numero) {
            return i;
        }
    }

    return -2;
}

static void cadastrarConta(Conta cadastro[], int *quantidade, int numero)
{
    Conta novaConta;

    if (buscarConta(cadastro, *quantidade, numero) >= 0) {
        printf("Esse numero de conta ja esta cadastrado.\n");
        return;
    }
    if (*quantidade == TOTAL_CONTAS) {
        printf("Limite de %d contas atingido.\n", TOTAL_CONTAS);
        return;
    }
    if (numero <= 0) {
        printf("O numero da conta deve ser positivo.\n");
        return;
    }

    novaConta.numero = numero;
    if (!lerLinha("Nome do titular: ", novaConta.nome, sizeof(novaConta.nome))) {
        return;
    }
    if (novaConta.nome[0] == '\0') {
        printf("O nome nao pode ficar vazio.\n");
        return;
    }
    if (!lerLinha("CPF: ", novaConta.cpf, sizeof(novaConta.cpf))) {
        return;
    }
    if (novaConta.cpf[0] == '\0') {
        printf("O CPF nao pode ficar vazio.\n");
        return;
    }
    if (!lerLinha("Telefone para contato: ", novaConta.telefone,
                  sizeof(novaConta.telefone))) {
        return;
    }
    if (novaConta.telefone[0] == '\0') {
        printf("O telefone nao pode ficar vazio.\n");
        return;
    }
    if (!lerValor("Saldo inicial: R$ ", &novaConta.saldo)) {
        return;
    }

    cadastro[*quantidade] = novaConta;
    (*quantidade)++;
    printf("Conta cadastrada com sucesso.\n");
}

static int solicitarNumeroConta(int *numero)
{
    return lerInteiro("Numero da conta: ", numero);
}

static void cadastrarContaInterativo(Conta cadastro[], int *quantidade)
{
    int numero;

    if (solicitarNumeroConta(&numero)) {
        cadastrarConta(cadastro, quantidade, numero);
    }
}

static void consultarSaldo(const Conta cadastro[], int quantidade, int numero)
{
    int posicao = buscarConta(cadastro, quantidade, numero);

    if (posicao < 0) {
        printf("A conta nao esta cadastrada.\n");
        return;
    }

    printf("Saldo da conta %d: R$ %.2f\n", numero, cadastro[posicao].saldo);
}

static void depositar(Conta cadastro[], int quantidade, int numero)
{
    int posicao = buscarConta(cadastro, quantidade, numero);
    double valor;

    if (posicao < 0) {
        printf("A conta nao esta cadastrada.\n");
        return;
    }
    if (!lerValor("Valor do deposito: R$ ", &valor)) {
        return;
    }
    if (valor <= 0.0 || cadastro[posicao].saldo > 1.0e100 - valor) {
        printf("Valor de deposito invalido.\n");
        return;
    }

    cadastro[posicao].saldo += valor;
    printf("Deposito realizado. Novo saldo: R$ %.2f\n",
           cadastro[posicao].saldo);
}

static void sacar(Conta cadastro[], int quantidade, int numero)
{
    int posicao = buscarConta(cadastro, quantidade, numero);
    double valor;

    if (posicao < 0) {
        printf("A conta nao esta cadastrada.\n");
        return;
    }
    if (!lerValor("Valor do saque: R$ ", &valor)) {
        return;
    }
    if (valor <= 0.0) {
        printf("O valor do saque deve ser positivo.\n");
        return;
    }
    if (cadastro[posicao].saldo < valor) {
        printf("Nao existe saldo suficiente.\n");
        return;
    }

    cadastro[posicao].saldo -= valor;
    printf("Saque realizado. Novo saldo: R$ %.2f\n",
           cadastro[posicao].saldo);
}

static void listarContas(const Conta cadastro[], int quantidade)
{
    int i;

    if (quantidade == 0) {
        printf("Nenhuma conta cadastrada.\n");
        return;
    }

    printf("\nContas cadastradas:\n");
    for (i = 0; i < quantidade; i++) {
        printf("Conta: %d | Titular: %s | Telefone: %s\n",
               cadastro[i].numero, cadastro[i].nome, cadastro[i].telefone);
    }
}

int main(void)
{
    Conta contasCorrentes[TOTAL_CONTAS];
    Conta contasPoupanca[TOTAL_CONTAS];
    int quantidadeCorrentes = 0;
    int quantidadePoupanca = 0;
    int opcao;
    int numero;

    do {
        printf("\n--- Banco Dinheiro Certo ---\n");
        printf("1. Cadastrar conta corrente\n");
        printf("2. Cadastrar conta poupanca\n");
        printf("3. Consultar saldo\n");
        printf("4. Fazer deposito\n");
        printf("5. Fazer saque\n");
        printf("6. Listar contas correntes\n");
        printf("7. Listar contas poupanca\n");
        printf("0. Sair\n");

        if (!lerInteiro("Opcao: ", &opcao)) {
            break;
        }

        switch (opcao) {
        case 1:
            cadastrarContaInterativo(contasCorrentes, &quantidadeCorrentes);
            break;
        case 2:
            cadastrarContaInterativo(contasPoupanca, &quantidadePoupanca);
            break;
        case 3:
        case 4:
        case 5: {
            int tipo;
            Conta *cadastro;
            int quantidade;

            printf("1. Conta corrente\n");
            printf("2. Conta poupanca\n");
            if (!lerInteiro("Tipo de conta: ", &tipo)) {
                return 0;
            }
            if (tipo == 1) {
                cadastro = contasCorrentes;
                quantidade = quantidadeCorrentes;
            } else if (tipo == 2) {
                cadastro = contasPoupanca;
                quantidade = quantidadePoupanca;
            } else {
                printf("Tipo de conta invalido.\n");
                break;
            }

            if (!solicitarNumeroConta(&numero)) {
                break;
            }
            if (opcao == 3) {
                consultarSaldo(cadastro, quantidade, numero);
            } else if (opcao == 4) {
                depositar(cadastro, quantidade, numero);
            } else {
                sacar(cadastro, quantidade, numero);
            }
            break;
        }
        case 6:
            listarContas(contasCorrentes, quantidadeCorrentes);
            break;
        case 7:
            listarContas(contasPoupanca, quantidadePoupanca);
            break;
        case 0:
            printf("Encerrando o programa.\n");
            break;
        default:
            printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    return 0;
}