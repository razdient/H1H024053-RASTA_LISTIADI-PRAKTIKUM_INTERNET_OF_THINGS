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
