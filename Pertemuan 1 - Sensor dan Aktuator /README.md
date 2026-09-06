# Percobaan 1A
Percobaan ini menggunakan ESP32 dan sensor DHT11 untuk melakukan akuisisi data suhu dan kelembaban, kemudian menampilkannya pada Serial Monitor setiap 2 detik. Program juga menggunakan kondisi isnan() untuk mendeteksi kegagalan pembacaan sensor dan menampilkan peringatan apabila data yang diperoleh tidak valid.
## Library
```cpp
#include <DHT.h>
```
Library DHT.h digunakan untuk membaca data suhu dan kelembaban dari sensor DHT11.
## Diagram Rangkaian

## Dokumentasi
## Penjelasan Kode

```cpp
#include <DHT.h>
```
Untuk memanggil library DHT agar program dapat berkomunikasi dengan sensor DHT11    
<br>

```cpp
#define DHTPIN 4 // pin data DHT11 terhubung ke GPIO 4
#define DHTTYPE DHT11 // tipe sensor yang digunakan
DHT dht(DHTPIN, DHTTYPE);
```
Konfigurasi Pin dan Tipe Sensor  
<br>

```cpp
void setup() {
  Serial.begin(115200);
  dht.begin(); // inisialisasi sensor DHT22
  Serial.println("Memulai akuisisi data sensor DHT22...");
}
```
Menyalakan dan menyiapkan komunikasi Serial & sensor DHT11, lalu memberi pesan bahwa pembacaan akan dimulai  
<br>

```cpp
void loop() {
  // Membaca data kelembaban dan suhu
  float kelembaban = dht.readHumidity();
  float suhu = dht.readTemperature();
```
Deklarasi variabel dari hasil bacaan sensor  
<br>

```cpp
  // Periksa apakah pembacaan berhasil
  if (isnan(kelembaban) || isnan(suhu)) {
    Serial.println("Gagal membaca data dari sensor DHT11!");
  } else {
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print(" °C, Kelembaban: ");
    Serial.print(kelembaban);
    Serial.println(" %");
  }
```
Kondisional untuk mengecek apakah sensor DHT11 berhasil memberikan data yang valid atau tidak. Kalau valid data tampil, kalau tidak valid muncul pesan gagal  
<br>

```cpp
  delay(2000); // jeda pembacaan setiap 2 detik
}
```
Delay untuk pembacaan sensor

## Modifikasi Kode Rata-Rata Data Tiap 5 Kali Pembacaan
```cpp
#include <DHT.h>

#define DHTPIN 4 // pin data DHT11 terhubung ke GPIO 4
#define DHTTYPE DHT11 // tipe sensor yang digunakan

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin(); // inisialisasi sensor DHT11
  Serial.println("Memulai akuisisi data sensor DHT11...");
}

void loop() { //Yanf dimodif
  float totalKelembaban = 0;
  float totalSuhu = 0;
  int jumlahPembacaan = 5;

  for (int i = 0; i < jumlahPembacaan; i++) {
    float kelembaban = dht.readHumidity();
    float suhu = dht.readTemperature();

    if (isnan(kelembaban) || isnan(suhu)) {
      Serial.println("Gagal membaca data dari sensor DHT11!");
      return;
    }

    totalKelembaban += kelembaban;
    totalSuhu += suhu;

    delay(2000);
  }

  float rataKelembaban = totalKelembaban / jumlahPembacaan;
  float rataSuhu = totalSuhu / jumlahPembacaan;

  Serial.print("Rata-rata Suhu: ");
  Serial.print(rataSuhu);
  Serial.print(" °C, Rata-rata Kelembaban: ");
  Serial.print(rataKelembaban);
  Serial.println(" %");

  delay(2000);
}
```
### Penjelasan Modifikasi
```cpp
float totalKelembaban = 0;
float totalSuhu = 0;
int jumlahPembacaan = 5;
```
Membuat variabel untuk menyimpan total suhu, total kelembaban, dan menentukan jumlah pembacaan sebanyak 5 kali  
<br>

```cpp
for (int i = 0; i < jumlahPembacaan; i++) {
```
Melakukan perulangan pembacaan sensor sebanyak 5 kali  
<br>

```cpp
totalKelembaban += kelembaban;
totalSuhu += suhu;
```
Menambahkan setiap hasil pembacaan ke total kelembaban dan total suhu  
<br>

```cpp
float rataKelembaban = totalKelembaban / jumlahPembacaan;
float rataSuhu = totalSuhu / jumlahPembacaan;
```
Menghitung rata-rata suhu dan kelembaban dari 5 kali pembacaan  
<br>

```cpp
Serial.print("Rata-rata Suhu: ");
Serial.print(rataSuhu);
Serial.print(" °C, Rata-rata Kelembaban: ");
Serial.print(rataKelembaban);
Serial.println(" %");
```
Menampilkan hasil rata-rata suhu dan kelembaban setelah 5 kali pembacaan selesai  
<br>

# Percobaan 2A
Percobaan ini menggunakan sensor DHT11 untuk membaca suhu dan mengendalikan relay yang disimulasikan dengan LED berdasarkan nilai threshold sebesar 30°C. Jika suhu melebihi 30°C aktuator menyala, sedangkan jika suhu berada di bawah atau sama dengan 30°C aktuator mati, dan statusnya ditampilkan pada Serial Monitor.
## Library
```cpp
#include <DHT.h>
```
Library DHT.h digunakan untuk membaca data suhu dan kelembaban dari sensor DHT11.
## Diagram Rangkaian
## Dokumentasi
## Penjelasan Kode
```cpp
#include <DHT.h>
```
Untuk memanggil library DHT agar program dapat berkomunikasi dengan sensor DHT11  
<br>

```cpp
#define DHTPIN 4 // pin data DHT11 terhubung ke GPIO 4
#define DHTTYPE DHT11
#define RELAYPIN 5 // pin kendali relay/LED indikator
DHT dht(DHTPIN, DHTTYPE);
```
Konfigurasi pin sensor DHT11 dan pin relay  
<br>

```cpp
const float suhuThreshold = 30.0; // ambang batas suhu (°C)
```
Menentukan batas suhu sebesar 30°C yang digunakan sebagai acuan untuk mengaktifkan atau mematikan aktuator  
<br>

```cpp
void setup() {
Serial.begin(115200);
dht.begin();
pinMode(RELAYPIN, OUTPUT);
digitalWrite(RELAYPIN, LOW); // pastikan aktuator mati di awal
}
```
Menyalakan dan menyiapkan komunikasi Serial, sensor DHT11, serta pin relay sebagai output. Aktuator dibuat dalam kondisi mati saat program pertama dijalankan  
<br>

```cpp
void loop() {
float suhu = dht.readTemperature();
```
Membaca data suhu dari sensor DHT11 dan menyimpannya ke variabel suhu  
<br>

```cpp
if (isnan(suhu)) {
Serial.println("Gagal membaca data sensor!");
} else {
Serial.print("Suhu: ");
Serial.print(suhu);
Serial.print(" °C -> ");
```
Kondisional untuk mengecek apakah sensor berhasil memberikan data suhu yang valid. Kalau tidak valid muncul pesan gagal, kalau valid nilai suhu ditampilkan  
<br>

```cpp
if (suhu > suhuThreshold) {
digitalWrite(RELAYPIN, HIGH); // aktifkan relay/LED
Serial.println("Aktuator: ON");
} else {
digitalWrite(RELAYPIN, LOW); // matikan relay/LED
Serial.println("Aktuator: OFF");
}
```
Kondisional untuk menentukan kondisi aktuator berdasarkan suhu. Kalau suhu lebih dari 30°C, relay/LED dinyalakan. Kalau suhu 30°C atau kurang, relay/LED dimatikan  

## Modifikasi Kode 2 Threshold
```cpp
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11
#define RELAYPIN 5

DHT dht(DHTPIN, DHTTYPE);

const float suhuOn = 30.0; //Yang dimodif
const float suhuOff = 28.0; //Yang dimodif

bool aktuatorOn = false; //Yang dimodif

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, LOW);
}

void loop() {
  float suhu = dht.readTemperature();

  if (isnan(suhu)) {
    Serial.println("Gagal membaca data sensor!");
  } else {
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print(" °C -> ");

    if (!aktuatorOn && suhu > suhuOn) { //Yang dimodif
      aktuatorOn = true;
      digitalWrite(RELAYPIN, HIGH);
      Serial.println("Aktuator: ON");
    } else if (aktuatorOn && suhu < suhuOff) {
      aktuatorOn = false;//Yang dimodif
      digitalWrite(RELAYPIN, LOW);
      Serial.println("Aktuator: OFF");
    } else {
      Serial.print("Aktuator: ");
      Serial.println(aktuatorOn ? "ON" : "OFF");
    }
  }

  delay(2000);
}
```
### Penjelasan Modifikasi
```cpp
const float suhuOn = 30.0;
const float suhuOff = 28.0;
```
Menambahkan dua ambang batas suhu untuk mengatur kapan aktuator menyala dan mati. Aktuator menyala di atas 30°C dan mati di bawah 28°C  
<br>

```cpp
bool aktuatorOn = false;
```
Menambahkan variabel tipe data boolean untuk menyimpan kondisi aktuator, apakah sedang menyala atau mati  
<br>

```cpp
if (!aktuatorOn && suhu > suhuOn) {
    aktuatorOn = true;
    digitalWrite(RELAYPIN, HIGH);
    Serial.println("Aktuator: ON");
}
```
Kondisional untuk menyalakan aktuator. Jika aktuator masih mati dan suhu lebih dari 30°C, aktuator akan dinyalakan  
<br>

```cpp
else if (aktuatorOn && suhu < suhuOff) {
    aktuatorOn = false;
    digitalWrite(RELAYPIN, LOW);
    Serial.println("Aktuator: OFF");
}
```
Kondisional untuk mematikan aktuator. Jika aktuator sedang menyala dan suhu turun di bawah 28°C, aktuator akan dimatikan  
<br>

```cpp
else {
    Serial.print("Aktuator: ");
    Serial.println(aktuatorOn ? "ON" : "OFF");
}
```
Jika suhu berada di antara 28°C sampai 30°C, kondisi aktuator tetap seperti sebelumnya sehingga tidak terjadi ON/OFF berulang
