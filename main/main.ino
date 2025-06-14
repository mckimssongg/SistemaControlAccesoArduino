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

// Pines de conexión del keypad (ajusta según tu conexión)
byte pinesFilas[FILAS] = {9, 8, 7, 6};    // Conectar a los pines de las filas
byte pinesColumnas[COLUMNAS] = {5, 4, 3, 2}; // Conectar a los pines de las columnas

// Crear objeto Keypad
Keypad keypad = Keypad(makeKeymap(keys), pinesFilas, pinesColumnas, FILAS, COLUMNAS);

const int MAX_CLAVE = 6;  // Longitud máxima de la clave
char clave[MAX_CLAVE + 1];  // +1 para el carácter nulo
int posicion = 0;

// Pin del buzzer
const int BUZZER_PIN = 10;  // Conectar el buzzer al pin 10

// Configuración de tonos
const int TONO_CORRECTO = 1000;  // 1kHz - tono agradable
const int TONO_INCORRECTO = 200;  // 200Hz - tono desagradable

// Clave correcta
const char CLAVE_CORRECTA[] = "123456";

// Crear objeto LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);


void setup() {
  lcd.init();
  lcd.backlight();
  
  // Configurar buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Mensaje inicial
  lcd.setCursor(0,0);
  lcd.print("Control de Acceso");
  lcd.setCursor(0,1);
  lcd.print("Ingrese Clave:");
}

void loop() {
  char key = keypad.getKey();
  
  if (key) {
    if (posicion < MAX_CLAVE) {
      clave[posicion] = key;
      posicion++;
      clave[posicion] = '\0';
      
      // Mostrar la clave actual
      lcd.setCursor(0,2);
      lcd.print("Clave: ");
      lcd.print(clave);
      
      // Verificar si se completó la clave
      if (posicion == MAX_CLAVE) {
        if (strcmp(clave, CLAVE_CORRECTA) == 0) {
          lcd.setCursor(0,3);
          lcd.print("Acceso Concedido!");
          beepCorrecto();
        } else {
          lcd.setCursor(0,3);
          lcd.print("Acceso Denegado!");
          beepIncorrecto();
        }
        
        // Reiniciar después de 2 segundos
        delay(2000);
        resetearSistema();
      }
    }
  }
}

// Función para el beep de acceso correcto
void beepCorrecto() {
  for(int i = 0; i < 5; i++) {
    tone(BUZZER_PIN, TONO_CORRECTO);
    delay(200);
    noTone(BUZZER_PIN);
    delay(100);
  }
}

// Función para el beep de acceso incorrecto
void beepIncorrecto() {
  for(int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, TONO_INCORRECTO);
    delay(200);
    noTone(BUZZER_PIN);
    delay(100);
  }
}

// Función para reiniciar el sistema
void resetearSistema() {
  posicion = 0;
  memset(clave, 0, sizeof(clave));
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Control de Acceso");
  lcd.setCursor(0,1);
  lcd.print("Ingrese Clave:");
}