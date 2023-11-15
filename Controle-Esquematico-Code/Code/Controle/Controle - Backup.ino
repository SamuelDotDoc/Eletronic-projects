
/*
 *Projeto: Iniciação Ciêntifica - HEXAPOD 2023
 *Empresa: FIAP
 *Microcontrolador: ESP32
 *Placa: DOIT ESP32 DEVKIT V1
 *Descrição: Código para programação das principais funções do
 *controle do Robô Hexapod.
 *Dependências: LiquidCrystal_I2C
 *
 *ATENÇÃO: Esse código tem dependência de uma bibliotecas externa,
 *não esqueça de importar ela.
*/

// BIBLIOTECAS
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <esp_now.h>
#include <WiFi.h>

// CONSTANTES DA PLACA - NÃO ALTERAR!!!
 
#define SDA 21 // ENTRADA SDA DO DISPLAY LCD
#define SCL 22 // ENTRADA SCL DO DISPLAY LCD

#define B_STP 36 // BOTÃO DE EMERGÊNCIA

#define JX 35 // EIXO X DO JOYSTICK
#define JY 34 // EIXO Y DO JOYSTICK
#define JZ 39 // EIXO Z DO JOYSTICK

#define B1 32 // BOTÃO LATERAL VERMELHO 1
#define B2 33 // BOTÃO LATERAL VERMELHO 2
#define B3 25 // BOTÃO LATERAL VERMELHO 3
#define B4 26 // BOTÃO LATERAL VERMELHO 4
#define B5 27 // BOTÃO LATERAL VERMELHO 5
#define B6 14 // BOTÃO LATERAL VERMELHO 6

/*
 * SEGUE ESSE PADRÃO NO CONTROLE: 
 * B1 B2
 * B3 B4
 * B5 B6
*/

#define SW1 19 // SWITCH NO FLANCO ESQUERDO DO DISPLAY 1
#define SW2 18 // SWITCH NO FLANCO ESQUERDO DO DISPLAY 2
#define SW3 5  // SWITCH NO FLANCO ESQUERDO DO DISPLAY 3
#define SW4 17 // SWITCH NO FLANCO DIREITO DO DISPLAY 4
#define SW5 16 // SWITCH NO FLANCO DIREITO DO DISPLAY 5
#define SW6 4  // SWITCH NO FLANCO DIREITO DO DISPLAY 6

/*
 * SEGUE ESSE PADRÃO NO CONTROLE: 
 * SW1 +─────────────+ SW4
 * SW2 | DISPLAY LCD | SW5
 * SW3 +─────────────+ SW6
*/

#define P1 12 // POTENCIOMETRO DE CIMA
#define P2 13 // POTENCIOMETRO DE BAIXO

LiquidCrystal_I2C lcd(0x27, 20, 4); // ---> I2C address do display = 0x27

uint8_t broadcastAddress[] = {0x0C, 0xB8, 0x15, 0xC2, 0x48, 0xDC};

typedef struct struct_message {
  char a;
} struct_message;


struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  pinMode(B_STP, INPUT);

  pinMode(JX, INPUT);
  pinMode(JY, INPUT);
  pinMode(JZ, INPUT);
  
  pinMode(B1, INPUT);
  pinMode(B2, INPUT);
  pinMode(B3, INPUT);
  pinMode(B4, INPUT);
  pinMode(B5, INPUT);
  pinMode(B6, INPUT);

  
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  pinMode(SW3, INPUT);
  pinMode(SW4, INPUT);
  pinMode(SW5, INPUT);
  pinMode(SW6, INPUT);

  
  pinMode(P1, INPUT);
  pinMode(P2, INPUT);

  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(4,1);
  lcd.print("HEXAPOD 2023");
  lcd.setCursor(4,2);
  lcd.print("CONTROLE V01");
  Serial.println("LCD Pronto!");

  delay(1000);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
    
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  if(digitalRead(B_STP) == LOW)
  { 
    int mappingX = map(analogRead(JX), 0, 4095, 0, 3);
    int mappingY = map(analogRead(JY), 0, 4095, 0, 3);
    //int mappingZ = map(analogRead(JZ), 0, 4095, 0, 3);

    if(digitalRead(SW1) == HIGH){
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("HEXAPOD 2023");
      lcd.setCursor(4,1);
      lcd.print("CONTROLE V01");
      lcd.setCursor(3,2);
      lcd.print("MODO: JOYSTICK");
      lcd.setCursor(1,3);
      lcd.print("MUDAR DE MODO: SW1");
      
      moverBotoes();
    }
    else{
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("HEXAPOD 2023");
      lcd.setCursor(4,1);
      lcd.print("CONTROLE V01");
      lcd.setCursor(4,2);
      lcd.print("MODO: MANUAL");
      lcd.setCursor(1,3);
      lcd.print("MUDAR DE MODO: SW1");
      
      //mover(mappingX, mappingY, mappingZ);
      mover(mappingX, mappingY);
    }

    if(digitalRead(B5) == HIGH){  
      myData.a = 'J'; // Brochar (Emote 1)

      // Send message via ESP-NOW
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
      if (result == ESP_OK) {
        Serial.println("Sent with success");
      }
      else {
        Serial.println("Error sending the data");
      }
      delay(100);
    }
    else{
      myData.a = ' ';

      // Send message via ESP-NOW
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
      if (result == ESP_OK) {
        Serial.println("Sent with success");
      }
      else {
        Serial.println("Error sending the data");
      }
      delay(100);
    }

    if(digitalRead(B6) == HIGH){  
      myData.a = 'R'; // Ainda a decidir (Emote 2)

      // Send message via ESP-NOW
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
      if (result == ESP_OK) {
        Serial.println("Sent with success");
      }
      else {
        Serial.println("Error sending the data");
      }
      delay(100);
    }
    else{
      myData.a = ' ';

      // Send message via ESP-NOW
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
      if (result == ESP_OK) {
        Serial.println("Sent with success");
      }
      else {
        Serial.println("Error sending the data");
      }
      delay(100);
    }
  }
}

//void mover(int X, int Y, int Z){
void mover(int X, int Y){
  if(X>1){
    myData.a = 'R';
    
    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    delay(100);
  }
  if(X<1){
    myData.a = 'L';
    
    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    delay(100);
  }
  if(Y>1){
    myData.a = 'F';

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    delay(100);
  }
  if(Y<1){
    myData.a = 'B';
    
    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    delay(100);
  }
  if(X==1 && Y==1){
    myData.a = ' ';

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    delay(100);
  }
  /**
  if(Z>1){
    myData.a = 'Z';
  }
  else if(Z<1){
    myData.a = 'X';
  }
  else{
    myData.a = ' ';
  }**/
  
  Serial.println(X);
  
  Serial.println(Y);

  Serial.println(myData.a);
}

void moverBotoes(){
  if(digitalRead(B1) == HIGH && digitalRead(B2) == HIGH){
    myData.a = 'F';

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    delay(100);
  }
  if(digitalRead(B3) == HIGH && digitalRead(B4) == HIGH){
    myData.a = 'B';

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    delay(100);
  }
  if(digitalRead(B3) == HIGH){
    myData.a = 'L';

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    delay(100);
  }
  if(digitalRead(B4) == HIGH){
    myData.a = 'R';

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    delay(100);
  }
  /**
  else if(digitalRead(B1) == HIGH){
    myData.a = 'X';
  }
  else if(digitalRead(B2) == HIGH){
    myData.a = 'Z';
  }**/
  if(digitalRead(B1) == LOW && digitalRead(B2) == LOW && digitalRead(B3) == LOW && digitalRead(B4) == LOW){
    myData.a = ' ';

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    delay(100);
  }
}
