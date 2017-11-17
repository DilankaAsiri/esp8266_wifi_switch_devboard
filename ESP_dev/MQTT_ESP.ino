void MQTT_connect() {
  int8_t ret;
  bool wifi_dis_serial_flag = true;

  // Stop if already connected.
  if (client.connected()) {
    digitalWrite(wifi_status, LOW);  // indicator
    digitalWrite(mqtt_status, LOW);
    client.loop();
    return;
  }

  while (!client.connected()) { // connect will return 0 for connected   //(ret = client.connect()) != 0
    long time_wifi_status_led = 0;

    while (WiFi.status() != WL_CONNECTED) {
      if (wifi_dis_serial_flag == true) {
        Serial.println("WiFi disconnected");
        digitalWrite(wifi_status, HIGH);
        digitalWrite(mqtt_status, HIGH);
        wifi_dis_serial_flag = false;
      }
      if ((millis() - time_wifi_status_led)  > 500) {
        time_wifi_status_led = millis();
        digitalWrite(wifi_status, !digitalRead(wifi_status));
      }
      touch_check();
    }

    if (wifi_dis_serial_flag == false) {
      Serial.println("WiFi Connected");
      wifi_dis_serial_flag = true;
    }

    if (client.connect("WiFi_SWITCH")) {
      client.set_callback(callback);
      client.subscribe(subscribe_topic);
      Serial.println("MQTT Connected..");
      Serial.println();
    } else {
      Serial.println("try again in 3 seconds");
      digitalWrite(mqtt_status, HIGH);
      for (int k = 0; k < 1000; k++) {
        touch_check();
        delay(1);
      }
    }
  }

  digitalWrite(wifi_status, LOW);  // indicator
  digitalWrite(mqtt_status, LOW);

  if (change_flag == true) {
    pubMQTT();
    change_flag = false;
  }
  return;
}



void pubMQTT() {
  String plaintext_pub = "{\"SW_STATUS\" : \"";
  if (ledState == LOW)
    plaintext_pub += "OFF";
  if (ledState == HIGH)
    plaintext_pub += "ON";
  plaintext_pub += "\"}";

  Serial.print("Published message : ");
  Serial.println(plaintext_pub);
  Serial.println();

  //client.publish("wifi_switch", plaintext_pub);
  client.publish(publish_topic, plaintext_pub);
}


void callback(const MQTT::Publish& pub) {
  Serial.print("Received message : ");
  //Serial.print(pub.topic());

  //Serial.print(" => ");
  if (pub.has_stream()) {
    uint8_t buf[BUFFER_SIZE];
    int read;
    while (read = pub.payload_stream()->read(buf, BUFFER_SIZE)) {
      Serial.write(buf, read);
    }
    pub.payload_stream()->stop();
    Serial.println("");
  } else {
    Serial.println(pub.payload_string());

    String plaintext_sub = pub.payload_string();
    //Serial.println(plaintext_sub);

    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(plaintext_sub);
    const char* light_state = root["SW_STATUS"];

    on_off_stat = (char *)light_state;
    if (on_off_stat == "ON" ) {
      digitalWrite(relay_pin, LOW);
      ledState = HIGH;
      Serial.println("ON");
    }
    else if (on_off_stat  == "OFF" ) {
      digitalWrite(relay_pin, HIGH);
      ledState = LOW;
      Serial.println("OFF");
    }
    Serial.println();
  }
  return;
}
