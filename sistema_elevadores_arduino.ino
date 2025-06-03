/* si esta menor que el nivel debera subir si es mayor que el nivel seleccionado debera bajar agrega al codigo esa funcion complementando con lo que ya esta porfavor 
l293d 
 */



// input 1 = 11
// input 2 = 12
// input 4 = 6
// input 3 = 7
//output4 = gnd motor 1
//output3 = vcc motor 1
//output2 = vcc motor 2
//output1 = gnd motor 2 

// control por pwm
//enabled 3 & 4 = 13
//enabled 1 & 2 = 13 #include <Wire.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// Crear LCD 16x2 en dirección 0x27. Usa 0x3F si no funciona.
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin del buzzer (conectado a digital 10)
const int buzzerPin = 10;

// Pines del L293D
const int IN1 = 11;   // Motor B (Elevador B)
const int IN2 = 12;
const int IN3 = 7;    // Motor A (Elevador A)
const int IN4 = 6;
const int EN12 = 13;  // Enable B
const int EN34 = 13;  // Enable A (comparten pin PWM 13)

// Velocidad del motor por PWM (0 a 255)
const int velocidad = 200;

// Estructura para representar un elevador
struct Elevador {
  int nivel;
  bool niveles[6]; // Cambiar de 5 a 6 para incluir nivel 5
  const char* nombre;
};

// Inicialización de elevadores con nombres "A" y "B"
Elevador elevadorA = {1, {false, true, false, false, false, false}, "A"};
Elevador elevadorB = {1, {false, true, false, false, false, false}, "B"};

Elevador* elevadores[] = { &elevadorA, &elevadorB };
const int numElevadores = 2;

// ==== Keypad setup ====
const byte ROWS = 3; // Cambiar de 2 a 3 para incluir nivel 5
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'}
};

byte rowPins[ROWS] = {9, 8}; // Agregar pin A0 para tercera fila
byte colPins[COLS] = {5, 4, 3, 2}; // columnas del keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variables para controlar la selección
char elevadorSeleccionado = 0; // 'A' o 'B'
int pisoSeleccionado = -1;     // nivel 0 a 5
unsigned long ultimaActividad = 0; // Para controlar inactividad
const unsigned long tiempoInactividad = 60000; // 1 minuto en milisegundos

// ==== PROTOTIPOS ====
void actualizarDisplay();
void procesarSolicitud(char elevadorLetra, int nivelSolicitado);
void simularMovimiento(Elevador* elevador, int nivelDestino);
Elevador* buscarElevadorPorNombre(char nombre);
void moverMotor(Elevador* elevador, bool subir);
void detenerMotor(Elevador* elevador);
void verificarInactividad(); // Nueva función
void regresarANivel1(); // Nueva función

// ==== SETUP ====
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(buzzerPin, OUTPUT);

  // Pines de motor como salida
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(EN12, OUTPUT);
  pinMode(EN34, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("Sistema Elevadores");
  delay(2000);
  lcd.clear();

  Serial.println("Sistema de Elevadores - Arduino");
  Serial.println("===============================");

  ultimaActividad = millis(); // Inicializar tiempo de actividad
  actualizarDisplay();
}

// ==== LOOP PRINCIPAL ====
void loop() {
  char key = keypad.getKey();

  if (key != NO_KEY) {
    ultimaActividad = millis(); // Actualizar tiempo de última actividad
    Serial.print("Tecla presionada: ");
    Serial.println(key);

    if (key == 'A' || key == 'B') {
      elevadorSeleccionado = key;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Elevador: ");
      lcd.print(elevadorSeleccionado);
      lcd.setCursor(0, 1);
      lcd.print("Seleccione piso:");
      pisoSeleccionado = -1;
    } 
    else if (elevadorSeleccionado != 0 && key >= '0' && key <= '5') { // Cambiar de '4' a '5'
      pisoSeleccionado = key - '0';
      lcd.setCursor(15, 1);
      lcd.print(pisoSeleccionado);

      procesarSolicitud(elevadorSeleccionado, pisoSeleccionado);

      elevadorSeleccionado = 0;
      pisoSeleccionado = -1;

      delay(1000);
      actualizarDisplay();
    }
  }

  verificarInactividad(); // Verificar si hay que regresar a nivel 1
}

// ==== FUNCIONES ====

Elevador* buscarElevadorPorNombre(char nombre) {
  for (int i = 0; i < numElevadores; i++) {
    if (elevadores[i]->nombre[0] == nombre) {
      return elevadores[i];
    }
  }
  return nullptr;
}

void procesarSolicitud(char elevadorLetra, int nivelSolicitado) {
  Serial.print("Solicitud: elevador ");
  Serial.print(elevadorLetra);
  Serial.print(", piso ");
  Serial.println(nivelSolicitado);

  Elevador* elevador = buscarElevadorPorNombre(elevadorLetra);
  if (elevador == nullptr) {
    Serial.println("Elevador no encontrado.");
    return;
  }

  simularMovimiento(elevador, nivelSolicitado);
}

void simularMovimiento(Elevador* elevador, int nivelDestino) {
  if (nivelDestino < 0 || nivelDestino > 5) { // Cambiar de 4 a 5
    Serial.println("Nivel destino inválido. Debe estar entre 0 y 5.");
    return;
  }

  Serial.print("Elevador ");
  Serial.print(elevador->nombre);
  Serial.print(" se mueve de ");
  Serial.print(elevador->nivel);
  Serial.print(" a ");
  Serial.println(nivelDestino);

  bool subir = nivelDestino > elevador->nivel;
  
  // Mostrar dirección en LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Elevador ");
  lcd.print(elevador->nombre);
  lcd.setCursor(0, 1);
  lcd.print(subir ? "Subiendo..." : "Bajando...");

  moverMotor(elevador, subir);

  while (elevador->nivel != nivelDestino) {
    delay(1000);
    elevador->nivel += subir ? 1 : -1;
    
    // Mostrar nivel actual durante movimiento
    lcd.setCursor(12, 1);
    lcd.print("N");
    lcd.print(elevador->nivel);
    
    Serial.print("Elevador ");
    Serial.print(elevador->nombre);
    Serial.print(" en nivel ");
    Serial.println(elevador->nivel);
  }

  detenerMotor(elevador);

  for (int i = 0; i < 6; i++) { // Cambiar de 5 a 6
    elevador->niveles[i] = (i == nivelDestino);
  }

  for (int i = 0; i < 3; i++) {
    digitalWrite(buzzerPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);
    delay(200);
  }

  Serial.print("Elevador ");
  Serial.print(elevador->nombre);
  Serial.print(" llegó a nivel ");
  Serial.println(nivelDestino);

  actualizarDisplay();
}

void moverMotor(Elevador* elevador, bool subir) {
  analogWrite(EN12, velocidad); // EN12 y EN34 comparten pin 13 (PWM)
  analogWrite(EN34, velocidad); // redundante pero seguro

  if (elevador->nombre[0] == 'A') {
    digitalWrite(IN3, subir ? HIGH : LOW);
    digitalWrite(IN4, subir ? LOW : HIGH);
  } else if (elevador->nombre[0] == 'B') {
    digitalWrite(IN1, subir ? HIGH : LOW);
    digitalWrite(IN2, subir ? LOW : HIGH);
  }
}

void detenerMotor(Elevador* elevador) {
  if (elevador->nombre[0] == 'A') {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  } else if (elevador->nombre[0] == 'B') {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }

  analogWrite(EN12, 0); // detener PWM
  analogWrite(EN34, 0);
}

// Nueva función para verificar inactividad
void verificarInactividad() {
  if (millis() - ultimaActividad >= tiempoInactividad) {
    // Verificar si algún elevador no está en nivel 1
    if (elevadorA.nivel != 1 || elevadorB.nivel != 1) {
      Serial.println("Inactividad detectada. Regresando elevadores a nivel 1...");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("INACTIVIDAD");
      lcd.setCursor(0, 1);
      lcd.print("Regresando...");
      delay(2000);
      
      regresarANivel1();
    }
    ultimaActividad = millis(); // Resetear contador
  }
}

// Nueva función para regresar ambos elevadores a nivel 1
void regresarANivel1() {
  // Regresar elevador A si no está en nivel 1
  if (elevadorA.nivel != 1) {
    Serial.println("Regresando Elevador A a nivel 1");
    simularMovimiento(&elevadorA, 1);
    delay(500);
  }
  
  // Regresar elevador B si no está en nivel 1
  if (elevadorB.nivel != 1) {
    Serial.println("Regresando Elevador B a nivel 1");
    simularMovimiento(&elevadorB, 1);
    delay(500);
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Elevadores en");
  lcd.setCursor(0, 1);
  lcd.print("posicion inicial");
  delay(2000);
  
  actualizarDisplay();
}

void actualizarDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("A:N");
  lcd.print(elevadorA.nivel);
  lcd.setCursor(8, 0);
  lcd.print("B:N");
  lcd.print(elevadorB.nivel);
  lcd.setCursor(0, 1);
  lcd.print("Sistema Activo");

  Serial.print("Estado - ");
  Serial.print(elevadorA.nombre);
  Serial.print(": Nivel ");
  Serial.print(elevadorA.nivel);
  Serial.print(" | ");
  Serial.print(elevadorB.nombre);
  Serial.print(": Nivel ");
  Serial.println(elevadorB.nivel);
}
