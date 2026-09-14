#include <ESP8266WiFi.h> // Library WiFi untuk ESP8266
const char* ap_ssid = "ESP32_RASTA"; // Nama Access Point
const char* ap_password = "12345678"; // Password Access Point
void setup() {
Serial.begin(115200); // Memulai komunikasi dengan Serial Monitor
// Mengatur ESP8266 sebagai Access Point
WiFi.mode(WIFI_AP);
WiFi.softAP(ap_ssid, ap_password); // Membuat jaringan WiFi
IPAddress apIP = WiFi.softAPIP(); // Mengambil IP Address Access Point
Serial.println("Access Point aktif!");
Serial.print("SSID : ");
Serial.println(ap_ssid); // Menampilkan nama WiFi
Serial.print("IP Address : ");
Serial.println(apIP); // Menampilkan IP Address
}
void loop() {
// Menghitung jumlah perangkat yang terhubung ke Access Point
int jumlahClient = WiFi.softAPgetStationNum();
Serial.print("Jumlah perangkat terhubung: ");
Serial.println(jumlahClient); // Menampilkan jumlah client
delay(5000); // Mengecek kembali setiap 5 detik
}
