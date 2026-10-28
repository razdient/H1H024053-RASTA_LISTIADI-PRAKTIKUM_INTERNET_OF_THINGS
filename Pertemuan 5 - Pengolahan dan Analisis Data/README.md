# Percobaan 5A
Percobaan 5A dilakukan dengan menyimulasikan pembacaan suhu menggunakan data dummy yang memiliki variasi acak dan anomali setiap 25 kali pembacaan. Data suhu kemudian diolah menggunakan metode *moving average* untuk menghaluskan perubahan suhu serta mencatat nilai suhu minimum dan maksimum yang ditampilkan melalui Serial Monitor setiap 2 detik.
## Library
```cpp
#include <math.h>
```
Library ini digunakan untuk menyediakan fungsi matematika, seperti sin(), yang digunakan untuk menghasilkan variasi suhu dasar secara bertahap pada data dummy.
<br>

## Diagram Rangkaian  
![alt text](https://github.com/razdient/H1H024053-RASTA_LISTIADI-PRAKTIKUM_INTERNET_OF_THINGS/blob/main/Pertemuan%205%20-%20Pengolahan%20dan%20Analisis%20Data/Dokumentasi/Rangkaian%20Percobaan%205A%20%26%205B.png?raw=true)
## Dokumentasi
![alt text](https://github.com/razdient/H1H024053-RASTA_LISTIADI-PRAKTIKUM_INTERNET_OF_THINGS/blob/main/Pertemuan%205%20-%20Pengolahan%20dan%20Analisis%20Data/Dokumentasi/Serial%20Monitor%20Percobaan%205A.png?raw=true)

## Penjelasan Kode
```cpp
#include <math.h> // Library matematika untuk menggunakan fungsi sin()

const int N = 5;       // Menentukan jumlah sampel untuk moving average, yaitu 5 sampel
float buffer[N];       // Array untuk menyimpan maksimal 5 data suhu
int indeks = 0;        // Menunjukkan posisi array yang akan diisi data berikutnya
int jumlah = 0;        // Menghitung jumlah data yang sudah tersimpan di buffer

float suhuMin = 1000.0;  // Nilai awal suhu minimum dibuat sangat tinggi
float suhuMax = -1000.0; // Nilai awal suhu maksimum dibuat sangat rendah

// Fungsi untuk menghasilkan suhu dummy sebagai pengganti sensor DHT22
float bacaSuhuMock() {
    static unsigned long jumlahBaca = 0; // Menyimpan jumlah pembacaan dan mempertahankan nilainya
    jumlahBaca++; // Menambah jumlah pembacaan setiap fungsi dipanggil

    // Membuat suhu dasar yang berubah secara bertahap menggunakan fungsi sinus
    float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);

    // Menghasilkan noise acak antara -0,5 sampai 0,5 derajat Celsius
    float noise = random(-5, 6) * 0.1;

    // Menambahkan noise ke suhu dasar agar hasilnya menyerupai pembacaan sensor nyata
    float suhu = suhuDasar + noise;

    // Memeriksa apakah pembacaan sudah mencapai kelipatan 25
    if (jumlahBaca % 25 == 0) {
        suhu += 8.0; // Menambahkan 8 derajat untuk menyimulasikan anomali suhu
    }

    return suhu; // Mengembalikan nilai suhu dummy yang sudah dibuat
}

// Fungsi untuk menghitung rata-rata bergerak (moving average)
float hitungMovingAverage(float nilaiBaru) {
    buffer[indeks] = nilaiBaru; // Menyimpan suhu terbaru pada posisi array saat ini

    indeks = (indeks + 1) % N; // Memindahkan indeks ke posisi berikutnya, kembali ke 0 jika penuh

    if (jumlah < N) { // Memeriksa apakah jumlah data belum mencapai kapasitas buffer
        jumlah++;     // Menambah jumlah data yang tersimpan
    }

    float total = 0; // Menyiapkan variabel untuk menjumlahkan seluruh data suhu

    for (int i = 0; i < jumlah; i++) { // Mengulang dari data pertama hingga data yang tersimpan
        total += buffer[i]; // Menambahkan setiap data suhu ke variabel total
    }

    return total / jumlah; // Mengembalikan rata-rata suhu dari data yang tersimpan
}

// Fungsi setup dijalankan satu kali saat ESP8266 mulai menyala
void setup() {
    Serial.begin(115200); // Memulai komunikasi serial dengan kecepatan 115200 baud
    randomSeed(analogRead(A0)); // Mengatur titik awal angka acak menggunakan pembacaan pin A0
}

// Fungsi loop dijalankan berulang kali selama ESP8266 menyala
void loop() {
    float suhu = bacaSuhuMock(); // Mengambil satu nilai suhu dummy terbaru

    float suhuFilter = hitungMovingAverage(suhu); // Menghitung suhu hasil penyaringan moving average

    if (suhu < suhuMin) { // Memeriksa apakah suhu terbaru lebih rendah dari suhu minimum
        suhuMin = suhu;   // Memperbarui suhu minimum jika ditemukan nilai yang lebih rendah
    }

    if (suhu > suhuMax) { // Memeriksa apakah suhu terbaru lebih tinggi dari suhu maksimum
        suhuMax = suhu;   // Memperbarui suhu maksimum jika ditemukan nilai yang lebih tinggi
    }

    Serial.print("Mentah: "); // Menampilkan label untuk suhu asli
    Serial.print(suhu);       // Menampilkan nilai suhu asli sebelum penyaringan

    Serial.print(" | Filter: "); // Menampilkan label untuk suhu hasil filter
    Serial.print(suhuFilter);   // Menampilkan nilai suhu setelah moving average

    Serial.print(" | Min: "); // Menampilkan label suhu minimum
    Serial.print(suhuMin);   // Menampilkan suhu minimum yang pernah tercatat

    Serial.print(" | Max: "); // Menampilkan label suhu maksimum
    Serial.println(suhuMax);  // Menampilkan suhu maksimum dan pindah ke baris berikutnya

    delay(2000); // Memberi jeda 2000 milidetik atau 2 detik sebelum pembacaan berikutnya
}```cpp
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

## Modifikasi Kode Menghitung Simpangan Baku Dari Data Pada Buffer
```cpp

#include <math.h> // Menggunakan fungsi matematika seperti sin() dan sqrt()

const int N = 5; // Menentukan jumlah maksimum sampel dalam buffer
float buffer[N]; // Menyimpan maksimal 5 data suhu terakhir
int indeks = 0; // Menentukan posisi array untuk data berikutnya
int jumlah = 0; // Menghitung jumlah data yang sudah tersimpan

float suhuMin = 1000.0; // Menginisialisasi suhu minimum dengan nilai tinggi
float suhuMax = -1000.0; // Menginisialisasi suhu maksimum dengan nilai rendah

// Fungsi untuk menghasilkan suhu dummy sebagai pengganti sensor DHT22
float bacaSuhuMock() {
    static unsigned long jumlahBaca = 0; // Menyimpan penghitung antar pemanggilan fungsi
    jumlahBaca++; // Menambah jumlah pembacaan suhu

    float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08); // Membuat suhu dasar yang berubah bertahap
    float noise = random(-5, 6) * 0.1; // Membuat variasi acak antara -0,5 hingga 0,5 derajat
    float suhu = suhuDasar + noise; // Menggabungkan suhu dasar dan noise

    if (jumlahBaca % 25 == 0) { // Memeriksa apakah pembacaan ke-25 atau kelipatannya
        suhu += 8.0; // Menambahkan 8 derajat untuk menyimulasikan anomali
    }

    return suhu; // Mengembalikan nilai suhu dummy
}

// Fungsi untuk menghitung moving average dari buffer
float hitungMovingAverage(float nilaiBaru) {
    buffer[indeks] = nilaiBaru; // Menyimpan suhu baru pada posisi indeks saat ini
    indeks = (indeks + 1) % N; // Memajukan indeks dan kembali ke 0 jika mencapai batas buffer

    if (jumlah < N) { // Memeriksa apakah buffer belum terisi penuh
        jumlah++; // Menambah jumlah data yang tersimpan
    }

    float total = 0.0; // Menyiapkan penampung jumlah seluruh suhu

    for (int i = 0; i < jumlah; i++) { // Mengulang semua data yang tersimpan
        total += buffer[i]; // Menambahkan suhu ke total
    }

    return total / jumlah; // Mengembalikan rata-rata suhu
}

// Fungsi untuk menghitung simpangan baku data dalam buffer
float hitungSimpanganBaku() {
    if (jumlah < 2) { // Memastikan minimal ada dua data untuk simpangan baku sampel
        return 0.0; // Mengembalikan 0 jika data belum cukup
    }

    float total = 0.0; // Menyiapkan penampung jumlah suhu
    for (int i = 0; i < jumlah; i++) { // Mengulang semua data suhu dalam buffer
        total += buffer[i]; // Menjumlahkan suhu untuk menghitung rata-rata
    }

    float rataRata = total / jumlah; // Menghitung rata-rata suhu dalam buffer
    float jumlahKuadrat = 0.0; // Menyiapkan penampung jumlah kuadrat selisih

    for (int i = 0; i < jumlah; i++) { // Mengulang semua data dalam buffer
        float selisih = buffer[i] - rataRata; // Menghitung selisih suhu terhadap rata-rata
        jumlahKuadrat += selisih * selisih; // Menambahkan kuadrat selisih ke total
    }

    float varians = jumlahKuadrat / (jumlah - 1); // Menghitung varians sampel
    return sqrt(varians); // Mengakarkan varians untuk mendapatkan simpangan baku
}

// Fungsi setup dijalankan satu kali saat ESP32 dinyalakan
void setup() {
    Serial.begin(115200); // Memulai komunikasi serial dengan kecepatan 115200 baud
    randomSeed(analogRead(34)); // Mengatur titik awal angka acak dari pin input analog ESP32
}

// Fungsi loop dijalankan terus-menerus
void loop() {
    float suhu = bacaSuhuMock(); // Membaca suhu dummy terbaru
    float suhuFilter = hitungMovingAverage(suhu); // Menghitung rata-rata bergerak
    float simpanganBaku = hitungSimpanganBaku(); // Menghitung simpangan baku buffer

    if (suhu < suhuMin) { // Memeriksa apakah suhu lebih rendah dari minimum sebelumnya
        suhuMin = suhu; // Memperbarui suhu minimum
    }

    if (suhu > suhuMax) { // Memeriksa apakah suhu lebih tinggi dari maksimum sebelumnya
        suhuMax = suhu; // Memperbarui suhu maksimum
    }

    Serial.print("Mentah: "); // Menampilkan label suhu asli
    Serial.print(suhu); // Menampilkan suhu sebelum filter

    Serial.print(" | Filter: "); // Menampilkan label suhu hasil filter
    Serial.print(suhuFilter); // Menampilkan suhu hasil moving average

    Serial.print(" | Simpangan Baku: "); // Menampilkan label simpangan baku
    Serial.print(simpanganBaku); // Menampilkan simpangan baku data dalam buffer

    Serial.print(" | Min: "); // Menampilkan label suhu minimum
    Serial.print(suhuMin); // Menampilkan suhu minimum yang pernah tercatat

    Serial.print(" | Max: "); // Menampilkan label suhu maksimum
    Serial.println(suhuMax); // Menampilkan suhu maksimum dan mengakhiri baris

    delay(2000); // Menunggu 2 detik sebelum melakukan pembacaan berikutnya
}
```
Perubahan utama ada pada fungsi ```hitungSimpanganBaku()```. Fungsi tersebut menghitung rata-rata data di dalam buffer, menjumlahkan kuadrat selisih setiap suhu terhadap rata-rata, lalu menghitung akar dari varians sampel.   
<br>

# Percobaan 5B
Percobaan 5B dilakukan dengan menyimulasikan pembacaan suhu menggunakan ESP8266 dan mengirimkan data ke Python melalui protokol MQTT. Data suhu yang diterima kemudian disimpan dalam file CSV, dianalisis menggunakan metode Z-score untuk mendeteksi anomali, serta divisualisasikan dalam bentuk grafik.
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
#include <math.h>
```
Library ini digunakan untuk menyediakan fungsi matematika, seperti sin(), yang digunakan untuk menghasilkan variasi suhu dasar secara bertahap pada data dummy.
<br>

```py
import json
```
Library ini digunakan untuk mengolah data dalam format JSON yang diterima dari MQTT.
<br>

```py
import time
```
Library ini digunakan untuk mengambil dan memformat waktu saat data suhu diterima.
<br>

```py
import paho.mqtt.client as mqtt
```
Library ini digunakan untuk melakukan komunikasi MQTT antara program Python dan broker MQTT.
<br>

```py
import panda as pd
```
Library ini digunakan untuk mengolah data suhu dalam bentuk tabel dan menyimpannya ke file CSV.
<br>

```py
import matplotlib.pylot as plt
```
Library ini digunakan untuk membuat dan menampilkan grafik suhu serta hasil deteksi anomali.
<br>

## Diagram Rangkaian
![alt text](https://github.com/razdient/H1H024053-RASTA_LISTIADI-PRAKTIKUM_INTERNET_OF_THINGS/blob/main/Pertemuan%205%20-%20Pengolahan%20dan%20Analisis%20Data/Dokumentasi/Rangkaian%20Percobaan%205A%20%26%205B.png?raw=true)   
<br>

## Dokumentasi
![alt text](https://github.com/razdient/H1H024053-RASTA_LISTIADI-PRAKTIKUM_INTERNET_OF_THINGS/blob/main/Pertemuan%205%20-%20Pengolahan%20dan%20Analisis%20Data/Dokumentasi/Output%20Program%20Python%20Percobaan%205B.png?raw=true)
![alt text](https://github.com/razdient/H1H024053-RASTA_LISTIADI-PRAKTIKUM_INTERNET_OF_THINGS/blob/main/Pertemuan%205%20-%20Pengolahan%20dan%20Analisis%20Data/Dokumentasi/Serial%20Monitor%20Percobaan%205B.png?raw=true)
![alt text](https://github.com/razdient/H1H024053-RASTA_LISTIADI-PRAKTIKUM_INTERNET_OF_THINGS/blob/main/Pertemuan%205%20-%20Pengolahan%20dan%20Analisis%20Data/Dokumentasi/grafik_suhu.png?raw=true)

## Penjelasan Kode
### Arduino IDE
```cpp
#include <ESP8266WiFi.h>  // Library untuk menghubungkan ESP8266 ke Wi-Fi
#include <PubSubClient.h> // Library untuk komunikasi MQTT
#include <math.h>         // Library matematika untuk fungsi sin()

// Nama Wi-Fi yang akan digunakan ESP8266
const char* ssid = "Nxxy";

// Password Wi-Fi
const char* password = "Nuxxccyy";

// Alamat broker MQTT HiveMQ
const char* mqtt_server = "broker.hivemq.com";

// Port MQTT tanpa enkripsi TLS
const int mqtt_port = 1883;

// Topic untuk mengirim data suhu ke Python
const char* topic = "unsoed/tk245004/kelompok2/data";

// Membuat objek koneksi Wi-Fi untuk komunikasi jaringan
WiFiClient espClient;

// Membuat objek MQTT menggunakan koneksi Wi-Fi
PubSubClient client(espClient);

// Fungsi untuk menghasilkan suhu dummy pengganti sensor DHT22
float bacaSuhuMock() {
    // Menyimpan jumlah pembacaan meskipun fungsi dipanggil berulang
    static unsigned long jumlahBaca = 0;

    // Menambah penghitung setiap pembacaan suhu
    jumlahBaca++;

    // Membuat suhu dasar yang berubah secara bertahap
    float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);

    // Membuat variasi acak antara -0,5 dan 0,5 derajat
    float noise = random(-5, 6) * 0.1;

    // Menggabungkan suhu dasar dengan variasi acak
    float suhu = suhuDasar + noise;

    // Memeriksa apakah pembacaan merupakan kelipatan 25
    if (jumlahBaca % 25 == 0) {
        suhu += 8.0; // Menambahkan 8 derajat sebagai simulasi anomali
    }

    // Mengembalikan nilai suhu yang dihasilkan
    return suhu;
}

// Fungsi untuk menghubungkan ESP8266 ke Wi-Fi
void setupWiFi() {
    delay(10); // Memberi jeda singkat sebelum memulai koneksi

    Serial.println(); // Membuat baris baru pada Serial Monitor
    Serial.print("Menghubungkan ke Wi-Fi: "); // Menampilkan status koneksi
    Serial.println(ssid); // Menampilkan nama Wi-Fi

    WiFi.mode(WIFI_STA); // Mengatur ESP8266 sebagai perangkat klien Wi-Fi
    WiFi.begin(ssid, password); // Memulai koneksi menggunakan kredensial Wi-Fi

    // Menunggu sampai ESP8266 berhasil terhubung
    while (WiFi.status() != WL_CONNECTED) {
        delay(500); // Menunggu setengah detik
        Serial.print("."); // Menampilkan tanda selama proses koneksi
    }

    Serial.println(); // Pindah ke baris baru
    Serial.println("Wi-Fi terhubung!"); // Menampilkan pesan berhasil
    Serial.print("IP ESP8266: "); // Menampilkan label alamat IP
    Serial.println(WiFi.localIP()); // Menampilkan alamat IP ESP8266
}

// Fungsi untuk menyambungkan kembali ke broker MQTT jika terputus
void reconnectMQTT() {
    // Mengulang proses sampai koneksi MQTT berhasil
    while (!client.connected()) {
        Serial.print("Menghubungkan ke MQTT..."); // Menampilkan status koneksi

        // Membuat ID klien MQTT
        String clientId = "ESP8266-Kelompok2-";

        // Menambahkan ID chip agar nama klien lebih unik
        clientId += String(ESP.getChipId(), HEX);

        // Mencoba menghubungkan klien ke broker MQTT
        if (client.connect(clientId.c_str())) {
            Serial.println("berhasil!"); // Menampilkan pesan jika berhasil
        } else {
            Serial.print("gagal, kode: "); // Menampilkan pesan kegagalan
            Serial.println(client.state()); // Menampilkan kode status MQTT
            delay(3000); // Menunggu 3 detik sebelum mencoba kembali
        }
    }
}

// Fungsi yang dijalankan sekali ketika ESP8266 dinyalakan
void setup() {
    Serial.begin(115200); // Memulai komunikasi serial pada 115200 baud

    // Mengatur titik awal generator angka acak menggunakan pin A0
    randomSeed(analogRead(A0));

    setupWiFi(); // Menghubungkan ESP8266 ke Wi-Fi

    // Menentukan alamat dan port broker MQTT
    client.setServer(mqtt_server, mqtt_port);
}

// Fungsi yang terus dijalankan selama ESP8266 menyala
void loop() {
    // Memeriksa apakah koneksi Wi-Fi terputus
    if (WiFi.status() != WL_CONNECTED) {
        setupWiFi(); // Mencoba menghubungkan kembali ke Wi-Fi
    }

    // Memeriksa apakah koneksi MQTT terputus
    if (!client.connected()) {
        reconnectMQTT(); // Mencoba menghubungkan kembali ke MQTT
    }

    client.loop(); // Menjaga komunikasi MQTT tetap berjalan

    // Mengambil suhu dummy terbaru
    float suhu = bacaSuhuMock();

    // Menyiapkan array karakter untuk menyimpan pesan JSON
    char payload[64];

    // Mengubah suhu menjadi format JSON, misalnya {"suhu":28.45}
    snprintf(payload, sizeof(payload), "{\"suhu\":%.2f}", suhu);

    // Mengirim pesan JSON ke topic MQTT dan menyimpan status pengiriman
    bool berhasil = client.publish(topic, payload);

    Serial.print("Suhu: "); // Menampilkan label suhu
    Serial.print(suhu); // Menampilkan nilai suhu
    Serial.print(" C | MQTT: "); // Menampilkan satuan dan status MQTT

    // Menampilkan status apakah pesan berhasil dikirim ke broker
    Serial.println(berhasil ? "Terkirim" : "Gagal");

    delay(2000); // Menunggu 2 detik sebelum mengirim data berikutnya
}
```
<br>   

### Python
```py
import json  # Mengolah data JSON yang diterima dari MQTT
import time  # Mengambil waktu saat data diterima
import paho.mqtt.client as mqtt  # Library untuk komunikasi MQTT
import pandas as pd  # Library untuk mengolah data dalam bentuk tabel
import matplotlib.pyplot as plt  # Library untuk membuat grafik

# Menentukan alamat broker MQTT HiveMQ
BROKER = "broker.hivemq.com"

# Menentukan port komunikasi MQTT
PORT = 1883

# Menentukan topic yang akan didengarkan oleh Python
# Topic harus sama dengan topic pada program Arduino
TOPIC = "unsoed/tk245004/kelompok2/data"

# Menentukan jumlah data suhu yang akan dikumpulkan
JUMLAH_SAMPEL = 30

# Membuat list kosong untuk menampung data suhu
data = []


# Fungsi yang dipanggil ketika Python terhubung ke broker MQTT
def on_connect(client, userdata, flags, reason_code, properties):
    # Menampilkan kode status koneksi
    print("Terhubung ke broker, kode:", reason_code)

    # Memeriksa apakah koneksi berhasil
    if reason_code == 0:
        # Berlangganan ke topic yang sudah ditentukan
        client.subscribe(TOPIC)

        # Menampilkan nama topic yang digunakan
        print("Berlangganan topic:", TOPIC)
    else:
        # Menampilkan pesan jika koneksi gagal
        print("Gagal terhubung ke broker MQTT")


# Fungsi yang dipanggil ketika pesan MQTT diterima
def on_message(client, userdata, msg):
    try:
        # Mengubah pesan dari byte menjadi teks lalu membaca JSON
        payload = json.loads(msg.payload.decode())

        # Mengambil nilai suhu dari bagian "suhu" pada JSON
        suhu = float(payload["suhu"])

    # Menangani data yang tidak valid atau tidak memiliki nilai suhu
    except (ValueError, KeyError, TypeError, json.JSONDecodeError):
        # Menampilkan pesan jika data tidak dapat diproses
        print("Data tidak valid, diabaikan")

        # Menghentikan fungsi untuk pesan yang tidak valid
        return

    # Mengambil waktu saat data diterima dalam format tahun-bulan-tanggal
    waktu = time.strftime("%Y-%m-%d %H:%M:%S")

    # Menambahkan waktu dan suhu ke dalam list data
    data.append({
        "waktu": waktu,  # Menyimpan waktu penerimaan data
        "suhu": suhu     # Menyimpan nilai suhu
    })

    # Menampilkan urutan sampel, jumlah target, dan nilai suhu
    print(f"[{len(data)}/{JUMLAH_SAMPEL}] Suhu = {suhu:.2f} °C")

    # Memeriksa apakah jumlah sampel sudah mencapai 30
    if len(data) >= JUMLAH_SAMPEL:
        # Menampilkan pesan bahwa pengumpulan data selesai
        print("30 sampel diterima. Mengakhiri akuisisi data...")

        # Memutuskan koneksi MQTT setelah target sampel tercapai
        client.disconnect()


# Tahap 1: Akuisisi data suhu melalui MQTT

# Membuat klien MQTT dengan sistem callback API versi 2
client = mqtt.Client(
    callback_api_version=mqtt.CallbackAPIVersion.VERSION2
)

# Mengatur fungsi yang dijalankan saat koneksi berhasil
client.on_connect = on_connect

# Mengatur fungsi yang dijalankan saat pesan diterima
client.on_message = on_message

try:
    # Menampilkan informasi bahwa koneksi sedang dimulai
    print("Menghubungkan ke broker MQTT...")

    # Menghubungkan Python ke broker dengan waktu keepalive 60 detik
    client.connect(BROKER, PORT, keepalive=60)

    # Menjalankan komunikasi MQTT sampai koneksi dihentikan
    client.loop_forever()

# Menangani kesalahan koneksi atau komunikasi MQTT
except Exception as e:
    # Menampilkan informasi kesalahan yang terjadi
    print("Koneksi MQTT gagal:", e)


# Tahap 2: Menyimpan data yang diterima ke file CSV

# Memeriksa apakah ada data yang berhasil dikumpulkan
if len(data) == 0:
    # Memberi tahu pengguna jika tidak ada data yang diterima
    print("Tidak ada data diterima. Periksa koneksi dan topic MQTT.")

else:
    # Mengubah list data menjadi DataFrame atau tabel pandas
    df = pd.DataFrame(data)

    # Menyimpan tabel ke file CSV tanpa kolom indeks tambahan
    df.to_csv("data_suhu.csv", index=False)

    # Menampilkan pesan bahwa data awal berhasil disimpan
    print("\nData tersimpan di data_suhu.csv")

    # Tahap 3: Analisis statistik dan deteksi anomali

    # Menghitung rata-rata seluruh nilai suhu
    rata2 = df["suhu"].mean()

    # Menghitung standar deviasi sampel suhu
    std = df["suhu"].std()

    # Memeriksa apakah standar deviasi tidak tersedia atau bernilai nol
    if pd.isna(std) or std == 0:
        # Memberikan nilai z-score nol jika standar deviasi tidak bisa digunakan
        df["zscore"] = 0.0

    else:
        # Menghitung z-score untuk setiap nilai suhu
        # Z-score menunjukkan jarak suhu dari rata-rata dalam satuan standar deviasi
        df["zscore"] = (df["suhu"] - rata2) / std

    # Menandai suhu sebagai anomali jika nilai absolut z-score lebih dari 2
    df["anomali"] = df["zscore"].abs() > 2

    # Menyimpan data beserta hasil analisis ke file CSV
    df.to_csv("data_suhu.csv", index=False)

    # Menampilkan judul hasil analisis
    print("\n=== HASIL ANALISIS SUHU ===")

    # Menampilkan jumlah sampel yang berhasil diterima
    print(f"Jumlah sampel : {len(df)}")

    # Menampilkan nilai rata-rata suhu dengan dua angka desimal
    print(f"Rata-rata     : {rata2:.2f} °C")

    # Menampilkan nilai standar deviasi
    print(f"Std deviasi   : {std:.2f}")

    # Menampilkan suhu paling rendah dalam kumpulan data
    print(f"Minimum       : {df['suhu'].min():.2f} °C")

    # Menampilkan suhu paling tinggi dalam kumpulan data
    print(f"Maksimum      : {df['suhu'].max():.2f} °C")

    # Menghitung dan menampilkan jumlah data yang terdeteksi sebagai anomali
    print(f"Jumlah anomali: {df['anomali'].sum()}")

    # Tahap 4: Membuat visualisasi data suhu

    # Membuat area grafik berukuran lebar 10 dan tinggi 4 inci
    plt.figure(figsize=(10, 4))

    # Membuat garis grafik suhu berdasarkan urutan sampel
    plt.plot(
        df.index,  # Sumbu X menggunakan indeks atau urutan data
        df["suhu"],  # Sumbu Y menggunakan nilai suhu
        marker="o",  # Memberikan tanda lingkaran pada setiap titik
        label="Suhu"  # Memberikan nama seri data pada legenda
    )

    # Memilih baris yang ditandai sebagai anomali
    anom = df[df["anomali"]]

    # Menampilkan titik anomali pada grafik
    plt.scatter(
        anom.index,  # Posisi X untuk data anomali
        anom["suhu"],  # Nilai suhu pada data anomali
        color="red",  # Memberi warna merah pada titik anomali
        s=100,  # Mengatur ukuran titik
        label="Anomali",  # Memberi nama titik pada legenda
        zorder=3  # Membuat titik tampil di depan garis grafik
    )

    # Menambahkan garis horizontal pada nilai rata-rata suhu
    plt.axhline(
        rata2,  # Posisi garis sesuai nilai rata-rata
        color="gray",  # Mengatur warna garis menjadi abu-abu
        linestyle="--",  # Menggunakan garis putus-putus
        label="Rata-rata"  # Memberi nama garis pada legenda
    )

    # Memberi label pada sumbu X
    plt.xlabel("Urutan Data")

    # Memberi label pada sumbu Y
    plt.ylabel("Suhu (°C)")

    # Memberi judul grafik
    plt.title("Data Suhu dan Deteksi Anomali")

    # Menampilkan legenda grafik
    plt.legend()

    # Menampilkan garis bantu pada grafik
    plt.grid(True, alpha=0.3)

    # Mengatur tata letak agar elemen grafik tidak saling bertumpuk
    plt.tight_layout()

    # Menyimpan grafik sebagai file gambar PNG
    plt.savefig("grafik_suhu.png")

    # Menampilkan informasi bahwa grafik sudah disimpan
    print("Grafik tersimpan di grafik_suhu.png")

    # Menampilkan grafik pada layar
    plt.show()
```   
<br>

## Modifikasi Kode Penerapan Moving Average (Jendela 5 Data) Sebelum Deteksi Anomali Menggunakan Z-Score.
```py
import pandas as pd  # Mengimpor pandas untuk membaca dan mengolah data CSV
import numpy as np  # Mengimpor numpy untuk operasi matematika

nama_file = "data_suhu.csv"  # Menentukan nama file CSV hasil percobaan 5B
df = pd.read_csv(nama_file)  # Membaca data dari file CSV ke DataFrame

kolom_suhu = "temperature"  # Menentukan nama kolom yang berisi data suhu
data_suhu = pd.to_numeric(df[kolom_suhu], errors="coerce")  # Mengubah data suhu menjadi angka dan data tidak valid menjadi NaN
data_suhu = data_suhu.dropna()  # Menghapus baris suhu yang kosong atau tidak valid

rata_asli = data_suhu.mean()  # Menghitung rata-rata suhu mentah
std_asli = data_suhu.std()  # Menghitung simpangan baku sampel suhu mentah

if std_asli != 0:  # Memastikan simpangan baku tidak nol agar pembagian aman
    z_asli = (data_suhu - rata_asli) / std_asli  # Menghitung z-score setiap data suhu mentah
else:  # Menjalankan bagian ini jika semua data suhu sama
    z_asli = pd.Series(0.0, index=data_suhu.index)  # Mengisi z-score dengan nol karena tidak ada variasi

anomali_asli = z_asli.abs() > 2  # Menandai anomali jika nilai absolut z-score lebih dari 2
jumlah_anomali_asli = int(anomali_asli.sum())  # Menghitung jumlah anomali pada data mentah

moving_average = data_suhu.rolling(window=5, min_periods=5).mean()  # Menghitung rata-rata bergerak dari setiap 5 data berurutan
data_filter = moving_average.dropna()  # Mengambil hasil moving average setelah tersedia 5 data

rata_filter = data_filter.mean()  # Menghitung rata-rata hasil moving average
std_filter = data_filter.std()  # Menghitung simpangan baku hasil moving average

if std_filter != 0:  # Memastikan simpangan baku hasil filter tidak nol
    z_filter = (data_filter - rata_filter) / std_filter  # Menghitung z-score dari data hasil moving average
else:  # Menjalankan bagian ini jika semua hasil moving average sama
    z_filter = pd.Series(0.0, index=data_filter.index)  # Mengisi z-score dengan nol karena tidak ada variasi

anomali_filter = z_filter.abs() > 2  # Menandai anomali pada hasil filter jika absolut z-score lebih dari 2
jumlah_anomali_filter = int(anomali_filter.sum())  # Menghitung jumlah anomali setelah moving average

print("=== HASIL PERBANDINGAN DETEKSI ANOMALI ===")  # Menampilkan judul hasil perbandingan
print(f"Jumlah data suhu valid: {len(data_suhu)}")  # Menampilkan jumlah data suhu yang valid
print(f"Anomali pada data asli: {jumlah_anomali_asli}")  # Menampilkan jumlah anomali sebelum moving average
print(f"Anomali setelah moving average: {jumlah_anomali_filter}")  # Menampilkan jumlah anomali setelah moving average
print(f"Data yang digunakan setelah moving average: {len(data_filter)}")  # Menampilkan jumlah data yang bisa dihitung setelah filter

selisih = jumlah_anomali_asli - jumlah_anomali_filter  # Menghitung selisih jumlah anomali sebelum dan sesudah filter

if selisih > 0:  # Memeriksa apakah jumlah anomali berkurang setelah filter
    print(f"Anomali berkurang sebanyak {selisih} data setelah moving average.")  # Menampilkan jumlah pengurangan anomali
elif selisih < 0:  # Memeriksa apakah jumlah anomali justru bertambah
    print(f"Anomali bertambah sebanyak {abs(selisih)} data setelah moving average.")  # Menampilkan jumlah penambahan anomali
else:  # Menjalankan bagian ini jika jumlah anomali sama
    print("Jumlah anomali sama sebelum dan sesudah moving average.")  # Menampilkan hasil bahwa jumlah anomali tidak berubah

hasil = pd.DataFrame(index=data_suhu.index)  # Membuat DataFrame baru untuk menyimpan hasil analisis
hasil["suhu_asli"] = data_suhu  # Menyimpan data suhu mentah
hasil["z_score_asli"] = z_asli  # Menyimpan z-score dari data mentah
hasil["anomali_asli"] = anomali_asli  # Menyimpan status anomali data mentah
hasil["moving_average_5"] = moving_average  # Menyimpan hasil moving average 5 data
hasil["z_score_filter"] = z_filter.reindex(hasil.index)  # Menyimpan z-score hasil filter sesuai indeks data
hasil["anomali_filter"] = anomali_filter.reindex(hasil.index).fillna(False).astype(bool)  # Menyimpan status anomali hasil filter

hasil.to_csv("hasil_perbandingan_anomali.csv", index=False)  # Menyimpan hasil analisis ke file CSV baru
print("Hasil detail disimpan ke hasil_perbandingan_anomali.csv")  # Menampilkan informasi file hasil
```
Modifikasi ini menerapkan metode moving average dengan jendela 5 data untuk menghaluskan data suhu dari file CSV sebelum menghitung z-score. Jumlah anomali dari data asli dan data hasil moving average kemudian dibandingkan untuk mengetahui pengaruh proses penghalusan terhadap deteksi anomali.
