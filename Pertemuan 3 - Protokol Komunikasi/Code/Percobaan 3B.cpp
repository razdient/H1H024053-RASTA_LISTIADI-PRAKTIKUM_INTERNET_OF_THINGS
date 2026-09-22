#include <ESP8266WiFi.h>       // Library untuk menghubungkan ESP8266 ke WiFi
#include <PubSubClient.h>      // Library untuk komunikasi menggunakan protokol MQTT
#include <ArduinoJson.h>       // Library untuk membuat dan mengolah data JSON


// ==================== KONFIGURASI WIFI ====================

// Nama WiFi yang akan digunakan
const char* ssid = "ESP32_RASTA";

// Password WiFi
const char* password = "12345678";


// ==================== KONFIGURASI MQTT ====================

// Alamat broker MQTT yang digunakan
const char* mqttServer = "broker.hivemq.com";

// Port MQTT
const int mqttPort = 1883;

// Topic yang digunakan untuk mengirim data sensor
const char* mqttTopic = "unsoed/tk245004/kelompok2/sensor";


// ==================== OBJEK WIFI DAN MQTT ====================

// Membuat objek untuk koneksi WiFi
WiFiClient espClient;

// Membuat objek MQTT menggunakan koneksi WiFi
PubSubClient client(espClient);


// ==================== FUNGSI HUBUNGKAN WIFI ====================

// Fungsi untuk menghubungkan ESP8266 ke jaringan WiFi
void hubungkanWiFi() {

  // Memulai koneksi menggunakan SSID dan password
  WiFi.begin(ssid, password);

  // Menampilkan pesan proses koneksi pada Serial Monitor
  Serial.print("Menghubungkan ke WiFi");

  // Menunggu sampai ESP8266 berhasil terhubung
  while (WiFi.status() != WL_CONNECTED) {

    // Menunggu selama 500 milidetik
    delay(500);

    // Menampilkan titik sebagai tanda proses koneksi
    Serial.print(".");
  }

  // Pindah ke baris baru
  Serial.println();

  // Menampilkan pesan bahwa WiFi berhasil terhubung
  Serial.println("WiFi berhasil terhubung!");
}


// ==================== FUNGSI HUBUNGKAN MQTT ====================

// Fungsi untuk menghubungkan ESP8266 ke broker MQTT
void hubungkanMQTT() {

  // Selama ESP8266 belum terhubung ke broker MQTT
  while (!client.connected()) {

    // Menampilkan pesan proses koneksi
    Serial.print("Menghubungkan ke broker MQTT...");

    // Membuat ID client MQTT secara acak
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);

    // Mencoba menghubungkan ESP8266 ke broker MQTT
    if (client.connect(clientId.c_str())) {

      // Jika berhasil, tampilkan pesan berhasil
      Serial.println("berhasil terhubung!");

    } else {

      // Jika gagal, tampilkan kode error
      Serial.print("gagal, rc=");
      Serial.print(client.state());

      // Memberikan informasi bahwa koneksi akan dicoba kembali
      Serial.println(" coba lagi dalam 2 detik");

      // Menunggu 2 detik sebelum mencoba kembali
      delay(2000);
    }
  }
}


// ==================== SETUP ====================

void setup() {

  // Memulai komunikasi Serial dengan baud rate 115200
  Serial.begin(115200);

  // Memanggil fungsi untuk menghubungkan ESP8266 ke WiFi
  hubungkanWiFi();

  // Mengatur alamat dan port broker MQTT
  client.setServer(mqttServer, mqttPort);
}


// ==================== LOOP ====================

void loop() {

  // Memeriksa apakah ESP8266 masih terhubung ke broker MQTT
  if (!client.connected()) {

    // Jika terputus, hubungkan kembali ke broker
    hubungkanMQTT();
  }

  // Menjaga koneksi MQTT tetap aktif
  client.loop();


  // ==================== MEMBUAT DATA JSON ====================

  // Membuat objek untuk menyimpan data JSON
  JsonDocument doc;

  // Menambahkan data suhu ke dalam JSON
  doc["suhu"] = 28.5;

  // Menambahkan data kelembaban ke dalam JSON
  doc["kelembaban"] = 65.0;


  // Membuat tempat untuk menyimpan JSON dalam bentuk teks
  char buffer[128];

  // Mengubah objek JSON menjadi teks dan menyimpannya di buffer
  serializeJson(doc, buffer);


  // ==================== MENGIRIM DATA MQTT ====================

  // Mempublikasikan data JSON ke topic MQTT
  client.publish(mqttTopic, buffer);

  // Menampilkan informasi topic pada Serial Monitor
  Serial.print("Data terkirim ke topic ");

  // Menampilkan nama topic
  Serial.print(mqttTopic);

  // Menampilkan tanda pemisah
  Serial.print(": ");

  // Menampilkan data JSON yang dikirim
  Serial.println(buffer);


  // Menunggu 5 detik sebelum mengirim data berikutnya
  delay(5000);
}

