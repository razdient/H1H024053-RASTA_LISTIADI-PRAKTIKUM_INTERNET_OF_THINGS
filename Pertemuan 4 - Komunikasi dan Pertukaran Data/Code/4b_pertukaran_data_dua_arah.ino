#include <ESP8266WiFi.h>                    // Library WiFi ESP32
#include <PubSubClient.h>            // Library komunikasi MQTT
#include <ArduinoJson.h>             // Library untuk JSON
#include <DHT.h>                     // Library sensor DHT

const char* ssid = "RASTA_32";             // Nama WiFi
const char* password = "12345678";     // Password WiFi

const char* mqttServer = "broker.hivemq.com";    // Alamat broker MQTT
const int mqttPort = 1883;                        // Port MQTT

const char* topicData = "unsoed/tk245004/kelompok2/rasta_data"; // Topic data suhu
const char* topicPerintah = "unsoed/tk245004/kelompok2/rasta_led"; // Topic perintah LED

#define DHTPIN 4                                  // Pin data DHT11 pada GPIO 4
#define DHTTYPE DHT11                             // Jenis sensor yang digunakan DHT11

const int ledPin = 5;                            // LED terhubung ke GPIO 5

DHT dht(DHTPIN, DHTTYPE);                         // Membuat objek sensor DHT
WiFiClient espClient;                             // Membuat koneksi WiFi
PubSubClient client(espClient);                   // Membuat koneksi MQTT

unsigned long waktuTerakhirPublish = 0;           // Menyimpan waktu publish terakhir
const long intervalPublish = 5000;                // Publish data setiap 5 detik

void callback(char* topic, byte* payload, unsigned int length) { // Fungsi saat pesan MQTT diterima
  String pesan;                                   // Variabel untuk menyimpan pesan

  for (unsigned int i = 0; i < length; i++) {     // Membaca payload
    pesan += (char)payload[i];                    // Mengubah byte menjadi karakter
  }

  Serial.print("Pesan diterima: ");               // Menampilkan label pesan
  Serial.println(pesan);                          // Menampilkan isi pesan

  JsonDocument doc;                               // Membuat objek JSON
  DeserializationError error = deserializeJson(doc, pesan); // Parsing pesan JSON

  if (error) {                                    // Mengecek parsing JSON
    Serial.println("Gagal parsing JSON");         // Menampilkan error
    return;                                       // Menghentikan callback
  }

  const char* perintah = doc["perintah"];         // Mengambil nilai perintah

  if (String(perintah) == "ON") {                // Jika perintah ON
    digitalWrite(ledPin, HIGH);                   // Menyalakan LED
    Serial.println("Perintah diterima -> Aktuator: ON"); // Menampilkan status
  } else if (String(perintah) == "OFF") {        // Jika perintah OFF
    digitalWrite(ledPin, LOW);                    // Mematikan LED
    Serial.println("Perintah diterima -> Aktuator: OFF"); // Menampilkan status
  }
}

void hubungkanWiFi() {                            // Fungsi menghubungkan WiFi
  WiFi.begin(ssid, password);                     // Memulai koneksi WiFi
  Serial.print("Menghubungkan ke WiFi");          // Menampilkan proses

  while (WiFi.status() != WL_CONNECTED) {         // Menunggu WiFi terhubung
    delay(500);                                   // Menunggu 500 ms
    Serial.print(".");                            // Menampilkan titik
  }

  Serial.println();                               // Pindah baris
  Serial.println("WiFi berhasil terhubung!");     // Menampilkan status WiFi
  Serial.print("IP Address: ");                   // Menampilkan label IP
  Serial.println(WiFi.localIP());                 // Menampilkan IP ESP32
}

void hubungkanMQTT() {                            // Fungsi menghubungkan MQTT
  while (!client.connected()) {                    // Mengecek koneksi MQTT
    Serial.print("Menghubungkan ke MQTT...");     // Menampilkan proses

    String clientId = "ESP32Client-" + String(random(0xffff), HEX); // Membuat ID client

    if (client.connect(clientId.c_str())) {       // Mencoba koneksi MQTT
      Serial.println("berhasil!");               // Menampilkan koneksi berhasil
      client.subscribe(topicPerintah);            // Subscribe topic LED
      Serial.print("Subscribe topic: ");          // Menampilkan label topic
      Serial.println(topicPerintah);              // Menampilkan nama topic
    } else {
      Serial.print("Gagal, rc=");                 // Menampilkan koneksi gagal
      Serial.println(client.state());             // Menampilkan kode error
      delay(2000);                                // Menunggu 2 detik
    }
  }
}

void setup() {                                    // Fungsi yang dijalankan sekali
  Serial.begin(115200);                           // Memulai Serial Monitor
  pinMode(ledPin, OUTPUT);                         // Mengatur GPIO LED sebagai output
  digitalWrite(ledPin, LOW);                      // Memastikan LED mati
  dht.begin();                                    // Memulai sensor DHT11
  hubungkanWiFi();                                // Menghubungkan WiFi
  client.setServer(mqttServer, mqttPort);         // Mengatur broker MQTT
  client.setCallback(callback);                   // Mengatur fungsi callback
}

void loop() {                                     // Fungsi yang dijalankan berulang
  if (!client.connected()) {                       // Mengecek koneksi MQTT
    hubungkanMQTT();                              // Menghubungkan kembali MQTT
  }

  client.loop();                                  // Memproses pesan MQTT

  if (millis() - waktuTerakhirPublish >= intervalPublish) { // Mengecek apakah sudah 5 detik
    waktuTerakhirPublish = millis();              // Menyimpan waktu publish terbaru

    float suhu = dht.readTemperature();           // Membaca suhu dari DHT11

    if (isnan(suhu)) {                            // Mengecek apakah pembacaan gagal
      Serial.println("Gagal membaca sensor DHT11"); // Menampilkan error sensor
      return;                                     // Kembali ke awal loop
    }

    JsonDocument doc;                             // Membuat objek JSON
    doc["suhu"] = suhu;                            // Memasukkan suhu ke JSON

    char buffer[128];                             // Menyediakan tempat untuk JSON
    serializeJson(doc, buffer);                   // Mengubah JSON menjadi teks

    client.publish(topicData, buffer);            // Mengirim data suhu ke MQTT

    Serial.print("Data terkirim: ");              // Menampilkan label
    Serial.println(buffer);                       // Menampilkan data yang dikirim
  }
}
