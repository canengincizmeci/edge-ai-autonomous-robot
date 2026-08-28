#include <Green_Balloon_FOMO_inferencing.h>
#include "esp_camera.h"


#define PWDN_GPIO_NUM  -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  5
#define Y9_GPIO_NUM    4
#define Y8_GPIO_NUM    6
#define Y7_GPIO_NUM    7
#define Y6_GPIO_NUM    14
#define Y5_GPIO_NUM    17
#define Y4_GPIO_NUM    21
#define Y3_GPIO_NUM    18
#define Y2_GPIO_NUM    16
#define VSYNC_GPIO_NUM 1
#define HREF_GPIO_NUM  2
#define PCLK_GPIO_NUM  15
#define SIOD_GPIO_NUM  8
#define SIOC_GPIO_NUM  9
#define LED_GPIO_NUM   47


#define WROOM_RX 44
#define WROOM_TX 43

HardwareSerial RobotSerial(1);

static uint8_t *snapshot_buf = nullptr;


char lastCmd = 'S';
unsigned long lastSeenTime = 0;
const unsigned long LOST_TIMEOUT = 700;
const float SCORE_LIMIT = 0.45;

int ei_camera_get_data(size_t offset, size_t length, float *out_ptr) {
  size_t pixel_ix = offset * 3;

  for (size_t i = 0; i < length; i++) {
    uint8_t r = snapshot_buf[pixel_ix + 0];
    uint8_t g = snapshot_buf[pixel_ix + 1];
    uint8_t b = snapshot_buf[pixel_ix + 2];

    out_ptr[i] = (r << 16) | (g << 8) | b;
    pixel_ix += 3;
  }

  return 0;
}

bool init_camera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;

  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;

  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_RGB565;
  config.frame_size = FRAMESIZE_96X96;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.grab_mode = CAMERA_GRAB_LATEST;

  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return false;
  }

  sensor_t *s = esp_camera_sensor_get();

  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);
    s->set_brightness(s, 1);
    s->set_saturation(s, -2);
  }

  Serial.println("Camera OK");
  return true;
}

bool capture_to_rgb888() {
  camera_fb_t *fb = esp_camera_fb_get();

  if (!fb) {
    Serial.println("Camera capture failed");
    return false;
  }

  if (fb->format != PIXFORMAT_RGB565) {
    Serial.println("Wrong pixel format");
    esp_camera_fb_return(fb);
    return false;
  }

  int pixel_count = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;

  for (int i = 0; i < pixel_count; i++) {
    uint16_t pixel = ((uint16_t)fb->buf[i * 2] << 8) | fb->buf[i * 2 + 1];

    uint8_t r = ((pixel >> 11) & 0x1F) << 3;
    uint8_t g = ((pixel >> 5) & 0x3F) << 2;
    uint8_t b = (pixel & 0x1F) << 3;

    snapshot_buf[i * 3 + 0] = r;
    snapshot_buf[i * 3 + 1] = g;
    snapshot_buf[i * 3 + 2] = b;
  }

  esp_camera_fb_return(fb);
  return true;
}

void send_robot_command(char cmd) {
  RobotSerial.write(cmd);

  Serial.print("ROBOT CMD: ");
  Serial.println(cmd);
}

void setup() {
  Serial.begin(115200);
  delay(3000);

  RobotSerial.begin(115200, SERIAL_8N1, WROOM_RX, WROOM_TX);

  Serial.println("GREEN BALLOON FOMO NO WIFI FINAL");

  Serial.print("Input Width: ");
  Serial.println(EI_CLASSIFIER_INPUT_WIDTH);

  Serial.print("Input Height: ");
  Serial.println(EI_CLASSIFIER_INPUT_HEIGHT);

  Serial.print("Arena Size: ");
  Serial.println(EI_CLASSIFIER_TFLITE_LARGEST_ARENA_SIZE);

  if (!psramFound()) {
    Serial.println("PSRAM NOT FOUND!");
    while (1) delay(1000);
  }

  snapshot_buf = (uint8_t *)ps_malloc(
    EI_CLASSIFIER_INPUT_WIDTH *
    EI_CLASSIFIER_INPUT_HEIGHT *
    3
  );

  if (!snapshot_buf) {
    Serial.println("snapshot_buf allocation failed");
    while (1) delay(1000);
  }

  if (!init_camera()) {
    while (1) delay(1000);
  }

  Serial.println("SETUP OK");
}

void loop() {
  if (!capture_to_rgb888()) {
    send_robot_command('S');
    delay(80);
    return;
  }

  signal_t signal;
  signal.total_length =
    EI_CLASSIFIER_INPUT_WIDTH *
    EI_CLASSIFIER_INPUT_HEIGHT;

  signal.get_data = &ei_camera_get_data;

  ei_impulse_result_t result = {0};

  EI_IMPULSE_ERROR err = run_classifier(&signal, &result, false);

  if (err != EI_IMPULSE_OK) {
    Serial.print("run_classifier failed: ");
    Serial.println((int)err);

    send_robot_command('S');
    delay(80);
    return;
  }

  bool found = false;
  int best_x = 0;
  int best_w = 0;
  float best_score = 0.0;

#if EI_CLASSIFIER_OBJECT_DETECTION == 1
  for (uint32_t i = 0; i < result.bounding_boxes_count; i++) {
    ei_impulse_result_bounding_box_t bb = result.bounding_boxes[i];

    if (bb.value == 0) continue;

    Serial.print("OBJ: ");
    Serial.print(bb.label);
    Serial.print(" score=");
    Serial.print(bb.value);
    Serial.print(" x=");
    Serial.print(bb.x);
    Serial.print(" y=");
    Serial.print(bb.y);
    Serial.print(" w=");
    Serial.print(bb.width);
    Serial.print(" h=");
    Serial.println(bb.height);

    if (bb.value > best_score) {
      best_score = bb.value;
      best_x = bb.x;
      best_w = bb.width;
      found = true;
    }
  }
#endif

  if (!found || best_score < SCORE_LIMIT) {
    if (millis() - lastSeenTime < LOST_TIMEOUT) {
      send_robot_command(lastCmd);
    } else {
      send_robot_command('S');
      lastCmd = 'S';
    }

    delay(80);
    return;
  }

  lastSeenTime = millis();

  int center_x = best_x + best_w / 2;
  char cmd;

  if (center_x < 35) {
    cmd = 'L';
  }
  else if (center_x > 61) {
    cmd = 'R';
  }
  else {
    cmd = 'F';
  }

  lastCmd = cmd;
  send_robot_command(cmd);

  delay(80);
}
}
