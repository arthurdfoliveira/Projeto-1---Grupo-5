
import serial
import time
import random
import os
import pygame

PORTA_SERIAL = 'COM3'  # Substitua pela porta correta
BAUD_RATE = 9600

botao_para_letra = {
    'Y': 'A',  # Amarelo
    'R': 'B',  # Vermelho
    'G': 'C',  # Verde
}

indice_para_letra = ['A', 'B', 'C']  # som1 = A, som2 = B, som3 = C

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

def aguardar_botao(ser):
    while True:
        if ser.in_waiting:
            entrada = ser.readline().decode().strip()
            if entrada in botao_para_letra:
                letra = botao_para_letra[entrada]
                print(f"Botão pressionado: {entrada} = {letra}")
                return letra

def escolher_com_botao(ser, lista, audio_instrucao):
    tocar(audio_instrucao)
    print("Aguardando botão para escolha...")
    while True:
        botao = aguardar_botao(ser)
        if botao == 'A':  # Amarelo
            return lista[0]
        elif botao == 'B':  # Vermelho
            return lista[1]
        elif botao == 'C':  # Verde
            return lista[2]

def jogo():
    ser = serial.Serial(PORTA_SERIAL, BAUD_RATE, timeout=1)
    time.sleep(2)

    materias = ["matematica", "ciencias", "geral"]
    categorias = ["iniciante", "intermediario", "avancado"]

    materia = escolher_com_botao(ser, materias, "inicio/escolha_materia.mp3")
    categoria = escolher_com_botao(ser, categorias, "inicio/escolha_categoria.mp3")

    pasta_base = f"materia/{materia}/{categoria}"
    sons_dir = os.path.join(pasta_base, "sons")
    perguntas_dir = os.path.join(pasta_base, "perguntas")
    respostas_dir = os.path.join(pasta_base, "respostas")

    if categoria in ["intermediario", "avancado"]:
        sons = sorted([os.path.join(sons_dir, f) for f in os.listdir(sons_dir) if f.endswith(".mp3")])

        print("Tocando sons de exemplo...")
        for som in sons:
            tocar(som)
            time.sleep(0.5)

        instrucoes_path = os.path.join(pasta_base, "instrucoes_sons.mp3")
        if os.path.exists(instrucoes_path):
            tocar(instrucoes_path)

    perguntas = sorted([f for f in os.listdir(perguntas_dir) if f.endswith(".mp3")])

    print("Aguardando botão para iniciar o jogo...")
    aguardar_botao(ser)

    for nivel in range(10):
        print(f"\n🔊 Nível {nivel + 1}")

        if categoria in ["intermediario", "avancado"]:
            qtd_sons = 3 if categoria == "intermediario" else 5
            indices = [random.randint(0, 2) for _ in range(qtd_sons)]

            print("Tocando sequência...")
            for i in indices:
                tocar(sons[i])
                time.sleep(0.5)

            print("Repita a sequência usando os botões.")
            for i_esperado in indices:
                letra_esperada = indice_para_letra[i_esperado]
                entrada = aguardar_botao(ser)
                if entrada != letra_esperada:
                    tocar(os.path.join(respostas_dir, "incorreta.mp3"))
                    print("Sequência errada. Fim de jogo.")
                    return
            print("Sequência correta!")

        pergunta_audio = os.path.join(perguntas_dir, perguntas[nivel])
        tocar(pergunta_audio)

        resposta_jogador = aguardar_botao(ser)
        print(f"Resposta: {resposta_jogador}")

        with open(os.path.join(respostas_dir, f"pergunta{nivel + 1}.txt")) as f:
            resposta_correta = f.read().strip().upper()

        if resposta_jogador == resposta_correta:
            tocar(os.path.join(respostas_dir, "correta.mp3"))
        else:
            tocar(os.path.join(respostas_dir, "incorreta.mp3"))
            print("Resposta errada. Fim de jogo.")
            return

    print("\n🏆 Parabéns! Você completou todas as perguntas com sucesso!")

if __name__ == "__main__":
    jogo()
