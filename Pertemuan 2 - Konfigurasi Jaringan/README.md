# Percobaan 2A
Percobaan 2A dilakukan dengan mengatur ESP8266 sebagai WiFi Station (STA) untuk terhubung ke jaringan WiFi menggunakan SSID dan password yang telah ditentukan. Program mengecek status koneksi hingga berhasil, kemudian menampilkan IP Address, MAC Address, dan kekuatan sinyal (RSSI) pada Serial Monitor. LED digunakan sebagai indikator, yaitu menyala saat ESP8266 terhubung dan mati saat koneksi terputus.
## Library
```cpp
#include <ESP8266WiFi.h>
```
Library ini digunakan agar ESP8266 dapat menjalankan fungsi jaringan WiFi, seperti terhubung ke WiFi (Station) atau membuat jaringan WiFi sendiri (Access Point).
## Diagram Rangkaian
![alt text](https://github.com/razdient/H1H024053-RASTA_LISTIADI-PRAKTIKUM_INTERNET_OF_THINGS/blob/main/Pertemuan%202%20-%20Konfigurasi%20Jaringan/Dokumentasi/Screenshot%202026-09-10%20215348.png?raw=true)
## Dokumentasi
![alt tezt](https://github.com/razdient/H1H024053-RASTA_LISTIADI-PRAKTIKUM_INTERNET_OF_THINGS/blob/main/Pertemuan%202%20-%20Konfigurasi%20Jaringan/Dokumentasi/WhatsApp%20Image%202026-09-16%20at%2010.26.31.jpeg?raw=true)
## Penjelasan Kode
```cpp
#include <ESP8266WiFi.h>
```
Untuk memanggil library ESP8266WiFi agar ESP8266 dapat menggunakan fungsi-fungsi jaringan WiFi, seperti terhubung ke jaringan WiFi dan membaca informasi koneksi.
<br>

```cpp
const char* ssid = "ESP - RASTA";
const char* password = "12345678";
```
Konfigurasi nama WiFi (SSID) dan password yang akan digunakan ESP8266 untuk terhubung ke jaringan WiFi.
<br>

```cpp
const int ledPin = 2;
```
Menentukan GPIO 2 sebagai pin yang digunakan untuk LED indikator status koneksi WiFi.
<br>

```cpp
void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
```
Menginisialisasi Serial Monitor dengan baud rate 115200, mengatur pin LED sebagai output, dan mematikan LED sebagai kondisi awal.
<br>

```cpp
WiFi.mode(WIFI_STA);
```
Mengatur ESP8266 ke dalam mode Station (STA), sehingga ESP8266 berperan sebagai perangkat yang terhubung ke jaringan WiFi yang sudah tersedia.
<br>

```cpp
WiFi.begin(ssid, password);
```
Memulai proses koneksi ESP8266 ke jaringan WiFi menggunakan SSID dan password yang telah ditentukan.
<br>

```cpp
Serial.print("Menghubungkan ke WiFi");
```
Menampilkan pesan pada Serial Monitor bahwa ESP8266 sedang mencoba terhubung ke jaringan WiFi.
<br>

```cpp
while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
}
```
Melakukan pengecekan status koneksi secara berulang. Jika belum terhubung, program menunggu 500 ms dan menampilkan tanda titik sampai koneksi berhasil.
<br>

```cpp
Serial.println();
Serial.println("WiFi berhasil terhubung!");
```
Menampilkan pesan bahwa ESP8266 berhasil terhubung ke jaringan WiFi.
<br>

```cpp
Serial.print("IP Address : ");
Serial.println(WiFi.localIP());
```
Menampilkan IP Address yang diperoleh ESP8266 setelah berhasil terhubung ke jaringan WiFi.
<br>

```cpp
Serial.print("MAC Address : ");
Serial.println(WiFi.macAddress());
```
Menampilkan MAC Address, yaitu alamat identitas unik perangkat ESP8266 pada jaringan.
<br>

```cpp
Serial.print("RSSI (dBm) : ");
Serial.println(WiFi.RSSI());
```
Menampilkan nilai RSSI (Received Signal Strength Indicator) yang menunjukkan kekuatan sinyal WiFi yang diterima ESP8266.
<br>

```cpp
digitalWrite(ledPin, HIGH);
```
Menyalakan LED sebagai indikator bahwa ESP8266 berhasil terhubung ke WiFi.
<br>

```cpp
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
```
Melakukan pengecekan status koneksi WiFi secara terus-menerus.
<br>

```cpp
Serial.println("Status: Terhubung");
digitalWrite(ledPin, HIGH);
```
Jika ESP8266 masih terhubung, Serial Monitor menampilkan "Status: Terhubung" dan LED tetap menyala.
<br>

```cpp
} else {
  Serial.println("Status: Terputus");
  digitalWrite(ledPin, LOW);
}
```
Jika koneksi WiFi terputus, Serial Monitor menampilkan "Status: Terputus" dan LED dimatikan.
<br>

```cpp
delay(5000);
```
Memberikan jeda selama 5 detik sebelum ESP8266 kembali mengecek status koneksi WiFi.

## Modifikasi Reconnect Otomatis ketika terputus
```cpp
#include <ESP8266WiFi.h> // Memanggil library WiFi untuk ESP8266

const char* ssid = "ESP - RASTA"; // Menentukan nama WiFi yang akan dihubungkan
const char* password = "12345678"; // Menentukan password WiFi

const int ledPin = 2; // Menentukan GPIO 2 sebagai pin LED indikator

void setup() {
  Serial.begin(115200); // Memulai komunikasi dengan Serial Monitor

  pinMode(ledPin, OUTPUT); // Mengatur pin LED sebagai output

  digitalWrite(ledPin, LOW); // Mematikan LED sebagai kondisi awal

  WiFi.mode(WIFI_STA); // Mengatur ESP8266 sebagai WiFi Station

  WiFi.begin(ssid, password); // Memulai koneksi ke jaringan WiFi

  Serial.print("Menghubungkan ke WiFi"); // Menampilkan pesan proses koneksi

  while (WiFi.status() != WL_CONNECTED) { // Mengecek apakah ESP8266 sudah terhubung
    delay(500); // Menunggu selama 500 milidetik
    Serial.print("."); // Menampilkan titik sebagai tanda masih mencoba terhubung
  }

  Serial.println(); // Membuat baris baru pada Serial Monitor
  Serial.println("WiFi berhasil terhubung!"); // Menampilkan pesan koneksi berhasil

  Serial.print("IP Address : "); // Menampilkan keterangan IP Address
  Serial.println(WiFi.localIP()); // Menampilkan IP Address ESP8266

  Serial.print("MAC Address : "); // Menampilkan keterangan MAC Address
  Serial.println(WiFi.macAddress()); // Menampilkan MAC Address ESP8266

  Serial.print("RSSI (dBm) : "); // Menampilkan keterangan kekuatan sinyal
  Serial.println(WiFi.RSSI()); // Menampilkan nilai RSSI WiFi

  digitalWrite(ledPin, HIGH); // Menyalakan LED karena WiFi berhasil terhubung
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) { // Mengecek apakah koneksi WiFi masih terhubung

    Serial.println("Status: Terhubung"); // Menampilkan status bahwa WiFi masih terhubung

    digitalWrite(ledPin, HIGH); // Memastikan LED tetap menyala

  } else { // Jika koneksi WiFi terputus

    Serial.println("Status: Terputus"); // Menampilkan status bahwa WiFi terputus

    digitalWrite(ledPin, LOW); // Mematikan LED sebagai indikator koneksi terputus

    Serial.println("Mencoba menghubungkan ulang..."); // Menampilkan pesan proses reconnect

    WiFi.disconnect(); // Memutuskan koneksi WiFi yang bermasalah

    WiFi.begin(ssid, password); // Mencoba menghubungkan kembali ke WiFi

    int percobaan = 0; // Membuat variabel untuk menghitung jumlah percobaan

    while (WiFi.status() != WL_CONNECTED && percobaan < 20) { // Mencoba reconnect maksimal 20 kali
      delay(500); // Menunggu selama 500 milidetik
      Serial.print("."); // Menampilkan titik setiap kali mencoba
      percobaan++; // Menambah jumlah percobaan sebanyak satu
    }

    Serial.println(); // Membuat baris baru pada Serial Monitor

    if (WiFi.status() == WL_CONNECTED) { // Mengecek apakah reconnect berhasil

      Serial.println("WiFi berhasil terhubung kembali!"); // Menampilkan pesan reconnect berhasil

      Serial.print("IP Address : "); // Menampilkan keterangan IP Address
      Serial.println(WiFi.localIP()); // Menampilkan IP Address setelah reconnect

      digitalWrite(ledPin, HIGH); // Menyalakan LED karena koneksi berhasil kembali

    } else { // Jika reconnect belum berhasil

      Serial.println("Gagal menghubungkan kembali."); // Menampilkan pesan reconnect gagal

      digitalWrite(ledPin, LOW); // Memastikan LED tetap mati
    }
  }

  delay(5000); // Menunggu 5 detik sebelum mengecek status WiFi kembali
}
```
ketika ```WiFi.status()``` tidak lagi ```WL_CONNECTED```, ESP8266 akan menjalankan ```WiFi.begin(ssid, password)``` kembali dan mencoba terhubung maksimal 20 kali. Jika berhasil, LED menyala kembali.
<br>

# Percobaan 2B
Percobaan ini dilakukan dengan mengatur ESP8266 sebagai Access Point (AP) sehingga ESP8266 dapat membuat jaringan WiFi sendiri. Program menentukan SSID dan password, kemudian menampilkan SSID dan IP Address Access Point pada Serial Monitor. Selain itu, program menghitung dan menampilkan jumlah perangkat (client) yang terhubung ke Access Point setiap 5 detik.
## Library
```cpp
#include <ESP8266WiFi.h>
```
Library ini digunakan agar ESP8266 dapat menjalankan fungsi jaringan WiFi, seperti terhubung ke WiFi (Station) atau membuat jaringan WiFi sendiri (Access Point).
## Diagram Rangkaian
![alt text](https://github.com/razdient/H1H024053-RASTA_LISTIADI-PRAKTIKUM_INTERNET_OF_THINGS/blob/main/Pertemuan%202%20-%20Konfigurasi%20Jaringan/Dokumentasi/Screenshot%202026-09-16%20220219.png?raw=true)
## Dokumentasi
![alt tezt](https://github.com/razdient/H1H024053-RASTA_LISTIADI-PRAKTIKUM_INTERNET_OF_THINGS/blob/main/Pertemuan%202%20-%20Konfigurasi%20Jaringan/Dokumentasi/WhatsApp%20Image%202026-09-16%20at%2010.26.31%20(1).jpeg?raw=true)
## Penjelasan Kode
```cpp
#include <ESP8266WiFi.h>
```
Untuk memanggil library ESP8266WiFi agar ESP8266 dapat menggunakan fungsi-fungsi jaringan WiFi.
<br>

```cpp
const char* ap_ssid = "ESP32_Rasta";
const char* ap_password = "12345678";
```
Menentukan nama jaringan WiFi (SSID) dan password yang akan dibuat oleh ESP8266 sebagai Access Point.
<br>

```cpp
void setup() {
```
```Fungsi setup()``` dijalankan satu kali saat ESP8266 pertama kali dinyalakan atau di-reset.
<br>

```cpp
Serial.begin(115200);
```
Memulai komunikasi dengan Serial Monitor menggunakan baud rate 115200 untuk menampilkan informasi dari ESP8266.
<br>

```cpp
WiFi.mode(WIFI_AP);
```
Mengatur ESP8266 ke dalam mode Access Point (AP) sehingga ESP8266 dapat membuat dan menyediakan jaringan WiFi sendiri.
<br>

```cpp
WiFi.softAP(ap_ssid, ap_password);
```
Membuat jaringan WiFi menggunakan SSID dan password yang telah ditentukan. Perangkat seperti smartphone atau laptop dapat terhubung ke jaringan tersebut.
<br>

```cpp
IPAddress apIP = WiFi.softAPIP();
```
Mengambil IP Address Access Point yang dibuat oleh ESP8266 dan menyimpannya ke dalam variabel apIP.
<br>

```cpp
Serial.println("Access Point aktif!");
```
Menampilkan pesan pada Serial Monitor bahwa Access Point telah berhasil diaktifkan.
<br>

```cpp
Serial.print("SSID : ");
Serial.println(ap_ssid);
```
Menampilkan nama jaringan WiFi (SSID) yang dibuat oleh ESP8266 pada Serial Monitor.
<br>

```cpp
Serial.print("IP Address : ");
Serial.println(apIP);
```
Menampilkan IP Address Access Point pada Serial Monitor.
<br>

```cpp
void loop() {
```
Fungsi loop() dijalankan secara berulang-ulang selama ESP8266 menyala.
<br>

```cpp
int jumlahClient = WiFi.softAPgetStationNum();
```
Mengambil dan menyimpan jumlah perangkat (client) yang sedang terhubung ke Access Point ESP8266 ke dalam variabel jumlahClient.
<br>

```cpp
Serial.print("Jumlah perangkat terhubung: ");
Serial.println(jumlahClient);
```
Menampilkan jumlah perangkat yang sedang terhubung ke Access Point pada Serial Monitor.
<br>

```cpp
delay(5000);
```
Memberikan jeda selama 5 detik sebelum ESP8266 kembali mengecek jumlah perangkat yang terhubung.
## Modifikasi Mode AP + STA
```cpp
#include <WiFi.h> // Memanggil library WiFi untuk ESP32

// Konfigurasi WiFi rumah
const char* sta_ssid = "NAMA_WIFI_RUMAH"; // Nama WiFi rumah
const char* sta_password = "PASSWORD_WIFI"; // Password WiFi rumah

// Konfigurasi Access Point ESP32
const char* ap_ssid = "ESP32_Rasta"; // Nama Access Point yang dibuat ESP32
const char* ap_password = "12345678"; // Password Access Point

void setup() {
  Serial.begin(115200); // Memulai komunikasi dengan Serial Monitor

  WiFi.mode(WIFI_AP_STA); // Mengatur ESP32 agar bekerja sebagai AP dan STA secara bersamaan

  WiFi.begin(sta_ssid, sta_password); // Menghubungkan ESP32 ke WiFi rumah

  Serial.print("Menghubungkan ke WiFi rumah"); // Menampilkan proses koneksi WiFi rumah

  while (WiFi.status() != WL_CONNECTED) { // Mengecek apakah ESP32 sudah terhubung
    delay(500); // Menunggu selama 500 milidetik
    Serial.print("."); // Menampilkan titik selama proses koneksi
  }

  Serial.println(); // Membuat baris baru
  Serial.println("WiFi rumah berhasil terhubung!"); // Menampilkan pesan koneksi berhasil

  Serial.print("IP Station : "); // Menampilkan keterangan IP Station
  Serial.println(WiFi.localIP()); // Menampilkan IP yang diperoleh dari WiFi rumah

  WiFi.softAP(ap_ssid, ap_password); // Membuat Access Point ESP32

  Serial.println("Access Point aktif!"); // Menampilkan pesan AP aktif

  Serial.print("SSID AP : "); // Menampilkan keterangan SSID AP
  Serial.println(ap_ssid); // Menampilkan nama Access Point

  Serial.print("IP AP : "); // Menampilkan keterangan IP Access Point
  Serial.println(WiFi.softAPIP()); // Menampilkan IP Access Point ESP32
}

void loop() {
  int jumlahClient = WiFi.softAPgetStationNum(); // Menghitung jumlah perangkat yang terhubung ke AP

  Serial.print("Jumlah perangkat terhubung ke AP: "); // Menampilkan keterangan jumlah client
  Serial.println(jumlahClient); // Menampilkan jumlah perangkat yang terhubung

  if (WiFi.status() == WL_CONNECTED) { // Mengecek apakah ESP32 masih terhubung ke WiFi rumah
    Serial.println("Status WiFi rumah: Terhubung"); // Menampilkan status terhubung
  } else {
    Serial.println("Status WiFi rumah: Terputus"); // Menampilkan status terputus
  }

  delay(5000); // Mengecek kembali setiap 5 detik
}
```
ESP32 menggunakan ```WIFI_AP_STA```, sehingga STA digunakan untuk terhubung ke WiFi rumah, sedangkan AP digunakan untuk membuat WiFi ```ESP32_Rasta``` yang dapat diakses oleh smartphone atau laptop. Modul memang menjelaskan AP+STA sebagai gabungan kedua mode tersebut, yaitu ESP32 dapat terhubung ke WiFi sekaligus menyediakan Access Point.
