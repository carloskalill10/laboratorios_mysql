#include <EEPROM.h>		// We are going to read and write PICC's UIDs from/to EEPROM
#include <ESP8266WiFi.h> // biblioteca para usar as funções de Wifi do módulo ESP8266
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>  // biblioteca JSON para sistemas embarcados

#include "index.h"

//************************************************************
// Define Area
//************************************************************

#define CONFIG_ID       0x55                    // Used to identify if valid board parameters in EEPROM
#define MSTR_TAG_ID     0x8F                    // Used to identify if there is a master ID in EEPROM

//
// Network configuration
//
#define SSID_LEN        50						// Numero de caracteres para o nome da rede (SSDI)
#define PASS_LEN        30						// Numero de caracteres para a senha da rede
#define DEVNAME_LEN		30						// Numero de caracteres para o nome do dispositivo
#define num_host    15
#define num_lab     5
//
// RFID configurtion
//
#define TOTAL_USERS		4						// Total de usuarios do sistema
#define TAG_AREA_LEN	(4 * TOTAL_USERS + 6)   // 6 = 1 byte for total users, 1 byte for Magic Number and 4 bytes for Master PICC

//
// LED and Button configuration
//
#define COMMON_ANODE

#define LED_ON 	        HIGH
#define LED_OFF         LOW
        
#define PRESSED	        LOW
#define RELAY_ON        LOW
#define RELAY_OFF       HIGH

#define LOCKER_DELAY    300             // Tempo de acionamento da fechadura
//
// NodeMCU pinout
//
#define D0    16				        // User Button
#define D3    0					        // Green LED
#define D2    4					        // SDA pin for MFRC522
#define D1    5					        // Red Led
#define D4    2         		    // RST pin for MFRC522 / User LED
#define D5    14				        // SCK pin for MFRC522
#define D6    12				        // MISO pin for MFRC522
#define D7    13				        // MOSI pin for MFRC522
#define D8    15		            // Locker Relay
#define D9    3		
#define D10   1		
		
//
// MRFC522 pinout
//
#define RST_PIN D4				        // RST-PIN MFRC522 - RFID - SPI - Modulo GPIO2
#define SS_PIN  D2				        // SDA-PIN MFRC522 - RFID - SPI - Modulo GPIO4

//
// EEPROM Addresses definitions
//
#define EE_ID_ADDR      0                     	// The EEPROM adress used to store the ID (to check if sketch already has ran) - 1 byte long
#define EE_WIFI_SSID    EE_ID_ADDR   + 1 	  	// Wifi SSID - SSID_LEN bytes long
#define EE_WIFI_PASS    EE_WIFI_SSID + SSID_LEN	// Wifi PASS - 30 bytes long
#define EE_MAC_ADDR     EE_WIFI_PASS + PASS_LEN // First byte address of the MAC - 6 bytes long
#define EE_IP_ADDR      EE_MAC_ADDR  + 6      	// First byte address of the IP - 4 bytes long
#define EE_GW_ADDR      EE_IP_ADDR   + 4      	// First byte address of the GATEWAY - 4 bytes long
#define EE_MASK_ADDR    EE_GW_ADDR   + 4      	// First byte address of the SUBNETMASK - 4 bytes long
#define EE_DEVNAME 		EE_MASK_ADDR + 4        // First byte address of the Device Name - DEVNAME_LEN bytes long

#define EE_USER_TAG		EE_DEVNAME + DEVNAME_LEN
#define EE_TOTAL_USERS  EE_USER_TAG
#define EE_MSTR_TAG     EE_TOTAL_USERS + 1
#define EE_LAST_ADDR    EE_MSTR_TAG + TAG_AREA_LEN

//************************************************************
// Area para definição de constantes e variáveis globais
//************************************************************

const uint8_t redLed = D1;		        // Set red Led Pin
const uint8_t grnLed = D3;              // Set green Led Pin
const uint8_t relay  = D8;     	        // Set Relay Pin
const uint8_t button = D0;     	        // Button Pin

boolean programMode = false;  	        // initialize programming mode to false

uint8_t successRead;    		        // Variable integer to keep if we have Successful Read from Reader
boolean redLedStatus = LED_OFF;
boolean grnLedStatus = LED_OFF;
uint32_t grnLedTimeout = 0;
uint32_t redLedTimeout = 0;

byte storedCard[4];   			        // Stores an ID read from EEPROM
byte readCard[4];   			        // Stores scanned ID read from RFID Module
byte masterCard[4];   			        // Stores master card's ID read from EEPROM
String readCardStr;   			        // Stores scanned ID read from RFID Module in String format

// Create MFRC522 instance.
MFRC522 mfrc522(SS_PIN, RST_PIN);

const char *SSID_AP = "iot_device";					// Nome da ESP como Access Point
const char *PASS_AP = "ifamcmdi";								// Sem senha no modo Access Point

// endereço IP local do Servidor Web instalado na Raspberry Pi 3
// onde será exibida a página web
char host [num_host]= "192.168.43.68";   
//-----------------------------------------------------------------
//para cada um laboratorio o id_lab será diferente, depende do id cadastrado no laboratório
char id_lab [num_lab]="1"; 
//-----------------------------------------------------------------

char SSID_STA[SSID_LEN] = "motog5splus";				// Nome padrão da rede como Station
char PASS_STA[PASS_LEN] = "carlos098";							// Senha padrao Station
char devname[DEVNAME_LEN]  = "moto g 5s plus";					// Nome padrao do Device

uint8_t mac[]    = {0x00,0x1A,0x90,0x00,0x00,0x01};		// MAC address do dispositivo
IPAddress ip(192,168,0,54);								// IP padrão da ESP
IPAddress subnet(255,255,255,0);						// Máscara de subrede padrão
IPAddress gateway(192,168,0,1);							// Gateway padrão
IPAddress serverIP(192,168,0,39);						// IP do servidor


WiFiClient client;

ESP8266WebServer server(80);

StaticJsonBuffer<300> jsonBuffer;
JsonObject& object = jsonBuffer.createObject();
JsonObject& dado = object.createNestedObject("acess");

//-------------------------------------------------------------------------
/*
 * função que ...
 * 
 */
void setup() 
{
  clientConnect();
  // Board Pin Configuration
  pinMode(redLed, OUTPUT);
  pinMode(grnLed, OUTPUT);
  pinMode(button, INPUT);
  pinMode(relay, OUTPUT);

  // Be careful how relay circuit behave on while resetting or power-cycling your Arduino
  digitalWrite(relay, RELAY_OFF); 		// Make sure door is locked
  digitalWrite(redLed, LED_OFF);  		// Make sure led is off
  digitalWrite(grnLed, LED_OFF);  	    // Make sure led is off

  // Protocol Configuration
  Serial.begin(115200); 				// Initialize serial communications with PC
  SPI.begin();          				// MFRC522 Hardware uses SPI protocol
  mfrc522.PCD_Init();   				// Initialize MFRC522 Hardware

  // If you set Antenna Gain to Max it will increase reading distance
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);

  // EEPROM initialization
  EEPROM.begin(512);

  ESP.eraseConfig();
  WiFi.setAutoConnect(false);
  delay(300); 

  Serial.println("Controle de Acesso Remoto v0.1");

  // Carrega os parametros da placa com os valores default ou a partir da EEPROM
  loadBoardConfig();

  // If the button is pressed while setup load default parameters
  if (monitorWipeButton(3000)) 
  {
    // Write a value different from CONFIG_ID to indicate changes in the parameters
    EEPROM.write(EE_ID_ADDR, 0);

    // ESP entra como Access Point
    setup_AP();

    // Configura o acesso às paginas WEB de configuração
    server.on("/", handleRoot);
    server.on("/load", handleLoad);
    server.onNotFound(handleNotFound);

    // Inicia o servidor WEB da ESP
    server.begin();

    Serial.println("-----------------------------");
    Serial.println("HTTP server started");

    // Pisca o LED vermelho para indicar que a ESP está como AP
    flashLed(redLed, 100, 5, LED_OFF);

    // Aguarda as configurações do usuário via http:\\192.168.4.1
    // Quando o usuário enviar as configurações na página WEB, será escrito
    // o byte CONFIG_ID na EEPROM par sair deste loop
    do
    {
      // Verifica se algum cliente se conectou e responde com a página adequada
      server.handleClient();
    }
    while (EEPROM.read(EE_ID_ADDR) == 0x00);
  }

  // Se não conseguir conectar na rede Wifi como Station configura um Access Point
  if (setup_STATION() == -1)
  {
    Serial.println("-----------------------------");
    Serial.print("Nao foi possivel conectar-se a rede "); Serial.println(SSID_STA);
    Serial.println("Reinicie e presisone o botao para entrar no modo AP");
  }
  
  // Check if master card is defined, if not let user choose a master card
  // You can keep other EEPROM records just write other than MSTR_TAG_ID to EEPROM address 1
  // EEPROM address 1 should hold magical number which is 'MSTR_TAG_ID'
  if (EEPROM.read(EE_USER_TAG + 1) != MSTR_TAG_ID) 
  {
    defineMasterTag();
  }
  else
  {
    Serial.println("-----------------------------");
    Serial.print("Master Card's UID: ");
    
    // Read Master Card's UID from EEPROM to 'masterCard' variable
    for (uint8_t i = 0; i < 4; i++ ) 
    {
      masterCard[i] = EEPROM.read(EE_USER_TAG + 2 + i);
      Serial.print(masterCard[i], HEX);
    }
  }
  
  Serial.println("-----------------------------");
  Serial.println("Everything is ready");
  Serial.println("Waiting PICCs to be scanned");
}


//-------------------------------------------------------------------------
/*
 * função que ...
 * 
 */
void loop() 
{
  // Reseta o WDT da ESP
  ESP.wdtFeed();
  
  // Verifica se houve aproximação de uma TAG
  if (getID())
  {
    // Verifica a conexao Wifi
    if (WiFi.status() == WL_CONNECTED) 
    {
      // Envia a Tag lida pra o servidor
      montaJSON();
      
      // Verifica se o servidor autoriza o acesso
      if (metodoPOST().equals("{\"ok\":\"ok\"}"))
      {
        // Tag cadastrada. Libera o acesso.
        Serial.println("-----------------------------");
        Serial.println("Welcome, You shall pass");
        
        // Libera a fechadura
        granted(LOCKER_DELAY);
      }
      else  // Tag não cadastrada. Acesso negado.
      {
        Serial.println("-----------------------------");
        Serial.println("You shall not pass");
        denied();
      }
    }
    else        // Sem conexão Wifi
    {
      if (programMode) 
        insertRemoveTag();        // Modo Programação. Adiciona ou Deleta a Tag
      else
        checkAccess();            // Modo Normal. Libera ou Bloqueia o acesso.
    }      
  }   
  
  // Verifica se o botão de definição de Tag Mestre ou limpeza de registros
  // está pressionado  
  if (monitorWipeButton(3000)) 
  {
    if (programMode)
      eraseRecords();
    else
      defineMasterTag();
  }

  // Controla o estado dos LEDs de acordo com o contexto
  ledControl();

  //-------------------------------------------------------------------
  //----- DEPURAÇÃO
  //-------------------------------------------------------------------
  if (Serial.available())
  {
    uint8_t dado = Serial.read();
  
    switch (dado)
    {
      case '1':
        eraseEEPROM(EE_ID_ADDR, EE_LAST_ADDR-EE_ID_ADDR, 0xFF);
      break;
  
      case '2':
	    dumpEEPROM(EE_ID_ADDR, EE_USER_TAG + TAG_AREA_LEN);
      break;

      case '3':
      clientConnect();
      break;
      
    }
  }
}
      

//-----------------------------------------------------------------------
/*
 * função que armazena a Tag em um objeto JSON
 * utiliza a biblioteca ArduinoJson
 */
void montaJSON()
{
  dado["lab_id"] = id_lab;
  dado["tag"] = readCardStr;
}


//-----------------------------------------------------------------------
/*
 * função que envia os dados do sensor para o servidor em formato JSON
 * faz um POST request ao servidor 
 */
String metodoPOST()
{
  if(!client.connect(host,3000) )     // aqui conectamos ao servidor
  {
    Serial.print("Nao foi possivel conectar ao servidor!\n");
  }else{    
    Serial.println("Conectado ao servidor");
    // Faz o HTTP POST request    
    client.println("POST /acess HTTP/1.1");
    client.println("Host: 192.168.43.196");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(object.measureLength());
    client.println();
    object.printTo(client);    // envio do JSON
    while (client.connected()) {
     String resposta = client.readStringUntil('\r');
      if (resposta == "\n") {
        break;
      }
    }
    
    String resposta = client.readStringUntil('\r');
    String resp=resposta.substring(1);
    Serial.println(resp);
    return resp; 
  }
}


//-------------------------------------------------------------------------
/*
 * função que deleta todas as Tags cadastradas na EEPROM
 * 
 */
void eraseRecords(void)
{
  // Clear all the records in the EEPROM
  EEPROM.write(EE_USER_TAG + 0, 0);
  EEPROM.commit();
  
  Serial.println("-----------------------------");
  Serial.println("All records in the EEPROM erased");
}

//-------------------------------------------------------------------------
/*
 * função que libera a fechadura eletrônica.
 * 
 */
void granted(uint16_t lockerDelay) 
{
  digitalWrite(redLed, LED_OFF);
  digitalWrite(grnLed, LED_ON);
  
  // Unlock door!
  digitalWrite(relay, RELAY_ON);
  
  // Hold door lock open for given seconds
  delay(lockerDelay);
  
  // Relock door
  digitalWrite(relay, RELAY_OFF);
  
  // Hold green LED on for a second
  delay(1000);
}

//-------------------------------------------------------------------------
/*
 * função que indica que o acesso foi negado, piscando o LED vermelho
 * 
 */
void denied() 
{
  // Make sure green LED is off
  digitalWrite(grnLed, LED_OFF);

  // Flash red LED
  flashLed(redLed, 100, 8, LED_OFF);
}
//------------------------------------------------------------------------
bool clientConnect(){
  if (!client.connect(host, 3000))     // aqui conectamos ao servidor
  {
    Serial.println("-----------------------------");
    Serial.print("Nao foi possivel conectar ao servidor!\n");
    return ("");
  }
  else
  {
    Serial.println("-----------------------------");
    Serial.println("Conectado ao servidor");  
  }
}

//-------------------------------------------------------------------------
/*
 * função que ...
 * 
 */
uint8_t getID() 
{
  // If a new PICC placed to RFID reader continue
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return (0);
  }
  
  //Since a PICC placed get Serial and continue
  if (!mfrc522.PICC_ReadCardSerial()) 
  {
    return (0);
  }
  
  // Read the PICC to 'readCard' variable
  Serial.println("-----------------------------");
  Serial.print("Scanned PICC's UID: ");
  
  readCardStr = "";
  
  for (uint8_t i = 0; i < 4; i++) 
  {
    readCard[i] = mfrc522.uid.uidByte[i];
    
    // Transforma o byte para string
    readCardStr.concat(String(readCard[i] < 0x10 ? "0" : ""));
    readCardStr.concat(String(readCard[i], HEX));
  }
  readCardStr.toUpperCase();
  Serial.println(readCardStr);
  
  // Stop reading
  mfrc522.PICC_HaltA();
  
  return (1);
}

//-------------------------------------------------------------------------
/*
 * função que ...
 * 
 */
void ShowReaderDetails() 
{
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  
  Serial.println("-----------------------------");
  Serial.print("MFRC522 Software Version: 0x");
  Serial.print(v, HEX);
  
  if (v == 0x91) Serial.println(" = v1.0");
  else if (v == 0x92) Serial.println(" = v2.0");
  else Serial.println(" (unknown), probably a chinese clone?");

  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) 
  {
    Serial.println("WARNING: Communication failure, is the MFRC522 properly connected?");
    Serial.println("SYSTEM HALTED: Check connections.");

    // Visualize system is halted
    digitalWrite(grnLed, LED_OFF);      // Make sure green LED is off
    digitalWrite(redLed, LED_ON);       // Turn on red LED
    
    // do not go further
    while (true);
  }
}


//-------------------------------------------------------------------------
/*
 * função que ...
 * 
 */
void cycleLeds() 
{
  digitalWrite(redLed, LED_OFF);    // Make sure red LED is off
  digitalWrite(grnLed, LED_ON);     // Make sure green LED is on
  delay(200);

  digitalWrite(redLed, LED_ON);     // Make sure red LED is on
  digitalWrite(grnLed, LED_OFF);    // Make sure green LED is off
  delay(200);
}


//-------------------------------------------------------------------------
/*
 * função que ...
 * 
 */
void normalModeOn() 
{
  digitalWrite(redLed, LED_OFF);        // Make sure Red LED is off
  digitalWrite(grnLed, grnLedStatus);   // Make sure Green LED is off
  digitalWrite(relay, HIGH);            // Make sure Door is Locked
  
  if (grnLedStatus == LED_ON)
  {
    if (millis() - grnLedTimeout > 50)
    {
      grnLedTimeout = millis();
      grnLedStatus = LED_OFF;
    }
  }    
  else if (millis() - grnLedTimeout > 1000)
  {
    grnLedTimeout = millis();
    grnLedStatus = LED_ON;
  }
}


//-------------------------------------------------------------------------
/*
 * função que Read ID from EEPROM
 * 
 */
void readID(uint8_t number) 
{
  // Figure out starting position
  uint8_t start = (number * 4 ) + 2;
  
  for ( uint8_t i = 0; i < 4; i++ ) 
  {
    // Assign values read from EEPROM to array
    storedCard[i] = EEPROM.read(EE_USER_TAG + start + i);
  }
}


//-------------------------------------------------------------------------
/*
 * função que Add ID to EEPROM
 * 
 */
void writeID(byte a[]) 
{
  // Before we write to the EEPROM, check to see if we have seen this card before!
  if (!findID(a)) 
  {
    // Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t num = EEPROM.read(EE_USER_TAG + 0);
    
    // Figure out where the next slot starts (6 = 2 bytes config + 4 UID)
    uint8_t start = (num * 4) + 6;
    
    // Increment the number of ID cards stored and write the new count to the counter
    EEPROM.write(EE_USER_TAG + 0, num + 1);
    
    // Write the array values to EEPROM in the right position
    for ( uint8_t j = 0; j < 4; j++ )
    {
      EEPROM.write(EE_USER_TAG + start + j, a[j]);
    }
    EEPROM.commit();
    
    // Sinaliza sucesso na escrita
    digitalWrite(redLed, LED_OFF);
    flashLed(grnLed, 100, 8, LED_OFF);
   
    Serial.println("-----------------------------");
    Serial.println("Succesfully added ID record to EEPROM");
  }
  else 
  {
    // Sinaliza falha na escrita
    digitalWrite(grnLed, LED_OFF);
    flashLed(redLed, 100, 5, LED_OFF);
    
    Serial.println("-----------------------------");
    Serial.println("Failed! There is something wrong with ID or bad EEPROM");
  }
}


//-------------------------------------------------------------------------
/*
 * função que Remove ID from EEPROM
 * 
 */
void deleteID(byte a[])
{
  uint8_t slot;       // Figure out the slot number of the card
  uint8_t start;      // Figure out where the next slot starts
  uint8_t looping;    // The number of times the loop repeats
  uint8_t j;

  // Before we delete from the EEPROM, check to see if we have this card!
  if (!findID(a)) 
  {
    // Sinaliza falha por não encontrar o ID na EEPROM
    digitalWrite(grnLed, LED_OFF);
    flashLed(redLed, 100, 5, LED_OFF);
    
    Serial.println("-----------------------------");
    Serial.println("Failed! There is something wrong with ID or bad EEPROM");
  }
  else 
  {
    // Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t num = EEPROM.read(EE_USER_TAG + 0);
    
    // Figure out the slot number of the card to delete
    slot = findIDSlot(a);
    start = (slot * 4) + 2;
    looping = ((num - slot) * 4);
    
    // Write the new count to the counter
    EEPROM.write(EE_USER_TAG + 0, num-1);
    
    // Loop the card shift times
    for (j = 0; j < looping; j++ ) 
    {
      // Reseta o WDT da ESP
      ESP.wdtFeed();

      // Shift the array values to 4 places earlier in the EEPROM
      EEPROM.write(EE_USER_TAG + start + j, EEPROM.read(EE_USER_TAG + start + 4 + j));
    }
    
    // Shifting loop
    for (uint8_t k = 0; k < 4; k++ ) 
    {
      EEPROM.write(EE_USER_TAG + start + j + k, 0);
    }
    EEPROM.commit();
    
    Serial.println("-----------------------------");
    Serial.println("Succesfully removed ID record from EEPROM");
    
    // Pisca o LED verde para indicar que a Tag foi removida com sucesso
    flashLed(grnLed, 100, 8, LED_OFF);
  }
}


//-------------------------------------------------------------------------
/*
 * função que Compare Two PICCs
 * 
 */
boolean comparePICC(byte a[], byte b[]) 
{
  boolean match = true;       // Assume they match at first

  for (uint8_t k = 0; k < 4; k++) 
  {
    if (a[k] != b[k]) match = false;
  }

  return (match);
}


//-------------------------------------------------------------------------
/*
 * função que Print a Tag
 * 
 */
void printTag(byte tag[])
{
  for (uint8_t uid = 0; uid < 4; uid++)
  {
    Serial.print(tag[uid], HEX); Serial.print(" ");
  }
}


//-------------------------------------------------------------------------
/*
 * função que Find Slot
 * 
 */
uint8_t findIDSlot(byte find[]) 
{
  // Get the numer of used spaces, position 0 stores the number of ID cards
  uint8_t count = EEPROM.read(EE_USER_TAG + 0);
  
  // Loop once for each EEPROM entry. Starts from ID card 1 because 0 is the Master
  for (uint8_t i = 1; i <= count; i++ ) 
  {
    // Reseta o WDT da ESP
    ESP.wdtFeed();
    
    // Read an ID from EEPROM, it is stored in 'storedCard'
    readID(i);
    
    // Check to see if the storedCard read from EEPROM is the same 
    // as the find[] ID card passed
    if (comparePICC(find, storedCard)) 
    {
      return (i);       // The slot number of the card
      break;            // Stop looking we found it
    }
  }
}


//-------------------------------------------------------------------------
/*
 * função que Find ID From EEPROM
 * 
 */
boolean findID(byte find[])
{
  // Get the numer of used spaces, position 0 stores the number of ID cards
  uint8_t count = EEPROM.read(EE_USER_TAG + 0);

  // Loop once for each EEPROM entry
  for (uint8_t i = 1; i <= count; i++) 
  {
    // Reseta o WDT da ESP
    ESP.wdtFeed();

    // Read an ID from EEPROM, it is stored in 'storedCard'
    readID(i);

    // Check to see if the storedCard read from EEPROM
    if (comparePICC(find, storedCard)) 
    {
      return true;
      break;  // Stop looking we found it
    }
  }
  return false;
}


//-------------------------------------------------------------------------
/*
 * função que pisca um LED por N vezes com delay programavel
 * 
 */
void flashLed(uint8_t ledPin, uint16_t flashDelay, uint8_t times, boolean lastState) 
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(ledPin, LED_ON); 
    delay(flashDelay);
    digitalWrite(ledPin, LED_OFF);  
    delay(flashDelay);
  }

  digitalWrite(ledPin, lastState);  
}


//-------------------------------------------------------------------------
/*
 * função que Check if the ID passed is the master card
 * 
 */
boolean isMaster(byte test[]) 
{
  if (comparePICC(test, masterCard))
    return true;
  else
    return false;
}

//-------------------------------------------------------------------------
/*
 * função que Check if the button is pressed for a number of milliseconds
 * 
 */
bool monitorWipeButton(uint32_t interval) 
{
  uint32_t now = (uint32_t)millis();
  
  // If the Button (button) Pressed while setup run (powered on) it wipes EEPROM
  if (digitalRead(button) == PRESSED) 
  {
    // Give some feedback
    digitalWrite(redLed, LED_ON);
    digitalWrite(grnLed, LED_OFF);

    Serial.println("-----------------------------");
    Serial.println("Button Pressed");

    while ((uint32_t)millis() - now < interval)  
    {
      // Reseta o WDT da ESP
      ESP.wdtFeed();
    
      // check on every half a second
      if (((uint32_t)millis() % 500) == 0) 
      {
        if (digitalRead(button) != PRESSED) 
        {
          Serial.print("Button released before "); 
          Serial.print(interval/1000);
          Serial.println(" seconds");
          return false;
        }
      }
    }
    return true;
  }
  return false;
}

//-------------------------------------------------------------------------
/*
 * função que Defines the Master Card
 * 
 */
bool defineMasterTag(void)
{
  Serial.println("-----------------------------");
  Serial.println("Scan A PICC to Define as Master Card");

  // Program will not go further while you not get a successful read
  do 
  {
    // Reseta o WDT da ESP
    ESP.wdtFeed();
      
    // sets successRead to 1 when we get read from reader otherwise 0
    successRead = getID();
      
    // Visualize Master Card need to be defined
    digitalWrite(grnLed, LED_ON);  delay(100);
    digitalWrite(grnLed, LED_OFF); delay(100);
  }
  while (!successRead);

  // Resets successRead variable
  successRead = false;
  
  // Write scanned PICC's UID to EEPROM, start from address 2
  for (uint8_t j = 0; j < 4; j++) 
  {
    EEPROM.write(EE_USER_TAG + 2 + j, readCard[j]);
  }

  // Write to EEPROM we defined Master Card.
  EEPROM.write(EE_USER_TAG + 1, MSTR_TAG_ID);
  EEPROM.commit();

  Serial.print("Master Card's UID: ");
  
  // Read Master Card's UID from EEPROM and write it to 'masterCard'
  for (uint8_t i = 0; i < 4; i++) 
  {
    masterCard[i] = EEPROM.read(EE_USER_TAG + 2 + i);
    Serial.print(masterCard[i], HEX); Serial.print(" ");
  }
  
  Serial.println();
}

//-------------------------------------------------------------------------
/*
 * função que Verifica uma tentativa de acesso
 * 
 */
void checkAccess(void)
{
  // Se houve tentativa de acesso com a Tag Master, entra no modo de programação
  if (isMaster(readCard)) 
  {
    // Faz a leitura da quantidade de registros na EEPROM
    uint8_t count = EEPROM.read(EE_USER_TAG + 0);
  
    // Indica que deve entrar no modo de programação
    programMode = true;
    Serial.println("-----------------------------");
    Serial.println("Hello Master - Entered Program Mode");
    Serial.print("I have "); Serial.print(count); Serial.println(" record(s) on EEPROM");
    Serial.println("Scan a PICC to ADD or REMOVE to EEPROM");
    Serial.println("Scan Master Card again to Exit Program Mode");
  }
  else  // A tentativa de acesso não foi com a Tag Master
  {
    // Verifica se a Tag está cadastrada na EEPROM
    if (findID(readCard)) 
    {
      // Tag cadastrada. Libera o acesso.
      Serial.println("-----------------------------");
      Serial.println("Welcome, You shall pass");
      
      // Open the door lock for LOCKER_DELAY ms
      granted(LOCKER_DELAY);
    }
    else  // Tag não cadastrada. Acesso negado.
    {
      Serial.println("-----------------------------");
      Serial.println("You shall not pass");
      denied();
    }
  }
}

//-------------------------------------------------------------------------
/*
 * função que Adiciona ou Deleta uma Tag da EEPROM
 * 
 */
void insertRemoveTag(void)
{
  // Se está no modo de programação e foi detectada a Tag Mestre, 
  // então sai do modo de programação
  if (isMaster(readCard)) 
  {
    Serial.println("-----------------------------");
    Serial.println("Master Card Scanned");
    Serial.println("Exiting Program Mode");
    programMode = false;
  }
  else // Se não é a Tag Mestre, adiciona ou deleta a Tag
  {
    // Se a Tag existe, deleta a mesma
    if (findID(readCard))
    {
      Serial.println("-----------------------------");
      Serial.println("I know this PICC, removing...");
      deleteID(readCard);
      Serial.println("Scan a PICC to ADD or REMOVE to EEPROM");
    }
    else     // Se a Tag não existe, adiciona a mesma
    {
      // Lê a quantidade de Tags cadastradas na EEPROM
      uint8_t count = EEPROM.read(EE_USER_TAG + 0);
  
      // Verifica se tem espaço para adicionar a Tag
      if (count < TOTAL_USERS)
      {
        Serial.println("-----------------------------");
        Serial.println("I do not know this PICC, adding...");
        
        // Escreve a Tag
        writeID(readCard);
        
        Serial.println("Scan a PICC to ADD or REMOVE to EEPROM");
      }
      else        
      {
        Serial.println("-----------------------------");
        Serial.println("EEPROM memory is FULL.");
        Serial.println("Remove a PICC from memory before adding another one");
      }
    }
  }
}

//-------------------------------------------------------------------------
/*
 * função que Erase EEPROM
 * 
 */
void eraseEEPROM(uint16_t beginAddr, uint16_t length, uint8_t value)
{
  // Loop end of EEPROM address
  for (uint16_t address = beginAddr; address < beginAddr + length; address++) 
  {
    // Reseta o WDT da ESP
    ESP.wdtFeed();

    if (EEPROM.read(address) != value)
    {
      EEPROM.write(address, value);
    }
  }
  EEPROM.commit();
}

//-------------------------------------------------------------------------
/*
 * função que Dump EEPROM
 * 
 */
void dumpEEPROM(uint16_t beginAddr, uint16_t endAddr)
{
	char tmp[10];

	// Read a memory
	Serial.print("\r\nReading EEPROM memory\r\n");
	Serial.print("        ");

	for (byte i = 0; i < 16; i++)
	{
		sprintf(tmp, "0x%02X ", i);
		Serial.print(tmp);
	}

	Serial.print("\r\n---------------------------------------------------------------------------------------\r\n");

	// Imprime o endereço inicial da linha
	Serial.print("0x0000: ");
	uint16_t addrCounter = 0;

	for (uint16_t address = beginAddr; address < 512; address++)
	{
        // Reseta o WDT da ESP
        ESP.wdtFeed();

        if (address == endAddr) break;

		sprintf(tmp, "0x%02X ", EEPROM.read(address));
		Serial.print(tmp);

		addrCounter++;

		// 16 bytes por linha
		if (addrCounter == 16)
		{
			addrCounter = 0;

			// Imprime o endereço inicial da linha
			Serial.println();
			sprintf(tmp, "0x%04X: ", address+1);
			Serial.print(tmp);
		}
	}
	Serial.println();
}

//-------------------------------------------------------------------------
/*
 * função que Controla o estado dos LEDs
 * 
 */
void ledControl()
{
  // Program Mode cycles through Red Green Blue waiting to read a new card
  if (programMode) 
  {
    cycleLeds();
  }
  else 
  {
    normalModeOn();     // Normal mode, flahs green LED, all others are off
  }
}


/*
 *=============================================================================
 *NAME: loadBoardConfig
 *
 *DESCRIPTION: Verifica se tem parâmetros armazenados na EEPROM e caso positivo
 *             faz a leitura dos mesmos e armazena nas variáveis correspondentes
 *=============================================================================
*/
void loadBoardConfig(void)
{
  int index;
  
  if (EEPROM.read(EE_ID_ADDR) == CONFIG_ID)
  {
    Serial.println();
    Serial.print("Carregando as configuracoes do dispositivo ... ");

	// Load WIFI SSID
    for (index = 0; index < SSID_LEN; index++){
      SSID_STA[index] = EEPROM.read(EE_WIFI_SSID + index);
    }
	
    // Load WIFI PASS
    for (index = 0; index < PASS_LEN; index++){
      PASS_STA[index] = EEPROM.read(EE_WIFI_PASS + index);
    }
	
    // Load MAC Address from EEPROM
    for (index = 0; index < 6; index++){
      mac[index] = EEPROM.read(EE_MAC_ADDR + index);
    }

    // Load IP Address from EEPROM
    for (index = 0; index < 4; index++){
      ip[index] = EEPROM.read(EE_IP_ADDR + index);
    }

    // Load SUBNET Address from EEPROM
    for (index = 0; index < 4; index++){
      subnet[index] = EEPROM.read(EE_MASK_ADDR + index);
    }

    // Load Gateway Address from EEPROM
    for (index = 0; index < 4; index++){
      gateway[index] = EEPROM.read(EE_GW_ADDR + index);
    }
	
	// Load Device Name
    for (index = 0; index < DEVNAME_LEN; index++){
      devname[index] = EEPROM.read(EE_DEVNAME + index);
    }
  }
  else
  {
    Serial.println();
    Serial.print("Carregando as configuracoes padrao do dispositivo ... ");
  }
  
  delay(300);
  Serial.println("OK!");
}


/*
 *=============================================================================
 *NAME: saveBoardConfig
 *
 *DESCRIPTION: Salva os parametros de configuração da placa na EEPROM
 *=============================================================================
*/
void saveBoardConfig(void)
{
  int index;
  
  Serial.println();
  Serial.print("Salvando as configuracoes do dispositivo ... ");

  // Save Wifi SSID in EEPROM
  for (index = 0 ; index < SSID_LEN; index++)
    EEPROM.write(EE_WIFI_SSID + index, SSID_STA[index]);
  
  // Save Wifi PASS in EEPROM
  for (index = 0 ; index < PASS_LEN; index++)
    EEPROM.write(EE_WIFI_PASS + index, PASS_STA[index]);
  
  // Save MAC Address in EEPROM
  for (index = 0 ; index < 6; index++)
    EEPROM.write(EE_MAC_ADDR + index, mac[index]);

  // Save IP Address in EEPROM
  for (index = 0 ; index < 4; index++)
    EEPROM.write(EE_IP_ADDR + index, ip[index]);

  // Save Subnet Mask in EEPROM
  for (index = 0 ; index < 4; index++)
    EEPROM.write(EE_MASK_ADDR + index, subnet[index]);

  // Save Gateway Address in EEPROM
  for (index = 0 ; index < 4; index++)
    EEPROM.write(EE_GW_ADDR + index, gateway[index]);

  // Save Device Name to EEPROM
  for (index = 0 ; index < DEVNAME_LEN; index++)
    EEPROM.write(EE_DEVNAME + index, devname[index]);

  // set ID to the known bit, so when you reset the Arduino is will use the EEPROM values
  EEPROM.write(EE_ID_ADDR, CONFIG_ID); 
  EEPROM.commit();

  delay(300);
  Serial.println("OK!");
}


//-------------------------------------------------------------------------
/*
 * função que conecta o NodeMCU na rede Wifi
 * SSID e PASSWORD devem ser indicados nas variáveis
 */
void reconnectWiFi() 
{
  if (WiFi.status() == WL_CONNECTED) return;

  WiFi.begin(SSID_STA, PASS_STA);

  while(WiFi.status() != WL_CONNECTED) 
  {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso na rede: "); Serial.println(SSID_STA);
  Serial.print("IP obtido: "); Serial.println(WiFi.localIP());  
}

//-------------------------------------------------------------------------
/*
 * função que ...
 * 
 */
void initWiFi()
{
  delay(10);
  Serial.print("Conectando-se na rede: "); Serial.println(SSID_STA);
  Serial.println("Aguarde ...");
  reconnectWiFi();
}


/*
 *=============================================================================
 *NAME: setup_AP      
 *
 *DESCRIPTION: Configura a ESP como ponto de acesso. O nome da rede será a que
               está indicada na constante SSID_AP e a senha é a que está indica-
			   da na constante PASS_AP
 *=============================================================================
*/
void setup_AP()
{
  Serial.println("-----------------------------");
  Serial.println("Configurando a ESP como Ponto de Acesso ...");

  WiFi.disconnect(true);
  delay(300);
  WiFi.mode(WIFI_AP);

  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(SSID_AP, PASS_AP);
  Serial.print("Acesse http:\\\\"); Serial.print(WiFi.softAPIP());
  Serial.println(" para alterar os parametros do dispositivo");
}


/*
 *=============================================================================
 *NAME: setup_STATION
 *
 *DESCRIPTION: COnfigura a ESP como Station e tenta se conectar na rede
 *             indicada na variável SSID_STA e o password é o que está indica-
 *             do na variável PASS_STA
 *=============================================================================
*/
int setup_STATION()
{
  if (WiFi.status() == WL_CONNECTED) return 0;

  Serial.println();
  Serial.println("Configurando a ESP como station...");

  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.begin(SSID_STA, PASS_STA);
  delay(100);
  //WiFi.config(ip, gateway, subnet);
  
  // Wait for connection
  double initTime = millis();
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
	delay(200);

    if ((millis() - initTime) > 8000) 
	{
	  Serial.println();
      Serial.println("Connection Failed!");
      return -1;
    }  
  }
  
  Serial.println();
  Serial.println("Conectado!");
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
  //Serial.print("SubnetMask: "); Serial.println(WiFi.subnetMask());
  //Serial.print("Gateway Ip: "); Serial.println(WiFi.gatewayIP());
  
  Serial.println();
  return 0;
}


/*
 *=============================================================================
 *NAME:         handleLoad
 *
 *DESCRIPTION:  Esta função é chamada sempre que a página CONF_page é exibida
 *              É responsável por enviar os parametros da placa para que sejam
 *              exibidos no browser
 *=============================================================================
*/
void handleLoad() 
{
  String data;

  data = SSID_STA;
  data += " ";
  data += PASS_STA;
  data += " ";
  data += ip.toString();
  data += " ";
  data += subnet.toString();
  data += " ";
  data += gateway.toString();
  data += " ";
  data += devname;
  data += " ";
  data += host;
  data += " ";
  data += id_lab;
  
  Serial.println(data);     
  server.send(200, "text/text", data);  
}


/*
 *=============================================================================
 *NAME:         handleRoot
 *
 *DESCRIPTION:  Essa função é chamada para exibir a página CONF_page. 
 *              Se o usuário acessar o endereçp http://ip_da_placa/ no Browser,
 *              então a página será exibida e mostrará os valores atuais dos
 *              parâmetros do dispositivo. Se o usuário clicar no botão "Envia"
 *              da página CONF_page, então os parâmetros indicados nos campos
 *              serão salvos na EEPROM e a página CONFIRMATION_page será exibi-
 *              da para indicar que os parâmetros foram enviados com sucesso.
 *=============================================================================
*/
void handleRoot()
{
  String sArg;
  
  Serial.println("Handle Config");
  
  // Verifica se vieram os parâmetros. Isso significa que o usuario pressionou
  // o botão "Enviar" na página "CONF_page"
  if (server.hasArg("rede"))
  {
	    sArg = server.arg("rede");
      Serial.println(sArg);
      sArg.toCharArray(SSID_STA, SSID_LEN);   

      sArg = server.arg("pass");
      Serial.println(sArg);
      sArg.toCharArray(PASS_STA, PASS_LEN);

      ip.fromString(server.arg("ip"));           
      Serial.println(ip);

      subnet.fromString(server.arg("subn"));
      Serial.println(subnet);

      gateway.fromString(server.arg("gatw"));
      Serial.println(gateway);                         

	    sArg = server.arg("devname");
      Serial.println(sArg);
      sArg.toCharArray(devname, DEVNAME_LEN);   

      sArg = server.arg("host");
      Serial.println(sArg);
      sArg.toCharArray(host, num_host);   

      sArg = server.arg("id_lab");
      Serial.println(sArg);
      sArg.toCharArray(id_lab, num_lab);   

      saveBoardConfig();
 	  server.send(200, "text/html", CONFIRMATION_page);
   }
   else	// Usuario acessou http://ip_da_placa/
   {
	  server.send(200, "text/html", CONF_page);
   }
}


/*
 *=============================================================================
 *NAME:         handleNotFound
 *
 *DESCRIPTION:  Essa função é usada pelo servidor web e é responsavel por
 *              enviar uma página de erro.
 *=============================================================================
*/
void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}


