//Programa que simula portas lógicas - para a disciplina de Circuitos Digitais
//Desenvolvido por: Gleison J Silva em 22/09/17

#include <ESP8266WiFi.h> // Importa a Biblioteca ESP8266WiFi
#include <PubSubClient.h> // Importa a Biblioteca PubSubClient
 
//defines:
//defines de id mqtt e tópicos para publicação e subscribe
#define TOPICO_SUBSCRIBE "CircuitosDigitais/PortasLogicas"     //tópico MQTT de escuta
#define TOPICO_PUBLISH   "CircuitosDigitais/PortasLogicas/ESP8266"    //tópico MQTT de envio de informações para Broker
#define ID_MQTT  "CD"     //id mqtt (para identificação de sessão)
#define A 2;
#define B 3;
#define saida 4;

int estadoEntradaA = 0; // Inicializando a variavel de estado da entrada digital A
int estadoEntradaB = 0; // Inicializando a variavel de estado da entrada digital B
int resultado = 0; // Inicializando a variavel de estado da saída digital
                               

// WIFI
const char* SSID = "G&V"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "jsilva996**"; // Senha da rede WI-FI que deseja se conectar
 
// MQTT
const char* BROKER_MQTT = "192.168.0.14"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT
 
 
//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient
char EstadoSaida = '0';  //variável que armazena o estado atual da saída
 
//Prototypes
void initSerial();
void initWiFi();
void initMQTT();
void reconectWiFi(); 
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void);

 
void setup() 
{
    //inicializações:
    InitOutput();
    initSerial();
    initWiFi();
    initMQTT();
    initpinMode();
   
    
}

void initpinMode(){
   pinMode(A, OUTPUT);
   pinMode(B, OUTPUT);
   pinMode(saida, OUTPUT);
   digitalWrite(A, LOW);
   digitalWrite(B, LOW);
   digitalWrite(saida, LOW);
}

void initSerial() 
{
    Serial.begin(115200);
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
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
    MQTT.setCallback(mqtt_callback);            //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
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
    //verifica se deve colocar nivel alto de tensão na saída D0:
    
    if (msg.equals("AND"))
    {
      resultado = estadoEntradaA && estadoEntradaB;
      digitalWrite(saida, resultado);
      delay(500)
        
        EstadoSaida = '1';
    }
 
    //verifica se deve colocar nivel alto de tensão na saída D0:
    if (msg.equals("OR"))
    {
      resultado = estadoEntradaA || estadoEntradaB;
      digitalWrite(saida, resultado);
      delay(500)
      EstadoSaida = '2';
    }

    if (msg.equals("NAND"))
    {
      resultado = !(estadoEntradaA && estadoEntradaB);
      digitalWrite(saida, resultado);
      delay(500)
      EstadoSaida = '3';
    }

     if (msg.equals("NOR"))
    {
      resultado = !(estadoEntradaA || estadoEntradaB);
      digitalWrite(saida, resultado);
      delay(500)
      EstadoSaida = '4';
    }

    if (msg.equals("XOR"))
    {
      resultado = estadoEntradaA ^ estadoEntradaB;
      digitalWrite(saida, resultado);
      delay(500)
      EstadoSaida = '5';
    }

     if (msg.equals("XNOR"))
    {
      resultado = !(estadoEntradaA ^ estadoEntradaB);
      digitalWrite(saida, resultado);
      delay(500)
      EstadoSaida = '6';
    }
    
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
            delay(2000);
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
    if (EstadoSaida == '1')
      MQTT.publish(TOPICO_PUBLISH, "AND");
 
    if (EstadoSaida == '2')
      MQTT.publish(TOPICO_PUBLISH, "OR");

    if (EstadoSaida == '3')
      MQTT.publish(TOPICO_PUBLISH, "NAND");

    if (EstadoSaida == '4')
      MQTT.publish(TOPICO_PUBLISH, "NOR");

    if (EstadoSaida == '5')
      MQTT.publish(TOPICO_PUBLISH, "XOR");

    if (EstadoSaida == '6')
      MQTT.publish(TOPICO_PUBLISH, "XNOR");
 
    Serial.println("- Estado de saida enviado ao broker!");
    delay(200);
}
 

void estadoLed(){
  
    estadoEntradaA = digitalRead(A); // Lê o estado da entrada A
    estadoEntradaB = digitalRead(B); // Lê o estado da entrada B

}


 
//programa principal
void loop() 
{   
    VerificaConexoesWiFIEMQTT();
    EnviaEstadoOutputMQTT();
    MQTT.loop();
    void estadoLed();
}



