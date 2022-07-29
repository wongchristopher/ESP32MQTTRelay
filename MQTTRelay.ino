/**
 * This is a simple control for a relay that will connect to MQTT
 * 
 */
#include <WiFiManager.h> 

const char* modes[] = { "NULL", "STA", "AP", "STA+AP" };

unsigned long mtime = 0;


WiFiManager wm;

void saveWifiCallback(){
  Serial.println("[CALLBACK] saveCallback fired");
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("[CALLBACK] configModeCallback fired");
}

void saveParamCallback(){
  Serial.println("[CALLBACK] saveParamCallback fired");
}

void bindServerCallback(){
  wm.server->on("/custom",handleRoute); // this is now crashing esp32 for some reason
}

void handleRoute(){
  Serial.println("[HTTP] handle route");
  wm.server->send(200, "text/plain", "hello from user code");
}

void handlePreOtaUpdateCallback(){
  Update.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("CUSTOM Progress: %u%%\r", (progress / (total / 100)));
  });
}

void setup() {
  // WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  
  // put your setup code here, to run once:
  Serial.begin(115200);
  // Serial1.begin(115200);

  // Serial.setDebugOutput(true);  

  Serial.println("\n Starting");
  // WiFi.setSleepMode(WIFI_NONE_SLEEP); // disable sleep, can improve ap stability

  Serial.println("Error - TEST");
  Serial.println("Information- - TEST");

  Serial.println("[ERROR]  TEST");
  Serial.println("[INFORMATION] TEST");  


  wm.setDebugOutput(true);
  wm.debugPlatformInfo();

  //reset settings - for testing
  // wm.resetSettings();
  // wm.erase();  

  // setup some parameters
    
  WiFiManagerParameter custom_html("<p style=\"color:pink;font-weight:Bold;\">This Is Custom HTML</p>"); // only custom html
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", "", 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", "", 6);
  WiFiManagerParameter custom_token("api_token", "api token", "", 16);
  WiFiManagerParameter custom_tokenb("invalid token", "invalid token", "", 0); // id is invalid, cannot contain spaces
  WiFiManagerParameter custom_ipaddress("input_ip", "input IP", "", 15,"pattern='\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}'"); // custom input attrs (ip mask)
  WiFiManagerParameter custom_input_type("input_pwd", "input pass", "", 15,"type='password'"); // custom input attrs (ip mask)

  const char _customHtml_checkbox[] = "type=\"checkbox\""; 
  WiFiManagerParameter custom_checkbox("my_checkbox", "My Checkbox", "T", 2, _customHtml_checkbox,WFM_LABEL_AFTER);

  const char *bufferStr = R"(
  <!-- INPUT CHOICE -->
  <br/>
  <p>Select Choice</p>
  <input style='display: inline-block;' type='radio' id='choice1' name='program_selection' value='1'>
  <label for='choice1'>Choice1</label><br/>
  <input style='display: inline-block;' type='radio' id='choice2' name='program_selection' value='2'>
  <label for='choice2'>Choice2</label><br/>

  <!-- INPUT SELECT -->
  <br/>
  <label for='input_select'>Label for Input Select</label>
  <select name="input_select" id="input_select" class="button">
  <option value="0">Option 1</option>
  <option value="1" selected>Option 2</option>
  <option value="2">Option 3</option>
  <option value="3">Option 4</option>
  </select>
  )";

  WiFiManagerParameter custom_html_inputs(bufferStr);

  // callbacks
  wm.setAPCallback(configModeCallback);
  wm.setWebServerCallback(bindServerCallback);
  wm.setSaveConfigCallback(saveWifiCallback);
  wm.setSaveParamsCallback(saveParamCallback);
  wm.setPreOtaUpdateCallback(handlePreOtaUpdateCallback);

  // add all your parameters here
  wm.addParameter(&custom_html);
  wm.addParameter(&custom_mqtt_server);
  wm.addParameter(&custom_mqtt_port);
  wm.addParameter(&custom_token);
  wm.addParameter(&custom_tokenb);
  wm.addParameter(&custom_ipaddress);
  wm.addParameter(&custom_checkbox);
  wm.addParameter(&custom_input_type);

  wm.addParameter(&custom_html_inputs);

  // set values later if you want
  custom_html.setValue("test",4);
  custom_token.setValue("test",4);

  // const char* icon = "
  // <link rel='icon' type='image/png' sizes='16x16' href='data:image/png;base64,
  // iVBORw0KGgoAAAANSUhEUgAAABAAAAAQBAMAAADt3eJSAAAAMFBMVEU0OkArMjhobHEoPUPFEBIu
  // O0L+AAC2FBZ2JyuNICOfGx7xAwTjCAlCNTvVDA1aLzQ3COjMAAAAVUlEQVQI12NgwAaCDSA0888G
  // CItjn0szWGBJTVoGSCjWs8TleQCQYV95evdxkFT8Kpe0PLDi5WfKd4LUsN5zS1sKFolt8bwAZrCa
  // GqNYJAgFDEpQAAAzmxafI4vZWwAAAABJRU5ErkJggg==' />";


  // set custom html head content , inside <head>
  // examples of favicon, or meta tags etc
  // const char* headhtml = "<link rel='icon' type='image/png' href='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAADQElEQVRoQ+2YjW0VQQyE7Q6gAkgFkAogFUAqgFQAVACpAKiAUAFQAaECQgWECggVGH1PPrRvn3dv9/YkFOksoUhhfzwz9ngvKrc89JbnLxuA/63gpsCmwCADWwkNEji8fVNgotDM7osI/x777x5l9F6JyB8R4eeVql4P0y8yNsjM7KGIPBORp558T04A+CwiH1UVUItiUQmZ2XMReSEiAFgjAPBeVS96D+sCYGaUx4cFbLfmhSpnqnrZuqEJgJnd8cQplVLciAgX//Cf0ToIeOB9wpmloLQAwpnVmAXgdf6pwjpJIz+XNoeZQQZlODV9vhc1Tuf6owrAk/8qIhFbJH7eI3eEzsvydQEICqBEkZwiALfF70HyHPpqScPV5HFjeFu476SkRA0AzOfy4hYwstj2ZkDgaphE7m6XqnoS7Q0BOPs/sw0kDROzjdXcCMFCNwzIy0EcRcOvBACfh4k0wgOmBX4xjfmk4DKTS31hgNWIKBCI8gdzogTgjYjQWFMw+o9LzJoZ63GUmjWm2wGDc7EvDDOj/1IVMIyD9SUAL0WEhpriRlXv5je5S+U1i2N88zdPuoVkeB+ls4SyxCoP3kVm9jsjpEsBLoOBNC5U9SwpGdakFkviuFP1keblATkTENTYcxkzgxTKOI3jyDxqLkQT87pMA++H3XvJBYtsNbBN6vuXq5S737WqHkW1VgMQNXJ0RshMqbbT33sJ5kpHWymzcJjNTeJIymJZtSQd9NHQHS1vodoFoTMkfbJzpRnLzB2vi6BZAJxWaCr+62BC+jzAxVJb3dmmiLzLwZhZNPE5e880Suo2AZgB8e8idxherqUPnT3brBDTlPxO3Z66rVwIwySXugdNd+5ejhqp/+NmgIwGX3Py3QBmlEi54KlwmjkOytQ+iJrLJj23S4GkOeecg8G091no737qvRRdzE+HLALQoMTBbJgBsCj5RSWUlUVJiZ4SOljb05eLFWgoJ5oY6yTyJp62D39jDANoKKcSocPJD5dQYzlFAFZJflUArgTPZKZwLXAnHmerfJquUkKZEgyzqOb5TuDt1P3nwxobqwPocZA11m4A1mBx5IxNgRH21ti7KbAGiyNn3HoF/gJ0w05A8xclpwAAAABJRU5ErkJggg==' />";
  // const char* headhtml = "<meta name='color-scheme' content='dark light'><style></style><script></script>";
  // wm.setCustomHeadElement(headhtml);

  // set custom html menu content , inside menu item "custom", see setMenu()
  const char* menuhtml = "<form action='/custom' method='get'><button>Custom</button></form><br/>\n";
  wm.setCustomMenuHTML(menuhtml);

  // invert theme, dark
  wm.setDarkMode(true);

  // show scan RSSI as percentage, instead of signal stength graphic
  // wm.setScanDispPerc(true);

/*
  Set cutom menu via menu[] or vector
  const char* menu[] = {"wifi","wifinoscan","info","param","close","sep","erase","restart","exit"};
  wm.setMenu(menu,9); // custom menu array must provide length
*/

  std::vector<const char *> menu = {"wifi","wifinoscan","info","param","custom","close","sep","erase","update","restart","exit"};
  wm.setMenu(menu); // custom menu, pass vector
  
  wm.setConfigPortalBlocking(false);

  wm.setConfigPortalTimeout(120);
  
  wm.setCleanConnect(true); // disconnect before connect, clean connect
  
  wm.setBreakAfterConfig(true); // needed to use saveWifiCallback

  wifiInfo();

  if(!wm.autoConnect("MQTTRelay_controller")) {
    Serial.println("failed to connect and hit timeout");
  } else {
    //if you get here you have connected to the WiFi
     Serial.println("connected...yeey :)");
  }
  
  wifiInfo();
}

void wifiInfo(){
  // can contain gargbage on esp32 if wifi is not ready yet
  Serial.println("[WIFI] WIFI INFO DEBUG");
  // WiFi.printDiag(Serial);
  Serial.println("[WIFI] SAVED: " + (String)(wm.getWiFiIsSaved() ? "YES" : "NO"));
  Serial.println("[WIFI] SSID: " + (String)wm.getWiFiSSID());
  Serial.println("[WIFI] PASS: " + (String)wm.getWiFiPass());
  Serial.println("[WIFI] HOSTNAME: " + (String)WiFi.getHostname());
}

void loop() {

  wm.process();
}