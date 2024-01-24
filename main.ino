// Максимальное количество посетителей, разрешенных до срабатывания светодиода
int maxPeople = 3;

#include <LiquidCrystal_I2C.h>

int currentPeople = 0;

// Инициализация LCD дисплея
LiquidCrystal_I2C LCD(0x3F, 16, 2);

// Пины для ультразвуковых датчиков
int sensor1[] = {9, 10};
int sensor2[] = {11, 12};
int sensor1Initial;
int sensor2Initial;

// Строка для хранения последовательности измерений
String sequence = "";

// Счетчик таймаута
int timeoutCounter = 0;

void setup() {
  Serial.begin(9600);
  
  // Инициализация LCD дисплея и подсветки
  LCD.init();
  LCD.backlight();
  
  // Настройка порта для светодиода
  pinMode(7, OUTPUT);

  // Задержка для стабилизации
  delay(500);

  // Измерение начальных значений ультразвуковых датчиков
  sensor1Initial = measureDistance(sensor1);
  sensor2Initial = measureDistance(sensor2);
}

void loop() {
  // Считывание данных с ультразвуковых датчиков
  int sensor1Val = measureDistance(sensor1);
  int sensor2Val = measureDistance(sensor2);
  
  // Обработка данных
  if (sensor1Val < sensor1Initial - 30 && sequence.charAt(0) != '1') {
    sequence += "1";
  } else if (sensor2Val < sensor2Initial - 30 && sequence.charAt(0) != '2') {
    sequence += "2";
  }
  
  // Увеличение счетчика посетителей при обнаружении последовательности "12"
  if (sequence.equals("12")) {
    currentPeople++;  
    sequence = "";
    delay(550);
  } else if (sequence.equals("21") && currentPeople > 0) {
    // Уменьшение счетчика посетителей при обнаружении последовательности "21"
    currentPeople--;  
    sequence = "";
    delay(550);
  }

  // Сброс последовательности, если она недействительна или таймаут
  if (sequence.length() > 2 || sequence.equals("11") || sequence.equals("22") || timeoutCounter > 50) {
    sequence = "";  
  }

  // Увеличение таймаута при длине последовательности равной 1, иначе сброс таймаута
  if (sequence.length() == 1) {
    timeoutCounter++;
  } else {
    timeoutCounter = 0;
  }

  // Вывод значений на последовательный монитор
  Serial.print("Seq: ");
  Serial.print(sequence);
  Serial.print(" S1: ");
  Serial.print(sensor1Val);
  Serial.print(" S2: ");
  Serial.println(sensor2Val);

  // Вывод информации на LCD дисплей
  LCD.setCursor(0, 1);
  if (maxPeople - currentPeople > 0) {
    digitalWrite(7, LOW);
    LCD.print(maxPeople - currentPeople);
    delay(100);
    LCD.clear(); 
  } else {
    digitalWrite(7, HIGH);
    LCD.print("crowded");
    delay(100);
    LCD.clear();
  }
}

// Функция для измерения расстояния ультразвукового датчика
// a[0] = echo, a[1] = trig
int measureDistance(int a[]) {
  pinMode(a[1], OUTPUT);
  digitalWrite(a[1], LOW);
  delayMicroseconds(2);
  digitalWrite(a[1], HIGH);
  delayMicroseconds(10);
  digitalWrite(a[1], LOW);
  pinMode(a[0], INPUT);
  long duration = pulseIn(a[0], HIGH, 100000);
  return duration / 29 / 2;
}
