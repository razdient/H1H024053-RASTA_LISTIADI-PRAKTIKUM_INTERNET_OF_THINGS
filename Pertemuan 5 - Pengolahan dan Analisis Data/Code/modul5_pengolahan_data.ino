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
}
