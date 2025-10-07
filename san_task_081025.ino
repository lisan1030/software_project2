
#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13




#define SND_VEL 346.0     
#define INTERVAL 25       
#define PULSE_DURATION 10 
#define _DIST_MIN 100     
#define _DIST_MAX 300    

#define TIMEOUT ((INTERVAL / 2) * 1000.0) 
#define SCALE (0.001 * 0.5 * SND_VEL)    

#define _EMA_ALPHA 0.3  
#define N_SAMPLES 3     


unsigned long last_sampling_time = 0;
float samples[N_SAMPLES];     
int sample_index = 0;         
bool buffer_filled = false;   
float dist_ema = 0.0;        

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  Serial.begin(57600);
}

float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; 
}

void loop() {
  float dist_raw, dist_median;

  if (millis() < last_sampling_time + INTERVAL)
    return;

  
  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  
  samples[sample_index] = dist_raw;
  sample_index = (sample_index + 1) % N_SAMPLES;
  if (sample_index == 0) buffer_filled = true;

  
  int valid_size = buffer_filled ? N_SAMPLES : xsample_index;
  float temp[valid_size];


  for (int i = 0; i < valid_size; i++) {
    temp[i] = samples[i];
  }


  for (int i = 0; i < valid_size - 1; i++) {
    for (int j = i + 1; j < valid_size; j++) {
      if (temp[j] < temp[i]) {
        float t = temp[i];
        temp[i] = temp[j];
        temp[j] = t;
      }
    }
  }

 
  if (valid_size % 2 == 1)
    dist_median = temp[valid_size / 2];
  else
    dist_median = (temp[valid_size / 2 - 1] + temp[valid_size / 2]) / 2.0;

  
  dist_ema = _EMA_ALPHA * dist_median + (1 - _EMA_ALPHA) * dist_ema;

  
  Serial.print("Min:");     Serial.print(_DIST_MIN);
  Serial.print(",raw:");    Serial.print(dist_raw);
  Serial.print(",median:"); Serial.print(dist_median);
  Serial.print(",ema:");    Serial.print(dist_ema);
  Serial.print(",Max:");    Serial.print(_DIST_MAX);
  Serial.println("");

  
  if ((dist_ema < _DIST_MIN) || (dist_ema > _DIST_MAX))
    digitalWrite(PIN_LED, HIGH);
  else
    digitalWrite(PIN_LED, LOW);

  
  last_sampling_time += INTERVAL;
}
