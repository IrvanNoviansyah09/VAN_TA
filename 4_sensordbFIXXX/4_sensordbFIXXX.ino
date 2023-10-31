#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 18          // Pin DHT11 terhubung ke pin GPIO 18
#define DHTTYPE DHT11     // Jenis sensor DHT

#define ONE_WIRE_BUS 4    // Pin DS18B20 terhubung ke pin GPIO 4

#define SOIL_PIN 34       // Pin soil moisture terhubung ke pin GPIO 34

#define PH_OFFSET -1.00   // Jika ada offset pada pH meter
#define SensorPin 35    // Pin output Analog pH meter terhubung ke pin A0

DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Set the LCD address and dimensions
const int LCD_ADDRESS = 0x27;
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

// Create an instance of the LiquidCrystal_I2C class
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

String URL = "http://192.168.43.216/dht11/datasuhus.php";  // Sesuaikan dengan URL script PHP Anda

const char* ssid = "virus";
const char* password = "1sampai09";

float temperature = 0;
float tempC = 0;
float soilMoisture = 0;
float phValue = 0;

unsigned long previousMillis = 0;
const long interval = 10000; // Interval waktu antara pengiriman data ke database (dalam milidetik)
const int lcdUpdateInterval = 5000; // Interval waktu antara pembaruan tampilan di LCD (dalam milidetik)
unsigned long lcdPreviousMillis = 0;

enum DisplayState {
  TEMPERATURE,
  TEMC,
  SOIL_MOISTURE,
  PH_VALUE
};
DisplayState displayState = TEMPERATURE;

void setup() {
  Serial.begin(9600);

  dht.begin();
  sensors.begin();

  lcd.init();
  lcd.backlight();

  connectWiFi();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    Load_Sensor_Data();
    if (temperature != 0 && tempC != -127.00 && soilMoisture != 0 && phValue != 0.0) {
      String postData = "temperature=" + String(temperature) + "&tempC=" + String(tempC) + "&soilMoisture=" + String(soilMoisture) + "&phValue=" + String(phValue);

      HTTPClient http;
      http.begin(URL);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      int httpCode = http.POST(postData);
      String payload = "";

      if (httpCode == 0) {
        Serial.println("Connection error or server timeout!");
      } else if (httpCode == HTTP_CODE_OK) {
        payload = http.getString();
        Serial.println(payload);
      } else {
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      }

      http.end();  // Close connection

      Serial.print("URL : "); Serial.println(URL);
      Serial.print("Data: "); Serial.println(postData);
      Serial.print("httpCode: "); Serial.println(httpCode);
      Serial.print("payload : "); Serial.println(payload);
      Serial.println("--------------------------------------------------");
    }
  }

  unsigned long lcdCurrentMillis = millis();
  if (lcdCurrentMillis - lcdPreviousMillis >= lcdUpdateInterval) {
    lcdPreviousMillis = lcdCurrentMillis;
    updateLCD();
  }
}

void updateLCD() {
  switch (displayState) {
    case TEMPERATURE:
      lcd.setCursor(0, 0);
      lcd.print("Suhu udara:    ");
      lcd.setCursor(0, 1);
      lcd.print(temperature);
      lcd.print(" C");
      break;

    case TEMC:
      lcd.setCursor(0, 0);
      lcd.print("Suhu tanah:    ");
      lcd.setCursor(0, 1);
      lcd.print(tempC);
      lcd.print(" C");
      break;

    case SOIL_MOISTURE:
      lcd.setCursor(0, 0);
      lcd.print("Lembab Tanah:    ");
      lcd.setCursor(0, 1);
      lcd.print(soilMoisture);
      lcd.print(" %");
      break;

    case PH_VALUE:
      lcd.setCursor(0, 0);
      lcd.print("Nilai pH:      ");
      lcd.setCursor(0, 2);
      lcd.print(phValue);
      lcd.print("");
      break;
  }

  displayState = static_cast<DisplayState>((displayState + 1) % 4);
}

void Load_Sensor_Data() {
  float Temperature = dht.readTemperature();
  if (!isnan(Temperature)) {
    temperature = Temperature;
    Serial.print("Suhu udara: ");
    Serial.print(temperature);
    Serial.println(" °C");
  }

  sensors.requestTemperatures();
  tempC = sensors.getTempCByIndex(0);
  if (tempC != -127.00) {
    Serial.print("Suhu tanah: ");
    Serial.print(tempC);
    Serial.println(" °C");
  }

  int soilMoistureValue = analogRead(SOIL_PIN);
  soilMoisture = map(soilMoistureValue, 4095, 0, 0, 100); 
  Serial.print("Kelembaban: ");
  Serial.print(soilMoisture);
  Serial.println(" %");

  unsigned long int avgValue = 0;
  int buf[10], temp;
  for (int i = 0; i < 10; i++) {
    buf[i] = analogRead(SensorPin);
    delay(10);
  }
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  for (int i = 2; i < 8; i++) {
    avgValue += buf[i];
  }
  float analogVoltage = (float)avgValue * 5.0 / 10244 / 6; // Konversi analog menjadi miliVolt
  phValue = 3.5 * analogVoltage; // Konversi miliVolt menjadi nilai pH
  phValue += PH_OFFSET; // Penyesuaian offset

  Serial.print("Nilai pH: ");
  Serial.println(phValue, 2);
}

void connectWiFi() {
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("Connected to: "); Serial.println(ssid);
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
}
