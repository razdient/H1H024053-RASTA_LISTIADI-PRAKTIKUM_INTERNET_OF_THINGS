#include <ESP8266WiFi.h>                    // Library WiFi ESP32
#include <PubSubClient.h>            // Library komunikasi MQTT
#include <ArduinoJson.h>             // Library untuk membaca JSON

const char* ssid = "RASTA_32";             // Nama WiFi
const char* password = "12345678";     // Password WiFi

const char* mqttServer = "broker.hivemq.com";    // Alamat broker MQTT
const int mqttPort = 1883;                        // Port MQTT

const char* topicPerintah = "unsoed/tk245004/kelompok2/rasta_led"; // Topic perintah LED
const int ledPin = 5;                            // LED terhubung ke GPIO 5

WiFiClient espClient;                             // Membuat koneksi WiFi
PubSubClient client(espClient);                   // Membuat koneksi MQTT

void callback(char* topic, byte* payload, unsigned int length) { // Fungsi saat pesan MQTT diterima
  String pesan;                                   // Variabel untuk menyimpan pesan

  for (unsigned int i = 0; i < length; i++) {     // Membaca payload satu per satu
    pesan += (char)payload[i];                    // Mengubah byte menjadi karakter
  }

  Serial.print("Pesan diterima [");               // Menampilkan informasi pesan
  Serial.print(topic);                            // Menampilkan nama topic
  Serial.print("]: ");                            // Menampilkan pemisah
  Serial.println(pesan);                          // Menampilkan isi pesan

  JsonDocument doc;                               // Membuat objek JSON
  DeserializationError error = deserializeJson(doc, pesan); // Mengubah pesan menjadi JSON

  if (error) {                                    // Mengecek apakah parsing gagal
    Serial.print("Gagal parsing JSON: ");         // Menampilkan pesan error
    Serial.println(error.c_str());                // Menampilkan jenis error
    return;                                       // Menghentikan callback
  }

  const char* perintah = doc["perintah"];         // Mengambil nilai "perintah" dari JSON

  if (String(perintah) == "ON") {                // Jika perintah adalah ON
    digitalWrite(ledPin, HIGH);                   // Menyalakan LED
    Serial.println("Aktuator: ON");              // Menampilkan status LED
  } else if (String(perintah) == "OFF") {        // Jika perintah adalah OFF
    digitalWrite(ledPin, LOW);                    // Mematikan LED
    Serial.println("Aktuator: OFF");             // Menampilkan status LED
  }
}

void hubungkanWiFi() {                            // Fungsi menghubungkan ke WiFi
  WiFi.begin(ssid, password);                     // Memulai koneksi WiFi
  Serial.print("Menghubungkan ke WiFi");          // Menampilkan proses koneksi

  while (WiFi.status() != WL_CONNECTED) {         // Selama WiFi belum terhubung
    delay(500);                                   // Menunggu 500 ms
    Serial.print(".");                            // Menampilkan titik
  }

  Serial.println();                               // Pindah baris
  Serial.println("WiFi berhasil terhubung!");     // Menampilkan WiFi berhasil
  Serial.print("IP Address: ");                   // Menampilkan label IP
  Serial.println(WiFi.localIP());                 // Menampilkan alamat IP ESP32
}

void hubungkanMQTT() {                            // Fungsi menghubungkan ke MQTT
  while (!client.connected()) {                    // Selama MQTT belum terhubung
    Serial.print("Menghubungkan ke broker MQTT..."); // Menampilkan proses koneksi

    String clientId = "ESP32Client-" + String(random(0xffff), HEX); // Membuat ID client acak

    if (client.connect(clientId.c_str())) {       // Mencoba terhubung ke broker
      Serial.println("berhasil terhubung!");      // Menampilkan koneksi berhasil
      client.subscribe(topicPerintah);            // Subscribe ke topic perintah
      Serial.print("Subscribe ke topic: ");       // Menampilkan informasi topic
      Serial.println(topicPerintah);              // Menampilkan nama topic
    } else {
      Serial.print("gagal, rc=");                 // Menampilkan koneksi gagal
      Serial.println(client.state());             // Menampilkan kode error MQTT
      delay(2000);                                 // Menunggu 2 detik
    }
  }
}

void setup() {                                    // Fungsi yang dijalankan sekali
  Serial.begin(115200);                           // Memulai Serial Monitor
  pinMode(ledPin, OUTPUT);                         // Mengatur GPIO 26 sebagai output
  digitalWrite(ledPin, LOW);                      // Membuat LED awalnya mati
  hubungkanWiFi();                                // Menghubungkan ESP32 ke WiFi
  client.setServer(mqttServer, mqttPort);         // Mengatur broker MQTT
  client.setCallback(callback);                   // Menentukan fungsi callback
}

void loop() {                                     // Fungsi yang dijalankan berulang
  if (!client.connected()) {                       // Mengecek koneksi MQTT
    hubungkanMQTT();                              // Menghubungkan kembali jika terputus
  }
  client.loop();
}
