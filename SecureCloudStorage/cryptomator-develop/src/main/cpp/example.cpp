/**
 * @authors Alekos Interrante Bonadia
 * @authors Lorenzo Sebastiano Mathis
 * @authors Pietro Mazza
 * @version 1.0
 */

#include <iostream>
#include "SECubeConfiguration.h"
#include "SECubeAPI.h"
#include "SECubeUtils.h"

using namespace std;

int main() {
    // Listing and selecting the device
    vector<pair<string, string>> devices;
    SECubeDeviceList(devices, true);

    // Trying to read the database
    SECubeDBDebugRead();

    /// Testing the APIs

    vector<pair<string, string>> devs = EnumerateSECubeDevices();
    for (auto &dev: devs) {
        cout << "Device: " << dev.first << " - " << dev.second << endl;
    }

    string vaultID = "vault1";
    string serial = devs[0].second;
    string pin = "user1234";
    uint8_t password[32] = {0};

    try {
        GenerateSecurePassword(

                vaultID,            // vault ID, it will be used to store the password in the database
                serial,             // serial number of the SEcube device
                pin,                // pin of the SEcube device
                password,           // buffer where the password will be stored
                32             // size of the buffer
        );

        cout << "Generated password" << endl;
        cout << "Raw: ";
        for (unsigned char i : password) {
            cout << i;
        }
        cout << endl;

        string hexPass = uint8ToHex(password, 32);
        cout << "Hex: " << hexPass << endl;

        uint8_t password_r[32] = {0};
        RetrieveSecurePassword(
                vaultID,            // vault ID, it will be used to retrieve the password from the database
                serial,             // serial number of the SEcube device
                pin,                // pin of the SEcube device
                password_r,// buffer where the password will be stored
                32             // size of the buffer
        );

        cout << "\n\n-----------\n" << endl;
        cout << "Retrieved password" << endl;
        cout << "Raw: ";
        for (unsigned char i : password) {
            cout << i;
        }
        cout << endl;

        string hexPass_r = uint8ToHex(password, 32);
        cout << "Hex: " << hexPass_r << endl;

    }
    catch (SECubeException &e) {
        cout << e.what() << endl;
    }
}