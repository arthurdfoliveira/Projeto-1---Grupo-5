#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Definição dos pinos dos botões
const int BOTAO_A = 2;  // Botão esquerdo
const int BOTAO_B = 3;  // Botão central
const int BOTAO_C = 4;  // Botão direito

// Definição dos pinos para o DFPlayer Mini
const int RX_PIN = 10;
const int TX_PIN = 11;

// Variáveis globais
SoftwareSerial mySoftwareSerial(RX_PIN, TX_PIN);
DFRobotDFPlayerMini myDFPlayer;

// Estados do jogo
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
int sequencia[10];  // Array para armazenar a sequência
int nivelAtual = 0;
int posicaoAtual = 0;
int materiaSelecionada = 0;  // 1-Matemática, 2-Ciências, 3-Conhecimentos Gerais
int dificuldadeSelecionada = 0;  // 1-Fácil, 2-Médio, 3-Difícil

void setup() {
  // Inicialização dos botões
  pinMode(BOTAO_A, INPUT_PULLUP);
  pinMode(BOTAO_B, INPUT_PULLUP);
  pinMode(BOTAO_C, INPUT_PULLUP);
  
  // Inicialização da comunicação serial
  Serial.begin(9600);
  mySoftwareSerial.begin(9600);
  
  // Inicialização do DFPlayer Mini com verificação
  Serial.println("Iniciando DFPlayer Mini...");
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("Erro ao inicializar DFPlayer Mini!");
    Serial.println("1. Verifique as conexões");
    Serial.println("2. Verifique se o cartão SD está inserido");
    Serial.println("3. Verifique se os arquivos estão numerados corretamente (0001.mp3, 0002.mp3, etc)");
    while(true);
  }
  
  Serial.println("DFPlayer Mini inicializado com sucesso!");
  myDFPlayer.volume(20);  // Volume inicial (0-30)
  
  // Teste inicial do áudio
  Serial.println("Testando áudio...");
  myDFPlayer.play(1);  // Arquivo 1 deve ser a mensagem de boas-vindas
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
        materiaSelecionada = 1;  // Matemática
        estadoAtual = SELECAO_DIFICULDADE;
        tocarInstrucoesDificuldade();
      }
      else if (digitalRead(BOTAO_B) == LOW) {
        materiaSelecionada = 2;  // Ciências
        estadoAtual = SELECAO_DIFICULDADE;
        tocarInstrucoesDificuldade();
      }
      else if (digitalRead(BOTAO_C) == LOW) {
        materiaSelecionada = 3;  // Conhecimentos Gerais
        estadoAtual = SELECAO_DIFICULDADE;
        tocarInstrucoesDificuldade();
      }
      break;
      
    case SELECAO_DIFICULDADE:
      if (digitalRead(BOTAO_A) == LOW) {
        dificuldadeSelecionada = 1;  // Fácil (5-7 anos)
        estadoAtual = SEQUENCIA;
        gerarSequencia();
      }
      else if (digitalRead(BOTAO_B) == LOW) {
        dificuldadeSelecionada = 2;  // Médio (8-10 anos)
        estadoAtual = SEQUENCIA;
        gerarSequencia();
      }
      else if (digitalRead(BOTAO_C) == LOW) {
        dificuldadeSelecionada = 3;  // Difícil (10-12 anos)
        estadoAtual = SEQUENCIA;
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
        nivelAtual++;
        if (nivelAtual >= 5) {  // Jogo completo após 5 níveis
          estadoAtual = FIM;
          tocarVitoria();
        } else {
          estadoAtual = SEQUENCIA;
          gerarSequencia();
        }
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
  return (digitalRead(BOTAO_A) == LOW || 
          digitalRead(BOTAO_B) == LOW || 
          digitalRead(BOTAO_C) == LOW);
}

void tocarInstrucoesMateria() {
  myDFPlayer.play(2);  // Arquivo 2 deve conter as instruções de seleção de matéria
}

void tocarInstrucoesDificuldade() {
  myDFPlayer.play(3);  // Arquivo 3 deve conter as instruções de seleção de dificuldade
}

void gerarSequencia() {
  for (int i = 0; i <= nivelAtual; i++) {
    sequencia[i] = random(1, 4);  // 1-A, 2-B, 3-C
  }
  posicaoAtual = 0;
}

void reproduzirSequencia() {
  for (int i = 0; i <= nivelAtual; i++) {
    myDFPlayer.play(10 + sequencia[i]);  // Arquivos 11, 12, 13 devem ser os sons dos botões
    delay(1000);
  }
}

bool verificarResposta() {
  int botaoPressionado = 0;
  
  while (botaoPressionado == 0) {
    if (digitalRead(BOTAO_A) == LOW) botaoPressionado = 1;
    else if (digitalRead(BOTAO_B) == LOW) botaoPressionado = 2;
    else if (digitalRead(BOTAO_C) == LOW) botaoPressionado = 3;
  }
  
  if (botaoPressionado == sequencia[posicaoAtual]) {
    posicaoAtual++;
    if (posicaoAtual > nivelAtual) {
      return true;
    }
  } else {
    tocarErro();
    delay(2000);
    reiniciarJogo();
  }
  return false;
}

void tocarPergunta() {
  // Tocar pergunta baseada na matéria e dificuldade selecionadas
  int arquivoPergunta = 20 + (materiaSelecionada - 1) * 3 + (dificuldadeSelecionada - 1);
  myDFPlayer.play(arquivoPergunta);
}

bool verificarRespostaPergunta() {
  int botaoPressionado = 0;
  
  while (botaoPressionado == 0) {
    if (digitalRead(BOTAO_A) == LOW) botaoPressionado = 1;
    else if (digitalRead(BOTAO_B) == LOW) botaoPressionado = 2;
    else if (digitalRead(BOTAO_C) == LOW) botaoPressionado = 3;
  }
  
  // Aqui você precisará implementar a lógica para verificar se a resposta está correta
  // baseado na pergunta atual e na resposta correta
  return true;  // Temporariamente retorna sempre verdadeiro
}

void tocarErro() {
  myDFPlayer.play(30);  // Arquivo 30 deve ser o som de erro
}

void tocarVitoria() {
  myDFPlayer.play(31);  // Arquivo 31 deve ser o som de vitória
}

void reiniciarJogo() {
  estadoAtual = INICIO;
  nivelAtual = 0;
  materiaSelecionada = 0;
  dificuldadeSelecionada = 0;
  tocarInstrucoesMateria();
} 