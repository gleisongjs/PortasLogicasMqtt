//Programa que simula portas lógicas - para a disciplina de Circuitos Digitais
//Desenvolvido por: Gleison J Silva em 22/09/17

#include <ESP8266WiFi.h> // Importa a Biblioteca ESP8266WiFi
#include <PubSubClient.h> // Importa a Biblioteca PubSubClient
 

#define TOPICO_SUBSCRIBE "CircuitosDigitais/PortasLogicas"     
#define TOPICO_PUBLISH   "CircuitosDigitais/PortasLogicas/ESP8266"    
#define ID_MQTT  "CD" 
    
#define A 0
#define B 1
#define C 2

boolean pinoA;
boolean pinoB;
boolean resultado;

int EstadoSaida = 0; 
                               

// WIFI
const char* SSID = "Transformers II"; 
const char* PASSWORD = "12345678*"; 
 
// MQTT
const char* BROKER_MQTT = "172.20.10.5"; 
int BROKER_PORT = 1883; 
 
 
//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient
 
void setup() 
{
 //inicializações:
 
   Serial.begin(115200);
   pinMode(A, OUTPUT);
   pinMode(B, OUTPUT);
   pinMode(C, OUTPUT);
   digitalWrite(A, LOW);
   digitalWrite(B, LOW);
   digitalWrite(C, LOW);
   
    initWiFi();
    initMQTT();   
    
}


void loop() 
{   
  
  EnviaEstadoOutputMQTT();
  VerificaConexoesWiFIEMQTT();
  //ligaLeds(); 
  MQTT.loop();
  
    
}



void initWiFi() 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
    
    reconectWiFi();
}
 

void initMQTT() 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   
    MQTT.setCallback(mqtt_callback);           
}


void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;
 
    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
  
    //toma ação dependendo da string recebida:
    
    if (msg.equals("OFF")){
      EstadoSaida = 0;
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
    }
      
    if (msg.equals("AND"))    
      EstadoSaida = 1;
    if (msg.equals("OR"))    
      EstadoSaida = 2;
    if (msg.equals("NAND"))    
      EstadoSaida = 3;
     if (msg.equals("NOR"))    
      EstadoSaida = 4;
    if (msg.equals("XOR"))    
      EstadoSaida = 5;
     if (msg.equals("XNOR"))    
      EstadoSaida = 6;
}


void reconnectMQTT() 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
        } 
        else 
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s");
            delay(1000);
        }
    }
}

void reconectWiFi() 
{
    //se já está conectado a rede WI-FI, nada é feito. 
    //Caso contrário, são efetuadas tentativas de conexão
    if (WiFi.status() == WL_CONNECTED)
        return;
        
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
    
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
  
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
}

void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
        reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
    
     reconectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}
 

void EnviaEstadoOutputMQTT(void)
{

    if (EstadoSaida == 0)    
      MQTT.publish(TOPICO_PUBLISH, "OFF");     
    if (EstadoSaida == 1){

      MQTT.publish(TOPICO_PUBLISH, "AND");
      ligaLeds();
        
    }
                    
    if (EstadoSaida == 2){
      MQTT.publish(TOPICO_PUBLISH, "OR");
      ligaLeds();          
    }   
      
    if (EstadoSaida == 3){
      MQTT.publish(TOPICO_PUBLISH, "NAND");
      ligaLeds();
    }
      
    if (EstadoSaida == 4){
      MQTT.publish(TOPICO_PUBLISH, "NOR");
      ligaLeds();
    }
      
    if (EstadoSaida == 5){
      MQTT.publish(TOPICO_PUBLISH, "XOR");
      ligaLeds();       
    }
      
    if (EstadoSaida == 6){
      MQTT.publish(TOPICO_PUBLISH, "XNOR");
      ligaLeds();      
    }
      
    //Serial.println("- Estado de saida enviado ao broker!");
    delay(500);
}

void ligaLeds(){
      
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      portasLogicas();
      delay(1000);
      digitalWrite(A, HIGH);
      digitalWrite(B, LOW);
      portasLogicas();
      delay(1000);
      digitalWrite(A, LOW);
      digitalWrite(B, HIGH);
      portasLogicas();
      delay(1000);
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      portasLogicas();
      delay(1000);
      
      
    
}

void portasLogicas(){
  
  pinoA = digitalRead(A);
  pinoB = digitalRead(B);
    
  if (EstadoSaida == 1)
  portaAND();  
  if (EstadoSaida == 2)
  portaOR();
  if (EstadoSaida == 3)
  portaNAND();
  if (EstadoSaida == 4)
  portaNOR();
  if (EstadoSaida == 5)
  portaXOR();
  if (EstadoSaida == 6)
  portaXNOR();
  
}

void portaAND(){
  resultado = (pinoA && pinoB);
  digitalWrite(C, resultado);
  delay(1000);
    
}

void portaOR(){
  resultado = (pinoA||pinoB);  
  digitalWrite(C, resultado);
  delay(1000);
  
}

void portaNAND(){

  resultado = (!(pinoA && pinoB));
  digitalWrite(C, resultado);
  delay(1000);
  
}

void portaNOR(){

  resultado = (!(pinoA || pinoB));
  digitalWrite(C, resultado);
  delay(1000);
  
}

void portaXOR(){

  resultado = (pinoA ^ pinoB);
  digitalWrite(C, resultado);
  delay(1000);
  
}

void portaXNOR(){

  resultado = (!(pinoA ^ pinoB));
  digitalWrite(C, resultado);
  delay(1000);
  
}






