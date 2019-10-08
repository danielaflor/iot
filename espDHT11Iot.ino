#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>

#define DHTTYPE DHT11
//#define pDHT 4
#define pLED 18

//const char* ssid = "Capel";
//const char* password = "14margaridas";
const char* ssid = "Robotica";
const char* password = "robotica";

//const char* ssid = "BL02_ALUNOS_INF05";
//const char* ssid = "CellDEF";
//const char* password = "dflor1917";

const char* mqttServer = "soldier.cloudmqtt.com";
const char* mqttID = "minicurso";
const char* mqttUser = "zycrtzhq";
const char* mqttPass = "speD0yMNRnqc";
const int   mqttPort = 11321;

const int intervaloLeitura = 1000;
int   cont = 0;
float vUmidade = 0;
float vTemperatura = 0;

char  iTemperatura[8];
char  iUmidade[8];

unsigned long ultimaLeitura = 0;
unsigned long leituraAtual = 0;
long intervaloSensores = 5000;

//DHT dht(pDHT, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

//--------------------------------------------------------------------------
void setupWifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando à rede ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  //WiFi.begin(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi conectado - endereço IP do ESP: ");
  Serial.println(WiFi.localIP());
}
//--------------------------------------------------------------------
void reconectaMQTT() {
  while (!client.connected()) {
    Serial.print("Conexão MQTT ");

    if (client.connect(mqttID, mqttUser, mqttPass)) {
      Serial.println("concluída");
      client.subscribe("minicurso/led");
    } else {
      Serial.print("falhou. Código de erro =");
      Serial.print(client.state());
      Serial.println(", nova tentativa em 5 segundos!");
      delay(5000);
    }
  }
}
//---------------------------------------------------------------------
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem chegou ao tópico: ");
  Serial.print(topic);
  Serial.print(". Mensagem: ");
  String msg;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    msg += (char)payload[i];
  }
  Serial.println();
  if (String(topic) == "minicurso/led") {
    Serial.print("Alterando o LED para ");
    if (msg == "true") {
      Serial.println("aceso");
      digitalWrite(pLED, HIGH);
    }
    else if (msg == "false") {
      Serial.println("apagado");
      digitalWrite(pLED, LOW);
    }
  }
}
//--------------------------------------------------------------------
/*
void lerSensores() {
  vUmidade = dht.readHumidity();
  vTemperatura = dht.readTemperature();
  if (isnan(vUmidade) || isnan(vTemperatura)) {
    Serial.println("Falha ao ler o sensor DHT!");
    return;
  }
}*/
//--------------------------------------------------------------------
void exibirDados() {
  Serial.println();
  cont++;
  Serial.print(cont);
  Serial.println("a Leitura!");
  Serial.print("    Umidade..........: ");
  //Serial.print(iUmidade);
  Serial.println("%");
  Serial.print("    Temperatura......: ");
  //Serial.print(iTemperatura);
  Serial.println("*C ");
}
//--------------------------------------------------------------------
void publicarDados() {
  //dtostrf(vTemperatura, 1, 2, iTemperatura);
  //dtostrf(vUmidade, 1, 2, iUmidade);
  //client.publish("minicurso/temperatura", iTemperatura);
  //client.publish("minicurso/umidade", iUmidade);
}
//--------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  pinMode(pLED,OUTPUT);
 // dht.begin();
  setupWifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}
//---------------------------------------------------------------------------
void loop() {

  if (!client.connected()) {
    reconectaMQTT();
  }
  if (!client.loop()) {
    client.connect(mqttID);
  }

  leituraAtual = millis();

  if (leituraAtual - ultimaLeitura > intervaloSensores) {
    ultimaLeitura = leituraAtual;
//    lerSensores();
    publicarDados();
    exibirDados();
  }
}
