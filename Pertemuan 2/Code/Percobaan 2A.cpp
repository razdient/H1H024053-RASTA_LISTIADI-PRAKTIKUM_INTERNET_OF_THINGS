#include <ESP8266WiFi.h> // Library WiFi ESP8266
const char* ssid = "ESPRASTA"; // Nama WiFi
const char* password = "12345678"; // Password WiFi
// LED indikator status koneksi
const int ledPin = 2; // Pin LED
void setup() {
Serial.begin(115200); // Memulai komunikasi Serial Monitor
pinMode(ledPin, OUTPUT); // Mengatur pin LED sebagai output
digitalWrite(ledPin, LOW); // Mematikan LED
// Mengatur ESP8266 sebagai WiFi Station
WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password); // Menghubungkan ke WiFi
Serial.print("Menghubungkan ke WiFi");
// Menunggu sampai ESP8266 berhasil terhubung
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println();
Serial.println("WiFi berhasil terhubung!");
// Menampilkan informasi koneksi
Serial.print("IP Address : ");
Serial.println(WiFi.localIP()); // Menampilkan IP ESP8266
Serial.print("MAC Address : ");
Serial.println(WiFi.macAddress()); // Menampilkan MAC Address
Serial.print("RSSI (dBm) : ");
Serial.println(WiFi.RSSI()); // Menampilkan kekuatan sinyal
// LED menyala jika berhasil terhubung
digitalWrite(ledPin, HIGH);
}
void loop() {
// Mengecek status WiFi setiap 5 detik
if (WiFi.status() == WL_CONNECTED) {
Serial.println("Status: Terhubung");
digitalWrite(ledPin, HIGH); // LED menyala
} else {
Serial.println("Status: Terputus");
digitalWrite(ledPin, LOW); // LED mati
}
delay(5000); // Jeda 5 detik
}
