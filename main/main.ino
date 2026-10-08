#include <LiquidCrystal_I2C.h> 

// Inisialisasi LCD pada alamat I2C 0x27, dengan ukuran 16 kolom dan 2 baris
LiquidCrystal_I2C lcd(0x27, 16, 2); //

// Definisi untuk mist maker
const int RELAY_PIN = A3; // Pin Digital ke IN relay
const unsigned long DURASI_MIST_TEST = 3000; // semprot 3 detik
const unsigned long DURASI_MIST_ALARM = 7000; // semprot 7 detik
void semprotMistMaker(unsigned long durasi);

// Definisi pin keypad pada Arduino Nano
const int pinTombol1 = 7; 
const int pinTombol2 = 6; 
const int pinTombol3 = 5; 
const int pinTombol4 = 4; 

// Status Mode Edit
bool isEditing = false;

// 8 Blok Karakter Kustom (Presisi 7-Segmen)
byte customChar[8][8] = {
  {31, 31, 28, 28, 28, 28, 28, 28}, // 0: TL (Sudut Kiri Atas ┌)
  {31, 31,  7,  7,  7,  7,  7,  7}, // 1: TR (Sudut Kanan Atas ┐)
  {28, 28, 28, 28, 28, 28, 31, 31}, // 2: BL (Sudut Kiri Bawah └)
  { 7,  7,  7,  7,  7,  7, 31, 31}, // 3: BR (Sudut Kanan Bawah ┘)
  {31, 31, 28, 28, 28, 28, 31, 31}, // 4: MID_L (Kiri dengan Garis Tengah ├)
  {31, 31,  7,  7,  7,  7, 31, 31}, // 5: MID_R (Kanan dengan Garis Tengah ┤)
  {31, 31,  0,  0,  0,  0, 31, 31}, // 6: HORIZ (Hanya Garis Mendatar Atas & Bawah =)
  { 0,  0,  0,  0,  0,  0, 31, 31}  // 7: BOT_ONLY (Hanya Garis Mendatar Bawah _)
};

// PETA ANGKA 0-9 (Kombinasi blok di atas)
// 32 = Spasi Kosong, 255 = Balok Penuh (█)
const byte font2x2[10][4] = {
  {0, 1, 2, 3},        // 0: Kotak penuh
  {32, 255, 32, 255},  // 1: Garis lurus di kanan
  {6, 5, 4, 7},        // 2: Membentuk kurva S
  {6, 5, 6, 5},        // 3: Membentuk huruf E terbalik
  {2, 3, 32, 255},     // 4: Bentuk kursi / U
  {4, 6, 6, 5},        // 5: Membentuk kurva S terbalik
  {4, 6, 4, 5},        // 6: Kotak bawah dengan kail di atas
  {0, 1, 32, 255},     // 7: Garis atas dan turun ke kanan
  {4, 5, 4, 5},        // 8: Dua kotak bertumpuk
  {4, 5, 7, 3}         // 9: Kotak atas dengan kail di bawah
};

// Definisi untuk waktu
// Mulai dari 12:00:00
int jam = 12;
int menit = 0;
int detik = 0;
unsigned long waktuSebelumnya = 0;

void setup() {
  // MODUL KEYPAD
  Serial.begin(9600);
  
  // Mengaktifkan resistor PULLUP internal Arduino
  pinMode(pinTombol1, INPUT_PULLUP);
  pinMode(pinTombol2, INPUT_PULLUP);
  pinMode(pinTombol3, INPUT_PULLUP);
  pinMode(pinTombol4, INPUT_PULLUP);

  Serial.println("=== Tes Keypad 1x4 Siap ===");

  // MODUL LCD
  lcd.init();           // Inisialisasi modul LCD
  lcd.backlight();      // Menyalakan lampu latar (backlight) LCD
  // Daftarkan karakter kustom ke memori LCD
  for (int i = 0; i < 8; i++) {
    lcd.createChar(i, customChar[i]);
  }
  lcd.clear();

  // Setup untuk input relay (pin IN)
  pinMode(RELAY_PIN, OUTPUT); 

  // Memastikan relay posisi mati saat awal nyala (HIGH = OFF)
  digitalWrite(RELAY_PIN, HIGH); 

  delay(1000); // Jeda 1 detik persiapan

  // Nyalakan Mist Maker sebentar untuk pengujian awal
  semprotMistMaker(DURASI_MIST_TEST);
}

void loop() {
  unsigned long waktuSekarang = millis();

  // Deteksi input keypad
  modeEditJam();

  // Jika tidak sedang diedit, jalankan waktu otomatis
  if (!isEditing) {
    // Update waktu tiap 1 detik
    if (waktuSekarang - waktuSebelumnya >= 1000) {
      waktuSebelumnya = waktuSekarang;

      detik++;
      if (detik >= 60) { detik = 0; menit++; }
      if (menit >= 60) { menit = 0; jam++; }
      if (jam >= 24)   { jam = 0; }

      // Pengecekan Jadwal Semprot Otomatis (05:30:00 & 15:00:00)
      if (detik == 0) {
        // Semprot pada jam 5:30 dan 15:00
        if (jam == 5 && menit == 30) {
          semprotMistMaker(DURASI_MIST_ALARM);
        } else if(jam == 15 && menit == 0) {
          semprotMistMaker(DURASI_MIST_TEST);
        }
      }
    }
  }

  tampilkanJamBesar();
}

void modeEditJam() {
  if (digitalRead(pinTombol1) == LOW) {
    // Trigger mode edit
    if (digitalRead(pinTombol4) == LOW) {
      Serial.println("Tombol 4 Ditekan!");
      isEditing = !isEditing;
      delay(400);
    }

    // Edit jam dan menit
    if(isEditing) {
      if (digitalRead(pinTombol3) == LOW) {
        Serial.println("Tombol 3 Ditekan!");
        jam++;
        if (jam >= 24) jam = 0;
        delay(250);
      }
      
      if (digitalRead(pinTombol2) == LOW) {
        Serial.println("Tombol 2 Ditekan!");
        menit += 5;
        if (menit >= 60) menit = 0;
        delay(250);
      }
    }
  }
}

// Fungsi mencetak 1 angka besar (2 kolom x 2 baris)
void cetakAngkaBesar(int angka, int col) {
  for (int row = 0; row < 2; row++) {
    lcd.setCursor(col, row);
    for (int c = 0; c < 2; c++) {
      byte code = font2x2[angka][row * 2 + c];
      
      if (code < 8) {
        // Tambahkan + 8 untuk menghindari blokir Control Character I2C
        lcd.write((byte)(code + 8)); 
      } else if (code == 255) {
        lcd.write((byte)255); // Cetak Balok Solid █
      } else {
        lcd.print(" ");       // Cetak Spasi Kosong
      }
    }
  }
}

// Fungsi menyusun seluruh tampilan jam di layar LCD
void tampilkanJamBesar() {
  // Jam
  cetakAngkaBesar(jam / 10, 1);
  cetakAngkaBesar(jam % 10, 3);

  // Titik Dua Berkedip saat Mode Edit
  if (isEditing && (millis() / 300) % 2 == 0) {
    lcd.setCursor(5, 0); lcd.print(" ");
    lcd.setCursor(5, 1); lcd.print(" ");
    lcd.setCursor(10, 0); lcd.print(" ");
    lcd.setCursor(10, 1); lcd.print(" ");
  } else {
    lcd.setCursor(5, 0); lcd.print(":");
    lcd.setCursor(5, 1); lcd.print(":");
    lcd.setCursor(10, 0); lcd.print(":");
    lcd.setCursor(10, 1); lcd.print(":");
  }

  // Menit
  cetakAngkaBesar(menit / 10, 6);
  cetakAngkaBesar(menit % 10, 8);

  // Detik
  cetakAngkaBesar(detik / 10, 11);
  cetakAngkaBesar(detik % 10, 13);

  // Indikator teks "ON" di Kolom Paling Kanan (Kolom 15) saat Mode Edit Aktif
  if (isEditing) {
    lcd.setCursor(15, 0); lcd.print("O");
    lcd.setCursor(15, 1); lcd.print("N");
  } else {
    lcd.setCursor(15, 0); lcd.print(" ");
    lcd.setCursor(15, 1); lcd.print(" ");
  }
}

void semprotMistMaker(unsigned long durasi) {
  digitalWrite(RELAY_PIN, LOW); 
  delay(durasi); 
  digitalWrite(RELAY_PIN, HIGH); // Matikan kembali

  // Kompensasi penambahan detik agar jam tidak tertinggal akibat delay()
  detik += (durasi / 1000);
  if (detik >= 60) {
    detik %= 60;
    menit++;
    if (menit >= 60) {
      menit = 0;
      jam = (jam + 1) % 24;
    }
  }
  waktuSebelumnya = millis(); // Synchronize ulang waktu millis
}
