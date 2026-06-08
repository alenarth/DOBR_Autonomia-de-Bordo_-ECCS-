# Roteiro do vídeo demonstrativo (máx. 3 minutos)

Disciplina: Edge Computing & Computer Systems · Dark Orbit BR — Autonomia de Bordo
Grave a tela do Tinkercad simulando + narração. Sugestão de tempos abaixo.

---

## 0:00 – 0:35 · O problema da missão (Contexto)
**Mostrar:** o circuito parado no Tinkercad.

> "Este é o módulo de autonomia de bordo do CubeSat-Rio-1, do projeto Dark Orbit BR.
> O Dark Orbit prevê risco de colisão a partir do solo — mas quando o satélite entra
> em blackout de comunicação, não há operador nem internet para mandar o comando.
> Se um detrito se aproxima nesse momento, o satélite precisa decidir e desviar
> sozinho. É isso que este sistema faz: ele mede, decide e atua localmente."

## 0:35 – 1:05 · A lógica de funcionamento
**Mostrar:** o esquema de ligação por alguns segundos e o display ligado.

> "Um sensor ultrassônico mede a distância do objeto que se aproxima; um potenciômetro
> simula o nível de propelente. A regra é simples: acima de 100 cm, NOMINAL; entre 50
> e 100, ALERTA; abaixo de 50 cm, CRÍTICO — e aí o servo executa o desvio
> automaticamente. Todo o tempo é controlado por millis(), sem nenhum delay() que
> pudesse congelar o sistema."

## 1:05 – 1:50 · Operando em tempo real
**Mostrar:** iniciar a simulação e arrastar o controle de distância do sensor de
"longe" para "perto", passando por cada faixa.

> "Com o objeto distante, o LED verde fica aceso e o display mostra NOMINAL.
> Conforme aproximo o objeto, entramos em ALERTA: LED âmbar piscando. Abaixo de
> 50 cm, CRÍTICO: o LED vermelho pisca e o servo executa a manobra de desvio —
> tudo sozinho, sem eu tocar em nada."

## 1:50 – 2:25 · O display de bordo (Telemetria)
**Mostrar:** de perto, as páginas do LCD trocando.

> "O display é a única janela da tripulação quando não há contato com a Terra.
> A linha de cima mostra sempre o estado e a distância. A de baixo alterna entre
> o nível de propelente, a última ação executada e o tempo em blackout."

## 2:25 – 2:50 · Robustez — modo degradado
**Mostrar:** girar o potenciômetro para baixo (propelente < 15%) e reaproximar o objeto.

> "E se faltar propelente durante uma ameaça? O sistema entra em DEGRADADO: avisa que
> NÃO consegue manobrar, em vez de fingir que está tudo bem. E se o sensor devolve uma
> leitura inválida, o código descarta o valor, mantém a última leitura boa e continua
> rodando — nunca trava."

## 2:50 – 3:00 · Fechamento
> "Edge computing de missão crítica: medir, decidir e agir localmente. Mesmo sem a
> Terra, o CubeSat-Rio-1 se protege sozinho. Obrigado."

---

### Checklist (exigência da disciplina)
- [ ] O problema da missão (autonomia offline)
- [ ] A lógica de funcionamento
- [ ] O sistema operando em tempo real
- [ ] O display exibindo telemetria
- [ ] Sensor e atuadores funcionando (ultrassom → servo + LEDs)

### Dica
Para o modo DEGRADADO, gire o potenciômetro para baixo (propelente < 15%) antes de
reaproximar o objeto. Suba o vídeo no YouTube como "não listado" e entregue o link.
