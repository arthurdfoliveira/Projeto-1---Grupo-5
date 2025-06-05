Bibliotecas utilizadas: pyserial, time, random, os, pygame

  
Código Phyton:

  
import serial
import time
import random
import os
import pygame

PORTA_SERIAL = 'COM3'
BAUD_RATE = 9600

botao_para_letra = {
    'Y': 'A', 'Yellow': 'A',
    'R': 'B', 'Red': 'B',
    'G': 'C', 'Green': 'C'
}

indice_para_letra = ['A', 'B', 'C']

pygame.init()
pygame.mixer.init()

def tocar(audio_path):
    if not os.path.exists(audio_path):
        print(f"[ERRO] Arquivo não encontrado: {audio_path}")
        return
    print(f"Tocando: {audio_path}")
    pygame.mixer.music.load(audio_path)
    pygame.mixer.music.play()
    while pygame.mixer.music.get_busy():
        pygame.time.Clock().tick(10)

def aguardar_botao_com_repeticao(ser, repetir_funcao=None):
    tempo_inicio = time.time()
    while True:
        if ser.in_waiting:
            entrada = ser.readline().decode().strip().capitalize()
            if entrada in botao_para_letra:
                letra = botao_para_letra[entrada]
                print(f"Botão pressionado: {entrada} = {letra}")
                return letra
        elif repetir_funcao and time.time() - tempo_inicio > 5:
            print("[INFO] Tempo excedido. Repetindo áudio...")
            repetir_funcao()
            tempo_inicio = time.time()

def escolher_com_botao(ser, lista, audio_instrucao):
    def repetir(): tocar(audio_instrucao)
    repetir()
    print("Aguardando botão para escolha...")
    while True:
        botao = aguardar_botao_com_repeticao(ser, repetir)
        if botao == 'A':
            return lista[0]
        elif botao == 'B':
            return lista[1]
        elif botao == 'C':
            return lista[2]

def jogo():
    ser = serial.Serial(PORTA_SERIAL, BAUD_RATE, timeout=1)
    time.sleep(2)

    tocar("inicio/introdutorio.mp3")

    materias = ["matematica", "ciencias", "geral"]
    categorias = ["iniciante", "intermediario", "avancado"]

    materia = escolher_com_botao(ser, materias, "inicio/escolha_materia.mp3")
    categoria = escolher_com_botao(ser, categorias, "inicio/escolha_categoria.mp3")

    pasta_base = f"materia/{materia}/{categoria}"
    sons_dir = os.path.join(pasta_base, "sons")
    instrucoes_dir = os.path.join(pasta_base, "instrucoes_sons")
    perguntas_dir = os.path.join(pasta_base, "perguntas")
    respostas_dir = os.path.join(pasta_base, "respostas")

    if categoria in ["intermediario", "avancado"]:
        instrucoes_com_sons = [
            ("amarelo_instrucoes_som.mp3", "audio1.mp3"),
            ("botao_vermelho.mp3", "audio2.mp3"),
            ("c_botao_verde.mp3", "audio3.mp3")
        ]

        for instrucao, som in instrucoes_com_sons:
            tocar(os.path.join(instrucoes_dir, instrucao))
            time.sleep(0.3)
            tocar(os.path.join(sons_dir, som))
            time.sleep(0.3)

    perguntas = sorted([f for f in os.listdir(perguntas_dir) if f.endswith(".mp3")])

    tocar("inicio/cliquebotao.mp3")
    aguardar_botao_com_repeticao(ser)

    for nivel in range(10):
        print(f"\n🔊 Nível {nivel + 1}")

        if categoria in ["intermediario", "avancado"]:
            qtd_sons = 3 if categoria == "intermediario" else 5
            sequencia = [random.randint(0, 2) for _ in range(qtd_sons)]

            def repetir_sequencia():
                for i in sequencia:
                    tocar(os.path.join(sons_dir, f"audio{i + 1}.mp3"))
                    time.sleep(0.4)

            print("Tocando sequência...")
            repetir_sequencia()

            print("Jogador deve repetir a sequência...")
            for esperado in sequencia:
                letra_esperada = indice_para_letra[esperado]
                letra_jogador = aguardar_botao_com_repeticao(ser, repetir_sequencia)
                print(f"Esperado: {letra_esperada}, Jogador: {letra_jogador}")
                if letra_jogador != letra_esperada:
                    tocar(os.path.join(respostas_dir, "incorreta.mp3"))
                    print("Sequência errada. Fim de jogo.")
                    return
            print("Sequência correta!")

        pergunta_audio = os.path.join(perguntas_dir, perguntas[nivel])

        def repetir_pergunta():
            tocar(pergunta_audio)

        repetir_pergunta()

        resposta_jogador = aguardar_botao_com_repeticao(ser, repetir_pergunta)
        print(f"Resposta: {resposta_jogador}")

        with open(os.path.join(respostas_dir, f"pergunta{nivel + 1}.txt")) as f:
            resposta_correta = f.read().strip().upper()

        if resposta_jogador == resposta_correta:
            tocar(os.path.join(respostas_dir, "correta.mp3"))
        else:
            tocar(os.path.join(respostas_dir, "incorreta.mp3"))
            print("Resposta errada. Fim de jogo.")
            return

    tocar(os.path.join(respostas_dir, "vitoria.mp3"))
    print("\n🏆 Parabéns! Você completou todas as perguntas com sucesso!")
    return

if _name_ == "_main_":
    jogo()




Código C++:

const int botaoAmarelo = 7;  
const int botaoVermelho = 6; 
const int botaoVerde = 5;    

bool estadoAnteriorAmarelo = HIGH;
bool estadoAnteriorVermelho = HIGH;
bool estadoAnteriorVerde = HIGH;

void setup() {
  Serial.begin(9600);

  pinMode(botaoAmarelo, INPUT_PULLUP);
  pinMode(botaoVermelho, INPUT_PULLUP);
  pinMode(botaoVerde, INPUT_PULLUP);
}

void loop() {
  bool atualAmarelo = digitalRead(botaoAmarelo);
  bool atualVermelho = digitalRead(botaoVermelho);
  bool atualVerde = digitalRead(botaoVerde);

  if (estadoAnteriorAmarelo == HIGH && atualAmarelo == LOW) {
    Serial.println("Y"); 
    delay(300);
  }

  if (estadoAnteriorVermelho == HIGH && atualVermelho == LOW) {
    Serial.println("R"); 
    delay(300);
  }

  if (estadoAnteriorVerde == HIGH && atualVerde == LOW) {
    Serial.println("G"); 
    delay(300);
  }

  estadoAnteriorAmarelo = atualAmarelo;
  estadoAnteriorVermelho = atualVermelho;
  estadoAnteriorVerde = atualVerde;
}
