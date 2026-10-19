#include <Servo.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Tested and working Psycho-Pass Dominator cosplay prop sketch.
constexpr uint8_t TRIGGER_PIN = 4;
constexpr uint8_t SERVO_COUNT = 3;
constexpr uint8_t SERVO_PINS[SERVO_COUNT] = {7, 5, 6};
constexpr uint8_t DF_RX_PIN = A1; // Arduino RX <- DFPlayer TX
constexpr uint8_t DF_TX_PIN = A2; // Arduino TX -> DFPlayer RX (through ~1k resistor)

constexpr uint16_t STARTUP_AUDIO_MS = 5000; // Adjust to actual 0001.mp3 duration
constexpr uint16_t DFPLAYER_BOOT_MS = 2000;
constexpr uint8_t DFPLAYER_VOLUME = 10;
constexpr uint8_t CLOSED_ANGLES[SERVO_COUNT] = {90, 90, 90}; // Calibrate!
constexpr uint8_t OPEN_ANGLES[SERVO_COUNT] = {120, 60, 120}; // Calibrate!
constexpr uint16_t SERVO_STEP_MS = 20;
constexpr uint16_t DEBOUNCE_MS = 35;

// Put files in /mp3/0001.mp3 through /mp3/0004.mp3 on microSD.
constexpr uint16_t SOUND_STARTUP = 1;
constexpr uint16_t SOUND_OPEN = 2;
constexpr uint16_t SOUND_PARALYZER = 3;
constexpr uint16_t SOUND_LETHAL = 4;

Servo servos[SERVO_COUNT];
SoftwareSerial dfSerial(DF_RX_PIN, DF_TX_PIN);
DFRobotDFPlayerMini dfPlayer;
bool audioReady = false;

bool allowServoMovement = false;
bool lethalMode = false;
bool moving = false;
uint8_t currentAngles[SERVO_COUNT];
uint8_t targetAngles[SERVO_COUNT];
unsigned long lastServoStep = 0;
unsigned long startupStarted = 0;

bool lastRawTrigger = HIGH;
bool stableTrigger = HIGH;
unsigned long triggerChanged = 0;

void playTrack(uint16_t number) {
  if (audioReady) dfPlayer.playMp3Folder(number);
}

void setTargets(bool open) {
  for (uint8_t i = 0; i < SERVO_COUNT; ++i) {
    targetAngles[i] = open ? OPEN_ANGLES[i] : CLOSED_ANGLES[i];
  }
  moving = true;
}

void updateServos() {
  if (!allowServoMovement || !moving) return;
  if (millis() - lastServoStep < SERVO_STEP_MS) return;
  lastServoStep = millis();

  bool anyDifference = false;
  for (uint8_t i = 0; i < SERVO_COUNT; ++i) {
    if (currentAngles[i] < targetAngles[i]) ++currentAngles[i];
    else if (currentAngles[i] > targetAngles[i]) --currentAngles[i];
    servos[i].write(currentAngles[i]);
    if (currentAngles[i] != targetAngles[i]) anyDifference = true;
  }
  moving = anyDifference;
}

void onTriggerPress() {
  if (!allowServoMovement || moving) return;

  if (!lethalMode) {
    lethalMode = true;
    setTargets(true);
    playTrack(SOUND_OPEN);
    Serial.println(F("Lethal Eliminator: transforming"));
  } else {
    playTrack(SOUND_LETHAL);
    Serial.println(F("Lethal Eliminator: simulated shot"));
  }
}

void updateTrigger() {
  bool raw = digitalRead(TRIGGER_PIN);
  if (raw != lastRawTrigger) triggerChanged = millis();
  lastRawTrigger = raw;

  if (millis() - triggerChanged >= DEBOUNCE_MS && raw != stableTrigger) {
    stableTrigger = raw;
    if (stableTrigger == LOW) onTriggerPress();
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);

  for (uint8_t i = 0; i < SERVO_COUNT; ++i) {
    currentAngles[i] = CLOSED_ANGLES[i];
    targetAngles[i] = CLOSED_ANGLES[i];
    servos[i].attach(SERVO_PINS[i]);
    servos[i].write(currentAngles[i]);
  }

  dfSerial.begin(9600);
  delay(DFPLAYER_BOOT_MS);
  for (uint8_t attempt = 0; attempt < 10; ++attempt) {
    if (dfPlayer.begin(dfSerial, true, true)) {
      audioReady = true;
      break;
    }
    delay(250);
  }

  if (audioReady) {
    dfPlayer.volume(DFPLAYER_VOLUME);
    delay(150);
    playTrack(SOUND_STARTUP);
  } else {
    Serial.println(F("DFPlayer not detected; running without audio"));
  }

  startupStarted = millis();
  Serial.println(F("Startup lock active"));
}

void loop() {
  if (!allowServoMovement && millis() - startupStarted >= STARTUP_AUDIO_MS) {
    allowServoMovement = true;
    Serial.println(F("Startup finished. Trigger enabled."));
  }

  updateTrigger();
  updateServos();

  // Optional serial commands for bench testing:
  // 'p' = Paralyzer (close), 'l' = Lethal (open), 'f' = simulated shot.
  if (Serial.available()) {
    char c = Serial.read();
    if (allowServoMovement && !moving) {
      if (c == 'p') {
        lethalMode = false;
        setTargets(false);
        playTrack(SOUND_PARALYZER);
      } else if (c == 'l') {
        lethalMode = true;
        setTargets(true);
        playTrack(SOUND_OPEN);
      } else if (c == 'f') {
        playTrack(lethalMode ? SOUND_LETHAL : SOUND_PARALYZER);
      }
    }
  }
}
