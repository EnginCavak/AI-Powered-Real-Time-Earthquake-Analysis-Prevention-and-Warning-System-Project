import time
import psycopg2
import firebase_admin
from firebase_admin import credentials, db
from datetime import datetime

# Firebase Ayarları
cred = credentials.Certificate("iotproject-efb83-firebase-adminsdk-eijnr-366e2149dc.json")  # Firebase Service Account JSON dosyası
firebase_admin.initialize_app(cred, {
    "databaseURL": "https://iotproject-efb83-default-rtdb.firebaseio.com/"
})

# PostgreSQL Bağlantısı
try:
    conn = psycopg2.connect(
        host="localhost",
        port="5432",
        database="IOT_PROJECT",
        user="postgres",
        password="12345"  # Şifre güncellendi
    )
    conn.autocommit = True
    cursor = conn.cursor()
    print("PostgreSQL'e bağlanıldı.")
except Exception as e:
    print("PostgreSQL bağlantı hatası:", e)
    exit()

# Firebase'den gelen son veri
last_fetched_data = None

def fetch_and_update_data():
    global last_fetched_data
    try:
        # Firebase'den veriyi çek
        ref = db.reference("/IoTProject")
        current_data = ref.get()

        # Veriyi kontrol et
        if current_data == last_fetched_data:
            print("Veri değişmedi, veritabanına yazılmayacak.")
            return

        last_fetched_data = current_data  # Son veriyi güncelle

        # Veriyi PostgreSQL'e yaz
        sql = """
        INSERT INTO IOT_PROJECT (
            buzzer_kontrol, buzzer_kontrol2, gaz_durumu,
            led_kontrol, led_kontrol2, nem,
            sicaklik, titresim_d10, titresim_d9, last_updated
        ) VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s);
        """
        cursor.execute(sql, (
            current_data.get("BuzzerKontrol", "UNKNOWN"),
            current_data.get("BuzzerKontrol2", "UNKNOWN"),
            current_data.get("GazDurumu", 0),
            current_data.get("LedKontrol", "UNKNOWN"),
            current_data.get("LedKontrol2", "UNKNOWN"),
            current_data.get("Nem", 0),
            current_data.get("Sicaklik", 0),
            current_data.get("TitresimD10", 0),
            current_data.get("TitresimD9", 0),
            datetime.now()
        ))

        print("Veri PostgreSQL'e yazıldı:", current_data)

    except Exception as e:
        print("Hata oluştu:", e)

# Döngü
if __name__ == "__main__":
    while True:
        fetch_and_update_data()
        time.sleep(3)