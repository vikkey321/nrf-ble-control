#include <bluefruit.h>

// BLE Service
BLEUart bleuart; // UART over BLE
int time_ = 100;
int data_;
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); // Wait for Serial to be ready

  // Initialize Bluefruit
  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Transmission Power
   Bluefruit.setName("Xiao_BLE"); // BLE Device Name
//    Bluefruit.setName("Xiao_BLE_1"); // BLE Device Name

  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // Initialize the BLE UART service
  bleuart.begin();
  bleuart.setRxCallback(bleuart_rx_callback);

  // Start Advertising
  startAdv();
    pinMode(LED_BUILTIN, OUTPUT);

  // pinMode(ledRed, OUTPUT);
  // pinMode(ledGreen, OUTPUT);
  // pinMode(ledBlue, OUTPUT);
}

void loop() {
  // No need to put code here, everything is handled in callbacks
 // Serial.println()
   digitalWrite(LED_BUILTIN, HIGH);   
delay(time_);
   digitalWrite(LED_BUILTIN, LOW);   
   delay(time_);


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
  // Serial.println("Connected");
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  // Serial.println("Disconnected");
}

void bleuart_rx_callback(uint16_t conn_handle) {
  (void)conn_handle;

  // Read incoming data
  uint8_t packet[20];
  int length = bleuart.read(packet, sizeof(packet));

//   Serial.print("Received: ");
//   for (int i = 0; i < length; i++) {
//     Serial.print(packet[i]);
//   }
//Serial.println(length);
//   Serial.println(packet[0]);
String labelNames = "";

switch(packet[0]){
  
  case 49:
    // Thunder
    labelNames += "Thunder";
    break;

  case 50:
    // Aurora
    labelNames += "Aurora";
    break;

  case 51:
    // Sunset
    labelNames += "Sunset";
    break;

  case 52:
    // Bioluminescence
    labelNames += "Bioluminescence";
    break;

  case 53:
    // Fluorescence
    labelNames += "Fluorescence";
    break;

  case 54:
    // Reflection
    labelNames += "Reflection";
    break;

  case 55:
    // Spectra
    labelNames += "Spectra";
    break;

  case 56:
    // Photonic
    labelNames += "Photonic";
    break;

  case 57:
    // Vulcanic
    labelNames += "Vulcanic";
    break;

  case 58:
    // Ocean
    labelNames += "Ocean";
    break;

  case 59:
    // Forest
    labelNames += "Forest";
    break;

  case 60:
    // Sky
    labelNames += "Sky";
    break;

  case 65:
    // A
    labelNames += "ON";
    break;

  case 66:
    // B
    labelNames += "Empty";
    break;

  case 67:
    // C
    labelNames += "Half";
    break;

  case 68:
    // D
    labelNames += "Full";
    break;

  default:
    // Default case
    break;
}

// Print the stored label names
Serial.println(labelNames);



}
