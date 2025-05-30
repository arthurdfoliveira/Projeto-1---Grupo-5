#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

const int BOTAO_A = 7;
const int BOTAO_B = 6;
const int BOTAO_C = 5;
const int RX_PIN = 10;
const int TX_PIN = 11;

SoftwareSerial mySoftwareSerial(RX_PIN, TX_PIN);
DFRobotDFPlayerMini myDFPlayer;

enum EstadoJogo {
  INICIO,
  SELECAO_MATERIA,
  SELECAO_DIFICULDADE,
  SEQUENCIA,
  RESPOSTA,
  PERGUNTA,
  FIM
};

EstadoJogo estadoAtual = INICIO;
int sequencia[10];
int posicaoAtual = 0;
int materiaSelecionada = 0;  // 1-Matemática, 2-Ciências, 3-Conhecimentos Gerais
int dificuldadeSelecionada = 0;  // 1-Fácil, 2-Médio, 3-Difícil
int perguntaAtual = 0;
int acertosConsecutivos = 0;

void setup() {
  pinMode(BOTAO_A, INPUT_PULLUP);
  pinMode(BOTAO_B, INPUT_PULLUP);
  pinMode(BOTAO_C, INPUT_PULLUP);

  Serial.begin(9600);
  mySoftwareSerial.begin(9600);

  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("Erro ao inicializar DFPlayer Mini!");
    while(true);
  }

  myDFPlayer.volume(20);
  myDFPlayer.play(1); // Boas-vindas
  delay(2000);
}

void loop() {
  switch (estadoAtual) {
    case INICIO:
      if (botaoPressionado()) {
        estadoAtual = SELECAO_MATERIA;
        tocarInstrucoesMateria();
      }
      break;

    case SELECAO_MATERIA:
      if (digitalRead(BOTAO_A) == LOW) {
        materiaSelecionada = 1;
        estadoAtual = SELECAO_DIFICULDADE;
        tocarInstrucoesDificuldade();
      } else if (digitalRead(BOTAO_B) == LOW) {
        materiaSelecionada = 2;
        estadoAtual = SELECAO_DIFICULDADE;
        tocarInstrucoesDificuldade();
      } else if (digitalRead(BOTAO_C) == LOW) {
        materiaSelecionada = 3;
        estadoAtual = SELECAO_DIFICULDADE;
        tocarInstrucoesDificuldade();
      }
      break;

    case SELECAO_DIFICULDADE:
      if (digitalRead(BOTAO_A) == LOW) {
        dificuldadeSelecionada = 1;
        estadoAtual = SEQUENCIA;
        perguntaAtual = 0;
        gerarSequencia();
      } else if (digitalRead(BOTAO_B) == LOW) {
        dificuldadeSelecionada = 2;
        estadoAtual = SEQUENCIA;
        perguntaAtual = 0;
        gerarSequencia();
      } else if (digitalRead(BOTAO_C) == LOW) {
        dificuldadeSelecionada = 3;
        estadoAtual = SEQUENCIA;
        perguntaAtual = 0;
        gerarSequencia();
      }
      break;

    case SEQUENCIA:
      reproduzirSequencia();
      estadoAtual = RESPOSTA;
      break;

    case RESPOSTA:
      if (verificarResposta()) {
        estadoAtual = PERGUNTA;
        tocarPergunta();
      }
      break;

    case PERGUNTA:
      if (verificarRespostaPergunta()) {
        acertosConsecutivos++;
        perguntaAtual++;

        if (perguntaAtual >= 10) {
          estadoAtual = FIM;
          tocarVitoria();
        } else {
          estadoAtual = SEQUENCIA;
          gerarSequencia();
        }
      } else {
        tocarErro();
        delay(2000);
        reiniciarJogo();
      }
      break;

    case FIM:
      if (botaoPressionado()) {
        reiniciarJogo();
      }
      break;
  }
}

bool botaoPressionado() {
  return digitalRead(BOTAO_A) == LOW || digitalRead(BOTAO_B) == LOW || digitalRead(BOTAO_C) == LOW;
}

void tocarInstrucoesMateria() {
  myDFPlayer.play(2); // Áudio com instruções para selecionar matéria
}

void tocarInstrucoesDificuldade() {
  myDFPlayer.play(3); // Áudio com instruções para selecionar dificuldade
}

void gerarSequencia() {
  int tamanho = 3 + (dificuldadeSelecionada - 1);
  for (int i = 0; i < tamanho; i++) {
    sequencia[i] = random(1, 4);
  }
  posicaoAtual = 0;
}

void reproduzirSequencia() {
  int tamanho = 3 + (dificuldadeSelecionada - 1);
  for (int i = 0; i < tamanho; i++) {
    myDFPlayer.play(10 + sequencia[i]); // Sons: 11, 12, 13
    delay(1000);
  }
}

bool verificarResposta() {
  int tamanho = 3 + (dificuldadeSelecionada - 1);
  while (posicaoAtual < tamanho) {
    int botao = esperarBotao();
    if (botao == sequencia[posicaoAtual]) {
      posicaoAtual++;
    } else {
      return false;
    }
  }
  return true;
}

int esperarBotao() {
  while (true) {
    if (digitalRead(BOTAO_A) == LOW) return 1;
    if (digitalRead(BOTAO_B) == LOW) return 2;
    if (digitalRead(BOTAO_C) == LOW) return 3;
    delay(50);
  }
}

void tocarPergunta() {
  int base = 40;  // Arquivo 40 em diante
  int index = (materiaSelecionada - 1) * 30 + (dificuldadeSelecionada - 1) * 10 + perguntaAtual;
  myDFPlayer.play(base + index);  // Ex: 40 a 129
}

bool verificarRespostaPergunta() {
  int respostaCorreta = 1; // Suporte futuro: leitura de gabarito por índice
  int resposta = esperarBotao();
  return resposta == respostaCorreta;
}

void tocarErro() {
  myDFPlayer.play(30);
}

void tocarVitoria() {
  myDFPlayer.play(31);
}

void reiniciarJogo() {
  estadoAtual = INICIO;
  materiaSelecionada = 0;
  dificuldadeSelecionada = 0;
  perguntaAtual = 0;
  acertosConsecutivos = 0;
  tocarInstrucoesMateria();
}