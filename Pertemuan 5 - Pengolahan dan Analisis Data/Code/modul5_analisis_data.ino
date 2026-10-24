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
