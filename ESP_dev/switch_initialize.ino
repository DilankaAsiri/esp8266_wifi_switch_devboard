void wifi_init() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  EEPROM.begin(512);

  Serial.println("Reading EEPROM ssid");
  for (int i = 0; i < 32; ++i)
  {
    WLAN_SSID += char(EEPROM.read(i));
  }
  Serial.print("SSID: ");
  Serial.println(WLAN_SSID);
  Serial.println("Reading EEPROM pass");
  String epass = "";
  for (int i = 32; i < 96; ++i)
  {
    WLAN_PASS += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(WLAN_PASS);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  //WiFi.begin(WLAN_SSID.c_str(),  WLAN_PASS.c_str());
   //WiFi.begin("PROLINK_H5004NK_D55B9", "ghost512dcchoco");
  //WiFi.begin("HTC_DESIRE", "ghost512");
  //WiFi.begin("Android", "chamdm94");
  WiFi.begin(ssidcon, passcon);
  
  delay(50);
  Serial.println();

  //Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
