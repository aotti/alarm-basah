#include <LiquidCrystal_I2C.h>

// Inisialisasi LCD pada alamat I2C 0x27, dengan ukuran 16 kolom dan 2 baris
// (Jika nanti layar tidak muncul apa-apa, coba ganti 0x27 menjadi 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();           // Inisialisasi modul LCD
  lcd.backlight();      // Menyalakan lampu latar (backlight) LCD

  // Tampilkan teks di Baris Pertama (Kolom 0, Baris 0)
  lcd.setCursor(0, 0);
  lcd.print("Hello, World!");

  // Tampilkan teks di Baris Kedua (Kolom 0, Baris 1)
  lcd.setCursor(0, 1);
  lcd.print("Tes LCD I2C OK");
}

void loop() {
  // Tidak ada proses di loop untuk tes ini
}