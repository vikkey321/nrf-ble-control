#include <bluefruit.h>

// BLE Service
BLEUart bleuart; // UART over BLE

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); // Wait for Serial to be ready

  // Initialize Bluefruit
  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Transmission Power
  Bluefruit.setName("Xiao_BLE"); // BLE Device Name
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // Initialize the BLE UART service
  bleuart.begin();
  bleuart.setRxCallback(bleuart_rx_callback);

  // Start Advertising
  startAdv();
}

void loop() {
  // No need to put code here, everything is handled in callbacks
}

void startAdv(void) {
  // Advertising Packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.addService(bleuart);

  // Start Advertising
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244); // Advertising interval in 0.625 ms units
  Bluefruit.Advertising.setFastTimeout(30); // Fast mode timeout in seconds
  Bluefruit.Advertising.start(0); // 0 = Don't stop advertising
}

void connect_callback(uint16_t conn_handle) {
  Serial.println("Connected");
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  Serial.println("Disconnected");
}

void bleuart_rx_callback(uint16_t conn_handle) {
  (void)conn_handle;

  // Read incoming data
  uint8_t packet[20];
  int length = bleuart.read(packet, sizeof(packet));

  // Print received data
  Serial.print("Received: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)packet[i]);
  }
  Serial.println();
}
