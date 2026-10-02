# Percobaan 4A
Percobaan 4A dilakukan dengan menghubungkan ESP32 ke WiFi dan broker MQTT untuk menerima pesan dalam format JSON. Percobaan ini bertujuan untuk memahami proses subscribe MQTT dan parsing pesan JSON yang berisi perintah "ON" atau "OFF". Perintah tersebut digunakan untuk mengontrol LED pada GPIO 26.
## Library
```cpp
#include <ESP8266WiFi.h>
```
Library ini digunakan agar ESP8266 dapat menjalankan fungsi jaringan WiFi, seperti terhubung ke WiFi (Station) atau membuat jaringan WiFi sendiri (Access Point).  
<br>

```cpp
#include <PubSubClient.h>
```
Library ini digunakan untuk melakukan komunikasi menggunakan protokol MQTT, termasuk menghubungkan ESP8266 ke broker dan mengirimkan data melalui topic.   
<br>

```cpp
#include <ArduinoJson.h>
```
Library ini digunakan untuk membuat, mengolah, dan mengubah data dalam format JSON agar dapat digunakan dalam pertukaran data.  
<br>

## Diagram Rangkaian
![alt text](https://github.com/razdient/H1H024053-RASTA_LISTIADI-PRAKTIKUM_INTERNET_OF_THINGS/blob/main/Pertemuan%204%20-%20Komunikasi%20dan%20Pertukaran%20Data/Dokumentasi/Gambar%20Rangkaian%20Percobaan%204A.png?raw=true)
## Dokumentasi
![alt text](https://github.com/razdient/H1H024053-RASTA_LISTIADI-PRAKTIKUM_INTERNET_OF_THINGS/blob/main/Pertemuan%204%20-%20Komunikasi%20dan%20Pertukaran%20Data/Dokumentasi/Foto%20Rangkaian%20Percobaan%204A.jpeg?raw=true)

## Penjelasan Kode
```cpp
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
```
<br>

## Modifikasi Pengaturan Intensitas Kecerahan LED Menggunakan PWM
```cpp
#include <WiFi.h>                    // Library untuk koneksi WiFi ESP32
#include <PubSubClient.h>            // Library untuk komunikasi MQTT
#include <ArduinoJson.h>             // Library untuk membaca data JSON

const char* ssid = "RASTA_32";                 // Nama WiFi
const char* password = "12345678";         // Password WiFi

const char* mqttServer = "broker.hivemq.com";        // Alamat broker MQTT
const int mqttPort = 1883;                            // Port MQTT

const char* topicPerintah = "unsoed/tk245004/kelompok2/rasta_led"; // Topic untuk menerima perintah

const int ledPin = 5;                                // Pin LED pada GPIO 5

WiFiClient espClient;                                 // Membuat objek koneksi WiFi
PubSubClient client(espClient);                       // Membuat objek komunikasi MQTT

void callback(char* topic, byte* payload, unsigned int length) { // Fungsi saat pesan MQTT diterima
  String pesan;                                       // Variabel untuk menyimpan pesan MQTT

  for (unsigned int i = 0; i < length; i++) {         // Membaca payload satu per satu
    pesan += (char)payload[i];                        // Mengubah byte menjadi karakter
  }

  Serial.print("Pesan diterima [");                   // Menampilkan informasi pesan
  Serial.print(topic);                                // Menampilkan nama topic
  Serial.print("]: ");                                // Menampilkan pemisah
  Serial.println(pesan);                              // Menampilkan isi pesan

  JsonDocument doc;                                   // Membuat objek untuk menyimpan JSON
  DeserializationError error = deserializeJson(doc, pesan); // Mengubah String menjadi JSON

  if (error) {                                        // Mengecek apakah JSON gagal diproses
    Serial.print("Gagal parsing JSON: ");             // Menampilkan pesan error
    Serial.println(error.c_str());                    // Menampilkan jenis error
    return;                                           // Menghentikan callback jika JSON tidak valid
  }

  const char* perintah = doc["perintah"];             // Mengambil nilai "perintah" dari JSON
  int intensitas = doc["intensitas"] | 255;           // Mengambil intensitas, default 255 jika tidak ada

  intensitas = constrain(intensitas, 0, 255);         // Membatasi intensitas agar hanya 0 sampai 255

  if (String(perintah) == "ON") {                    // Mengecek apakah perintah adalah ON
    analogWrite(ledPin, intensitas);                 // Menyalakan LED sesuai nilai PWM/intensitas
    Serial.print("Aktuator: ON, Intensitas: ");      // Menampilkan status LED
    Serial.println(intensitas);                      // Menampilkan nilai intensitas
  } 
  else if (String(perintah) == "OFF") {              // Mengecek apakah perintah adalah OFF
    analogWrite(ledPin, 0);                           // Mematikan LED dengan PWM 0
    Serial.println("Aktuator: OFF");                 // Menampilkan status LED
  }
}

void hubungkanWiFi() {                                // Fungsi untuk menghubungkan ESP32 ke WiFi
  WiFi.begin(ssid, password);                         // Memulai koneksi WiFi
  Serial.print("Menghubungkan ke WiFi");              // Menampilkan proses koneksi

  while (WiFi.status() != WL_CONNECTED) {             // Menunggu sampai WiFi berhasil terhubung
    delay(500);                                       // Menunggu 500 milidetik
    Serial.print(".");                                // Menampilkan titik sebagai indikator
  }

  Serial.println();                                   // Pindah ke baris baru
  Serial.println("WiFi berhasil terhubung!");         // Menampilkan status WiFi
  Serial.print("IP Address: ");                       // Menampilkan label IP
  Serial.println(WiFi.localIP());                     // Menampilkan IP ESP32
}

void hubungkanMQTT() {                                // Fungsi untuk menghubungkan ESP32 ke MQTT
  while (!client.connected()) {                       // Mengecek apakah MQTT belum terhubung
    Serial.print("Menghubungkan ke broker MQTT...");// Menampilkan proses koneksi

    String clientId = "ESP32Client-" + String(random(0xffff), HEX); // Membuat ID client acak

    if (client.connect(clientId.c_str())) {           // Mencoba menghubungkan ESP32 ke broker
      Serial.println("berhasil terhubung!");          // Menampilkan koneksi berhasil
      client.subscribe(topicPerintah);                // Subscribe ke topic perintah
      Serial.print("Subscribe ke topic: ");           // Menampilkan informasi topic
      Serial.println(topicPerintah);                  // Menampilkan nama topic
    } 
    else {                                             // Jika koneksi MQTT gagal
      Serial.print("gagal, rc=");                      // Menampilkan pesan kegagalan
      Serial.println(client.state());                 // Menampilkan kode status MQTT
      delay(2000);                                     // Menunggu 2 detik sebelum mencoba lagi
    }
  }
}

void setup() {                                        // Fungsi yang dijalankan satu kali saat ESP32 mulai
  Serial.begin(115200);                               // Memulai Serial Monitor dengan baud rate 115200
  pinMode(ledPin, OUTPUT);                             // Mengatur GPIO 26 sebagai output
  analogWrite(ledPin, 0);                             // Memastikan LED awalnya mati
  hubungkanWiFi();                                    // Menghubungkan ESP32 ke WiFi
  client.setServer(mqttServer, mqttPort);             // Mengatur broker dan port MQTT
  client.setCallback(callback);                       // Menentukan fungsi callback untuk pesan masuk
}

void loop() {                                         // Fungsi yang dijalankan terus-menerus
  if (!client.connected()) {                           // Mengecek apakah koneksi MQTT terputus
    hubungkanMQTT();                                  // Menghubungkan kembali ke broker MQTT
  }

  client.loop();                                      // Memproses pesan MQTT yang masuk
}
```   
<br>

# Percobaan 4B
Percobaan 4B dilakukan dengan menambahkan sensor DHT11 pada program percobaan 4a untuk membaca data suhu. Percobaan ini bertujuan untuk memahami proses publish data sensor melalui MQTT dalam format JSON sekaligus tetap menerima perintah untuk mengontrol LED. Penggunaan millis() diterapkan agar proses publish data setiap 5 detik tidak mengganggu proses subscribe MQTT.
## Library
```cpp
#include <ESP8266WiFi.h>
```
Library ini digunakan agar ESP8266 dapat menjalankan fungsi jaringan WiFi, seperti terhubung ke WiFi (Station) atau membuat jaringan WiFi sendiri (Access Point).  
<br>

```cpp
#include <ArduinoJson.h>
```
Library ini digunakan untuk membuat, mengolah, dan mengubah data dalam format JSON agar dapat digunakan dalam pertukaran data.  
<br>

```cpp
#include <PubSubClient.h>
```
Library ini digunakan untuk melakukan komunikasi menggunakan protokol MQTT, termasuk menghubungkan ESP8266 ke broker dan mengirimkan data melalui topic.   
<br>

```cpp
#include <DHT.h>
```
Library ini digunakan untuk enghubungkan dan membaca data dari sensor DHT, seperti DHT11 dan DHT22, termasuk data suhu dan kelembapan.  
<br>

## Diagram Rangkaian
![alt text](https://github.com/razdient/H1H024053-RASTA_LISTIADI-PRAKTIKUM_INTERNET_OF_THINGS/blob/main/Pertemuan%204%20-%20Komunikasi%20dan%20Pertukaran%20Data/Dokumentasi/Gambar%20Rangkaian%20Percobaan%204B.png?raw=true)   <br>
## Dokumentasi
![alt text](https://github.com/razdient/H1H024053-RASTA_LISTIADI-PRAKTIKUM_INTERNET_OF_THINGS/blob/main/Pertemuan%204%20-%20Komunikasi%20dan%20Pertukaran%20Data/Dokumentasi/Foto%20Rangkaian%20Percobaan%204B.jpeg?raw=true)

## Penjelasan Kode
```cpp
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
```
<br>

## Modifikasi Penambahan Topic MQTT untuk Mengendalikan Buzzer
```cpp
#include <WiFi.h>                    // Library untuk koneksi WiFi ESP32
#include <PubSubClient.h>            // Library untuk komunikasi MQTT
#include <ArduinoJson.h>             // Library untuk membaca data JSON
#include <DHT.h>                     // Library untuk sensor DHT11

const char* ssid = "RASTA_32";                 // Nama WiFi
const char* password = "12345678";         // Password WiFi

const char* mqttServer = "broker.hivemq.com";        // Alamat broker MQTT
const int mqttPort = 1883;                            // Port MQTT

const char* topicData = "unsoed/tk245004/kelompok2/rasta_data";       // Topic untuk data suhu
const char* topicPerintah = "unsoed/tk245004/kelompok2/rasta_led";    // Topic untuk perintah LED
const char* topicBuzzer = "unsoed/tk245004/kelompok2/rasta_buzzer";   // Topic baru untuk perintah buzzer

#define DHTPIN 4                                      // Pin DATA DHT11 pada GPIO 4
#define DHTTYPE DHT11                                 // Jenis sensor yang digunakan adalah DHT11

const int ledPin = 5;                                // LED terhubung ke GPIO 
const int buzzerPin = 0;                             // Buzzer terhubung ke GPIO 

DHT dht(DHTPIN, DHTTYPE);                             // Membuat objek sensor DHT11
WiFiClient espClient;                                 // Membuat objek koneksi WiFi
PubSubClient client(espClient);                       // Membuat objek komunikasi MQTT

unsigned long waktuTerakhirPublish = 0;               // Menyimpan waktu publish terakhir
const long intervalPublish = 5000;                    // Data suhu dikirim setiap 5 detik


void callback(char* topic, byte* payload, unsigned int length) { // Fungsi ketika pesan MQTT diterima

  String pesan;                                       // Variabel untuk menyimpan pesan MQTT

  for (unsigned int i = 0; i < length; i++) {         // Membaca payload satu per satu
    pesan += (char)payload[i];                        // Mengubah byte menjadi karakter
  }

  Serial.print("Pesan diterima [");                   // Menampilkan informasi pesan
  Serial.print(topic);                                // Menampilkan topic yang menerima pesan
  Serial.print("]: ");                                // Menampilkan pemisah
  Serial.println(pesan);                              // Menampilkan isi pesan

  JsonDocument doc;                                   // Membuat objek JSON
  DeserializationError error = deserializeJson(doc, pesan); // Mengubah pesan menjadi JSON

  if (error) {                                        // Mengecek apakah parsing JSON gagal
    Serial.println("Gagal parsing JSON");             // Menampilkan pesan kesalahan
    return;                                           // Menghentikan fungsi callback
  }

  const char* perintah = doc["perintah"];             // Mengambil nilai "perintah" dari JSON


  // ===============================
  // MEMERIKSA TOPIC LED
  // ===============================

  if (String(topic) == topicPerintah) {               // Mengecek apakah pesan berasal dari topic LED

    if (String(perintah) == "ON") {                   // Jika perintah LED adalah ON
      digitalWrite(ledPin, HIGH);                     // Menyalakan LED
      Serial.println("LED: ON");                      // Menampilkan status LED
    }

    else if (String(perintah) == "OFF") {             // Jika perintah LED adalah OFF
      digitalWrite(ledPin, LOW);                      // Mematikan LED
      Serial.println("LED: OFF");                     // Menampilkan status LED
    }
  }


  // ===============================
  // MEMERIKSA TOPIC BUZZER
  // ===============================

  else if (String(topic) == topicBuzzer) {            // Mengecek apakah pesan berasal dari topic buzzer

    if (String(perintah) == "ON") {                   // Jika perintah buzzer adalah ON
      digitalWrite(buzzerPin, HIGH);                  // Menyalakan buzzer
      Serial.println("Buzzer: ON");                   // Menampilkan status buzzer
    }

    else if (String(perintah) == "OFF") {             // Jika perintah buzzer adalah OFF
      digitalWrite(buzzerPin, LOW);                   // Mematikan buzzer
      Serial.println("Buzzer: OFF");                  // Menampilkan status buzzer
    }
  }
}


void hubungkanWiFi() {                                // Fungsi untuk menghubungkan ESP32 ke WiFi

  WiFi.begin(ssid, password);                         // Memulai koneksi WiFi

  Serial.print("Menghubungkan ke WiFi");              // Menampilkan proses koneksi

  while (WiFi.status() != WL_CONNECTED) {             // Menunggu sampai WiFi terhubung
    delay(500);                                       // Menunggu 500 milidetik
    Serial.print(".");                                // Menampilkan titik
  }

  Serial.println();                                   // Pindah ke baris baru
  Serial.println("WiFi berhasil terhubung!");         // Menampilkan status WiFi
  Serial.print("IP Address: ");                       // Menampilkan label IP
  Serial.println(WiFi.localIP());                     // Menampilkan alamat IP ESP32
}


void hubungkanMQTT() {                                // Fungsi untuk menghubungkan ESP32 ke MQTT

  while (!client.connected()) {                       // Selama MQTT belum terhubung

    Serial.print("Menghubungkan ke MQTT...");         // Menampilkan proses koneksi

    String clientId = "ESP32Client-" + String(random(0xffff), HEX); // Membuat ID client acak

    if (client.connect(clientId.c_str())) {           // Mencoba menghubungkan ke broker

      Serial.println("berhasil!");                    // Menampilkan koneksi berhasil

      client.subscribe(topicPerintah);                // Subscribe ke topic LED
      client.subscribe(topicBuzzer);                  // Subscribe ke topic buzzer

      Serial.println("Subscribe topic LED dan Buzzer"); // Menampilkan status subscribe
    }

    else {                                            // Jika koneksi gagal

      Serial.print("Gagal, rc=");                     // Menampilkan kode error
      Serial.println(client.state());                 // Menampilkan status MQTT
      delay(2000);                                    // Menunggu 2 detik sebelum mencoba lagi
    }
  }
}


void setup() {                                        // Fungsi yang dijalankan sekali saat ESP32 mulai

  Serial.begin(115200);                               // Memulai Serial Monitor

  pinMode(ledPin, OUTPUT);                             // Mengatur GPIO LED sebagai output
  pinMode(buzzerPin, OUTPUT);                          // Mengatur GPIO buzzer sebagai output

  digitalWrite(ledPin, LOW);                           // Memastikan LED awalnya mati
  digitalWrite(buzzerPin, LOW);                        // Memastikan buzzer awalnya mati

  dht.begin();                                        // Memulai sensor DHT11

  hubungkanWiFi();                                    // Menghubungkan ESP32 ke WiFi

  client.setServer(mqttServer, mqttPort);             // Mengatur broker MQTT dan portnya

  client.setCallback(callback);                       // Menentukan fungsi callback
}


void loop() {                                         // Fungsi yang dijalankan terus-menerus

  if (!client.connected()) {                           // Mengecek apakah MQTT terputus
    hubungkanMQTT();                                  // Menghubungkan kembali ke MQTT
  }

  client.loop();                                      // Memproses pesan MQTT yang masuk


  if (millis() - waktuTerakhirPublish >= intervalPublish) { // Mengecek apakah sudah 5 detik

    waktuTerakhirPublish = millis();                  // Menyimpan waktu publish terbaru

    float suhu = dht.readTemperature();               // Membaca suhu dari DHT11

    if (isnan(suhu)) {                                // Mengecek apakah pembacaan sensor gagal
      Serial.println("Gagal membaca sensor DHT11");   // Menampilkan pesan error
      return;                                         // Menghentikan proses loop saat ini
    }

    JsonDocument doc;                                 // Membuat objek JSON

    doc["suhu"] = suhu;                                // Memasukkan nilai suhu ke JSON

    char buffer[128];                                 // Menyediakan tempat untuk JSON

    serializeJson(doc, buffer);                       // Mengubah JSON menjadi teks

    client.publish(topicData, buffer);                // Mengirim data suhu ke topicData

    Serial.print("Data terkirim: ");                  // Menampilkan label data
    Serial.println(buffer);                           // Menampilkan data suhu
  }
}
```
