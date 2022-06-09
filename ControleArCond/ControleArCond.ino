/*
Controle de ar condicionado
23/04/2022  - Versão inicial (falta intertravamento e menus)
25/04/2022  - Adicionada função para verificar falhas no equipamento
            - Suporte básico para display
            - Melhoria de funções
*/
#define compressorPin 7
#define ventLentoPin 8
#define ventRapidoPin 9
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

double setPoint = 25.0;
double histerese = 2.0;
int delayReligar = 10;
int delayLigar = 10;
int temporizadorCompressor = 0;
boolean ativaCompressor = 0;

void setup() {
  lcd.begin(16, 2);
  pinMode(compressorPin,OUTPUT);
  pinMode(ventLentoPin,OUTPUT);
  pinMode(ventRapidoPin,OUTPUT);
  Serial.begin(9600);
  lcd.setCursor(0, 0);
  lcd.print("Temperatura");
  }
int verificaFalhas(){
  return(0);
}

void controleVentilador(int velocidade){ // Função para ligar e desligar o ventilador
  if (velocidade == 10) {
    digitalWrite(ventLentoPin,HIGH);
    digitalWrite(ventRapidoPin,LOW);
  }
  if (velocidade == 20) {
    digitalWrite(ventLentoPin,LOW);
    digitalWrite(ventRapidoPin,HIGH);
  }
  if (velocidade == 0) {
    digitalWrite(ventRapidoPin,LOW);
    digitalWrite(ventLentoPin,LOW);
  }
}

int controleCompressor(boolean ldesliga){ // liga e desliga o compressor
  if(ldesliga){
    int falhas = verificaFalhas();
    if (falhas<1){
      digitalWrite(compressorPin,HIGH);  
      return(0);
    }
    else return(falhas);
  }
  if(!ldesliga){
    digitalWrite(compressorPin,LOW);
    return(0);
  }
}
double leTemp(){ // Função para ler temperatura do sensor
  int leituraSensor = analogRead(A0);
  double temperaturaAtual = (leituraSensor/16);
  return(temperaturaAtual);  
}

void timerCompressor(boolean ldesliga){ // Função para ativar o timer compressor (temporizador para ativação)
  if (ldesliga){
    ativaCompressor = 1;
    if (!digitalRead(compressorPin) && temporizadorCompressor==0) temporizadorCompressor = delayLigar;
  }
  else{
    ativaCompressor = 0;
    if (digitalRead(compressorPin) && temporizadorCompressor==0) temporizadorCompressor = delayReligar;
  }
}

void atualizaDisplay(){ // Função para atualizar o display
  Serial.print("Compressor=");
  Serial.println(ativaCompressor);
  Serial.print("Temporizador=");
  Serial.println(temporizadorCompressor);
  Serial.print("Temperatura=");
  Serial.println(leTemp());
  lcd.setCursor(0, 0);
  lcd.print("Temperatura     ");
  lcd.setCursor(11, 0);
  if (digitalRead(ventLentoPin)) lcd.print(" VL");
  if (digitalRead(ventRapidoPin)) lcd.print(" VR");
  if (digitalRead(compressorPin)) lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("S=");
  lcd.print(setPoint,1);
  lcd.print(" T=");
  lcd.print(leTemp(),1);
}

void iteradorTemporal(){
  int retornoCompressor;
  if(leTemp() >(setPoint+(histerese/2)))
  {
    timerCompressor(true);
  }
  if(leTemp() <(setPoint-(histerese/2)))
  {
    timerCompressor(false);
  }
  
  if (ativaCompressor){
    if (temporizadorCompressor < 2){
      retornoCompressor = controleCompressor(true);
      temporizadorCompressor = 0;
    }
    else
    temporizadorCompressor--;
  }
  else {
    if (temporizadorCompressor < 2){
      retornoCompressor = controleCompressor(false);
      temporizadorCompressor = 0;
    }
    else
    temporizadorCompressor--;
  }
}

void loop() {
  delay(500);
  
  controleVentilador(10);
  atualizaDisplay();
  iteradorTemporal();
}
