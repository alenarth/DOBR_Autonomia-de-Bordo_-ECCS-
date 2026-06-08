#include <LiquidCrystal.h>
#include <Servo.h>

// LCD paralelo: LiquidCrystal(RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Servo propulsor;

// ----------------------------- PINOS -------------------------------
const int PINO_TRIG = 9;
const int PINO_ECHO = 10;
const int PINO_POT  = A0;
const int PINO_SERVO = 6;
const int LED_VERDE    = 8;
const int LED_AMBAR    = 13;
const int LED_VERMELHO = 7;

// --------------------------- LIMIARES ------------------------------
const int DIST_ALERTA   = 100;  // cm: abaixo disso = atenção
const int DIST_CRITICO  = 50;   // cm: abaixo disso = manobra
const int DIST_MIN = 2;         // faixa válida do sensor
const int DIST_MAX = 400;
const int PROPELENTE_MIN = 15;  // % mínimo para poder manobrar

// --------------------- INTERVALOS (millis) -------------------------
const unsigned long T_DIST    = 120;
const unsigned long T_DISPLAY = 400;
const unsigned long T_PAGINA  = 2500;
const unsigned long T_RELOGIO = 1000;
const unsigned long T_SERVO   = 12;
const unsigned long COOLDOWN  = 6000;

// ------------------------ ESTADO GLOBAL ----------------------------
enum Estado { NOMINAL, ALERTA, CRITICO, DEGRADADO };
Estado estado = NOMINAL;

int  distancia = 400;       // último valor VÁLIDO de distância
bool distOk = true;         // a última leitura foi válida?
int  propelente = 100;      // % (do potenciômetro)
unsigned int erros = 0;     // contagem de leituras inválidas
char ultimaAcao[12] = "NENHUMA";
unsigned long segundosOffline = 0;

unsigned long tDist = 0, tDisp = 0, tPag = 0, tRel = 0, tServo = 0, tFim = 0;
int pagina = 0;

// manobra do servo (não-bloqueante): 0=parado 1->150 2->30 3->volta 90
int fase = 0, angulo = 90;
bool manobrando = false;
bool ledOn = false;
unsigned long tLed = 0;

// =================================================================
void setup() {
  Serial.begin(9600);
  pinMode(PINO_TRIG, OUTPUT);
  pinMode(PINO_ECHO, INPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMBAR, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);

  propulsor.attach(PINO_SERVO);
  propulsor.write(angulo);

  lcd.begin(16, 2);
  lcd.print(" DARK ORBIT BR ");
  lcd.setCursor(0, 1);
  lcd.print("Autonomia ON-BRD");

  Serial.println("Edge de bordo iniciado.");
}

// LOOP: so chama tarefas agendadas por millis(). Nenhum delay() aqui.
void loop() {
  unsigned long agora = millis();

  if (agora - tDist >= T_DIST) { lerDistancia(); lerPropelente(); tDist = agora; }

  decidir();
  moverServo(agora);
  atualizarLeds(agora);

  if (agora - tDisp >= T_DISPLAY) { mostrarDisplay(); tDisp = agora; }
  if (agora - tPag  >= T_PAGINA)  { pagina = (pagina + 1) % 3; tPag = agora; }
  if (agora - tRel  >= T_RELOGIO) { segundosOffline++; tRel = agora; }
}

// ---------------- SENSORES (com validação) -------------------------
void lerDistancia() {
  digitalWrite(PINO_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PINO_TRIG, HIGH);
  delayMicroseconds(10);          // pulso do sensor (us), nao e delay de loop
  digitalWrite(PINO_TRIG, LOW);

  long dur = pulseIn(PINO_ECHO, HIGH, 30000UL);  // timeout: nunca trava
  int d = dur * 0.0343 / 2.0;

  // VALIDACAO: descarta leitura invalida e mantem o ultimo valor bom
  if (dur == 0 || d < DIST_MIN || d > DIST_MAX) {
    distOk = false;
    erros++;
  } else {
    distancia = d;
    distOk = true;
  }
}

void lerPropelente() {
  propelente = map(analogRead(PINO_POT), 0, 1023, 0, 100);
}

// ------------------- DECISAO (maquina de estados) ------------------
void decidir() {
  Estado anterior = estado;

  if (distOk && distancia < DIST_CRITICO) {
    if (propelente >= PROPELENTE_MIN) {
      estado = CRITICO;
      iniciarManobra();           // desvia sozinho
    } else {
      estado = DEGRADADO;         // sem propelente: nao pode manobrar
      strncpy(ultimaAcao, "S/PROPEL", sizeof(ultimaAcao));
    }
  } else if (distOk && distancia < DIST_ALERTA) {
    estado = ALERTA;
  } else {
    estado = NOMINAL;
  }

  if (estado != anterior) {
    Serial.print("[ESTADO] ");
    Serial.println(nomeEstado());
  }
}

// --------------------- ATUACAO: servo (nao-bloqueante) -------------
void iniciarManobra() {
  if (manobrando) return;
  if (millis() - tFim < COOLDOWN && segundosOffline > 0) return; // espaca manobras
  manobrando = true;
  fase = 1;
  strncpy(ultimaAcao, "DESVIO+", sizeof(ultimaAcao));
  Serial.println("[ACAO] Manobra de desvio iniciada.");
}

void moverServo(unsigned long agora) {
  if (!manobrando) return;
  if (agora - tServo < T_SERVO) return;
  tServo = agora;

  int alvo = (fase == 1) ? 150 : (fase == 2) ? 30 : 90;
  if (angulo < alvo) angulo += 2;
  else if (angulo > alvo) angulo -= 2;
  propulsor.write(angulo);

  if (abs(angulo - alvo) <= 1) {
    angulo = alvo;
    fase++;
    if (fase > 3) {                 // sequencia concluida
      manobrando = false;
      fase = 0;
      tFim = millis();
      Serial.println("[ACAO] Manobra concluida.");
    }
  }
}

// --------------------- LEDs (piscam por millis) --------------------
void atualizarLeds(unsigned long agora) {
  unsigned long intervalo =
      (estado == CRITICO)   ? 150 :
      (estado == DEGRADADO) ? 250 :
      (estado == ALERTA)    ? 500 : 0;

  if (intervalo > 0 && agora - tLed >= intervalo) { ledOn = !ledOn; tLed = agora; }
  if (intervalo == 0) ledOn = true;

  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMBAR, LOW);
  digitalWrite(LED_VERMELHO, LOW);

  if (estado == NOMINAL)        digitalWrite(LED_VERDE, HIGH);
  else if (estado == ALERTA)    digitalWrite(LED_AMBAR, ledOn);
  else if (estado == CRITICO)   digitalWrite(LED_VERMELHO, ledOn);
  else { // DEGRADADO: vermelho e ambar alternados
    digitalWrite(LED_VERMELHO, ledOn);
    digitalWrite(LED_AMBAR, !ledOn);
  }
}

// --------------------- DISPLAY (telemetria) ------------------------
void mostrarDisplay() {
  char l0[17], l1[17];

  // Linha 0: estado + distancia (sempre visivel)
  char dStr[5];
  if (distOk) snprintf(dStr, sizeof(dStr), "%03d", constrain(distancia, 0, 999));
  else        strncpy(dStr, "---", sizeof(dStr));
  snprintf(l0, sizeof(l0), "%-8s D:%s", nomeEstado(), dStr);

  // Linha 1: telemetria rotativa (3 paginas)
  if (pagina == 0) {
    snprintf(l1, sizeof(l1), "Propelente: %02d%%", constrain(propelente, 0, 99));
  } else if (pagina == 1) {
    snprintf(l1, sizeof(l1), "Acao: %-9s", ultimaAcao);
  } else {
    unsigned int mm = segundosOffline / 60, ss = segundosOffline % 60;
    snprintf(l1, sizeof(l1), "Offline   %02u:%02u", mm % 100, ss);
  }

  preencher16(l0);
  preencher16(l1);
  lcd.setCursor(0, 0); lcd.print(l0);
  lcd.setCursor(0, 1); lcd.print(l1);
}

// completa com espacos ate 16 colunas (evita residuo no LCD)
void preencher16(char* b) {
  int n = strlen(b);
  for (int i = n; i < 16; i++) b[i] = ' ';
  b[16] = '\0';
}

const char* nomeEstado() {
  if (estado == NOMINAL)   return "NOMINAL";
  if (estado == ALERTA)    return "ALERTA";
  if (estado == CRITICO)   return "CRITICO";
  return "DEGRAD";
}
