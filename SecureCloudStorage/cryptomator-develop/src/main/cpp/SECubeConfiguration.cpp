/**
 * @authors Alekos Interrante Bonadia
 * @authors Lorenzo Sebastiano Mathis
 * @authors Pietro Mazza
 * @version 1.0
 */

#include "SECubeConfiguration.h"
#include "sources/L1/L1.h"
#include "configuration.h"
#include "sefile/environment.h"
#include "sefile/SEfile.h"
#include "sqlite/sqlite3.h"
#include <iostream>
#include <thread>

unique_ptr<L0> l0_c = make_unique<L0>();
unique_ptr<L1> l1_c = make_unique<L1>();

int DBPrintCallback(void *NotUsed, int argc, char **argv, char **azColName) {
    for(int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}
int DBVaultDumpCallback(void *NotUsed, int argc, char **argv, char **azColName) {
    cout << "Column count: " << argc << endl;
    cout << "Columns: [";
    for (int i = 0; i < argc; i++) {
        cout << azColName[i];
        if (i < argc - 1) {
            cout << ", ";
        }
    }
    cout << "]" << endl;

    if (argc >= 2) {
        cout << "Vault ID: " << argv[0] << endl;
        cout << "Key: [" << strlen(argv[1]) << " bytes]" << endl;
        for (int i = 0; i < strlen(argv[1]); i++) {
            cout << "*";
        }
    }

    return 0;
}

ERROR_CODE SECubeDeviceList(vector<pair<string, string>> &devices, bool select) {
    cout << "Searching for SECube devices..." << endl;

    int dev_n = l0_c->GetNumberDevices();
    if (dev_n == 0) {
        cout << "No SECube devices found." << endl;
        return ERROR_CODE::NO_DEVICE_FOUND;
    }

    cout << "Found " << dev_n << " SECube devices." << endl;

    int r = l0_c->GetDeviceList(devices);
    if (r != 0) {
        cout << "Error getting device list." << endl;
        return ERROR_CODE::COMM_ERROR;
    }

    cout << "Found following devices:" << endl;
    int i = 0;
    for (auto &dev: devices) {
        if (dev.second.empty()) {
            cout << i << ". " << dev.first << " <<No name set>>" << endl;
        } else {
            cout << i << ". " << dev.first << " (" << dev.second << ")" << endl;
        }
    }

    if (select) {
        cout << "Please select the device to setup: ";
        cin >> i;

        if (i < 0 || i >= dev_n) {
            cout << "Invalid device number." << endl;
            return ERROR_CODE::INVALID_DEVICE;
        }

        try {
            l1_c->L1SelectSEcube(i);
        }
        catch (exception &e) {
            cout << "Error selecting device: " << e.what() << endl;
            return ERROR_CODE::COMM_ERROR;
        }

        cout << "Selected device: " << devices[i].first << endl;
    }

    return ERROR_CODE::NONE;
}

ERROR_CODE SECubeFactoryInitRoutine() {
    cout << "Setting up device..." << endl;

    try {
        cout << "Please enter the serial number of the device (max " << L0Communication::Size::SERIAL
             << " characters): ";
        string serial;
        cin >> serial;

        if (serial.size() > L0Communication::Size::SERIAL) {
            cout << "Serial number too long." << endl;
            return ERROR_CODE::MALFORMED_INPUT;
        }

        array<uint8_t, L0Communication::Size::SERIAL> sn = {0};
        copy(
                serial.begin(),
                serial.begin() + min(serial.size(), sn.size()),
                sn.begin()
        );


        l1_c->L1FactoryInit(sn);
    } catch (DeviceAlreadyInitializedException &e) {
        cout << "The device is already initialized." << endl;
        cout << "Please flash again the firmware on the SECube device with the mass erase option." << endl;
        cout
                << "In STM32CubeIDE: Run -> Run Configurations, select the configuration -> Startup -> Put in the 'Initialization Commands' box the following command: 'monitor flash mass_erase'."
                << endl;
        return ERROR_CODE::DEVICE_ALREADY_INITIALIZED;
    } catch (exception &e) {
        cout << "Error setting up device: " << e.what() << endl;
        return ERROR_CODE::DEVICE_ERROR;
    }

    array<uint8_t, L1Parameters::Size::PIN> default_pin = {0};
    try {
        cout << "Trying to login to the device after the initialization..." << endl;
        l1_c->L1Login(default_pin, SE3_ACCESS_ADMIN, true);
    }
    catch (exception &e) {
        cout << "Error logging in to the device: " << e.what() << endl;
        return ERROR_CODE::LOGIN_ERROR;
    }

    if (!l1_c->L1GetSessionLoggedIn()) {
        cout << "Error logging in to the device." << endl;
        return ERROR_CODE::LOGIN_ERROR;
    }

    cout << "Please enter the admin PIN: (max " << L1Parameters::Size::PIN << " characters): ";
    string pin;
    cin >> pin;

    if (pin.size() > L1Parameters::Size::PIN) {
        cout << "PIN too long." << endl;
        return ERROR_CODE::MALFORMED_INPUT;
    }

    array<uint8_t, L1Parameters::Size::PIN> admin_pin = {0};
    copy(
            pin.begin(),
            pin.begin() + min(pin.size(), admin_pin.size()),
            admin_pin.begin()
    );

    try {
        l1_c->L1SetAdminPIN(admin_pin);
        cout << "Admin PIN set successfully." << endl;
    } catch (exception &e) {
        cout << "Error setting admin PIN: " << e.what() << endl;
        return ERROR_CODE::DEVICE_ERROR;
    }

    cout << "Please enter the user PIN: (max " << L1Parameters::Size::PIN << " characters): ";
    cin >> pin;

    if (pin.size() > L1Parameters::Size::PIN) {
        cout << "PIN too long." << endl;
        return ERROR_CODE::MALFORMED_INPUT;
    }

    array<uint8_t, L1Parameters::Size::PIN> user_pin = {0};
    copy(
            pin.begin(),
            pin.begin() + min(pin.size(), user_pin.size()),
            user_pin.begin()
    );

    try {
        l1_c->L1SetUserPIN(user_pin);
        cout << "User PIN set successfully." << endl;
    } catch (exception &e) {
        cout << "Error setting user PIN: " << e.what() << endl;
        return ERROR_CODE::DEVICE_ERROR;
    }

    cout << "Trying to login with the new PINs..." << endl;

    try {
        l1_c->L1Logout();
        l1_c->L1Login(admin_pin, SE3_ACCESS_ADMIN, true);
        cout << "Logged in with the admin PIN." << endl;
    } catch (exception &e) {
        cout << "Error logging in to the device with the admin PIN: " << e.what() << endl;
        return ERROR_CODE::LOGIN_ERROR;
    }

    try {
        l1_c->L1Logout();
        l1_c->L1Login(user_pin, SE3_ACCESS_USER, true);
        l1_c->L1Logout();
        cout << "Logged in with the user PIN." << endl;
    } catch (exception &e) {
        cout << "Error logging in to the device with the user PIN: " << e.what() << endl;
        return ERROR_CODE::LOGIN_ERROR;
    }

    cout << "Device initialization completed successfully." << endl;
    return ERROR_CODE::NONE;
}

ERROR_CODE SECubeDBKeyInitRoutine() {
    cout << "Setting up Cryptomator..." << endl;

    array<uint8_t, L1Parameters::Size::PIN> admin_pin = {0};
    cout << "Please enter the admin PIN: ";
    string pin;
    cin >> pin;

    if (pin.size() > L1Parameters::Size::PIN) {
        cout << "PIN too long." << endl;
        return ERROR_CODE::MALFORMED_INPUT;
    }

    copy(
            pin.begin(),
            pin.begin() + min(pin.size(), admin_pin.size()),
            admin_pin.begin()
    );

    try {
        l1_c->L1Login(admin_pin, SE3_ACCESS_ADMIN, true);

        bool already_exists = false;
        l1_c->L1FindKey(DB_KEY_ID, already_exists);

        if (already_exists) {
            cout << "Cryptomator key already exists. Do you want to regenerate the key? (y/n)" << endl;

            char c;
            cin >> c;
            if (c != 'y') {
                cout << "Cryptomator key not regenerated." << endl;
                l1_c->L1Logout();
                return ERROR_CODE::NONE;
            } else {
                cout << "Deleting old Cryptomator key..." << endl;
                se3Key key;
                key.id = DB_KEY_ID;
                key.dataSize = 0;
                key.data = nullptr;

                try {
                    l1_c->L1KeyEdit(key, L1Commands::KeyOpEdit::SE3_KEY_OP_DELETE);
                    cout << "Cryptomator key deleted successfully." << endl;
                }
                catch (exception &e) {
                    cout << "Error deleting Cryptomator key: " << e.what() << endl;
                    return ERROR_CODE::KEY_ERROR;
                }
            }
        }

        se3Key key;
        key.id = DB_KEY_ID;
        key.dataSize = 32;  // AES-256 key size

        try {
            l1_c->L1KeyEdit(key,
                            L1Commands::KeyOpEdit::SE3_KEY_OP_ADD_TRNG); // the key is generated by the TRNG and stored in the device
            cout << "Cryptomator key added successfully." << endl;
        }
        catch (exception &e) {
            cout << "Error adding Cryptomator key: " << e.what() << endl;
            return ERROR_CODE::KEY_ERROR;
        }

        l1_c->L1Logout();
    }
    catch (exception &e) {
        cout << "Error logging in to the device: " << e.what() << endl;
        return ERROR_CODE::LOGIN_ERROR;
    }

    return ERROR_CODE::NONE;
}

ERROR_CODE SECubeDBInitRoutine() {
    cout << "Setting up Cryptomator..." << endl;

    array<uint8_t, L1Parameters::Size::PIN> admin_pin = {0};
    cout << "Please enter the user PIN: ";
    string pin;
    cin >> pin;

    if (pin.size() > L1Parameters::Size::PIN) {
        cout << "PIN too long." << endl;
        return ERROR_CODE::MALFORMED_INPUT;
    }

    copy(
            pin.begin(),
            pin.begin() + min(pin.size(), admin_pin.size()),
            admin_pin.begin()
    );

    try {
        l1_c->L1Login(admin_pin, SE3_ACCESS_USER, true);
        SEcube = l1_c.get();

        vector<pair<basic_string<char>, basic_string<char>>> items;
        basic_string<char> path = l0_c->GetDevicePath();
        secure_ls(path, items, l1_c.get());

        for (auto &item: items) {
            if (item.second == DB_NAME) {
                cout << "Database already exists. Do you want to recreate it from scratch? (y/n)" << endl;

                char c;
                cin >> c;
                if (c != 'y') {
                    cout << "Database not recreated." << endl;
                    l1_c->L1Logout();
                    return ERROR_CODE::NONE;
                } else {
                    cout << "Deleting old Database..." << endl;
                    string full_name = path + "/" + item.first;
                    remove(full_name.c_str());
                    cout << "Database deleted successfully." << endl;
                }
            }
        }

        sqlite3 *db;
        unique_ptr<SEfile> sefile_db = make_unique<SEfile>();
        sefile_db->secure_init(l1_c.get(), DB_KEY_ID, L1Algorithms::Algorithms::AES_HMACSHA256);

        memcpy(sefile_db->handleptr->name, DB_NAME, strlen(DB_NAME));
        databases.push_back(std::move(sefile_db));

        string db_path = l0_c->GetDevicePath();
        db_path += "/";
        db_path += DB_NAME;
        int r = sqlite3_open(db_path.c_str(), &db);
        if (r != SQLITE_OK) {
            cout << "Error opening database: " << sqlite3_errmsg(db) << endl;
            l1_c->L1Logout();
            return ERROR_CODE::DATABASE_ERROR;
        }
        this_thread::sleep_for(chrono::milliseconds(1000));
        cout << "Database created and opened successfully." << endl;

        const char *sql = "CREATE TABLE IF NOT EXISTS vaults (vault_id TEXT PRIMARY KEY, key BLOB NOT NULL);";
        r = sqlite3_exec(db, sql, NULL, 0, NULL);
        if (r != SQLITE_OK) {
            cout << "Error creating table: " << sqlite3_errmsg(db) << endl;
            l1_c->L1Logout();
            return ERROR_CODE::DATABASE_ERROR;
        }

        r = sqlite3_exec(db, "SELECT * FROM sqlite_master WHERE type='table';", DBPrintCallback, 0, 0);

        cout << "Table created successfully." << endl;
        sqlite3_close(db);

        cout << "Cryptomator setup completed successfully." << endl;
    }
    catch (exception &e) {
        cout << "Error logging in to the device: " << e.what() << endl;
        return ERROR_CODE::LOGIN_ERROR;
    }

    l1_c->L1Logout();
    return ERROR_CODE::NONE;
}

ERROR_CODE SECubeDBDebugRead() {
    array<uint8_t, L1Parameters::Size::PIN> admin_pin = {0};
    cout << "Please enter the user PIN: ";
    string pin;
    cin >> pin;

    if (pin.size() > L1Parameters::Size::PIN) {
        cout << "PIN too long." << endl;
        return ERROR_CODE::MALFORMED_INPUT;
    }

    copy(
            pin.begin(),
            pin.begin() + min(pin.size(), admin_pin.size()),
            admin_pin.begin()
    );

    try {
        l1_c->L1Login(admin_pin, SE3_ACCESS_USER, true);


        SEcube = l1_c.get();

        sqlite3 *db;
        unique_ptr<SEfile> sefile_db = make_unique<SEfile>();
        sefile_db->secure_init(l1_c.get(), DB_KEY_ID, L1Algorithms::Algorithms::AES_HMACSHA256);

        memcpy(sefile_db->handleptr->name, DB_NAME, strlen(DB_NAME));
        databases.push_back(std::move(sefile_db));

        string db_path = l0_c->GetDevicePath();
        db_path += "/";
        db_path += DB_NAME;
        int r = sqlite3_open_v2(db_path.c_str(), &db, SQLITE_OPEN_READONLY, NULL);
        if (r != SQLITE_OK) {
            cout << "Error opening database: " << sqlite3_errmsg(db) << endl;
            l1_c->L1Logout();
            return ERROR_CODE::DATABASE_ERROR;
        }

        r = sqlite3_exec(db, "SELECT * FROM sqlite_master WHERE type='table';", DBPrintCallback, 0, 0);
        if (r != SQLITE_OK) {
            cout << "Error reading database: " << sqlite3_errmsg(db) << endl;
            l1_c->L1Logout();
            return ERROR_CODE::DATABASE_ERROR;
        }

        r = sqlite3_exec(db, "SELECT * FROM vaults", DBVaultDumpCallback, 0, 0);
        if (r != SQLITE_OK) {
            cout << "Error reading database: " << sqlite3_errmsg(db) << endl;
            l1_c->L1Logout();
            return ERROR_CODE::DATABASE_ERROR;
        }

        sqlite3_close_v2(db);
        l1_c->L1Logout();
    }
    catch (exception &e) {
        cout << "Error logging in to the device: " << e.what() << endl;
        return ERROR_CODE::LOGIN_ERROR;
    }

    return ERROR_CODE::NONE;
}