#include "keyboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>

#define LARGURA_TELA 40
#define ALTURA_TELA 20
#define NUM_BLOCOS_LINHA 10
#define NUM_BLOCOS_COLUNA 2 // Duas fileiras de blocos
#define NUM_BLOCOS (NUM_BLOCOS_LINHA * NUM_BLOCOS_COLUNA)
#define NUM_BLOCOS_FACIL 10   // Menos blocos para facilitar
#define NUM_BLOCOS_MEDIO 16   // Mais blocos para dificultar
#define NUM_BLOCOS_DIFICIL 20 // Ainda mais blocos para maior dificuldade

struct Paleta {
  int x, y;
};

struct Bola {
  int x, y;
  int dx, dy;
};

struct Jogador {
  char nome[50];
  int pontuacao;
};

struct Bloco {
  int x, y;
  int quebrado;
};

void inicializarBlocos(struct Bloco blocos[], int quantidadeTijolos) {
  for (int i = 0; i < NUM_BLOCOS; i++) {
    blocos[i].quebrado = 0;
  }

  // Defina o número de fileiras de tijolos com base na quantidade desejada
  int numFileiras = quantidadeTijolos / NUM_BLOCOS_LINHA;
  int larguraBloco = LARGURA_TELA / NUM_BLOCOS_LINHA;
  int alturaBloco = 1;
  int espacamentoHorizontal = larguraBloco;
  int espacamentoVertical = 2;

  for (int i = 0; i < numFileiras; i++) {
    for (int j = 0; j < NUM_BLOCOS_LINHA; j++) {
      int indiceBloco = i * NUM_BLOCOS_LINHA + j;

      blocos[indiceBloco].x = j * larguraBloco + espacamentoHorizontal;
      blocos[indiceBloco].y = i * espacamentoVertical;
      blocos[indiceBloco].quebrado = 0;
    }
  }
}
void limparTela() {
  printf("\033[H\033[J"); // Código ANSI para limpar a tela
}
void desenhar(struct Paleta *paleta, struct Bola *bola, struct Bloco blocos[]) {
  limparTela();

  // Calcular a largura de um bloco com base no tamanho da tela e no número de
  // blocos por linha
  int larguraBloco = LARGURA_TELA / NUM_BLOCOS_LINHA;

  for (int i = 0; i < ALTURA_TELA; i++) {
    for (int j = 0; j < LARGURA_TELA; j++) {
      if (i == paleta->y && j >= paleta->x && j < paleta->x + 5) {
        printf("-");
      } else if (i == bola->y && j == bola->x) {
        printf("O");
      } else {
        // Calcule as coordenadas do bloco atual
        int colunaBloco = j / larguraBloco;
        int linhaBloco = i / 2;
        int indiceBloco = linhaBloco * NUM_BLOCOS_LINHA + colunaBloco;

        // Verifique se estamos dentro dos limites dos blocos
        if (colunaBloco >= 0 && colunaBloco < NUM_BLOCOS_LINHA &&
            linhaBloco >= 0 && linhaBloco < NUM_BLOCOS_COLUNA) {
          if (!blocos[indiceBloco].quebrado) {
            printf("#"); // Tijolo não quebrado
          } else {
            printf(" "); // Espaço vazio onde o tijolo foi quebrado
          }
          j += larguraBloco - 1; // Pule as células ocupadas pelos blocos
        } else {
          printf(" "); // Espaço vazio fora dos blocos
        }
      }
    }
    printf("\n");
  }
}
int jogarJogo(int velocidade, int quantidadeTijolos) {
  struct Paleta paleta = {LARGURA_TELA / 2 - 2,
                          ALTURA_TELA -
                              2}; // A paleta agora fica uma linha acima
  struct Bola bola = {LARGURA_TELA / 2, ALTURA_TELA / 2, 1, -1};
  struct Bloco blocos[NUM_BLOCOS];

  inicializarBlocos(blocos, quantidadeTijolos);
  keyboardInit(); // Inicializa o teclado para captura não bloqueante

  int blocosQuebrados = 0;
  int jogoAtivo = 1; // Variável para controlar se o jogo está ativo

  while (jogoAtivo) {
    if (keyhit()) {
      char tecla = readch();
      if ((tecla == 'a' || tecla == 'A') && paleta.x > 0) {
        paleta.x--;
      }
      if ((tecla == 'd' || tecla == 'D') && paleta.x < LARGURA_TELA - 5) {
        paleta.x++;
      }
    }
    bola.x += bola.dx;
    bola.y += bola.dy;

    if (bola.x <= 0 || bola.x >= LARGURA_TELA - 1) {
      bola.dx = -bola.dx;
    }

    if (bola.y <= 0) {
      bola.dy = -bola.dy;
    }

    for (int i = 0; i < NUM_BLOCOS; i++) {
      if (!blocos[i].quebrado && bola.y == blocos[i].y &&
          bola.x >= blocos[i].x && bola.x <= blocos[i].x + 1) {
        blocos[i].quebrado = 1;
        bola.dy = -bola.dy;
        blocosQuebrados++;
      }
    }

    if (bola.y == paleta.y && bola.x >= paleta.x && bola.x <= paleta.x + 4) {
      bola.dy = -bola.dy;
    }

    desenhar(&paleta, &bola, blocos);

    if (bola.y >= ALTURA_TELA - 1) {
      // A bola tocou no chão
      jogoAtivo = 0; // O jogo não está mais ativo
    }

    usleep(velocidade);
  }

  keyboardDestroy(); // Restaura as configurações do teclado
  return blocosQuebrados;
}

void exibirMenu() {
  limparTela();
  printf("===================================\n");
  printf("           BREAKOUT GAME\n");
  printf("===================================\n");
  printf("1. Iniciar Jogo\n");
  printf("2. Ver Ranking\n");
  printf("3. Sair\n");
  printf("===================================\n");
  printf("Escolha uma opcao: ");
  fflush(stdout); // Garante que a saída do printf seja enviada imediatamente
                  // para o terminal
}

int obterEscolhaMenu() {
  keyboardInit();
  int escolha = 0;
  while (!escolha) {
    if (keyhit()) {
      char tecla = readch();
      if (tecla >= '1' && tecla <= '3') {
        escolha = tecla - '0'; // Converte o caractere numérico para o número
                               // inteiro correspondente
      }
    }
    usleep(10000); // Aguarda um pouco para não sobrecarregar a CPU
  }
  keyboardDestroy();
  return escolha;
}

void exibirMenuModoJogo(int *escolhaModo) {
  printf("\nEscolha o modo de jogo:\n");
  printf("1. Fácil\n");
  printf("2. Médio\n");
  printf("3. Dificil\n");
  printf("===================================\n");
  printf("Escolha uma opcao: ");
  fflush(stdout); // Garante que a saída do printf seja enviada imediatamente
                  // para o terminal

  *escolhaModo = obterEscolhaMenu();
}

void digitarNome(char nome[]) {
  printf("Digite o seu nome: ");
  fflush(stdout);

  int i = 0;
  while (1) {
    if (keyhit()) {
      char tecla = readch();
      if (tecla == '\n') {
        break; // Se o jogador pressionar Enter, encerra a digitação
      } else if (tecla == 127) {
        // Se o jogador pressionar Backspace, apaga o último caractere
        if (i > 0) {
          i--;
          nome[i] = '\0';
          printf("\b \b"); // Apaga o caractere digitado no terminal
          fflush(stdout);
        }
      } else if (i < 49) { // Evita estouro de buffer
        nome[i] = tecla;
        i++;
        nome[i] = '\0';      // Garante que a string seja terminada corretamente
        printf("%c", tecla); // Exibe o caractere digitado no terminal
        fflush(stdout);
      }
    }
    usleep(10000); // Aguarda um pouco para não sobrecarregar a CPU
  }
}

int main() {
  srand(time(NULL)); // Inicialize a semente do gerador de números aleatórios

  struct Jogador *jogadores = malloc(50 * sizeof(struct Jogador));
  if (jogadores == NULL) {
    fprintf(stderr, "Erro ao alocar memória.\n");
    return 1;
  }
  int numJogadores = 0;

  keyboardInit(); // Inicializa o teclado para captura não bloqueante

  while (1) {
    exibirMenu();

    int escolha = obterEscolhaMenu(); // Usa a função obterEscolhaMenu para
                                      // obter a escolha do usuário

    switch (escolha) {
    case 1: {
      int escolhaModo;
      exibirMenuModoJogo(&escolhaModo);

      int quantidadeTijolos;
      int velocidade;
      switch (escolhaModo) {
      case 1:
        quantidadeTijolos = NUM_BLOCOS_FACIL;
        velocidade = 200000; // 200ms
        break;
      case 2:
        quantidadeTijolos = NUM_BLOCOS_MEDIO;
        velocidade = 170000; // 170 ms
        break;
      case 3:
        quantidadeTijolos = NUM_BLOCOS_DIFICIL;
        velocidade = 150000; // 150 ms
        break;
      default:
        printf("Escolha inválida. Modo Fácil selecionado por padrão.\n");
        quantidadeTijolos = NUM_BLOCOS_FACIL;
        velocidade = 50000; // 50 ms
        break;
      }

      char nome[50];
      digitarNome(nome);

      limparTela();
      printf("Preparando o jogo...");
      fflush(stdout); // Força a saída do buffer
      sleep(3);       // Aguarde 3 segundos antes de começar o jogo

      strcpy(jogadores[numJogadores].nome,
             nome); // Copia o nome digitado para a estrutura
      jogadores[numJogadores].pontuacao =
          jogarJogo(velocidade, quantidadeTijolos);
      numJogadores++;

      break;
    }
    case 2: {
      // Ordenar jogadores com base na pontuação (do maior para o menor)
      for (int i = 0; i < numJogadores - 1; i++) {
        for (int j = i + 1; j < numJogadores; j++) {
          if (jogadores[i].pontuacao < jogadores[j].pontuacao) {
            struct Jogador temp = jogadores[i];
            jogadores[i] = jogadores[j];
            jogadores[j] = temp;
          }
        }
      }

      printf("\nRanking dos Jogadores:\n");
      for (int i = 0; i < numJogadores; i++) {
        printf("%d. %s: %d\n", i + 1, jogadores[i].nome,
               jogadores[i].pontuacao);
      }

      printf("\nClique Enter para voltar ao menu principal...");
      fflush(stdout); // Força a saída do buffer
      while (1) {
        if (keyhit()) {
          char tecla = readch();
          if (tecla == '\n') {
            break; // Sai do loop quando o usuário pressiona Enter
          }
        }
      }
      break;
    }
    case 3:
      printf("\nJogo finalizado. Obrigado por jogar!\n");
      free(jogadores);   // Libera a memória alocada dinamicamente
      keyboardDestroy(); // Restaura as configurações do teclado
      exit(0);           // Sai do programa
    }
  }
  keyboardDestroy(); // Nunca será alcançado se o loop for infinito
  free(jogadores);   // Nunca será alcançado se o loop for infinito
  return 0;          // Nunca será alcançado se o loop for infinito
}