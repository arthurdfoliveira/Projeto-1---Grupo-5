#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// ← CORREÇÃO AQUI: pinos onde seus botões estão realmente conectados
#define BOTAO_A 7
#define BOTAO_B 6
#define BOTAO_C 5

SoftwareSerial mySerial(10, 11); // RX, TX do DFPlayer
DFRobotDFPlayerMini player;

int estado = 0;
int materiaSelecionada = 0;
int dificuldadeSelecionada = 0;
int perguntaAtual = 0;

// Gabarito com 90 respostas (1=A, 2=B, 3=C)
int respostasCorretas[90] = {
  1,2,3,1,2,3,1,2,3,1,  2,3,1,2,3,1,2,3,1,2,  3,1,2,3,1,2,3,1,2,3,
  1,1,2,2,3,3,1,2,3,1,  2,3,1,2,3,1,2,3,1,2,  3,1,2,3,1,2,3,1,2,3,
  1,2,3,1,2,3,1,2,3,1,  2,3,1,2,3,1,2,3,1,2,  3,1,2,3,1,2,3,1,2,3
};

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  pinMode(BOTAO_A, INPUT_PULLUP);
  pinMode(BOTAO_B, INPUT_PULLUP);
  pinMode(BOTAO_C, INPUT_PULLUP);

  if (!player.begin(mySerial)) {
    Serial.println("Falha ao inicializar o DFPlayer");
    while (true);
  }
  player.setTimeOut(500);
  player.volume(20);

  Serial.println("Iniciando o jogo...");
  player.play(1); // 0001.mp3 - Boas-vindas
  delay(43000);
  estado = 1;
}

void loop() {
  switch (estado) {
    case 1: // instruções de matéria
      Serial.println("Tocando instruções para escolher a matéria...");
      player.play(2); // 0002.mp3
      delay(24000);
      estado = 2;
      break;

    case 2: // seleciona matéria
      Serial.println("Esperando seleção da matéria...");
      if (botaoPressionado(BOTAO_A))  escolhaMateria(1, 11, "Matemática");
      else if (botaoPressionado(BOTAO_B)) escolhaMateria(2, 12, "Ciências");
      else if (botaoPressionado(BOTAO_C)) escolhaMateria(3, 13, "Conhecimentos Gerais");
      break;

    case 3: // instruções de dificuldade
      Serial.println("Tocando instruções para escolher dificuldade...");
      player.play(3); // 0003.mp3
      delay(24000);
      estado = 4;
      break;

    case 4: // seleciona dificuldade
      Serial.println("Esperando seleção da dificuldade...");
      if (botaoPressionado(BOTAO_A))  escolhaDificuldade(1, 11, "Fácil");
      else if (botaoPressionado(BOTAO_B)) escolhaDificuldade(2, 12, "Média");
      else if (botaoPressionado(BOTAO_C)) escolhaDificuldade(3, 13, "Difícil");
      break;

    case 5: // toca pergunta
      if (perguntaAtual < 10) {
        int idx = (materiaSelecionada - 1)*30 + (dificuldadeSelecionada - 1)*10 + perguntaAtual;
        int faixa = 40 + idx;
        Serial.print("Tocando pergunta ");
        Serial.print(perguntaAtual + 1);
        Serial.print(" (faixa ");
        Serial.print(faixa);
        Serial.println(")");
        player.play(faixa);
        delay(10000); // tempo para áudio da pergunta
        estado = 6;
      } else {
        Serial.println("Todas as perguntas concluídas!");
        player.play(31); // vitória
        delay(3000);
        estado = 7;
      }
      break;

    case 6: // espera resposta
      Serial.println("Esperando resposta...");
      if (botaoPressionado(BOTAO_A)) verificaResposta(1);
      else if (botaoPressionado(BOTAO_B)) verificaResposta(2);
      else if (botaoPressionado(BOTAO_C)) verificaResposta(3);
      break;

    case 7: // fim / reinício
      Serial.println("Jogo encerrado. Reiniciando...");
      perguntaAtual = 0;
      estado = 1;
      break;
  }
}

void escolhaMateria(int mat, int somBotao, const char* nome) {
  materiaSelecionada = mat;
  player.play(somBotao);
  delay(1000);
  Serial.print("Matéria escolhida: ");
  Serial.println(nome);
  estado = 3;
}

void escolhaDificuldade(int dif, int somBotao, const char* nome) {
  dificuldadeSelecionada = dif;
  player.play(somBotao);
  delay(1000);
  Serial.print("Dificuldade: ");
  Serial.println(nome);
  perguntaAtual = 0;
  estado = 5;
}

void verificaResposta(int resp) {
  int idx = (materiaSelecionada - 1)*30 + (dificuldadeSelecionada - 1)*10 + perguntaAtual;
  Serial.print("Resposta recebida: ");
  Serial.println(resp);
  Serial.print("Resposta correta: ");
  Serial.println(respostasCorretas[idx]);
  if (resp == respostasCorretas[idx]) {
    Serial.println("Resposta correta!");
    perguntaAtual++;
    estado = 5;
  } else {
    Serial.println("Resposta incorreta!");
    player.play(30); // erro
    delay(3000);
    estado = 6;
  }
}

// debounce + espera soltar
bool botaoPressionado(int pino) {
  if (digitalRead(pino) == LOW) {
    delay(20);
    if (digitalRead(pino) == LOW) {
      while (digitalRead(pino) == LOW);
      delay(20);
      return true;
    }
  }
  return false;
}