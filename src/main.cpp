/**
 *  NimBLE_Server Demo:
 *
 *  Demonstrates many of the available features of the NimBLE server library.
 *
 *  Created: on March 22 2020
 *      Author: H2zero
 */


#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978



#include <Arduino.h>
#include <NimBLEDevice.h>
#include <Stepper.h>

const int stepsPerRevolution = 100;  // change this to fit the number of steps per revolution
int direcao = 0;
Stepper myStepper(stepsPerRevolution, 5, 18, 19, 21);

int melody[80];
int noteDurations[80];


static NimBLEServer* pServer;

/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        Serial.printf("Client address: %s\n", connInfo.getAddress().toString().c_str());

        /**
         *  We can use the connection handle here to ask for different connection parameters.
         *  Args: connection handle, min connection interval, max connection interval
         *  latency, supervision timeout.
         *  Units; Min/Max Intervals: 1.25 millisecond increments.
         *  Latency: number of intervals allowed to skip.
         *  Timeout: 10 millisecond increments.
         */
        pServer->updateConnParams(connInfo.getConnHandle(), 24, 48, 0, 180);
    }

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        Serial.printf("Client disconnected - start advertising\n");
        NimBLEDevice::startAdvertising();
    }

    void onMTUChange(uint16_t MTU, NimBLEConnInfo& connInfo) override {
        Serial.printf("MTU updated: %u for connection ID: %u\n", MTU, connInfo.getConnHandle());
    }

    /********************* Security handled here *********************/
    uint32_t onPassKeyDisplay() override {
        Serial.printf("Server Passkey Display\n");
        /**
         * This should return a random 6 digit number for security
         *  or make your own static passkey as done here.
         */
        return 123456;
    }

    void onConfirmPassKey(NimBLEConnInfo& connInfo, uint32_t pass_key) override {
        Serial.printf("The passkey YES/NO number: %" PRIu32 "\n", pass_key);
        /** Inject false if passkeys don't match. */
        NimBLEDevice::injectConfirmPasskey(connInfo, true);
    }

    void onAuthenticationComplete(NimBLEConnInfo& connInfo) override {
        /** Check that encryption was successful, if not we disconnect the client */
        if (!connInfo.isEncrypted()) {
            NimBLEDevice::getServer()->disconnect(connInfo.getConnHandle());
            Serial.printf("Encrypt connection failed - disconnecting client\n");
            return;
        }

        Serial.printf("Secured connection to: %s\n", connInfo.getAddress().toString().c_str());
    }
} serverCallbacks;

/** Handler class for characteristic actions */
class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        Serial.printf("%s : onRead(), value: %s\n",
                      pCharacteristic->getUUID().toString().c_str(),
                      pCharacteristic->getValue().c_str());           
    }

    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        Serial.printf("%s : onWrite(), value: %s\n",
                      pCharacteristic->getUUID().toString().c_str(),
                      pCharacteristic->getValue().c_str());


        if (strcmp(pCharacteristic->getValue().c_str(), "sentido1") == 0) direcao = 1;
        else if (strcmp(pCharacteristic->getValue().c_str(), "sentido2") == 0) direcao = -1;
        else if (strcmp(pCharacteristic->getValue().c_str(), "sentido0") == 0) direcao = 0;
        //MARIO
        else if (strcmp(pCharacteristic->getValue().c_str(), "musica2") == 0) {
            melody[0] = NOTE_E7;  melody[1] = NOTE_E7;  melody[2] = 0;        melody[3] = NOTE_E7;  melody[4] = 0;         melody[5] = NOTE_C7;   melody[6] = NOTE_E7;  melody[7] = 0;
            melody[8] = NOTE_G7;  melody[9] = 0;        melody[10] = 0;       melody[11] = 0;       melody[12] = NOTE_G6;  melody[13] = 0;        melody[14] = 0;       melody[15] = 0;
            melody[16] = NOTE_C7; melody[17] = 0;       melody[18] = 0;       melody[19] = NOTE_G6; melody[20] = 0;        melody[21] = 0;        melody[22] = NOTE_E6; melody[23] = 0;
            melody[24] = 0;       melody[25] = NOTE_A6; melody[26] = 0;       melody[27] = NOTE_B6; melody[28] = 0;        melody[29] = NOTE_AS6; melody[30] = NOTE_A6; melody[31] = 0;
            melody[32] = NOTE_G6; melody[33] = NOTE_E7; melody[34] = NOTE_G7; melody[35] = NOTE_A7; melody[36] = 0;        melody[37] = NOTE_F7;  melody[38] = NOTE_G7; melody[39] = 0;
            melody[40] = NOTE_E7; melody[41] = 0;       melody[42] = NOTE_C7; melody[43] = NOTE_D7; melody[44] = NOTE_B6;  melody[45] = 0;        melody[46] = 0;       //melody[47] = ;
            melody[47] = NOTE_C7; melody[48] = 0;       melody[49] = 0;       melody[50] = NOTE_G6; melody[51] = 0;        melody[52] = 0;        melody[53] = NOTE_E6; melody[54] = 0;
            melody[55] = 0;       melody[56] = NOTE_A6; melody[57] = 0;       melody[58] = NOTE_B6; melody[59] = 0;        melody[60] = NOTE_AS6; melody[61] = NOTE_A6; melody[62] = 0;
            melody[63] = NOTE_G6; melody[64] = NOTE_E7; melody[65] = NOTE_G7; melody[66] = NOTE_A7; melody[67] = 0;        melody[68] = NOTE_F7;  melody[69] = NOTE_G7; melody[70] = 0;
            melody[71] = NOTE_E7; melody[72] = 0;       melody[73] = NOTE_C7; melody[74] = NOTE_D7; melody[75] = NOTE_B6;  melody[76] = 0;        melody[77] = 0;       //melody[79] = ;

            noteDurations[0]=8; noteDurations[1]=8; noteDurations[2]=8; noteDurations[3]=8; noteDurations[4]=8; noteDurations[5]=8; noteDurations[6]=8; noteDurations[7]=8;
            noteDurations[8]=4; noteDurations[9]=8; noteDurations[10]=8; noteDurations[11]=8; noteDurations[12]=4; noteDurations[13]=8; noteDurations[14]=8; noteDurations[15]=8;
            noteDurations[16]=4; noteDurations[17]=8; noteDurations[18]=8; noteDurations[19]=8; noteDurations[20]=4; noteDurations[21]=8; noteDurations[22]=8; noteDurations[23]=8;
            noteDurations[24]=4; noteDurations[25]=8; noteDurations[26]=8; noteDurations[27]=8; noteDurations[28]=8; noteDurations[29]=8; noteDurations[30]=8; noteDurations[31]=8;
            noteDurations[32]=4; noteDurations[33]=8; noteDurations[34]=8; noteDurations[35]=4; noteDurations[36]=8; noteDurations[37]=8; noteDurations[38]=8; noteDurations[39]=8;
            noteDurations[40]=4; noteDurations[41]=8; noteDurations[42]=8; noteDurations[43]=8; noteDurations[44]=8; noteDurations[45]=8; noteDurations[46]=8;
            noteDurations[47]=4; noteDurations[48]=8; noteDurations[49]=8; noteDurations[50]=8; noteDurations[51]=4; noteDurations[52]=8; noteDurations[53]=8; noteDurations[54]=8;
            noteDurations[55]=4; noteDurations[56]=8; noteDurations[57]=8; noteDurations[58]=8; noteDurations[59]=8; noteDurations[60]=8; noteDurations[61]=8; noteDurations[62]=8;
            noteDurations[63]=4; noteDurations[64]=8; noteDurations[65]=8; noteDurations[66]=4; noteDurations[67]=8; noteDurations[68]=8; noteDurations[69]=8; noteDurations[70]=8;
            noteDurations[71]=4; noteDurations[72]=8; noteDurations[73]=8; noteDurations[74]=8; noteDurations[75]=8; noteDurations[76]=8; noteDurations[77]=8;

            //for (int i = 0; i < sizeof(melody)/sizeof(int); i++) {
            for (int i = 0; i < 78; i++) {
                int noteDuration = 1000 / noteDurations[i];
                tone(12, melody[i], noteDuration);

                int pauseBetweenNotes = noteDuration * 1.30;
                delay(pauseBetweenNotes);
                noTone(12);
            }

        }
        
        //STAR WARS
        else if (strcmp(pCharacteristic->getValue().c_str(), "musica3") == 0) {
            melody[0] = NOTE_A4;  melody[1] = NOTE_A4;  melody[2] = NOTE_F4;        melody[3] = NOTE_C5;  melody[4] = NOTE_A4;         melody[5] = NOTE_F4;   melody[6] = NOTE_C5;  melody[7] = NOTE_A4;
            melody[8] = NOTE_E5;  melody[9] = NOTE_E5;        melody[10] = NOTE_E5;       melody[11] = NOTE_F5;       melody[12] = NOTE_C5;  melody[13] = NOTE_GS4;        melody[14] = NOTE_F4;       melody[15] = NOTE_C5; melody[16] = NOTE_A4;
            melody[17] = NOTE_A5; melody[18] = NOTE_A4;       melody[19] = NOTE_A4;       melody[20] = NOTE_A5; melody[21] = NOTE_GS5;        melody[22] = NOTE_G5;        melody[23] = NOTE_FS5; melody[24] = NOTE_F5; melody[25] = NOTE_FS5;
            melody[26] = 0;       melody[27] = NOTE_AS4; melody[28] = NOTE_DS5;       melody[29] = NOTE_D5; melody[30] = NOTE_CS5;        melody[31] = NOTE_C5; melody[32] = NOTE_B4; melody[33] = NOTE_C5;
            
            noteDurations[0]=500; noteDurations[1]=500; noteDurations[2]=500; noteDurations[3]=350; noteDurations[4]=150; noteDurations[5]=500; noteDurations[6]=350; noteDurations[7]=150;
            noteDurations[8]=500; noteDurations[9]=500; noteDurations[10]=500; noteDurations[11]=350; noteDurations[12]=150; noteDurations[13]=500; noteDurations[14]=350; noteDurations[15]=150; noteDurations[16]=1000;
            noteDurations[17]=500; noteDurations[18]=350; noteDurations[19]=150; noteDurations[20]=500; noteDurations[21]=250; noteDurations[22]=250; noteDurations[23]=125; noteDurations[24]=125; noteDurations[25]=250;
            noteDurations[26]=250; noteDurations[27]=500; noteDurations[28]=250; noteDurations[29]=250; noteDurations[30]=125; noteDurations[31]=125; noteDurations[32]=250; noteDurations[33]=250;
        
            for (int i = 0; i < 34; i++) {
                int noteDuration = noteDurations[i];
                if (melody[i] == 0) {
                    delay(noteDuration); // pause for rest
                } else {
                    tone(12, melody[i], noteDuration);
                    delay(noteDuration * 1.3);
                    noTone(12);
                }
            }
        }


        else if (strcmp(pCharacteristic->getValue().c_str(), "musica1") == 0) {

            melody[0] = NOTE_E4; melody[1] = NOTE_E4; melody[2] = NOTE_F4; melody[3] = NOTE_G4; 
            melody[4] = NOTE_G4; melody[5] = NOTE_F4; melody[6] = NOTE_E4; melody[7] = NOTE_D4; 
            melody[8] = NOTE_C4; melody[9] = NOTE_C4; melody[10] = NOTE_D4; melody[11] = NOTE_E4; 
            melody[12] = NOTE_E4; melody[13] = NOTE_D4; melody[14] = NOTE_D4; 
            melody[15] = NOTE_E4; melody[16] = NOTE_E4; melody[17] = NOTE_F4; melody[18] = NOTE_G4; 
            melody[19] = NOTE_G4; melody[20] = NOTE_F4; melody[21] = NOTE_E4; melody[22] = NOTE_D4; 
            melody[23] = NOTE_C4; melody[24] = NOTE_C4; melody[25] = NOTE_D4; melody[26] = NOTE_E4; 
            melody[27] = NOTE_D4; melody[28] = NOTE_C4; melody[29] = NOTE_C4; 

            noteDurations[0] = 4; noteDurations[1] = 4; noteDurations[2] = 4; noteDurations[3] = 4; 
            noteDurations[4] = 4; noteDurations[5] = 4; noteDurations[6] = 4; noteDurations[7] = 4; 
            noteDurations[8] = 4; noteDurations[9] = 4; noteDurations[10] = 4; noteDurations[11] = 4; 
            noteDurations[12] = 4; noteDurations[13] = 4; noteDurations[14] = 2; 
            noteDurations[15] = 4; noteDurations[16] = 4; noteDurations[17] = 4; noteDurations[18] = 4; 
            noteDurations[19] = 4; noteDurations[20] = 4; noteDurations[21] = 4; noteDurations[22] = 4; 
            noteDurations[23] = 4; noteDurations[24] = 4; noteDurations[25] = 4; noteDurations[26] = 4; 
            noteDurations[27] = 4; noteDurations[28] = 4; noteDurations[29] = 2; 

            for (int i = 0; i < 30; i++) {
                int noteDuration = 1000 / noteDurations[i];
                tone(12, melody[i], noteDuration);

                // Pause between notes for clarity
                int pauseBetweenNotes = noteDuration * 1.3;
                delay(pauseBetweenNotes);

                noTone(12);
            }

        }

    }

    /**
     *  The value returned in code is the NimBLE host return code.
     */
    void onStatus(NimBLECharacteristic* pCharacteristic, int code) override {
        Serial.printf("Notification/Indication return code: %d, %s\n", code, NimBLEUtils::returnCodeToString(code));
    }

    /** Peer subscribed to notifications/indications */
    void onSubscribe(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo, uint16_t subValue) override {
        std::string str  = "Client ID: ";
        str             += connInfo.getConnHandle();
        str             += " Address: ";
        str             += connInfo.getAddress().toString();
        if (subValue == 0) {
            str += " Unsubscribed to ";
        } else if (subValue == 1) {
            str += " Subscribed to notifications for ";
        } else if (subValue == 2) {
            str += " Subscribed to indications for ";
        } else if (subValue == 3) {
            str += " Subscribed to notifications and indications for ";
        }
        str += std::string(pCharacteristic->getUUID());

        Serial.printf("%s\n", str.c_str());
    }
} chrCallbacks;

/** Handler class for descriptor actions */
class DescriptorCallbacks : public NimBLEDescriptorCallbacks {
    void onWrite(NimBLEDescriptor* pDescriptor, NimBLEConnInfo& connInfo) override {
        std::string dscVal = pDescriptor->getValue();
        Serial.printf("Descriptor written value: %s\n", dscVal.c_str());
    }

    void onRead(NimBLEDescriptor* pDescriptor, NimBLEConnInfo& connInfo) override {
        Serial.printf("%s Descriptor read\n", pDescriptor->getUUID().toString().c_str());
    }
} dscCallbacks;

void setup(void) {

    //INICIALIZAR LEDC PRA NAO DAR PROBLEMA COM TONE
    int channel = 0;           // LEDC channel (0-7)
    int freq = 5000;           // PWM frequency in Hz
    int resolution = 8;        // PWM resolution in bits (1-13)
    int gpio = 12;             // Output GPIO pin
    ledcSetup(channel, freq, resolution);  // ledcSetup(uint8_t channel, double freq, uint8_t resolution_bits)
    ledcAttachPin(gpio, channel);         // ledcAttachPin(uint8_t pin, uint8_t channel)


    myStepper.setSpeed(60);

    Serial.begin(115200);
    Serial.printf("Starting NimBLE Server\n");

    /** Initialize NimBLE and set the device name */
    NimBLEDevice::init("NimBLE");

    /**
     * Set the IO capabilities of the device, each option will trigger a different pairing method.
     *  BLE_HS_IO_DISPLAY_ONLY    - Passkey pairing
     *  BLE_HS_IO_DISPLAY_YESNO   - Numeric comparison pairing
     *  BLE_HS_IO_NO_INPUT_OUTPUT - DEFAULT setting - just works pairing
     */
    // NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY); // use passkey
    // NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_YESNO); //use numeric comparison

    /**
     *  2 different ways to set security - both calls achieve the same result.
     *  no bonding, no man in the middle protection, BLE secure connections.
     *
     *  These are the default values, only shown here for demonstration.
     */
    // NimBLEDevice::setSecurityAuth(false, false, true);

    NimBLEDevice::setSecurityAuth(/*BLE_SM_PAIR_AUTHREQ_BOND | BLE_SM_PAIR_AUTHREQ_MITM |*/ BLE_SM_PAIR_AUTHREQ_SC);
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(&serverCallbacks);

    NimBLEService*        pDeadService = pServer->createService("DEAD");
    NimBLECharacteristic* pBeefCharacteristic =
        pDeadService->createCharacteristic("BEEF",
                                           NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE |
                                               /** Require a secure connection for read and write access */
                                               NIMBLE_PROPERTY::READ_ENC | // only allow reading if paired / encrypted
                                               NIMBLE_PROPERTY::WRITE_ENC  // only allow writing if paired / encrypted
        );

    pBeefCharacteristic->setValue("Burger");
    pBeefCharacteristic->setCallbacks(&chrCallbacks);

    /**
     *  2902 and 2904 descriptors are a special case, when createDescriptor is called with
     *  either of those uuid's it will create the associated class with the correct properties
     *  and sizes. However we must cast the returned reference to the correct type as the method
     *  only returns a pointer to the base NimBLEDescriptor class.
     */
    NimBLE2904* pBeef2904 = pBeefCharacteristic->create2904();
    pBeef2904->setFormat(NimBLE2904::FORMAT_UTF8);
    pBeef2904->setCallbacks(&dscCallbacks);

    NimBLEService*        pBaadService = pServer->createService("BAAD");
    NimBLECharacteristic* pFoodCharacteristic =
        pBaadService->createCharacteristic("F00D", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);

    pFoodCharacteristic->setValue("Fries");
    pFoodCharacteristic->setCallbacks(&chrCallbacks);

    /** Custom descriptor: Arguments are UUID, Properties, max length of the value in bytes */
    NimBLEDescriptor* pC01Ddsc =
        pFoodCharacteristic->createDescriptor("C01D",
                                              NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_ENC,
                                              20);
    pC01Ddsc->setValue("Send it back!");
    pC01Ddsc->setCallbacks(&dscCallbacks);

    /** Start the services when finished creating all Characteristics and Descriptors */
    pDeadService->start();
    pBaadService->start();

    /** Create an advertising instance and add the services to the advertised data */
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setName("NimBLE-Server");
    pAdvertising->addServiceUUID(pDeadService->getUUID());
    pAdvertising->addServiceUUID(pBaadService->getUUID());
    /**
     *  If your device is battery powered you may consider setting scan response
     *  to false as it will extend battery life at the expense of less data sent.
     */
    pAdvertising->enableScanResponse(true);
    pAdvertising->start();

    Serial.printf("Advertising Started\n");

}

void loop() {
    /** Loop here and send notifications to connected peers */
    /*
    delay(2000);
    if (pServer->getConnectedCount()) {
        NimBLEService* pSvc = pServer->getServiceByUUID("BAAD");
        if (pSvc) {
            NimBLECharacteristic* pChr = pSvc->getCharacteristic("F00D");
            if (pChr) {
                pChr->notify();
            }
        }
    }
    */
/*
    // step one revolution  in one direction:
    Serial.println("clockwise");
    myStepper.step(stepsPerRevolution);
    //delay(500);

    // step one revolution in the other direction:
    Serial.println("counterclockwise");
    myStepper.step(-stepsPerRevolution);
    //delay(500);
*/



    myStepper.step(direcao);
}