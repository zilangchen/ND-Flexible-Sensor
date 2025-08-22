// esp32_fdc2214_10x10.ino
// ESP32 (WROOM-32) + FDC2214 board (CH0 only) + 2×CD74HC4067 → 10×10 capacitance matrix
// Wiring summary is in hardware_config.md. This sketch scans all 100 pixels and prints ΔC matrix.
//
// 2025-08-21

#include <Arduino.h>
#include <Wire.h>

/* ---------------- I2C (FDC2214) ---------------- */
#define I2C_SDA 21
#define I2C_SCL 22
#define FDC_ADDR 0x2B        // ADDR tied to 3.3V on your board → 0x2B

/* ---------------- FDC2214 registers ---------------- */
#define DATA_CHx_MSB(ch)     (0x00 + (ch)*2)
#define DATA_CHx_LSB(ch)     (0x01 + (ch)*2)
#define RCOUNT_CHx(ch)       (0x08 + (ch))
#define SETTLE_CHx(ch)       (0x10 + (ch))
#define CLOCK_DIV_CHx(ch)    (0x14 + (ch))
#define DRV_CURRENT_CHx(ch)  (0x1E + (ch))
#define STATUS               0x18
#define CONFIG               0x1A
#define MUX_CONFIG           0x1B
#define RESET_DEV            0x1C
#define MANUF_ID             0x7E
#define DEVICE_ID            0x7F

/* ---------------- CD74HC4067 control pins ----------------
   ROW S0..S3  → GPIO18, GPIO19, GPIO17, GPIO16
   COL S0..S3  → GPIO33, GPIO25, GPIO26, GPIO27
   EN pins of both 4067 tied to GND (always enabled)
----------------------------------------------------------- */
const int ROW_S[4] = {18, 19, 17, 16};   // S0, S1, S2, S3  (ROW mux)
const int COL_S[4] = {33, 25, 26, 27};   // S0, S1, S2, S3  (COL mux)

/* ---------------- SD (shutdown) control ----------------
   SD is tied to ESP32 GPIO23 per your wiring. HIGH=active, LOW=shutdown.
--------------------------------------------------------- */
const int PIN_SD = 23;

/* ---------------- Sensor / math constants ---------------- */
static const float FREF_HZ = 40e6;   // On-board 40 MHz reference oscillator on FDC board
static const float L_H     = 33e-6;  // Inductor on CH0 (consult board BOM; typical 33 uH)

/* ---------------- Matrix size ---------------- */
static const int ROWS = 10, COLS = 10;

/* ---------------- Buffers ---------------- */
static float baseline[ROWS][COLS];
static float mat[ROWS][COLS];

/* ---------------- Output mode flags ---------------- */
static bool g_print_csv = false;
static bool g_print_ascii = true;

/* ---------------- ASCII heatmap rendering ---------------- */
static void printCSV(){
  for(int r=0;r<ROWS;r++){
    for(int c=0;c<COLS;c++){
      Serial.print(mat[r][c], 9);
      if(c<COLS-1) Serial.print(',');
    }
    Serial.println();
  }
  Serial.println();
}

static inline float clampf(float v, float lo, float hi){
  if(v < lo) return lo; if(v > hi) return hi; return v;
}

static void printAsciiHeatmap(){
  // Compute per-frame min/max for normalization
  float vmin = mat[0][0], vmax = mat[0][0];
  for(int r=0;r<ROWS;r++){
    for(int c=0;c<COLS;c++){
      float v = mat[r][c];
      if(v < vmin) vmin = v;
      if(v > vmax) vmax = v;
    }
  }
  float span = (vmax - vmin);
  if(span < 1e-12f) span = 1e-12f;

  // Grayscale ramps; choose characters by density
  const char* ramp = " .:-=+*#%@"; // 10 levels
  const int rampLen = 10;

  // Clear screen and move cursor home for an in-place refresh (ANSI)
  Serial.print("\x1b[2J\x1b[H");
  Serial.println("ASCII Heatmap (ΔC, normalized per-frame)  —  'h':toggle ASCII  'c':toggle CSV  'b':baseline  'q':quiet");

  for(int r=0;r<ROWS;r++){
    for(int c=0;c<COLS;c++){
      float norm = (mat[r][c] - vmin) / span;     // 0..1
      int idx = (int)(norm * (rampLen - 1) + 0.5f);
      idx = idx < 0 ? 0 : (idx >= rampLen ? rampLen-1 : idx);
      Serial.print(ramp[idx]);
      Serial.print(ramp[idx]); // double width for squarish pixels
    }
    Serial.println();
  }
  Serial.println();
}

static void handleSerial(){
  while(Serial.available() > 0){
    int ch = Serial.read();
    if(ch == 'h' || ch == 'H'){
      g_print_ascii = !g_print_ascii; if(g_print_ascii) g_print_csv = false;
    } else if(ch == 'c' || ch == 'C'){
      g_print_csv = !g_print_csv; if(g_print_csv) g_print_ascii = false;
    } else if(ch == 'q' || ch == 'Q'){
      g_print_ascii = false; g_print_csv = false;
    } else if(ch == 'b' || ch == 'B'){
      Serial.println("Re-taking baseline ...");
      takeBaseline(12);
      Serial.println("Baseline updated.");
    } else if(ch == 's' || ch == 'S'){
      Serial.printf("Mode: ASCII=%d, CSV=%d\n", (int)g_print_ascii, (int)g_print_csv);
    }
  }
}

/* ---------------- Utilities ---------------- */
static inline void set4067(const int p[4], uint8_t idx){
  // idx 0..15 → S0..S3
  for(int b=0;b<4;b++) digitalWrite(p[b], (idx>>b)&1);
}

static void wr16(uint8_t reg, uint16_t v){
  Wire.beginTransmission(FDC_ADDR);
  Wire.write(reg); Wire.write(v>>8); Wire.write(v&0xFF);
  Wire.endTransmission();
}
static uint16_t rd16(uint8_t reg){
  Wire.beginTransmission(FDC_ADDR); Wire.write(reg); Wire.endTransmission(false);
  Wire.requestFrom(FDC_ADDR, (uint8_t)2);
  return ((uint16_t)Wire.read()<<8) | Wire.read();
}
static uint32_t read28(int ch){
  uint16_t msb = rd16(DATA_CHx_MSB(ch));
  uint16_t lsb = rd16(DATA_CHx_LSB(ch));
  return (((uint32_t)msb<<16) | lsb) & 0x0FFFFFFF; // 28-bit valid
}

static inline float codeToFreq(uint32_t code28){
  return (code28 * (FREF_HZ / (float)(1UL<<28)));
}
static inline float freqToCap(float f_hz){
  float w = 2.0f * PI * f_hz;
  return 1.0f / (w*w*L_H);
}

/* ---------------- FDC2214 init ---------------- */
void fdcInit(uint16_t rcount=0x0200, uint16_t settle=0x0200){
  wr16(RESET_DEV, 0x8000); delay(5);
  // External reference, active, normal operation (typical 0x1E01)
  wr16(CONFIG, 0x1E01);
  // Deglitch=10 MHz (b101), default mux config otherwise
  wr16(MUX_CONFIG, 0xC20D);
  // CH0 timing
  wr16(RCOUNT_CHx(0), rcount);        // conversion time ≈ (RCOUNT*16)/Fref
  wr16(SETTLE_CHx(0), settle);        // settle time after switching
  wr16(CLOCK_DIV_CHx(0), 0x2001);     // FIN_SEL/FREF_DIV = 1:1
  wr16(DRV_CURRENT_CHx(0), 0x5800);   // medium drive; adjust to 1.2–1.8 Vpp by scope
}

/* ---------------- Baseline & scanning ---------------- */
void takeBaseline(int avgN=8){
  for(int r=0;r<ROWS;r++){
    set4067(ROW_S, r);
    for(int c=0;c<COLS;c++){
      set4067(COL_S, c);
      delayMicroseconds(20);          // 4067 settling
      double acc=0.0;
      for(int k=0;k<avgN;k++){
        uint32_t code = read28(0);
        float f = codeToFreq(code);
        acc += freqToCap(f);
      }
      baseline[r][c] = acc/avgN;
    }
  }
}

void scanFrame(bool printCSV=false){
  for(int r=0;r<ROWS;r++){
    set4067(ROW_S, r);
    for(int c=0;c<COLS;c++){
      set4067(COL_S, c);
      delayMicroseconds(250);         // match RCOUNT=0x0200
      uint32_t code = read28(0);
      float f = codeToFreq(code);
      float cap = freqToCap(f);
      mat[r][c] = cap - baseline[r][c]; // ΔC
    }
  }
}

/* ---------------- Setup & Loop ---------------- */
void setup(){
  Serial.begin(115200);

  // 4067 select pins
  for(int i=0;i<4;i++){ pinMode(ROW_S[i], OUTPUT); pinMode(COL_S[i], OUTPUT); }

  // SD control
  pinMode(PIN_SD, OUTPUT);
  digitalWrite(PIN_SD, HIGH);       // bring FDC2214 out of shutdown
  delay(10);                        // wait device ready

  // I2C
  Wire.begin(I2C_SDA, I2C_SCL, 400000);  // 400 kHz

  // Optional: read IDs
  uint16_t mid, did;
  wr16(RESET_DEV, 0x8000); delay(5);
  mid = rd16(MANUF_ID); did = rd16(DEVICE_ID);
  Serial.printf("FDC221x MID=0x%04X, DID=0x%04X\n", mid, did);

  // Init FDC2214 channel 0
  fdcInit(/*rcount=*/0x0200, /*settle=*/0x0200);

  // Baseline
  Serial.println("Taking baseline ...");
  takeBaseline(12);
  Serial.println("Baseline done.");

  Serial.println("Commands: 'h' toggle ASCII heatmap, 'c' toggle CSV, 'q' quiet, 'b' baseline, 's' status");
}

void loop(){
  uint32_t t0 = millis();
  scanFrame(false);
  uint32_t t1 = millis();
  float fps = 1000.0f / (t1 - t0 + 1e-3f);
  if(g_print_ascii){
    printAsciiHeatmap();
  } else if(g_print_csv){
    printCSV();
  }
  Serial.printf("Frame time = %lu ms, ~%.1f FPS\n", (unsigned long)(t1 - t0), fps);
  handleSerial();
  delay(5);
}