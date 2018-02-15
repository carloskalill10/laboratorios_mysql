#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#include "index.h"

//************************************************************
// Define Area
//************************************************************

// EEPROM Addresses

#define ID_LEN          50		// Numero de caracteres para o nome da rede (SSDI)
#define PASS_LEN        30		// Numero de caracteres para a senha da rede
#define DEVNAME_LEN		30		// Numero de caracteres para o nome do dispositivo

#define EE_ID_ADDR      0                     	// The EEPROM adress used to store the ID (to check if sketch already has ran) - 1 byte long
#define EE_WIFI_SSID    EE_ID_ADDR   + 1 	  	// Wifi SSID - ID_LEN bytes long
#define EE_WIFI_PASS    EE_WIFI_SSID + ID_LEN 	// Wifi PASS - 30 bytes long
#define EE_MAC_ADDR     EE_WIFI_PASS + PASS_LEN // First byte address of the MAC - 6 bytes long
#define EE_IP_ADDR      EE_MAC_ADDR  + 6      	// First byte address of the IP - 4 bytes long
#define EE_GW_ADDR      EE_IP_ADDR   + 4      	// First byte address of the GATEWAY - 4 bytes long
#define EE_MASK_ADDR    EE_GW_ADDR   + 4      	// First byte address of the SUBNETMASK - 4 bytes long
#define EE_DEVNAME 	  	EE_MASK_ADDR + 4        // First byte address of the Device Name - DEVNAME_LEN bytes long
#define ID              0x00                  	// Used to identify if valid date in EEPROM the "know" bit,
												// if this is written in EEPROM the sketch has ran before
//defines - mapeamento de pinos do NodeMCU
#define D0    16
#define D1    5
#define D2    4
#define D3    0
#define D4    2         // Led do usuário
#define D5    14
#define D6    12
#define D7    13
#define D8    15
#define D9    3
#define D10   1

// Parâmetros Default da placa
// Estes serão sobrescritos caso a placa ja tenha algo armazenado na EEPROM
const char *ssid_AP     = "iot_device";
const char *password_AP = "";

char ssid_STAT[ID_LEN]       = "cmdi_wlan";				// Nome padrão da rede como Station
char password_STAT[PASS_LEN] = "IFAM_CMDI";				// Senha padrao Station
char devname[DEVNAME_LEN]  = "*****";					// Nome padrao do Device

uint8_t mac[]    = {0x00,0x1A,0x90,0x00,0x00,0x01};
IPAddress ip(192,168,0,54);
IPAddress subnet(255,255,255,0);
IPAddress gateway(192,168,0,1);

ESP8266WebServer server(80);


/*
 *=============================================================================
 *NAME:         void setup(void){
 *
 *DESCRIPTION:  Essa é a função de inicialização do sistema, nessa função
 *              é feita a configuração de todoo hardware que será utilizado
 *=============================================================================
*/
void setup(void)
{
  delay(2000);

  Serial.begin(115200);

  EEPROM.begin(512);  

  // Carrega os parametros da placa
  readBoardConfig();
  
  ESP.eraseConfig();
  WiFi.setAutoConnect(false);
  delay(300); 

  if (setup_STATION() == -1){
    setup_AP();
  }

  server.on("/", handleRoot);
  server.on("/load", handleLoad);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
 }


/*
 *=============================================================================
 *NAME:         void loop(void)
 *
 *DESCRIPTION:  Essa é a função a função principal, essa função é chamada
 *              repetidamente. 
 *=============================================================================
*/

static int valor1 = 0;

void loop(void)
{  
  server.handleClient();
  
  // Depuracao
  if (Serial.available())
  {
    byte valor = Serial.read();
    switch (valor)
	{
      // Configura a ESP como AP
	  case '1':
		setup_AP();
      break;

      // Tenta conectar como Station e se não conseguir volta para AP
	  case '2':          
		if (setup_STATION() == -1)
		{
		  setup_AP();
		}
      break;     
    }
  }        
}


/*
 *=============================================================================
 *NAME:         void handleLoad()
 *
 *DESCRIPTION:  Esta função é chamada sempre que a página CONF_page é exibida
 *              É responsável por enviar os parametros da placa para que sejam
 *              exibidos no browser
 *=============================================================================
*/
void handleLoad() 
{
  String data;

  data = ssid_STAT;
  data += " ";
  data += password_STAT;
  data += " ";
  data += ip.toString();
  data += " ";
  data += subnet.toString();
  data += " ";
  data += gateway.toString();
  data += " ";
  data += devname;

  Serial.println(data);     
  server.send(200, "text/text", data);  
}


/*
 *=============================================================================
 *NAME:         void handleRoot()
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
      sArg.toCharArray(ssid_STAT, ID_LEN);   

      sArg = server.arg("pass");
      Serial.println(sArg);
      sArg.toCharArray(password_STAT, ID_LEN);

      ip.fromString(server.arg("ip"));           
      Serial.println(ip);

      subnet.fromString(server.arg("subn"));
      Serial.println(subnet);

      gateway.fromString(server.arg("gatw"));
      Serial.println(gateway);                         

	  sArg = server.arg("devname");
      Serial.println(sArg);
      sArg.toCharArray(devname, DEVNAME_LEN);   

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
 *NAME:         handleNotFound(){
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


/*
 *=============================================================================
 *NAME: setup_AP      
 *
 *DESCRIPTION: Configura a ESP como ponto de acesso. O nome da rede será a que
               está indicada na variável ssid_AP e a senha é a que está indica-
			   da na variável password_AP
 *=============================================================================
*/
void setup_AP()
{
  Serial.println();
  Serial.println("Configurando a ESP como Ponto de Acesso ...");

  WiFi.disconnect(true);
  delay(200);
  WiFi.mode(WIFI_AP);

  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid_AP, password_AP);
  Serial.print("AP IP address: "); Serial.println(WiFi.softAPIP());

  Serial.println("");
}


/*
 *=============================================================================
 *NAME: setup_STATION
 *
 *DESCRIPTION: COnfigura a ESP como Station e tenta se conectar na rede
 *             indicada na variável ssid_STAT e o password é o que está indica-
 *             do na variável password_STAT
 *=============================================================================
*/
int setup_STATION()
{
  Serial.println();
  Serial.println("Configurando a ESP como station...");

  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.begin(ssid_STAT, password_STAT);
  delay(100);
  WiFi.config(ip, gateway, subnet);
  
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
  Serial.print("SubnetMask: "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway Ip: "); Serial.println(WiFi.gatewayIP());
  
  Serial.println();
  return 0;
}


/*
 *=============================================================================
 *NAME: readBoardConfig
 *
 *DESCRIPTION: Verifica se tem parâmetros armazenados na EEPROM e caso positivo
 *             faz a leitura dos mesmos e armazena nas variáveis correspondentes
 *=============================================================================
*/
void readBoardConfig(void)
{
  int index;
  
  if (EEPROM.read(EE_ID_ADDR) == ID)
  {
	// Load WIFI SSID
    for (index = 0; index < ID_LEN; index++){
      ssid_STAT[index] = EEPROM.read(EE_WIFI_SSID + index);
    }
	
    // Load WIFI PASS
    for (index = 0; index < PASS_LEN; index++){
      password_STAT[index] = EEPROM.read(EE_WIFI_PASS + index);
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
    Serial.println("Iniciando com parametros Default");
  }
}


/*
 *=============================================================================
 *NAME:         
 *
 *DESCRIPTION:  
 *=============================================================================
*/
void saveBoardConfig(void)
{
  int index;
  Serial.println("Save board config");

  // Save Wifi SSID in EEPROM
  for (index = 0 ; index < ID_LEN; index++)
    EEPROM.write(EE_WIFI_SSID + index, ssid_STAT[index]);
  
  // Save Wifi PASS in EEPROM
  for (index = 0 ; index < PASS_LEN; index++)
    EEPROM.write(EE_WIFI_PASS + index, password_STAT[index]);
  
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
  EEPROM.write(EE_ID_ADDR, ID); 
  EEPROM.commit();
  delay(300);
}
