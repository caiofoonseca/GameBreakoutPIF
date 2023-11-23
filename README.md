# Script

gcc -o breakout breakoutgamefinal.c Keyboard.c -lm

if [ $? -eq 0 ]; then
  ./breakout
else
  echo "Erro na compilação."
fi





# Linhas de comando

Assim que inicializar o código, o usuário se depara com a tela de menu, com o nome do nosso jogo e 3 opções para ele selecionar, dentre elas estão:

1. Iniciar jogo;
2. Ver Ranking;
3. Sair;

Ao pressionar o botão equivalente a opção, ele será levado à alguma tela no terminal. Ao clicar 1, ele terá a opção de selecionar a dificuldade do jogo, logo em seguida irá colocar seu nome e iniciará o jogo, onde pode mover a paleta para rebater na "bolinha" e quebrar os tijolos que estão na parte superior. Caso ele deixe a bolinha passar da paleta e encostar no chão, ele perde e será redirecionado para a página inicial. Ao clicar 2, ele verá o ranking de jogadores, seus nomes e as pontuações, do melhor (1°) para o pior.


## Observações 
O codigo esta funcionando, mas esta com um bug na hora de printar os 'tijolos' na tela, eles estao sendo impressos apenas no lado esquerdo.
