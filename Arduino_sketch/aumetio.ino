#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include <GP2Y1010AU0F.h>

#define DHTPIN 4 // Инициализация пина к которому подключаем датчик влажности и температуры
#define DHTTYPE DHT11   // Указываем тип датчика

int measurePin = 35;   // Подключаю дачтик загрязненности воздуха к пину 35
int ledPin     = 27;    // Подключаем светодиод к 27


float dustDensity = 0; // Создаем переменную где будем хранить информацию
GP2Y1010AU0F dustSensor(ledPin, measurePin); // Создаем обьект датчика пыли
DHT dht(DHTPIN, DHTTYPE); // Создаем обьект датчика влажности и температуры

const char* ssid = "DIR-300A-bf05"; // Переменная с названием Wi-Fi сети
const char* password = "11411253"; // Переменная с паролем от вай фай сети
const char* serverName = "https://aumetio-default-rtdb.firebaseio.com/data/.json"; //URL адрес базы данных


unsigned long lastTime = 0; // Переменная для хранения последнего времени с результата функции millis()
unsigned long timerDelay = 5000; // Переменная с задержкой времени

void setup() {
  Serial.begin(9600); // Начинаем работу с серийным портом
  
  WiFi.begin(ssid, password); // Вызываем метод у обьекта WiFI для инициализации подключения к вай файнике
  Serial.println("Connecting"); // Пытаемся подключиться
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); // В этом цикле мы пытаемся подключиться к вай файнике
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");// После успешного подключения
  Serial.println(WiFi.localIP()); // Выводим информацию про АЙПИ адрес нашей вай фай сети
   
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");// Таймер делей отвечает 
  // временную задержку между запросами к базе данных
  dht.begin(); // запускаем датчик температуры и влажности
  dustSensor.begin(); // запускаем датчик пыли
}

void loop() {
   StaticJsonDocument<100> jsonDocument; // Формируем JSON документ
   
  if ((millis() - lastTime) > timerDelay) { // Условие для проверки того что время пришло  и надо отправлять запрос
    dustDensity = dustSensor.read(); // Считываю данные с датчика пыли и сохраняю в dustDensity
    float h = dht.readHumidity(); // Считываю влажность и тоже сохраняю в переменной
    float t = dht.readTemperature();//Считываю температуру и тоже сохраняю в переменной
    jsonDocument["Temperature"] = t; // в JSON документ сохраняю информацию из переменной t где я сохранил температуру
    jsonDocument["Humidity"] = h; // то же самое с влажностью
    jsonDocument["AirQuality"]=dustDensity; // то же самое с загрязнением воздуха
    char buffer[100]; // Создаю буффер где храниться данный jsonDocument
    serializeJsonPretty(jsonDocument, buffer); // Делаю сериализацию данных и формирую полноценный JSON формат
    if(WiFi.status()== WL_CONNECTED){ //Проверяю как с подключением к интернету
      HTTPClient http; // Создаю обьект http 
      
 
      http.begin(serverName); // Запускаю http агента

      // Specify content-type header
      http.addHeader("Content-Type", "application/json"); //Говорю агенту что буду отправлять документ типа JSON
      int httpResponseCode = http.POST(buffer);// Отправляю запрос и получаю ответ от сервера в переменной httpResponseCode
     
      Serial.print("HTTP Response code: "); // То что ниже не так важно....
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end(); // Закрываю сессию http агента
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
