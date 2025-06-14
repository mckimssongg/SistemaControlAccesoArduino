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

// Crear objeto LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  lcd.init();
  lcd.backlight();
  
  // Mensaje inicial
  lcd.setCursor(0,0);
  lcd.print("Control de Acceso");
  lcd.setCursor(0,1);
  lcd.print("Ingrese Clave:");
}

const int MAX_CLAVE = 6;  // Longitud máxima de la clave
char clave[MAX_CLAVE + 1];  // +1 para el carácter nulo
int posicion = 0;

void loop() {
  char key = keypad.getKey();
  
  if (key) {
    if (posicion < MAX_CLAVE) {
      clave[posicion] = key;
      posicion++;
      clave[posicion] = '\0';  // Agregar terminador de cadena
      
      // Mostrar la clave actual
      lcd.setCursor(0,2);
      lcd.print("Clave: ");
      lcd.print(clave);
    }
  }
}