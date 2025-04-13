#include <Wire.h>

void setup() {
    Serial.begin(115200);
    while (!Serial);  // Attendre l'ouverture du port série (utile sur certaines cartes)

    Wire.begin(21, 22);  // Initialiser l'I2C avec SDA=GPIO 21, SCL=GPIO 22

    Serial.println("Scan I2C en cours...");
    byte count = 0;

    for (byte address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
            Serial.print("Périphérique I2C trouvé à l'adresse 0x");
            Serial.println(address, HEX);
            count++;
            delay(5);
        }
    }

    if (count == 0) {
        Serial.println("Aucun périphérique I2C détecté.");
    } else {
        Serial.println("Scan terminé.");
    }
}

void loop() {
    // Ne rien faire après le scan
}
