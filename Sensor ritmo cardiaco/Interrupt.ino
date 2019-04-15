
volatile int rate[10];                    // matriz para mantener los últimos diez valores IBI
volatile unsigned long sampleCounter = 0;          // utilizado para determinar el tiempo de pulso
volatile unsigned long lastBeatTime = 0;           // Usado para encontrar IBI
volatile int P =512;                      // Se utiliza para encontrar el pico en la onda del pulso
volatile int T = 512;                     // Se utiliza para encontrar canal en la onda de pulso.
volatile int thresh = 512;                // Utilizado para ​​encontrar momento instantáneo de latido del corazón
volatile int amp = 100;                   // utilizado para mantener la amplitud de la forma de onda del pulso
volatile boolean firstBeat = true;        // Utilizado para la matriz de velocidad, así que iniciamos con BPM razonables
volatile boolean secondBeat = false;      // Utilizado para la matriz de velocidad, así que iniciamos con BPM razonables

void interruptSetup(){     
  // Inicializa Timer2 para lanzar una interrupción cada 2 ms.
  TCCR2A = 0x02;     // DESHABILITAR PWM EN LOS PINS DIGITALES 3 Y 11, Y ENTRAR EN EL MODO CTC
  TCCR2B = 0x06;     // NO COMPARAR FORZOSAMENTE, 256 PRESCALER 
  OCR2A = 0X7C;      // ESTABLECE LA PARTE SUPERIOR DEL CUENTA A 124 PARA 500Hz TASA DE MUESTRA
  TIMSK2 = 0x02;     // HABILITAR LA INTERRUPCIÓN EN EL PARTIDO ENTRE TIMER2 Y OCR2A
  sei();             // ASEGÚRESE DE QUE LOS INTERRUPTORES GLOBALES ESTÁN HABILITADOS      
} 

// ESTA ES LA RUTINA DEL SERVICIO DE INTERRUPCIÓN TIMER 2.
// El temporizador 2 se asegura de que tomemos una lectura cada 2 milisegundos
ISR(TIMER2_COMPA_vect){                         // se dispara cuando el Timer2 cuenta hasta 124
  cli();                                      // deshabilita las interrupciones mientras hacemos esto
  Signal = analogRead(pulsePin);              // lee el sensor de pulso 
  sampleCounter += 2;                         // Hace seguimiento del tiempo en mS con esta variable
  int N = sampleCounter - lastBeatTime;       // Monitoriza el tiempo transcurrido desde el último tiempo para evitar ruidos.

    //  encontrar el pico y el canal de la onda del pulso
  if(Signal < thresh && N > (IBI/5)*3){       // evitar el ruido dicrótico esperando 3/5 del último IBI
    if (Signal < T){                        // T es el canal
      T = Signal;                         // realiza un seguimiento del punto más bajo en la onda de pulso
      
    }
  }

  if(Signal > thresh && Signal > P){          // condición de trilla ayuda a evitar el ruido
    P = Signal;                             // P es el pico
  }                                        // realizar un seguimiento del punto más alto en la onda del pulso

  //  AHORA ES HORA DE BUSCAR EL LATIDO DEL CORAZÓN
  // la señal aumenta de valor cada vez que hay un pulso
  if (N > 250){                                   // evitar el ruido de alta frecuencia
    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) ){        
      Pulse = true;                               // establecer la bandera de pulso cuando creemos que hay un pulso

        IBI = sampleCounter - lastBeatTime;         // medir el tiempo entre latidos en mS
      lastBeatTime = sampleCounter;               // mantener un registro del tiempo para el próximo pulso

      if(secondBeat){                        // si este es el segundo tiempo, si secondBeat == VERDADERO
        secondBeat = false;                  // borrar la bandera de secondBeat
        for(int i=0; i<=9; i++){             // Siembra el total acumulado para obtener un BPM realista en el inicio
          rate[i] = IBI;                      
        }
      }

      if(firstBeat){                         // si es la primera vez que encontramos un compás, si firstBeat == TRUE
        firstBeat = false;                   // borrar la bandera de FirstBeat
        secondBeat = true;                   // configurar la segunda bandera de tiempo
        sei();                               // habilitar interrupciones de nuevo
        return;                              // El valor de IBI no es confiable, por lo tanto, deséchelo
      }   


      // mantener un total acumulado de los últimos 10 valores IBI
      word runningTotal = 0;                  // borrar la variable runningTotal   

      for(int i=0; i<=8; i++){                // desplazar datos en la matriz de velocidad
        rate[i] = rate[i+1];                  // y soltar el valor más antiguo del IBI. 
        runningTotal += rate[i];              // Sume los 9 valores IBI más antiguos.
      }

      rate[9] = IBI;                          // añadir el último IBI a la matriz de tasas
      runningTotal += rate[9];                // agrega el último IBI a runningTotal
      runningTotal /= 10;                     // promedia los últimos 10 valores IBI
      BPM = 60000/runningTotal;               // ¿Cuántos latidos pueden caber en un minuto? eso es BPM!
      QS = true;                              // establecer la bandera de auto cuantificada
      // LA BANDERA QS NO ESTÁ BORRADA DENTRO DE ESTA ISR
    }                       
  }

  if (Signal < thresh && Pulse == true){   // Cuando los valores están bajando, el ritmo ha terminado.

    Pulse = false;                         // restablece la bandera de pulso para que podamos hacerlo de nuevo
    amp = P - T;                           // obtener la amplitud de la onda del pulso
    thresh = amp/2 + T;                    // ajustar la trilla al 50% de la amplitud
    P = thresh;                            // restablecer estos para la próxima vez
    T = thresh;
  }

  if (N > 2500){                           // Si pasan 2,5 segundos sin latido.
    thresh = 512;                          // establecer trilla por defecto
    P = 512;                               // establecer P predeterminado
    T = 512;                               // establecer T predeterminado
    lastBeatTime = sampleCounter;          // actualizar lastBeatTime       
    firstBeat = true;                      // configúralos para evitar el ruido
    secondBeat = false;                    // cuando recuperemos el latido del corazón
  }

  sei();                                   // habilitar interrupciones cuando hayas terminado!
}// end isr
