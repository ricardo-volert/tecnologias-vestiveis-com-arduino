#include <SPI.h>
#include <RF24.h>

RF24 radio(9, 8);  // Pinos CE e CSN do módulo nRF24L01
const byte endereco[6] = "00001";  // Endereço do receptor

const int buzzerEsquerda = 2;
const int buzzerDireita = 3;
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
  radio.openReadingPipe(1, endereco);  // Define o endereço do receptor para leitura dos dados
  radio.startListening();  // Inicia a escuta do módulo NRF24L01
  pinMode(buzzerEsquerda, OUTPUT);  // Define o pino do buzzer da esquerda como saída
  pinMode(buzzerDireita, OUTPUT);  // Define o pino do buzzer da direita como saída
  pinMode(triggerPin, OUTPUT);  // Define o pino do trigger do sensor ultrassônico como saída
  pinMode(echoPin, INPUT);  // Define o pino do echo do sensor ultrassônico como entrada
}

void loop() {
  digitalWrite(triggerPin, LOW);  // Configura o trigger como LOW para iniciar a leitura do sensor
  delayMicroseconds(2);  // Pequeno atraso antes de iniciar o pulso de trigger
  digitalWrite(triggerPin, HIGH);  // Inicia o pulso de trigger
  delayMicroseconds(10);  // Pulso de trigger com duração de 10 microssegundos
  digitalWrite(triggerPin, LOW);  // Termina o pulso de trigger

  float distancia_local = pulseIn(echoPin, HIGH) / 58.0; // Distância em centímetros (sem decimais) lida pelo sensor ultrassônico

  Serial.print("Distancia local: ");
  Serial.println(distancia_local); // Imprime a distância lida pelo sensor ultrassônico

  // Calcula o tempo de toque dos buzzers com base na distância local
  int tempoToqueLocal = map(distancia_local, 0, 50, 50, 500); // Ajuste os valores conforme necessário

  if (distancia_local <= 15 && distancia_local > 0.1){
    // Ativa ambos os buzzers com base na distância local
    digitalWrite(buzzerEsquerda, HIGH);
    digitalWrite(buzzerDireita, HIGH);
    delay(tempoToqueLocal);
    digitalWrite(buzzerEsquerda, LOW);
    digitalWrite(buzzerDireita, LOW);
    delay(tempoToqueLocal);
  }

  if (radio.available()) {
    DadosSensor dados;

    // Recebemos a estrutura DadosSensor em uma única mensagem
    radio.read(&dados, sizeof(dados));

    Serial.print("Distancia via radio: ");
    Serial.print(dados.distancia);
    Serial.print(" cm, Indicador: ");
    Serial.println(dados.indicador); // Imprime a distância e o indicador recebidos via rádio

    // Calcula o tempo de toque dos buzzers com base na distância via rádio
    int tempoToqueRadio = map(dados.distancia, 0, 50, 50, 500); // Ajuste os valores conforme necessário

    if (dados.indicador == IDENTIFICADOR_BUZZER_ESQUERDA && dados.distancia <= 15){
      // Ativa o buzzer da esquerda apenas quando recebe o sinal via rádio do primeiro transmissor
      digitalWrite(buzzerEsquerda, HIGH);
      digitalWrite(buzzerDireita, LOW);
      delay(tempoToqueRadio);
      digitalWrite(buzzerEsquerda, LOW);
      digitalWrite(buzzerDireita, LOW);
      delay(tempoToqueRadio);
    }
    if (dados.indicador == IDENTIFICADOR_BUZZER_DIREITA && dados.distancia <= 15){
      // Ativa o buzzer da direita apenas quando recebe o sinal via rádio do segundo transmissor
      digitalWrite(buzzerEsquerda, LOW);
      digitalWrite(buzzerDireita, HIGH);
      delay(tempoToqueRadio);
      digitalWrite(buzzerEsquerda, LOW);
      digitalWrite(buzzerDireita, LOW);
      delay(tempoToqueRadio);
    }
  }
}