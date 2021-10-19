
int lectura = 35; //pin de conexion de señal del oximetro
int led  = 2;
///// FRECUENCIOMETRO ///
int limite = 100;
long rango = 50; // este es el rango por el que se disparará la salida 2 y pasa a estado lógico 1
long ultimamedicion; // contiene el valor de la última medición que disparó a lógico 1, la salida 2
int ciclo = 0; // 1=alto 0=bajo cada 60seg
int cambiodeciclo = 0;
int picodetension;//max senal
int valledetension = 1023;//todo el rango se actualiza...
long contadorciclo;
///////********************/////////
boolean estadoBPM = true;  //guarda el estado del led (encendido o apagado)
boolean estadoLed = true;  //guarda el estado del led (encendido o apagado)
int intervaloEncendido = 200;  // tiempo que esta encendido el led TIMER
int IntervaloApagado = 50;  // tiempo que esta apagado el led

int IntervaloBPM = 15000;  // CADA 15 SEG TOMA MUESTRA  tiempo que esta apagado el led
int IntervaloBPM2 = 1;  // tiempo que esta apagado el led

unsigned long tiempoAnteriorEncendido = 0;  //guarda tiempo de referencia para comparar
unsigned long tiempoAnteriorApagado = 0;  //guarda tiempo de referencia para comparar
unsigned long tiempoAnteriorBPM = 0;  //guarda tiempo de referencia para comparar
unsigned long tiempoAnteriorBPM2 = 0;  //guarda tiempo de referencia para comparar
/////////////////////////////
int senal = 0; //variable donde guardamos el valor de la señal
int pulsos = 0; //guarda el num de picos de tension q detecta: los pulsos x min
int pulsos2 = 0;

void setup()
{
  Serial.begin(9600); //comunicacion serial para DEBUGGEAR
  pinMode(led, OUTPUT); //configura el pin como salida

  for ( int i = 0 ; i < 5 ; i++ ) { //el led parpadea 5 veces para saludar!
    digitalWrite(led, 1 ) ;
    delay ( 20 ) ;
    digitalWrite (led, 0 ) ;
  }
  digitalWrite (led, LOW);//apago el led por las dudas
}


void loop()
{
  senal = analogRead(lectura); //guardamos el valor en la variable senal
  /////////***********************************************************////////////////// DETECTAR VALOR MAXIMO
  if (senal >= ( ultimamedicion + limite) ) // 200 RANGO  La salida 2 pasa a 1 logico si la tensión medida en la entrada analógica 0 es mayor que la anterior lectura + latensión de RANGO
  {
    ultimamedicion = senal;  // SE ASIGANA A LA VARIABLE ULTIMAMEDICION EL VALOR LEIDO POR LA ENTRADA ANALÓGICA CERO
    ciclo = 1; //true fue un pico de tension y creo un ciclo
    if (senal > picodetension) // SI LA TENSIÓN MEDIDA POR LA ENTRADA CERO, ES LA MAYOR DETECTADA, SE ASIGNA A LA VARIABLE PICODETENSIÓN EL VALOR LEYDO POR AL ENTRADA CERO ANALÓGICA
    {
      picodetension = senal; // SE ASIGNA EL VALOR LEYDO POR LA ENTRADA CERO ANALÓGICA A LA VARIABLE PICODETENSIÓN.
    }
  }

  if (senal <= ( ultimamedicion - limite)) // NO HAY CAMBIO. La salida 2 pasa a 1 lógico si la tensión medida en la entrada analógica 0 es menor que la anterior lectura - la tensión de RANGO
  {
    ultimamedicion = senal; // SE ASIGNA A LA VARIABLE ULTIMAMEDICIÓN LA LECTURA MEDIDA POR EL PUERTO ANALÓGICO CERO
    ciclo = 0; // EL CICLO SE PONE A CERO, ES DECIR, QUE EL VOLTAJE EMPIEZA A BAJAR DESDE EL PICO DE TENSIÓN MÁS ALTA
    if (senal < valledetension) // SE CUMPLE LA CONDICIÓN SI LA TENSIÓN DETECTADA POR EL PUERTO ANALÓGICO CERO ES MENOR QUE LA CONTENIDA EN LA VARIABLE VALLEDETENSIÓN
    {
      valledetension = senal; //Se asigna a la variable valledetensión el valor medido por la entrada analógica cero
    }
  }

  //////////////////// 1 minuto //////////// tarda 15 seg en medir e imprime
  if (millis() - tiempoAnteriorBPM >= IntervaloBPM) { //si ha transcurrido el periodo programado
    estadoBPM = false;
    pulsos2 = pulsos;
    //Serial.print("  BPM: ");
    //Serial.print(pulsos2);
    tiempoAnteriorBPM = millis(); //guarda el tiempo actual para comenzar a contar el tiempo apagad
    pulsos = 0;

  }
  if (millis() - tiempoAnteriorBPM2 >= IntervaloBPM2) { //si ha transcurrido el periodo programado
    estadoBPM = true;
    //Serial.print("BPM: ");
    pulsos2 = (pulsos2 + pulsos2) / 2;
    // Serial.print(pulsos2 * 4);
    tiempoAnteriorBPM2 = millis(); //guarda el tiempo actual para comenzar a contar el tiempo apagado
  }
  ///////////////////////////////////////
  if ((millis() - tiempoAnteriorEncendido >= intervaloEncendido) && estadoLed == true && ciclo == 0) { //si ha transcurrido el periodo programado
    estadoLed = false; //actualizo la variable para apagar el led
    picodetension = senal; //SE ASIGNA A LA VARIABLE PICODETENSIÓN EL VALOR DE LA TENSIÒN LEIDA POR EL PUERTO ANALÓGICO CERO
    valledetension = senal; //SE ASIGNA A LA VALLEDETENSION EL VALOR DE LA TENSIÒN LEIDA POR EL PUERTO ANALÓGICO CERO
    digitalWrite(led, estadoLed); //apago el led*******************************
    //Serial.println("1");

    tiempoAnteriorApagado = millis(); //guarda el tiempo actual para comenzar a contar el tiempo apagado
  }
  if ((millis() - tiempoAnteriorApagado >= IntervaloApagado) && estadoLed == false && ciclo == 1) //PULSO
  { //si ha transcurrido el periodo programado
    pulsos++;
    picodetension = senal; //SE ASIGNA A LA VARIABLE PICODETENSIÓN EL VALOR DE LA TENSIÒN LEIDA POR EL PUERTO ANALÓGICO CERO
    valledetension = senal; //SE ASIGNA A LA VALLEDETENSION EL VALOR DE LA TENSIÒN LEIDA POR EL PUERTO ANALÓGICO CERO
    estadoLed = true; //actualizo la variable para encender el led
    digitalWrite(led, estadoLed); //enciendo el led******************************
    //datoP = 2;
    Serial.println("beat");

    tiempoAnteriorEncendido = millis(); //guarda el tiempo actual para comenzar a contar el tiempo encendido
  }

  //Serial.print("     Senal: ");
  //Serial.println(senal); //imprimimos en monitor serial la señal

  delay(200);// cualquier cosa modifica este delay

}
