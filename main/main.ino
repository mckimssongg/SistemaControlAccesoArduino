#include <Keypad.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Configuración del Keypad 4x4
const byte FILAS = 4;
const byte COLUMNAS = 4;
char keys[FILAS][COLUMNAS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte pinesFilas[FILAS] = {9, 8, 7, 6};
byte pinesColumnas[COLUMNAS] = {5, 4, 3, 2};
Keypad keypad = Keypad(makeKeymap(keys), pinesFilas, pinesColumnas, FILAS, COLUMNAS);

// LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Pines
const int BUZZER_PIN = 10;
const int LED_VERDE = A0;
const int LED_ROJO = A1;  
const int PIR_PIN = 7;    // Sensor PIR en pin digital 7
// Pines HC-SR05
const int TRIG_PIN = 11;
const int ECHO_PIN = 12;

// Clave y control de intentos
const char CLAVE_CORRECTA[] = "1234";
const int MAX_CLAVE = 4;
char clave[MAX_CLAVE + 1];
int posicion = 0;
int intentos = 0;
bool accesoPermitido = false;
bool alarmaActiva = false;

// Tiempos
unsigned long tiempoAcceso = 0;
const unsigned long TIEMPO_ACCESO = 5000; // 5 segundos

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(9600);

  lcd.setCursor(0,0);
  lcd.print("Control de Acceso");
  lcd.setCursor(0,1);
  lcd.print("Ingrese Clave:");
  Serial.println("Sistema iniciado.");
}

// Función para medir distancia con HC-SR05
long medirDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duracion = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30ms
  if (duracion == 0) return 9999; // Sin eco, fuera de rango
  long distancia = duracion * 0.034 / 2;
  return distancia;
}

void loop() {
  // Detección de movimiento solo por HC-SR05
  long distancia = medirDistancia();

  // Mostrar distancia para depuración
  // Serial.print("Distancia ultrasónica: ");
  // Serial.println(distancia);

  // Solo activar si la distancia es válida y menor o igual a 3cm
  bool movimientoUltrasonico = (distancia > 0 && distancia <= 3 && distancia < 400);

  if (!accesoPermitido && !alarmaActiva && movimientoUltrasonico) {
    Serial.print("Movimiento ultrasónico detectado a ");
    Serial.print(distancia);
    Serial.println(" cm.");
    activarAlarma("Movimiento ultrasónico <= 3cm");
    // Esperar a que el objeto se aleje antes de permitir otra alarma
    while (medirDistancia() <= 3) {
      delay(100);
    }
  }

  // Ingreso de clave
  if (!accesoPermitido && !alarmaActiva) {
    char key = keypad.getKey();
    if (key) {
      if (posicion < MAX_CLAVE && key >= '0' && key <= '9') {
        clave[posicion] = key;
        posicion++;
        clave[posicion] = '\0';
        lcd.setCursor(0,2);
        lcd.print("Clave: ");
        lcd.print(clave);
      }
      if (posicion == MAX_CLAVE) {
        if (strcmp(clave, CLAVE_CORRECTA) == 0) {
          accesoPermitido = true;
          intentos = 0;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Acceso concedido!");
          Serial.println("Acceso autorizado.");
          digitalWrite(LED_VERDE, HIGH);
          beepCorrecto();
          for (int i = 5; i >= 1; i--) {
            lcd.setCursor(0,1);
            lcd.print("Se cerrara en: ");
            lcd.setCursor(0,2);
            lcd.print("        ");
            lcd.setCursor(0,2);
            lcd.print("  " + String(i) + " seg");
            delay(1000);
          }
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Puerta cerrada");
          Serial.println("Puerta cerrada");
          tiempoAcceso = millis();
        } else {
          intentos++;
          lcd.setCursor(0,3);
          lcd.print("Acceso Denegado!    ");
          Serial.print("Intento fallido: ");
          Serial.println(intentos);
          beepIncorrecto();
          if (intentos >= 3) {
            activarAlarma("3 intentos fallidos");
          }
        }
        delay(2000);
        resetearClave();
      }
    }
  }

  // Acceso autorizado: LED verde encendido 5 segundos
  if (accesoPermitido && (millis() - tiempoAcceso >= TIEMPO_ACCESO)) {
    accesoPermitido = false;
    digitalWrite(LED_VERDE, LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Control de Acceso");
    lcd.setCursor(0,1);
    lcd.print("Ingrese Clave:");
    Serial.println("Puerta cerrada.");
  }
}

// Función para el beep de acceso correcto
void beepCorrecto() {
  for(int i = 0; i < 2; i++) {
    tone(BUZZER_PIN, 1000);
    delay(200);
    noTone(BUZZER_PIN);
    delay(100);
  }
}

// Función para el beep de acceso incorrecto
void beepIncorrecto() {
  for(int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 200);
    delay(200);
    noTone(BUZZER_PIN);
    delay(100);
  }
}

// Función para activar la alarma
void activarAlarma(const char* motivo) {
  alarmaActiva = true;
  digitalWrite(LED_ROJO, HIGH);
  lcd.setCursor(0,3);
  lcd.print("ALARMA ACTIVADA!    ");
  Serial.print("ALARMA: ");
  Serial.println(motivo);
  for(int i = 0; i < 10; i++) {
    tone(BUZZER_PIN, 200);
    delay(100);
    noTone(BUZZER_PIN);
    delay(100);
  }
  delay(3000);
  digitalWrite(LED_ROJO, LOW);
  alarmaActiva = false;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Control de Acceso");
  lcd.setCursor(0,1);
  lcd.print("Ingrese Clave:");
  intentos = 0;
}

// Función para reiniciar la clave ingresada
void resetearClave() {
  posicion = 0;
  memset(clave, 0, sizeof(clave));
  lcd.setCursor(0,2);
  lcd.print("                "); // Borra la línea de clave
  lcd.setCursor(0,3);
  lcd.print("                "); // Borra la línea de mensajes
}