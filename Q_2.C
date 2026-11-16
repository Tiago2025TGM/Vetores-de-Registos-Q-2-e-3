#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_PRODUTOS 40
#define TAM_DESCRICAO 100

typedef struct {
    int codigo;
    char descricao[TAM_DESCRICAO];
    double valorUnitario;
    int quantidadeEstoque;
    int ativo;
} Produto;

static Produto produtos[TOTAL_PRODUTOS];

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

static int buscarProduto(int codigo)
{
    int i;

    for (i = 0; i < TOTAL_PRODUTOS; i++) {
        if (produtos[i].ativo && produtos[i].codigo == codigo) {
            return i;
        }
    }

    return -1;
}

static int solicitarCodigoExistente(void)
{
    int codigo;

    if (!lerInteiro("Codigo do produto: ", &codigo)) {
        return -1;
    }

    return buscarProduto(codigo);
}

static void cadastrarProduto(void)
{
    int i;
    int codigo;
    int estoque;
    double valor;
    char descricao[TAM_DESCRICAO];

    for (i = 0; i < TOTAL_PRODUTOS && produtos[i].ativo; i++) {
    }
    if (i == TOTAL_PRODUTOS) {
        printf("Limite de %d produtos atingido.\n", TOTAL_PRODUTOS);
        return;
    }

    if (!lerInteiro("Codigo: ", &codigo)) {
        return;
    }
    if (codigo <= 0) {
        printf("O codigo deve ser positivo.\n");
        return;
    }
    if (buscarProduto(codigo) != -1) {
        printf("Ja existe um produto com esse codigo.\n");
        return;
    }
    if (!lerLinha("Descricao: ", descricao, sizeof(descricao))) {
        return;
    }
    if (descricao[0] == '\0') {
        printf("A descricao nao pode ficar vazia.\n");
        return;
    }
    if (!lerValor("Valor unitario: ", &valor)) {
        return;
    }
    if (!lerInteiro("Quantidade em estoque: ", &estoque)) {
        return;
    }
    if (estoque < 0) {
        printf("A quantidade em estoque nao pode ser negativa.\n");
        return;
    }

    produtos[i].codigo = codigo;
    strcpy(produtos[i].descricao, descricao);
    produtos[i].valorUnitario = valor;
    produtos[i].quantidadeEstoque = estoque;
    produtos[i].ativo = 1;
    printf("Produto cadastrado com sucesso.\n");
}

static void alterarValorUnitario(void)
{
    int indice = solicitarCodigoExistente();
    double valor;

    if (indice == -1) {
        printf("Produto nao encontrado.\n");
        return;
    }
    if (!lerValor("Novo valor unitario: ", &valor)) {
        return;
    }

    produtos[indice].valorUnitario = valor;
    printf("Valor unitario atualizado.\n");
}

static double consultarValorUnitario(int codigo)
{
    int indice = buscarProduto(codigo);

    return indice == -1 ? -1.0 : produtos[indice].valorUnitario;
}

static int consultarQuantidadeEstoque(int codigo)
{
    int indice = buscarProduto(codigo);

    return indice == -1 ? -1 : produtos[indice].quantidadeEstoque;
}

static void consultarProduto(void)
{
    int indice = solicitarCodigoExistente();

    if (indice == -1) {
        printf("Produto nao encontrado.\n");
        return;
    }

    printf("Valor unitario: R$ %.2f\n",
           consultarValorUnitario(produtos[indice].codigo));
    printf("Quantidade em estoque: %d\n",
           consultarQuantidadeEstoque(produtos[indice].codigo));
}

static void realizarVenda(void)
{
    int indice = solicitarCodigoExistente();
    int quantidade;
    int quantidadeVendida;
    char resposta[16];

    if (indice == -1) {
        printf("Produto nao encontrado.\n");
        return;
    }
    if (produtos[indice].quantidadeEstoque == 0) {
        printf("Produto sem estoque.\n");
        return;
    }
    if (!lerInteiro("Quantidade desejada: ", &quantidade)) {
        return;
    }
    if (quantidade <= 0) {
        printf("A quantidade deve ser positiva.\n");
        return;
    }

    if (quantidade > produtos[indice].quantidadeEstoque) {
        printf("Estoque insuficiente. Ha %d unidade(s) disponivel(is).\n",
               produtos[indice].quantidadeEstoque);
        if (!lerLinha("Deseja comprar as unidades disponiveis? (s/n): ",
                      resposta, sizeof(resposta))) {
            return;
        }
        if (tolower((unsigned char)resposta[0]) != 's') {
            printf("Venda cancelada.\n");
            return;
        }
        quantidadeVendida = produtos[indice].quantidadeEstoque;
    } else {
        quantidadeVendida = quantidade;
    }

    produtos[indice].quantidadeEstoque -= quantidadeVendida;
    printf("Venda realizada. Valor a pagar: R$ %.2f\n",
           quantidadeVendida * produtos[indice].valorUnitario);
}

static void atualizarEstoque(void)
{
    int indice = solicitarCodigoExistente();
    int quantidade;

    if (indice == -1) {
        printf("Produto nao encontrado.\n");
        return;
    }
    if (!lerInteiro("Nova quantidade em estoque: ", &quantidade)) {
        return;
    }
    if (quantidade < 0) {
        printf("A quantidade em estoque nao pode ser negativa.\n");
        return;
    }

    produtos[indice].quantidadeEstoque = quantidade;
    printf("Estoque atualizado.\n");
}

static void listarProdutos(void)
{
    int i;
    int encontrou = 0;

    printf("\nProdutos cadastrados:\n");
    for (i = 0; i < TOTAL_PRODUTOS; i++) {
        if (produtos[i].ativo) {
            printf("Codigo: %d | Descricao: %s\n",
                   produtos[i].codigo, produtos[i].descricao);
            encontrou = 1;
        }
    }
    if (!encontrou) {
        printf("Nenhum produto cadastrado.\n");
    }
}

static void listarProdutosSemEstoque(void)
{
    int i;
    int encontrou = 0;

    printf("\nProdutos com estoque zero:\n");
    for (i = 0; i < TOTAL_PRODUTOS; i++) {
        if (produtos[i].ativo && produtos[i].quantidadeEstoque == 0) {
            printf("Codigo: %d | Descricao: %s\n",
                   produtos[i].codigo, produtos[i].descricao);
            encontrou = 1;
        }
    }
    if (!encontrou) {
        printf("Nenhum produto com estoque zero.\n");
    }
}

int main(void)
{
    int opcao;

    do {
        printf("\n--- Papelaria Escolar: cadastro de produtos ---\n");
        printf("1. Cadastrar produto\n");
        printf("2. Alterar valor unitario\n");
        printf("3. Consultar valor e estoque\n");
        printf("4. Realizar venda\n");
        printf("5. Atualizar estoque\n");
        printf("6. Listar todos os produtos\n");
        printf("7. Listar produtos sem estoque\n");
        printf("0. Sair\n");

        if (!lerInteiro("Opcao: ", &opcao)) {
            break;
        }

        switch (opcao) {
        case 1:
            cadastrarProduto();
            break;
        case 2:
            alterarValorUnitario();
            break;
        case 3:
            consultarProduto();
            break;
        case 4:
            realizarVenda();
            break;
        case 5:
            atualizarEstoque();
            break;
        case 6:
            listarProdutos();
            break;
        case 7:
            listarProdutosSemEstoque();
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