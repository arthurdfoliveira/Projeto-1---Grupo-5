// Pinos atualizados
const int botaoAmarelo = 7;  // A = Y
const int botaoVermelho = 6; // B = R
const int botaoVerde = 5;    // C = G

// Estados anteriores
bool estadoAnteriorAmarelo = HIGH;
bool estadoAnteriorVermelho = HIGH;
bool estadoAnteriorVerde = HIGH;

void setup() {
  Serial.begin(9600);

  // Configura os botões com pull-up interno
  pinMode(botaoAmarelo, INPUT_PULLUP);
  pinMode(botaoVermelho, INPUT_PULLUP);
  pinMode(botaoVerde, INPUT_PULLUP);
}

void loop() {
  // Leitura atual de cada botão
  bool atualAmarelo = digitalRead(botaoAmarelo);
  bool atualVermelho = digitalRead(botaoVermelho);
  bool atualVerde = digitalRead(botaoVerde);

  // Detecta pressionamento do botão amarelo
  if (estadoAnteriorAmarelo == HIGH && atualAmarelo == LOW) {
    Serial.println("Y"); // Envia 'Y' para o Python
    delay(300); // Debounce
  }

  // Detecta pressionamento do botão vermelho
  if (estadoAnteriorVermelho == HIGH && atualVermelho == LOW) {
    Serial.println("R"); // Envia 'R' para o Python
    delay(300);
  }

  // Detecta pressionamento do botão verde
  if (estadoAnteriorVerde == HIGH && atualVerde == LOW) {
    Serial.println("G"); // Envia 'G' para o Python
    delay(300);
  }

  // Atualiza os estados
  estadoAnteriorAmarelo = atualAmarelo;
  estadoAnteriorVermelho = atualVermelho;
  estadoAnteriorVerde = atualVerde;
}
