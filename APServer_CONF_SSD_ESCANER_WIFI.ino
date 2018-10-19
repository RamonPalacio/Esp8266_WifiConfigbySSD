#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <FS.h>   //Libreria para la memoria SSD del Nodemcu V3 Lolin




//-------------------VARIABLES GLOBALES--------------------------
unsigned long previousMillis = 0;

const char *ssidAp = "NO_WIFI";
const char *passwordAP = "12345678";


String mensaje = "";
int Valor = 0;

WiFiClient espClient;
ESP8266WebServer server(80);

 String wifi="";
 String pass="";
//--------------------------------------------------------------


//--------------------------- BOTON GUARDAR ----------------------------
void SetComando(){

  wifi = server.arg("wifi");
  pass = server.arg("pass");
 
  
  
  Serial.println(wifi);
  Serial.println(pass);
  



  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["WiFi"] = wifi;
  json["Pass"] = pass;
  // sensor = root["sensor"].as<String>();
  
  File configFile = SPIFFS.open("/wifi.json", "w");
  if (!configFile) {
    server.send(200, "text/plain", "Error al Guardar");
  }
  else {
    server.send(200, "text/plain", "OK");
  }
  
  json.printTo(configFile);

}

//---------------------------ESCANEAR----------------------------
void escanear() {  
  String mensaje = "";
  int n = WiFi.scanNetworks(); //devuelve el número de redes encontradas
  Serial.println("escaneo terminado");
  if (n == 0) { //si no encuentra ninguna red
    Serial.println("no se encontraron redes");
    mensaje = "no se encontraron redes";
  }  
  else
  {
    Serial.print(n);
    Serial.println(" redes encontradas");
    mensaje = "";
    for (int i = 0; i < n; ++i)
    {



        
      // agrega al STRING "mensaje" la información de las redes encontradas 

     // mensaje = (mensaje) + "<input type=\"submit\" value=\"" + WiFi.SSID(i) + "\">";
       mensaje = (mensaje) + "<button onclick=\"getElementById('wifi').value='" + WiFi.SSID(i) + "'\">" + WiFi.SSID(i) + "</button><br><br>";
      //mensaje = (mensaje) + "<p>" + String(i + 1) + ": " + WiFi.SSID(i) + " (" + WiFi.RSSI(i) + ") Ch: " + WiFi.channel(i) + " Enc: " + WiFi.encryptionType(i) + " </p>\r\n";
      //WiFi.encryptionType 5:WEP 2:WPA/PSK 4:WPA2/PSK 7:open network 8:WPA/WPA2/PSK
      delay(10);
    }

    
    String s = "";
    if (WiFi.status() == WL_CONNECTED) {
    s = "<html lang=\"es\"><head>  <meta charset=\"UTF-8\">  <title>Configuracion</title></head><body>  <form action=\"/setcomando\" method=\"get\">Nombre de Red: <input type=\"text\" name=\"wifi\" id=\"wifi\"><br> Contraseña: <input type=\"password\" name=\"pass\"><br> <input type=\"submit\" value=\"Guardar\"> <br>Estado:<h1>Conectado!</h1>  </form></body></html>";
    } else {
    s = "<html lang=\"es\"><head>  <meta charset=\"UTF-8\">  <title>Configuracion</title></head><body>  <form action=\"/setcomando\" method=\"get\">Nombre de Red: <input type=\"text\" name=\"wifi\" id=\"wifi\"><br> Contraseña: <input type=\"password\" name=\"pass\"><br> <input type=\"submit\" value=\"Guardar\"> <br>Estado:<h1>desconectado!</h1> </form></body></html>";
    }
    server.send(200, "text/html", mensaje + s);

  }
}
//---------------------------------------------------------------------------------------------------

void ini_wifi() {
                  if (!SPIFFS.begin()) {
                        Serial.println("Failed to mount file system");
                        return;
                  }
                  File configFile = SPIFFS.open("/wifi.json", "r");
                  if (!configFile) {
                    Serial.println("Failed to open config file");
                  }
                  
                  size_t size = configFile.size();
                  if (size > 1024) {
                    Serial.println("Config file size is too large");
                  }
                  
                  std::unique_ptr<char[]> buf(new char[size]);
                  configFile.readBytes(buf.get(), size);
                  
                  StaticJsonBuffer<200> jsonBuffer;
                  JsonObject& json = jsonBuffer.parseObject(buf.get());
                  
                  if (!json.success()) {
                    Serial.println("Failed to parse config file");
                  }
                  
                  const char* Wifi_Nombre = json["WiFi"] ;
                  const char* Wifi_Contra = json["Pass"];
                  Serial.println(Wifi_Nombre);
                  Serial.println(Wifi_Contra);
                  
                  WiFi.mode(WIFI_AP_STA);
                  WiFi.setAutoReconnect (true);
                  WiFi.begin(Wifi_Nombre, Wifi_Contra);
                  
                  
                  uint8_t timeout = 30; // 10 * 500 ms = 3 sec time out
                  
                  while ( (WiFi.status() != WL_CONNECTED) && timeout ){
                      delay(500);
                      --timeout;
                      Serial.println(".");
                  }
                  
                  if ( WiFi.status() == WL_CONNECTED) {
                      Serial.println("Conexion WiFi Exitosa");
                  
                      Serial.print("IP address   : "); 
                      Serial.println(WiFi.localIP());
                      Serial.print("MAC address  : "); 
                      Serial.println(WiFi.macAddress());
                      
                  } else {
                      WiFi.softAP(ssidAp,passwordAP);
                      Serial.println("WiFi.softAP(");
                      Serial.print("IP address   : "); 
                      Serial.println(WiFi.softAPIP());
                      Serial.print("MAC address  : "); 
                      Serial.println(WiFi.softAPmacAddress());
                  }

                  //********************** Server Web de Configuracion ****************
                  server.onNotFound(escanear);
                  server.on("/setcomando",SetComando);
                  
                  server.begin();
                  //*******************************************************************
  
}
//------------------------SETUP-----------------------------
void setup() {

  pinMode(D4, OUTPUT); // D4 
  // Inicia Serial
  Serial.begin(115200);
  Serial.println("");
  Serial.println(ESP.getChipId());

  ini_wifi();

}

//--------------------------LOOP--------------------------------
void loop() {
  server.handleClient();
  digitalWrite(D4, HIGH);
  delay(50);
  digitalWrite(D4, LOW);
  delay(50);
}

