#include <SPI.h>
#include <RF24.h>

RF24 radio(9, 8);  // Pinos CE e CSN do módulo nRF24L01
const byte endereco[6] = "00001";  // Endereço do receptor

const int triggerPin = 4;
const int echoPin = 5;

const byte IDENTIFICADOR_BUZZER_ESQUERDA = 1;
const byte IDENTIFICADOR_BUZZER_DIREITA = 2;

struct DadosSensor {
  float distancia;
  byte indicador;
};

void setup() {
  Serial.begin(9600);  // Inicializa a comunicação serial para debug
  radio.begin();  // Inicializa o módulo NRF24L01
  radio.openWritingPipe(endereco);  // Define o endereço do receptor para envio dos dados
  pinMode(triggerPin, OUTPUT);  // Define o pino do trigger do sensor ultrassônico como saída
  pinMode(echoPin, INPUT);  // Define o pino do echo do sensor ultrassônico como entrada
}

void loop() {
  digitalWrite(triggerPin, LOW);  // Configura o trigger como LOW para iniciar a leitura do sensor
  delayMicroseconds(2);  // Pequeno atraso antes de iniciar o pulso de trigger
  digitalWrite(triggerPin, HIGH);  // Inicia o pulso de trigger
  delayMicroseconds(10);  // Pulso de trigger com duração de 10 microssegundos
  digitalWrite(triggerPin, LOW);  // Termina o pulso de trigger

  float distancia = pulseIn(echoPin, HIGH) / 58.0; // Distância em centímetros (sem decimais) lida pelo sensor ultrassônico

  DadosSensor dados;
  dados.distancia = distancia;
  dados.indicador = IDENTIFICADOR_BUZZER_ESQUERDA;  // Define o indicador como IDENTIFICADOR_BUZZER_ESQUERDA para tocar o buzzer da esquerda
  //dados.indicador = IDENTIFICADOR_BUZZER_DIREITA; // Caso queira tocar o buzzer da direita, basta descomentar esta linha e comentar a linha acima
  
  // Enviamos a estrutura DadosSensor em uma única mensagem
  radio.write(&dados, sizeof(dados));  // Envia os dados para o receptor via rádio

  Serial.print("Distancia enviada via radio: ");
  Serial.print(dados.distancia);  // Imprime a distância lida pelo sensor ultrassônico
  Serial.print(" cm, Indicador: ");
  Serial.println(dados.indicador); // Imprime o indicador enviado junto com a distância via rádio

  delay(100);  // Atraso entre as leituras do sensor e envio dos dados via rádio
}