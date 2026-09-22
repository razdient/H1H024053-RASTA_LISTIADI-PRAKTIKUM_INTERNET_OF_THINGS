# Percobaan 3A
Percobaan 3A dilakukan dengan menghubungkan ESP8266 ke jaringan WiFi, kemudian membuat data suhu dan kelembaban dalam format JSON. Data tersebut dikirim ke server httpbin.org menggunakan metode HTTP POST melalui koneksi HTTPS. Setelah pengiriman, kode dan isi respons dari server ditampilkan pada Serial Monitor setiap 10 detik.
## Library
```cpp
#include <ESP8266WiFi.h>
```
Library ini digunakan agar ESP8266 dapat menjalankan fungsi jaringan WiFi, seperti terhubung ke WiFi (Station) atau membuat jaringan WiFi sendiri (Access Point).  
<br>

```cpp
#include <ESP8266HTTPClient.h>
```
Library ini digunakan untuk melakukan komunikasi menggunakan protokol HTTP/HTTPS, seperti mengirimkan data dari ESP8266 ke server.  
<br>

```cpp
#include <WiFiClientSecureBearSSL.h>
```
Library ini digunakan untuk membuat koneksi HTTPS yang lebih aman antara ESP8266 dan server menggunakan protokol SSL/TLS.  
<br>

```cpp
#include <ArduinoJson.h>
```
Library ini digunakan untuk membuat, mengolah, dan mengubah data dalam format JSON agar dapat digunakan dalam pertukaran data.  
<br>

## Diagram Rangkaian
![alt text](https://github.com/razdient/H1H024053-RASTA_LISTIADI-PRAKTIKUM_INTERNET_OF_THINGS/blob/main/Pertemuan%203%20-%20Protokol%20Komunikasi/Dokumentasi/Gambar%20Rangkaian%20Percobaan%203A%20&%203B.png?raw=true)
## Dokumentasi
![alt text](https://github.com/razdient/H1H024053-RASTA_LISTIADI-PRAKTIKUM_INTERNET_OF_THINGS/blob/main/Pertemuan%203%20-%20Protokol%20Komunikasi/Dokumentasi/Serial%20Monitor%20Percobaan%203A.png?raw=true)

## Penjelasan Kode
```cpp
#include <ESP8266WiFi.h>                 // Library untuk menghubungkan ESP8266 ke WiFi
#include <ESP8266HTTPClient.h>           // Library untuk melakukan komunikasi HTTP
#include <WiFiClientSecureBearSSL.h>     // Library untuk koneksi HTTPS
#include <ArduinoJson.h>                 // Library untuk membuat dan mengolah data JSON


// Nama WiFi yang akan digunakan ESP8266
const char* ssid = "ESP32_RASTA";

// Password WiFi
const char* password = "12345678";

// Alamat server yang digunakan untuk menerima data HTTP POST
const char* serverUrl = "https://httpbin.org/post";


void setup() {

  // Memulai komunikasi Serial dengan baud rate 115200
  Serial.begin(115200);

  // Menghubungkan ESP8266 ke jaringan WiFi
  WiFi.begin(ssid, password);

  // Menampilkan pesan saat proses menghubungkan WiFi
  Serial.print("Menghubungkan ke WiFi");

  // Menunggu sampai ESP8266 berhasil terhubung ke WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);            // Menunggu selama 500 milidetik
    Serial.print(".");     // Menampilkan titik sebagai tanda proses koneksi
  }

  // Menampilkan pesan setelah WiFi berhasil terhubung
  Serial.println();
  Serial.println("WiFi berhasil terhubung!");
}


void loop() {

  // Memeriksa apakah ESP8266 masih terhubung ke WiFi
  if (WiFi.status() == WL_CONNECTED) {

    // Membuat koneksi HTTPS menggunakan BearSSL
    std::unique_ptr<BearSSL::WiFiClientSecure> client(
      new BearSSL::WiFiClientSecure
    );

    // Mengabaikan pemeriksaan sertifikat SSL
    // Digunakan agar ESP8266 dapat melakukan koneksi HTTPS
    client->setInsecure();

    // Membuat objek HTTPClient untuk komunikasi dengan server
    HTTPClient http;

    // Memulai koneksi ke alamat server yang telah ditentukan
    if (http.begin(*client, serverUrl)) {

      // Memberitahu server bahwa data yang dikirim memiliki format JSON
      http.addHeader("Content-Type", "application/json");


      // Membuat objek untuk menyimpan data dalam format JSON
      JsonDocument doc;

      // Menambahkan data suhu ke dalam JSON
      doc["suhu"] = 28.5;

      // Menambahkan data kelembaban ke dalam JSON
      doc["kelembaban"] = 65.0;


      // Membuat variabel untuk menyimpan JSON dalam bentuk teks
      String requestBody;

      // Mengubah objek JSON menjadi teks/string
      serializeJson(doc, requestBody);


      // Menampilkan data JSON yang akan dikirim
      Serial.print("Mengirim data: ");
      Serial.println(requestBody);


      // Mengirim data ke server menggunakan metode HTTP POST
      int httpResponseCode = http.POST(requestBody);


      // Memeriksa apakah server memberikan response
      if (httpResponseCode > 0) {

        // Menampilkan kode response HTTP dari server
        Serial.print("Kode Response HTTP: ");
        Serial.println(httpResponseCode);

        // Menampilkan isi response yang diberikan oleh server
        Serial.println("Isi Response:");
        Serial.println(http.getString());

      } else {

        // Menampilkan pesan jika pengiriman data gagal
        Serial.print("Pengiriman gagal, kode error: ");
        Serial.println(httpResponseCode);
      }

      // Mengakhiri koneksi HTTP
      http.end();

    } else {

      // Ditampilkan jika koneksi ke server gagal dimulai
      Serial.println("Gagal memulai koneksi HTTP");
    }
  }

  // Menunggu 10 detik sebelum mengirim data berikutnya
  delay(10000);
}
```
<br>

## Modifikasi dapat mengirimkan data tambahan berupa waktu ke JSON
```cpp
#include <WiFi.h>                    // Library ini digunakan untuk menghubungkan ESP32 ke jaringan WiFi
#include <HTTPClient.h>              // Library ini digunakan untuk melakukan komunikasi HTTP/HTTPS
#include <WiFiClientSecure.h>        // Library ini digunakan untuk membuat koneksi HTTPS
#include <ArduinoJson.h>             // Library ini digunakan untuk membuat dan mengolah data JSON


// ==================== KONFIGURASI WIFI ====================

// Nama WiFi yang akan digunakan ESP32
const char* ssid = "ESP32_RASTA";

// Password WiFi
const char* password = "12345678";

// Alamat server yang akan menerima data HTTP POST
const char* serverUrl = "https://httpbin.org/post";


void setup() {

  // Memulai komunikasi Serial dengan baud rate 115200
  Serial.begin(115200);

  // Memulai koneksi ESP32 ke jaringan WiFi menggunakan SSID dan password
  WiFi.begin(ssid, password);

  // Menampilkan pesan bahwa ESP32 sedang mencoba terhubung ke WiFi
  Serial.print("Menghubungkan ke WiFi");

  // Menunggu sampai ESP32 berhasil terhubung ke WiFi
  while (WiFi.status() != WL_CONNECTED) {

    // Menunggu selama 500 milidetik sebelum memeriksa kembali koneksi
    delay(500);

    // Menampilkan titik sebagai tanda proses koneksi
    Serial.print(".");
  }

  // Pindah ke baris baru setelah proses koneksi selesai
  Serial.println();

  // Menampilkan pesan bahwa ESP32 berhasil terhubung ke WiFi
  Serial.println("WiFi berhasil terhubung!");
}


void loop() {

  // Memeriksa apakah ESP32 masih terhubung ke jaringan WiFi
  if (WiFi.status() == WL_CONNECTED) {

    // Membuat objek WiFiClientSecure untuk koneksi HTTPS
    WiFiClientSecure client;

    // Mengabaikan pemeriksaan sertifikat SSL agar dapat terhubung ke server HTTPS
    client.setInsecure();

    // Membuat objek HTTPClient untuk melakukan komunikasi HTTP
    HTTPClient http;

    // Memulai koneksi HTTP/HTTPS ke alamat server yang telah ditentukan
    if (http.begin(client, serverUrl)) {

      // Memberitahu server bahwa data yang dikirim memiliki format JSON
      http.addHeader("Content-Type", "application/json");


      // Membuat objek untuk menyimpan data dalam format JSON
      JsonDocument doc;

      // Menambahkan data suhu sebesar 28.5 ke dalam JSON
      doc["suhu"] = 28.5;

      // Menambahkan data kelembaban sebesar 65.0 ke dalam JSON
      doc["kelembaban"] = 65.0;

      // Mengambil waktu sejak ESP32 dinyalakan dalam satuan milidetik
      unsigned long waktu = millis();

      // Menambahkan nilai waktu ke dalam JSON
      doc["waktu"] = waktu;


      // Membuat variabel untuk menyimpan data JSON dalam bentuk teks
      String requestBody;

      // Mengubah objek JSON menjadi teks/string
      serializeJson(doc, requestBody);


      // Menampilkan pesan bahwa data JSON akan dikirim
      Serial.print("Mengirim data: ");

      // Menampilkan isi JSON yang akan dikirim ke server
      Serial.println(requestBody);


      // Mengirim data JSON ke server menggunakan metode HTTP POST
      int httpResponseCode = http.POST(requestBody);


      // Memeriksa apakah server memberikan response
      if (httpResponseCode > 0) {

        // Menampilkan kode response HTTP dari server
        Serial.print("Kode Response HTTP: ");

        // Menampilkan nilai kode response HTTP
        Serial.println(httpResponseCode);

        // Menampilkan judul untuk isi response dari server
        Serial.println("Isi Response:");

        // Mengambil dan menampilkan isi response dari server
        Serial.println(http.getString());

      } else {

        // Menampilkan pesan jika pengiriman data mengalami kegagalan
        Serial.print("Pengiriman gagal, kode error: ");

        // Menampilkan kode error dari proses HTTP
        Serial.println(httpResponseCode);
      }

      // Mengakhiri koneksi HTTP setelah proses selesai
      http.end();

    } else {

      // Menampilkan pesan jika koneksi ke server gagal dimulai
      Serial.println("Gagal memulai koneksi HTTP");
    }
  }

  // Menunggu selama 10 detik sebelum melakukan pengiriman data berikutnya
  delay(10000);
}
```
Bagian modifikasi utamanya ada pada tiga baris ini:
```cpp
unsigned long waktu = millis();
doc["waktu"] = waktu;
serializeJson(doc, requestBody);
```
```millis()``` menghasilkan waktu dalam milidetik sejak ESP32 mulai menjalankan program, lalu nilai tersebut dimasukkan ke JSON dengan nama "waktu". Jadi data yang dikirim sekarang kurang lebih berbentuk:

```{"suhu":28.5,"kelembaban":65,"waktu":10342}```

Artinya, pada saat data dikirim, ESP32 sudah berjalan selama 10.342 milidetik atau sekitar 10,3 detik.
<br>

# Percobaan 3B
Percobaan 3B dilakukan dengan menghubungkan ESP8266 ke jaringan WiFi dan broker MQTT untuk melakukan pertukaran data. Data suhu dan kelembaban dibuat dalam format JSON, kemudian dipublikasikan ke topic MQTT melalui broker broker.hivemq.com. Data dikirim secara berkala setiap 5 detik dan informasi data yang dikirim ditampilkan pada Serial Monitor.
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

## Diagram Rangkaian
![alt text](https://github.com/razdient/H1H024053-RASTA_LISTIADI-PRAKTIKUM_INTERNET_OF_THINGS/blob/main/Pertemuan%203%20-%20Protokol%20Komunikasi/Dokumentasi/Gambar%20Rangkaian%20Percobaan%203A%20&%203B.png?raw=true)
## Dokumentasi
![alt tezt](https://github.com/razdient/H1H024053-RASTA_LISTIADI-PRAKTIKUM_INTERNET_OF_THINGS/blob/main/Pertemuan%203%20-%20Protokol%20Komunikasi/Dokumentasi/Serial%20Monitor%20Percobaan%203B.png?raw=true)
## Penjelasan Kode
```cpp
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
```
<br>

