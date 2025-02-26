#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>

// Wi-Fi bilgileri
#define WIFI_SSID "Galaxys23"
#define WIFI_PASSWORD "tugra123"

// Firebase bilgileri
FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

// Pin tanımlamaları
#define BUZZER1_PIN D5
#define BUZZER2_PIN D6
#define LED1_PIN D1
#define LED2_PIN D2

void setup() {
  Serial.begin(9600); // Seri haberleşme hızı
  Serial.println("NodeMCU is ready to receive data.");

  // Wi-Fi bağlantısı
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi bağlantısı tamamlandı!");

  // Firebase yapılandırması
  firebaseConfig.database_url = "https://iotproject-efb83-default-rtdb.firebaseio.com";
  firebaseConfig.signer.tokens.legacy_token = "ic9NilNN5fuiQnWCL7CCtqcQzQxpKvYDuGiZhvjl";

  // Firebase başlat
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  // Pin modları
  pinMode(BUZZER1_PIN, OUTPUT);
  pinMode(BUZZER2_PIN, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  // Başlangıç durumu
  digitalWrite(BUZZER1_PIN, HIGH); // Negatif tetikleme, başlangıçta kapalı
  digitalWrite(BUZZER2_PIN, HIGH);
  digitalWrite(LED1_PIN, LOW); // LED'ler başlangıçta kapalı
  digitalWrite(LED2_PIN, LOW);
}

void loop() {
  // Arduino'dan veri alma
  if (Serial.available() > 0) { 
    String data = Serial.readStringUntil('\n'); // Gelen veriyi '\n' kadar oku

    // Gelen veriyi ayrıştır
    int firstComma = data.indexOf(',');
    int secondComma = data.indexOf(',', firstComma + 1);
    int thirdComma = data.indexOf(',', secondComma + 1);
    int fourthComma = data.indexOf(',', thirdComma + 1);

    if (firstComma != -1 && secondComma != -1 && thirdComma != -1 && fourthComma != -1) {
      String d9Value = data.substring(0, firstComma);
      String d10Value = data.substring(firstComma + 1, secondComma);
      String temperature = data.substring(secondComma + 1, thirdComma);
      String humidity = data.substring(thirdComma + 1, fourthComma);
      String gasValue = data.substring(fourthComma + 1);

      // Titreşim sensörlerinden gelen verileri işleme
      int d9Status = (d9Value.toInt() > 0) ? 1 : 0;
      int d10Status = (d10Value.toInt() > 0) ? 1 : 0;

      // Gaz sensöründen gelen veriyi işleme
      int gasStatus = (gasValue.toInt() > 190) ? 1 : 0;

      // Firebase'e verileri gönder
      Firebase.RTDB.setInt(&firebaseData, "/IoTProject/TitresimD9", d9Status);
      Firebase.RTDB.setInt(&firebaseData, "/IoTProject/TitresimD10", d10Status);
      Firebase.RTDB.setFloat(&firebaseData, "/IoTProject/Sicaklik", temperature.toFloat());
      Firebase.RTDB.setFloat(&firebaseData, "/IoTProject/Nem", humidity.toFloat());
      Firebase.RTDB.setInt(&firebaseData, "/IoTProject/GazDurumu", gasStatus);

      // Konsola yazdır
      Serial.println("Veriler Firebase'e gönderildi:");
      Serial.println("TitresimD9 (0/1): " + String(d9Status));
      Serial.println("TitresimD10 (0/1): " + String(d10Status));
      Serial.println("Sıcaklık: " + temperature);
      Serial.println("Nem: " + humidity);
      Serial.println("Gaz Durumu (0/1): " + String(gasStatus));
    } else {
      Serial.println("Veri ayrıştırma hatası!");
    }
  }

  // Buzzer1 kontrol
  if (Firebase.RTDB.getString(&firebaseData, "/IoTProject/BuzzerKontrol")) {
    String buzzer1State = firebaseData.stringData();
    buzzer1State.trim();

    if (buzzer1State.equalsIgnoreCase("BUZZER_ON")) {
      tone(BUZZER1_PIN, 1000); // 1 kHz frekansla Buzzer1 çalıştırılır
      Serial.println("Buzzer1 Açık (Tone Aktif)");
    } else if (buzzer1State.equalsIgnoreCase("BUZZER_OFF")) {
      noTone(BUZZER1_PIN); // Buzzer1 durdurulur
      Serial.println("Buzzer1 Kapalı (Tone Pasif)");
    }
  }

  // Buzzer2 kontrol
  if (Firebase.RTDB.getString(&firebaseData, "/IoTProject/BuzzerKontrol2")) {
    String buzzer2State = firebaseData.stringData();
    buzzer2State.trim();

    if (buzzer2State.equalsIgnoreCase("BUZZER_ON")) {
      tone(BUZZER2_PIN, 1500); // 1.5 kHz frekansla Buzzer2 çalıştırılır
      Serial.println("Buzzer2 Açık (Tone Aktif)");
    } else if (buzzer2State.equalsIgnoreCase("BUZZER_OFF")) {
      noTone(BUZZER2_PIN); // Buzzer2 durdurulur
      Serial.println("Buzzer2 Kapalı (Tone Pasif)");
    }
  }

  // LED1 kontrol
  if (Firebase.RTDB.getString(&firebaseData, "/IoTProject/LedKontrol")) {
    String led1State = firebaseData.stringData();
    led1State.trim();

    if (led1State.equalsIgnoreCase("LED_ON")) {
      digitalWrite(LED1_PIN, HIGH); // LED1 açılır
      Serial.println("LED1 Açık");
    } else if (led1State.equalsIgnoreCase("LED_OFF")) {
      digitalWrite(LED1_PIN, LOW); // LED1 kapatılır
      Serial.println("LED1 Kapalı");
    }
  }

  // LED2 kontrol
  if (Firebase.RTDB.getString(&firebaseData, "/IoTProject/LedKontrol2")) {
    String led2State = firebaseData.stringData();
    led2State.trim();

    if (led2State.equalsIgnoreCase("LED_ON")) {
      digitalWrite(LED2_PIN, HIGH); // LED2 açılır
      Serial.println("LED2 Açık");
    } else if (led2State.equalsIgnoreCase("LED_OFF")) {
      digitalWrite(LED2_PIN, LOW); // LED2 kapatılır
      Serial.println("LED2 Kapalı");
    }
  }

  delay(1000); // Firebase sorguları arasında gecikme
}
