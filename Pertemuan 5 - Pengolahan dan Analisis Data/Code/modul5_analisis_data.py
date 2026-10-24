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
