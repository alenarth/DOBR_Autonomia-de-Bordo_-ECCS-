# Dark Orbit BR — Autonomia de Bordo (Edge) · Tinkercad

Sistema embarcado de **missão crítica** para o CubeSat-Rio-1 (NORAD 55001), do projeto **Dark Orbit BR**. Entrega da disciplina **Edge Computing & Computer Systems (ECCS)** · Global Solution 2026 · FIAP — Engenharia de Software.

Simulação em **Tinkercad Circuits** (Arduino Uno).

---

## 1. O problema da missão

O **Dark Orbit BR** prevê risco de colisão de satélites a partir do solo. Mas há um momento em que essa previsão não chega: o **blackout de comunicação**. Quando o CubeSat-Rio-1 passa por uma região sem contato com a estação terrestre, **não há operador, não há nuvem e não há segunda chance**. Se um detrito se aproxima nesse intervalo, esperar o comando da Terra pode significar a perda do satélite.

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

### Ligação do LCD 16x2 (modo 4 bits) — nomes como aparecem no Tinkercad

> Importante: o LCD tem 8 pinos de dados (DB0–DB7), mas usamos o **modo 4 bits**, então **só DB4, DB5, DB6 e DB7 são ligados**. **DB0, DB1, DB2 e DB3 ficam vazios** (sem fio) — é assim mesmo. Veja a imagem `imagens/ligacao_lcd.png`.

| Pino do LCD | Vai para | | Pino do LCD | Vai para |
|---|---|---|---|---|
| GND | trilha − (GND) | | DB0–DB3 | **não conecta (vazio)** |
| VCC | trilha + (5V) | | DB4 | Arduino **5** |
| V0 (contraste) | trilha − (GND) | | DB5 | Arduino **4** |
| RS | Arduino **12** | | DB6 | Arduino **3** |
| RW | trilha − (GND) | | DB7 | Arduino **2** |
| E | Arduino **11** | | LED (ânodo +) | trilha + (5V) |
| | | | LED (catodo −) | trilha − (GND) |

No código: `LiquidCrystal lcd(12, 11, 5, 4, 3, 2);` → ordem (RS, E, DB4, DB5, DB6, DB7).

> Só **5 pinos** do LCD vão ao Arduino (RS, E, DB4, DB5, DB6, DB7). O resto é energia (+/−). Os "2 LED" são a luz de fundo.

### Esquema visual

![Esquema de ligação](imagens/esquema_ligacao.png)

> Se o texto do LCD aparecer fraco ou em blocos, troque o V0 (contraste) por um potenciômetro de 10 kΩ (pino central no V0, laterais em 5V e GND).

---

## 4. O display de bordo (telemetria)

- **Linha de cima (sempre visível):** estado + distância. Ex.: `CRITICO  D:042`.
- **Linha de baixo (rotativa, a cada ~2,5 s):**
  1. `Propelente: 78%`
  2. `Acao: DESVIO+`
  3. `Offline   03:12` (tempo em blackout)

---

## 5. Robustez (decisões de engenharia)

- **Temporização não-bloqueante:** todo o agendamento (ler sensor, atualizar display, piscar LEDs, mover servo) usa `millis()`. **Nenhum `delay()` no loop.** Assim o sistema continua medindo o detrito enquanto move o servo e atualiza a tela.
  - Os `delayMicroseconds(2)` e `delayMicroseconds(10)` são apenas o **pulso de gatilho do HC-SR04** (micros­segundos), parte do protocolo do sensor — não são `delay()` de agendamento.
- **Validação de leitura:** se o ultrassônico devolve 0 (timeout) ou um valor fora da faixa física (2–400 cm), a leitura é **descartada**, mantém-se o último valor válido e conta-se o erro. O sistema **nunca trava** por causa de um sensor.

---

## 6. Como montar e rodar no Tinkercad (passo a passo)

1. Entre em **https://www.tinkercad.com** → **Circuits** → **Create new Circuit**.
2. Arraste para a área: **Arduino Uno**, **Ultrasonic Distance Sensor (4 pinos)**, **Potentiometer**, **Micro Servo**, **3 LEDs**, **3 resistores (220 Ω)** e o **LCD 16 x 2**.
3. Faça as ligações exatamente como na tabela da seção 3 (comece pela alimentação 5V/GND, depois sinais).
4. Clique em **Code** → mude para **Text** (em vez de Blocks) → apague o exemplo e **cole todo o conteúdo de `codigo/main.ino`**.
5. Clique em **Start Simulation**.
6. O LCD acende com a tela de boot e o sistema entra em operação.

### Como testar (e gravar o vídeo)
- **Clique no sensor ultrassônico** e arraste o controle de distância de "longe" para "perto": o estado percorre **NOMINAL → ALERTA → CRÍTICO** e o servo executa o desvio sozinho.
- **Gire o potenciômetro** para baixo (propelente < 15%) e reaproxime o objeto para ver o estado **DEGRADADO**.
- Veja o LCD trocar de página e os LEDs responderem a cada estado.

### Obter o link de entrega
No Tinkercad: botão **Share** → **Invite people / Copy link** (ou apenas copie o link público do projeto). Cole esse link no campo da seção 8.

---

## 7. Estrutura do repositório

```
projeto-edge-space/
├── README.md
├── integrantes.txt
├── codigo/
│   └── main.ino            # Sketch (millis, validação, máquina de estados)
├── imagens/
│   └── esquema_ligacao.png
├── docs/
│   ├── esquema_ligacao.svg
│   └── roteiro_video.md    # Roteiro do vídeo de até 3 min
└── video/
    └── LEIA-ME.txt         # Onde colar o link do vídeo
```

---

## 8. Links de entrega

- **Projeto no Tinkercad:** _cole aqui o link público (botão Share)_
- **Vídeo demonstrativo (≤ 3 min):** _cole aqui o link do YouTube (não listado) ou Google Drive_

---

## 9. Conexão com o Dark Orbit BR

A plataforma Dark Orbit BR **prevê** o risco de colisão no solo (telas e modelo), e este sistema embarcado **executa a defesa** quando o satélite está sem comunicação. Mesma missão (CubeSat-Rio-1), duas camadas que se completam — solo e bordo.
