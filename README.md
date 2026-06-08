# Dark Orbit BR — Autonomia de Bordo (Edge) · Tinkercad

Sistema embarcado de **missão crítica** para o CubeSat-Rio-1 (NORAD 55001), do projeto **Dark Orbit BR**. Entrega da disciplina **Edge Computing & Computer Systems (ECCS)** · Global Solution 2026 · FIAP — Engenharia de Software.

Simulação em **Tinkercad Circuits** (Arduino Uno).

---

## 1. O problema da missão

O **Dark Orbit BR** prevê risco de colisão de satélites a partir do solo, especialmente na região da **Anomalia Magnética do Atlântico Sul (SAMA)**. Mas há um momento em que essa previsão não chega: o **blackout de comunicação**. Quando o CubeSat-Rio-1 passa por uma região sem contato com a estação terrestre, **não há operador, não há nuvem e não há segunda chance**. Se um detrito se aproxima nesse intervalo, esperar o comando da Terra pode significar a perda do satélite.

A resposta é **autonomia de bordo**: o satélite precisa **medir, decidir e atuar localmente**, sem depender de nada externo. Este módulo resolve um subsistema crítico — o **desvio autônomo de colisão** — e é o complemento embarcado da plataforma Dark Orbit BR (o solo prevê; o satélite reage sozinho quando está cego).

---

## 2. O que o sistema faz

1. **Mede** a distância de um objeto que se aproxima (sensor ultrassônico).
2. **Decide** o nível de risco com uma máquina de estados.
3. **Atua** sozinho: dispara a manobra de desvio (servo) quando o risco é crítico — se houver propelente.
4. **Informa** o estado pelo **display de bordo**, a única janela da tripulação durante o blackout.

### Máquina de decisão

| Estado | Condição | Resposta |
|---|---|---|
| **NOMINAL** | objeto a mais de 100 cm | LED verde fixo |
| **ALERTA** | objeto entre 50 e 100 cm | LED âmbar piscando |
| **CRÍTICO** | objeto a menos de 50 cm **e** propelente ≥ 15% | LED vermelho + **manobra de desvio automática** |
| **DEGRADADO** | objeto a menos de 50 cm **e** propelente < 15% | LED vermelho + âmbar: avisa que **não pode** manobrar |

O estado **DEGRADADO** é proposital: em vez de fingir que desviou, o sistema **admite que não pode agir** — comportamento essencial em missão crítica.

---

## 3. Componentes e ligação

| Componente | Função | Pino no Arduino |
|---|---|---|
| Arduino Uno | Microcontrolador de bordo | — |
| Sensor ultrassônico HC-SR04 | Detecta o detrito | TRIG = 9, ECHO = 10, VCC = 5V, GND = GND |
| Potenciômetro | Nível de propelente | pino central = A0; laterais = 5V e GND |
| Servo motor | Propulsor de desvio | sinal = 6; VCC = 5V; GND = GND |
| LED verde | Estado NOMINAL | 8 (+ resistor 220 Ω ao GND) |
| LED âmbar | Estado ALERTA | 13 (+ resistor 220 Ω ao GND) |
| LED vermelho | Estado CRÍTICO/falha | 7 (+ resistor 220 Ω ao GND) |
| LCD 16x2 (paralelo) | Telemetria local | ver tabela abaixo |

### Ligação do LCD 16x2 (modo 4 bits)

| Pino do LCD | Vai para | | Pino do LCD | Vai para |
|---|---|---|---|---|
| GND | trilha − (GND) | | DB0–DB3 | **vazio** |
| VCC | trilha + (5V) | | DB4 | Arduino **5** |
| V0 | trilha − (GND) | | DB5 | Arduino **4** |
| RS | Arduino **12** | | DB6 | Arduino **3** |
| RW | trilha − (GND) | | DB7 | Arduino **2** |
| E | Arduino **11** | | LED (+/−) | 5V / GND |

### Esquema visual

![Esquema de ligação](https://private-us-east-1.manuscdn.com/sessionFile/FLxIDz8YRIGRoSmVHKDibS/sandbox/WtRWPuBiP7iLZdayHqttyg-images_1780941713758_na1fn_L2hvbWUvdWJ1bnR1L2VjY3Nfd29yay9pbWFnZW5zL2NpcmN1aXRv.png?Policy=eyJTdGF0ZW1lbnQiOlt7IlJlc291cmNlIjoiaHR0cHM6Ly9wcml2YXRlLXVzLWVhc3QtMS5tYW51c2Nkbi5jb20vc2Vzc2lvbkZpbGUvRkx4SUR6OFlSSUdSb1NtVkhLRGliUy9zYW5kYm94L1d0UldQdUJpUDdpTFpkYXlIcXR0eWctaW1hZ2VzXzE3ODA5NDE3MTM3NThfbmExZm5fTDJodmJXVXZkV0oxYm5SMUwyVmpZM05mZDI5eWF5OXBiV0ZuWlc1ekwyTnBjbU4xYVhSdi5wbmciLCJDb25kaXRpb24iOnsiRGF0ZUxlc3NUaGFuIjp7IkFXUzpFcG9jaFRpbWUiOjE3OTg3NjE2MDB9fX1dfQ__&Key-Pair-Id=K2HSFNDJXOU9YS&Signature=EPrP8lRJYJC5hjh8-2Tf5L4UE3ggGWs4iR~FCUGYALiUrLFgomXeRX4nVq8ss-4LiQlJGpL5nWnQIyAiAs2CZcikPIQ62WyYc9UKc9L~ysC2CD34xlE2P7W5TxFWQ3DsFGETHbhoTMXfwsYRzQ9K-HuMldf-gHrZWWM67sMYewv~2k6EbzZyImgygpVganqojqZJ~Uu3oJuUkiRDBlUc0n~0w1y6LbJKEN~6HjTdWJGuOcTDcPCorC7s~MiNApVvEic7eRDjjJfBxcDXZyd8~5UehmaYHH9IyGZPV8FIiOLZX9sZPzGr7LZwi0GIhETLwtxON9ZjhG4M2nPjoT7Slg__)

---

## 4. O display de bordo (telemetria)

- **Linha de cima (sempre visível):** estado + distância. Ex.: `CRITICO  D:042`.
- **Linha de baixo (rotativa, a cada ~2,5 s):**
  1. `Propelente: 78%`
  2. `Acao: DESVIO+`
  3. `Offline   03:12` (tempo em blackout)

![Display em operação](https://private-us-east-1.manuscdn.com/sessionFile/FLxIDz8YRIGRoSmVHKDibS/sandbox/WtRWPuBiP7iLZdayHqttyg-images_1780941713758_na1fn_L2hvbWUvdWJ1bnR1L2VjY3Nfd29yay9pbWFnZW5zL2Rpc3BsYXk.png?Policy=eyJTdGF0ZW1lbnQiOlt7IlJlc291cmNlIjoiaHR0cHM6Ly9wcml2YXRlLXVzLWVhc3QtMS5tYW51c2Nkbi5jb20vc2Vzc2lvbkZpbGUvRkx4SUR6OFlSSUdSb1NtVkhLRGliUy9zYW5kYm94L1d0UldQdUJpUDdpTFpkYXlIcXR0eWctaW1hZ2VzXzE3ODA5NDE3MTM3NThfbmExZm5fTDJodmJXVXZkV0oxYm5SMUwyVmpZM05mZDI5eWF5OXBiV0ZuWlc1ekwyUnBjM0JzWVhrLnBuZyIsIkNvbmRpdGlvbiI6eyJEYXRlTGVzc1RoYW4iOnsiQVdTOkVwb2NoVGltZSI6MTc5ODc2MTYwMH19fV19&Key-Pair-Id=K2HSFNDJXOU9YS&Signature=GOhQzFzNRhpm59imAcV-CvJISv4uXuWoix73adBhyfN0blyibo7uWZUsbNZbCXz4pM8iJKmIJnAbt93n-N~3lTkQpQncl4BzPKeZ6jicoverfgLkaADEn2Eu6CAb~N4nQq1Smr~nQSCNgxn7TMcU5OUnZzev~MNeNAQ3R6LCGcYw~5kIE95DSOsMBO3QhloIggeXmPCH0TboOoqG-fPTsjDFMRaq2LNKRT3gX~fgy69wFtzuCwl~wH0CCZHHy0rgwkzDM7HhAAW9ZDa~0PWgeLyZ5F-XmjxmB0ULhZKLykUICuf0cvDltCQ6DrM4bHfvRdjCONnYbTlcIPZkt1-Vbg__)

---

## 5. Robustez (decisões de engenharia)

- **Temporização não-bloqueante:** todo o agendamento usa `millis()`. **Nenhum `delay()` no loop.**
- **Validação de leitura:** se o sensor devolve 0 ou valor fora da faixa (2–400 cm), a leitura é descartada e mantém-se o último valor válido. O sistema **nunca trava**.

---

## 6. Como rodar no Tinkercad

1. Entre no link do projeto fornecido na seção 8.
2. Clique em **"Simulate"** ou **"Copy and Tinker"**.
3. Clique em **Start Simulation**.
4. **Para testar:** clique no sensor ultrassônico e arraste o controle de distância. Gire o potenciômetro para simular falta de combustível.

---

## 7. Estrutura do repositório

```
projeto-edge-space/
├── README.md
├── integrantes.txt
├── codigos/
│   └── main.ino            # Código-fonte (Arduino)
├── imagens/
│   ├── circuito.png        # Print do circuito completo
│   └── display.png         # Print da telemetria no LCD
├── docs/
│   ├── Esquema de Ligação (Tinkercad).pdf
│   └── roteiro_video.md    # Roteiro para gravação
└── video/
    └── link_video.txt      # Link para o vídeo demonstrativo
```

---

## 8. Links de entrega

- **Projeto no Tinkercad:** [Link do Projeto](https://www.tinkercad.com/things/lRdVb9jhzPp-dark-orbit?sharecode=Qcv7JdDmNU3-Gvxk1kuy3piqQ7JG9n8cZW_wdc11a-c)
- **Vídeo demonstrativo:** (O link deve ser colado em `video/link_video.txt`)

---

## 9. Conexão com o Dark Orbit BR

Este sistema embarcado é o braço executor da plataforma. Enquanto o solo (FED/WD) monitora a frota, este núcleo de autonomia garante que o **CubeSat-Rio-1** sobreviva a aproximações perigosas mesmo quando está incomunicável sobre o Atlântico Sul.
