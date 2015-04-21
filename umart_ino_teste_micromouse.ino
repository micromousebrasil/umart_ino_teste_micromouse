/**
 *****************************************************************
 * Arquivo  umart_ino_teste_micromouse.ino
 * Autor    Kleber Lima da Silva (kleber.ufu@hotmail.com)
 * Versão   V1.0.0
 * Data     19-Abril-2015
 * Resumo   Programa para teste do hardware do Robô uMaRT-INO no
 *         modo micromouse (solucionador de labirinto)
 *****************************************************************
 */

// Bibliotecas ---------------------------------------------------
#include <Encoder.h>

// Definição dos pinos no modo MICROMOUSE ------------------------
const int FE_RECEPTOR = A0;  // Fototransistor frontal esquerdo
const int LE_RECEPTOR = A1;  // Fototransistor lateral esquerdo
const int LD_RECEPTOR = A2;  // Fototransistor lateral direito
const int FD_RECEPTOR = A3;  // Fototransistor frontal direito
const int FE_EMISSOR = A4;   // LED frontal esquerdo
const int L_EMISSORES = 13;  // LEDs laterais
const int FD_EMISSOR = A5;   // LED frontal direito
const int LED1 = 0;        // LED1
const int LED2 = 1;        // LED2
const int B_ENC_E = 2;     // Encoder do motor esquerdo (sinal B)
const int A_ENC_D = 3;     // Encoder do motor direito (sinal A)
const int A_ENC_E = 4;     // Encoder do motor esquerdo (sinal A)
const int B_ENC_D = 5;     // Encoder do motor direito (sinal B)
const int PWM_E = 6;       // PWM do motor esquerdo
const int IN2_E = 7;       // IN2 da ponte H do motor esquerdo
const int IN1_E = 8;       // IN1 da ponte H do motor esquerdo
const int IN1_D = 9;       // IN1 da ponte H do motor direito
const int IN2_D = 10;      // IN2 da ponte H do motor direito
const int PWM_D = 11;      // PWM do motor direito
const int SW1 = 12;        // Botão SW1

// Configuração dos Encoders -------------------------------------
Encoder encoderEsquerda(B_ENC_E, A_ENC_E);
Encoder encoderDireita(A_ENC_D, B_ENC_D);

// Definição das constantes do programa --------------------------
#define THRESHOLD 300  // Limiar para reconhecer parede ou não


// Variáveis do programa -----------------------------------------
int frontal_esquerdo = 0;
int lateral_esquerdo = 0;
int lateral_direito = 0;
int frontal_direito = 0;


// Protótipos das funções ----------------------------------------
int lerSensoresParede(void);
void setMotores(int pwm_esquerda, int pwm_direita);


// Inicialização dos pinos ---------------------------------------
void setup()
{ 
  pinMode(FE_RECEPTOR, INPUT);
  pinMode(LE_RECEPTOR, INPUT);
  pinMode(LD_RECEPTOR, INPUT);
  pinMode(FD_RECEPTOR, INPUT);
  pinMode(FE_EMISSOR, OUTPUT);
  pinMode(L_EMISSORES, OUTPUT);
  pinMode(FD_EMISSOR, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(PWM_E, OUTPUT);
  pinMode(IN1_E, OUTPUT);
  pinMode(IN2_E, OUTPUT);
  pinMode(PWM_D, OUTPUT);
  pinMode(IN1_D, OUTPUT);
  pinMode(IN2_D, OUTPUT);
  pinMode(SW1, INPUT);
  
  Serial.begin(9600);
}


// LOOP principal do programa ------------------------------------
void loop()
{
  unsigned long t0;
  
  // Verifica a leitura dos sensores e mostra na "serial"
  Serial.print("Sensores de Parede: ");
  Serial.println(getSensoresParede(), BIN);
  Serial.println(frontal_esquerdo);
  Serial.println(lateral_esquerdo);
  Serial.println(lateral_direito);
  Serial.println(frontal_direito);
  
  // Atualiza a leitura dos encoders e mostra na "serial"
  Serial.println("Encoders: ");
  Serial.println(encoderEsquerda.read());
  Serial.println(encoderDireita.read());
  Serial.println();


  // Se o botão for pressionado: liga os motores por 1 segundo
  // .. ou aguarda 1 segundo sem fazer nada
  t0 = millis();
  while((millis() - t0) < 1000)
  {
    if(digitalRead(SW1) == HIGH)
    {
      setMotores(50, 50);
      delay(1000);
      setMotores(0, 0);
    }
  }
}


/* Função para leitura dos sensores de parede --------------------
 * Atualiza as leituras dos sensores frontais e laterais. 
 * Retorna uma máscara de bits indicando presença (1) ou não (0)
 * de paredes. O bit mais significativo representa a parede da
 * esquerda. Ex.: 011 = presença de parede frontal e direita.
 */
int getSensoresParede(void)
{
  int paredes = 0;
  
  frontal_esquerdo = analogRead(FE_RECEPTOR);
  lateral_esquerdo = analogRead(LE_RECEPTOR);
  lateral_direito = analogRead(LD_RECEPTOR);
  frontal_direito = analogRead(FD_RECEPTOR);
  
  // Registra o tempo atual
  unsigned long t0 = micros();
  
  // Sensor frontal esquerdo
  digitalWrite(FE_EMISSOR, HIGH);
  while((micros() - t0) < 60);
  frontal_esquerdo = analogRead(FE_RECEPTOR) - frontal_esquerdo;
  digitalWrite(FE_EMISSOR, LOW);
  if(frontal_esquerdo < 0) frontal_esquerdo = 0;
  while((micros() - t0) < 140);
  
  // Sensor frontal direito
  digitalWrite(FD_EMISSOR, HIGH);
  while((micros() - t0) < 200);
  frontal_direito = analogRead(FD_RECEPTOR) - frontal_direito;
  digitalWrite(FD_EMISSOR, LOW);
  if(frontal_direito < 0) frontal_direito = 0;
  while((micros() - t0) < 280);
  
  // Sensores laterais
  digitalWrite(L_EMISSORES, HIGH);
  while((micros() - t0) < 340);
  lateral_esquerdo = analogRead(LE_RECEPTOR) - lateral_esquerdo;
  lateral_direito = analogRead(LD_RECEPTOR) - lateral_direito;
  digitalWrite(L_EMISSORES, LOW);
  if(lateral_esquerdo < 0) lateral_esquerdo = 0;
  if(lateral_direito < 0) lateral_direito = 0;
  
  // Realiza a máscara de bits
  if(frontal_esquerdo > THRESHOLD || frontal_direito > THRESHOLD)
  {
    paredes |= 0b010;
  }
  
  if(lateral_esquerdo > THRESHOLD)
  {
    paredes |= 0b100;
  }
  
  if(lateral_direito > THRESHOLD)
  {
    paredes |= 0b001;
  }
  
  return paredes;
}


/* Função para acionamento dos motores ---------------------------
 * pwm_esquerda e pwm_direita recebem valores entre -255 e 255
 * (valores negativos giram o respectivo motor para trás)
 */
void setMotores(int pwm_esquerda, int pwm_direita)
{
  if(pwm_esquerda < 0)
  {
    pwm_esquerda *= -1;
  
    digitalWrite(IN1_E, LOW);
    digitalWrite(IN2_E, HIGH);
  }
  else
  {
    digitalWrite(IN1_E, HIGH);
    digitalWrite(IN2_E, LOW);
  }
  
  if(pwm_direita < 0)
  {
    pwm_direita *= -1;
  
    digitalWrite(IN1_D, LOW);
    digitalWrite(IN2_D, HIGH);
  }
  else
  {
    digitalWrite(IN1_D, HIGH);
    digitalWrite(IN2_D, LOW);
  }
  
  if(pwm_esquerda > 255) pwm_esquerda = 255;
  if(pwm_direita > 255) pwm_direita = 255;
  
  analogWrite(PWM_E, pwm_esquerda);
  analogWrite(PWM_D, pwm_direita);
}

