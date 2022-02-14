#define PERIOD 680
#define COMM_DELAY 1000

#define BUFF_SIZE 200
#define NUM_PARAMS 3 // command, x-amount, speed, later add y-amount and z-amount
#define STEPS_TO_MM 5
#define MIN_PERIOD 600  // 1.2ms/step, 6ms/mm  (167 mm/s)
#define MAX_PERIOD 3000 // 6.0ms/step, 30ms/mm (33 mm/s)
#define MIN_SPEED 33
#define MAX_SPEED 166

// Pins
#define X 2
#define DX 5

char buf[BUFF_SIZE + 1];

void setup() {
  pinMode(X, OUTPUT);
  pinMode(DX, OUTPUT);
  Serial.begin(9600);
  Serial.println("Initializing");
//  command.reserve(200);
}

void moveX(int dir, int mm, int spd) {
  int steps = mm * STEPS_TO_MM;
  if (dir == 1) {
    digitalWrite(DX, HIGH);  
  } else if (dir == -1) {
    digitalWrite(DX, LOW);
  }

  int real_spd = spd;
  if (spd < MIN_SPEED) real_spd = MIN_SPEED;
  if (spd > MAX_SPEED) real_spd = MAX_SPEED;
  
  int period = (int) (100000/(float)real_spd);
  int t0 = millis();
  for(int x = 0;x < steps; x++) {
    digitalWrite(X, LOW);
    delayMicroseconds(period);
    digitalWrite(X, HIGH);
    delayMicroseconds(period);
  }
  int delta = millis() - t0;
  Serial.println("Movement perdormed in " + (String)delta + "ms");
}

void readCommands(String (& params)[NUM_PARAMS]) {
  Serial.readBytesUntil('\n', buf, BUFF_SIZE);
  char *com = strtok(buf, " ");
  for (int i = 0; i < NUM_PARAMS; i++) {
    params[i] = (String) com;
    com = strtok(NULL, " ");
  }
}

void printCommands(String params[NUM_PARAMS]) {
  Serial.print("[");
  for (int i = 0; i < NUM_PARAMS; i++) {
    Serial.print(params[i]);
    if (i < NUM_PARAMS - 1)
      Serial.print(","); 
  }
  Serial.println("]");
}

void loop() {
  while(Serial.available() > 0) {
    memset(&buf[0], 0, sizeof(buf));
    Serial.println(buf);
    String params[NUM_PARAMS];
    readCommands(params);
    printCommands(params);
    if (params[0].equals("M")) {
      if (params[1].startsWith("X") && params[NUM_PARAMS-1].startsWith("S")) {
        String amount = params[1].substring(1);
        String _spd = params[NUM_PARAMS-1].substring(1);
        Serial.println("Moving X " + amount + "mm at " + _spd + "mm/s");
        int mm = amount.toInt();
        int spd = _spd.toInt();
        if (mm >= 0) {
          moveX(1, mm, spd); 
        } else {
          moveX(-1, -1 * mm, spd);
        }
      } else {
        Serial.println("Invalid command");
      }
    } else {
      Serial.println("Unkown command " + params[0]);
    }
  }
}
