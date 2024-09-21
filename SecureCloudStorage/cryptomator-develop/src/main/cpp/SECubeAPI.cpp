/**
 * @authors Alekos Interrante Bonadia
 * @authors Lorenzo Sebastiano Mathis
 * @authors Pietro Mazza
 * @version 1.0
 */
#include <locale>
#include <codecvt>
#include "SECubeAPI.h"

#include "configuration.h"
#include "SECubeUtils.h"
#include "sources/L0/L0.h"
#include "sources/L1/L1.h"
#include "sqlite/sqlite3.h"
#include "sefile/SEfile.h"
#include "sefile/environment.h"

#define SQL_KEY_SELECT "SELECT key FROM vaults WHERE vault_id = ?"
#define SQL_KEY_INSERT "INSERT INTO vaults (vault_id, key) VALUES (?, ?)"

/**
 * @brief Login to the SEcube device
 * @param [string] serial : serial number of the SEcube device
 * @param [string] pin : user pin of the SEcube device
 * @return [void]
 *
 * @throws SECubeException if L1 object is null or parameters are malformed
 * @throws SECubeLoginException on login failure
 */
void SECubeLogin(unique_ptr<L1>& l1, const string &serial, const string &pin);

/**
 * @brief Return the path of the database file on the SEcube device
 * @param [const char *'] databaseName : name of the database
 * @return [string] : path of the database file
 */
string SEFileDatabasePath(unique_ptr<L0>& l0, const char * databaseName);

/**
 * @brief Retrieve the key from the database file on the SEcube device
 * @param [const char *'] vaultId : vault ID
 * @param [uint8_t *'] key : key buffer where the key will be stored
 * @param [int] size : size of the buffer
 * @return [void]
 *
 * @throws SECubeException on generic errors
 * @throws SECubeDatabaseException on database errors
 */
void SEFileDatabaseRetrieveKey(unique_ptr<L0>& l0, unique_ptr<L1>& l1, const char * vaultId, const uint8_t * key, int size);

/**
 * @brief Store the key in the database file on the SEcube device
 * @param [const char *'] vaultId : vault ID
 * @param [const uint8_t *'] key : key buffer where the key will be stored
 * @param [int] size : size of the buffer
 * @return [void]
 *
 * @throws SECubeException on generic errors
 * @throws SECubeDatabaseException on database errors
 */
void SEFileDatabaseStoreKey(unique_ptr<L0>& l0, unique_ptr<L1>& l1, const char * vaultId, const uint8_t * key, int size);

/**
 * @brief Generate a secure password relying on the onboard TRNG, it generate a new temporary key with the TRNG and encrypt random data with it and a random IV, the algorithm used is AES in CTR mode
 * @param [uint8_t *'] password : destination buffer for the encrypted password
 * @param [int] size : size of the buffer
 * @return [void]
 *
 * @throws SECubeLoginException if not logged in
 * @throws SECubeKeyException on key generation failure
 */
void SECubeGenerateSecurePassword(unique_ptr<L1>& l1, const uint8_t * password, int size);

/**
 * @brief Generate a key on the SEcube device using the onboard TRNG
 * @param [uint32_t] keyId : key ID
 * @return [void]
 *
 * @throws SECubeLoginException if not logged in
 * @throws SECubeKeyException on key generation failure
 */
void SECubeGenerateKey(unique_ptr<L1>& l1, uint32_t keyId);

/**
 * @brief Delete a key from the SEcube device
 * @param [uint32_t] keyId : key ID
 * @return [void]
 *
 * @throws SECubeLoginException if not logged in
 * @throws SECubeKeyException on key generation failure
 */
void SECubeDeleteKey(unique_ptr<L1>& l1, uint32_t keyId);



/* ------------------------ PUBLIC FUNCTIONS ------------------------ */

/**
 * @public
 * @see SECubeAPI.h
 */
vector<pair<string, string>> EnumerateSECubeDevices() {
    unique_ptr<L0> l0 = make_unique<L0>();
    vector<pair<string, string>> devices;
    l0->GetDeviceList(devices);
    l0.reset();
    return devices;
}

/**
 * @public
 * @see SECubeAPI.h
 */
void GenerateSecurePassword(const string &vaultID, const string &serial, const string &pin, const uint8_t * password, int size) {
    unique_ptr<L0> l0 = make_unique<L0>();
    unique_ptr<L1> l1 = make_unique<L1>();

    SECubeLogin(l1, serial, pin);

    try {
        SECubeGenerateSecurePassword(l1, password, size);
        SEFileDatabaseStoreKey(l0, l1, vaultID.c_str(), password, size);
        l1->L1Logout();
        l1.release();
        l0.release();
    } catch (exception &e) {
        l1->L1Logout();
        l1.release();
        l0.release();
        throw SECubeException(e.what());
    }
}

/**
 * @public
 * @see SECubeAPI.h
 */
void RetrieveSecurePassword(const string &vaultID, const string &serial, const string &pin, const uint8_t * password, int size) {
    unique_ptr<L0> l0 = make_unique<L0>();
    unique_ptr<L1> l1 = make_unique<L1>();

    SECubeLogin(l1, serial, pin);

    try {
        SEFileDatabaseRetrieveKey(l0, l1, vaultID.c_str(), password, size);
        l1->L1Logout();
        l1.release();
        l0.release();
    } catch (exception &e) {
        l1->L1Logout();
        l1.release();
        l0.release();
        throw SECubeException(e.what());
    }
}

/* ------------------------ PRIVATE FUNCTIONS ------------------------ */

/**
 * @private
 */
void SECubeLogin(unique_ptr<L1>& l1, const string &serial, const string &pin) {
    if (l1 == nullptr) throw SECubeException("L1 object is null");

    array<uint8_t, L0Communication::Size::SERIAL> sn = {0};
    array<uint8_t, L1Parameters::Size::PIN> p = {0};

    if (serial.length() > L0Communication::Size::SERIAL) throw SECubeException("Serial number too long");
    if (pin.length() > L1Parameters::Size::PIN) throw SECubeException("PIN too long");

    memcpy(sn.data(), serial.data(), serial.length());
    memcpy(p.data(), pin.data(), pin.length());

    try {
        l1->L1SelectSEcube(sn);
    }
    catch (exception &e) {
        throw SECubeException("Impossible to select the SECube device");
    }

    try {
        l1->L1Login(p, SE3_ACCESS_USER, true);
    } catch (L1Exception &e) {
        throw SECubeLoginException(e.what());
    }
}

/**
 * @private
 */
#if _WIN32
string SEFileDatabasePath(const char * databaseName) {
    if (l0 == nullptr) throw SECubeException("L0 object is null");

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string basePathStr = converter.to_bytes(l0->GetDevicePath());

    std::string db_path;
    db_path.append(basePathStr);
    db_path.append("/");
    db_path.append(databaseName);
    return db_path;
}
#else
string SEFileDatabasePath(unique_ptr<L0>& l0, const char * databaseName) {
    if (l0 == nullptr) throw SECubeException("L0 object is null");
    string db_path;
    db_path.append(l0->GetDevicePath());
    db_path.append("/");
    db_path.append(databaseName);
    return db_path;
}
#endif

/**
 * @private
 */
void SEFileDatabaseRetrieveKey(unique_ptr<L0>& l0, unique_ptr<L1>& l1, const char * vaultId, const uint8_t * key, int size) {
    if (l0 == nullptr) throw SECubeException("L0 object is null");
    if (l1 == nullptr) throw SECubeException("L1 object is null");

    SEcube = l1.get();
    sqlite3 *db;
    unique_ptr<SEfile> sefile_db = make_unique<SEfile>();
    if (0 != sefile_db->secure_init(l1.get(), DB_KEY_ID, L1Algorithms::Algorithms::AES_HMACSHA256))
        throw SECubeException("Failed to initialize SEfile");

    memcpy(sefile_db->handleptr->name, DB_NAME, strlen(DB_NAME));
    databases.push_back(std::move(sefile_db));

    int rc = sqlite3_open_v2(SEFileDatabasePath(l0, DB_NAME).c_str(), &db, SQLITE_OPEN_READONLY, nullptr);
    if (rc != SQLITE_OK) SQLite3ErrorHandler(db);

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, SQL_KEY_SELECT, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) SQLite3ErrorHandler(db);

    sqlite3_bind_text(stmt, 1, vaultId, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) SQLite3ErrorHandler(db, "No key found for the given vault ID");

    const void* blob = sqlite3_column_blob(stmt, 0);
    memcpy((void*)key, blob, size);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

/**
 * @private
 */
void SEFileDatabaseStoreKey(unique_ptr<L0>& l0, unique_ptr<L1>& l1, const char * vaultId, const uint8_t * key, int size) {
    if (l0 == nullptr) throw SECubeException("L0 object is null");
    if (l1 == nullptr) throw SECubeException("L1 object is null");

    SEcube = l1.get();
    sqlite3 *db;
    unique_ptr<SEfile> sefile_db = make_unique<SEfile>();
    if (0 != sefile_db->secure_init(l1.get(), DB_KEY_ID, L1Algorithms::Algorithms::AES_HMACSHA256))
        throw SECubeException("Failed to initialize SEfile");

    memcpy(sefile_db->handleptr->name, DB_NAME, strlen(DB_NAME));
    databases.push_back(std::move(sefile_db));

    int rc = sqlite3_open_v2(SEFileDatabasePath(l0, DB_NAME).c_str(), &db, SQLITE_OPEN_READWRITE, nullptr);
    if (rc != SQLITE_OK) SQLite3ErrorHandler(db);

    sqlite3_stmt *stmt;

    rc = sqlite3_prepare_v2(db, SQL_KEY_INSERT, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) SQLite3ErrorHandler(db);

    sqlite3_bind_text(stmt, 1, vaultId, -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 2, key, size, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) SQLite3ErrorHandler(db);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

/**
 * @private
 */
void SECubeGenerateSecurePassword(unique_ptr<L1>& l1, const uint8_t * password, int size) {
    if (!l1->L1GetSessionLoggedIn()) throw SECubeLoginException("Not logged in");

    try { SECubeDeleteKey(l1, TMP_KEY_ID); } catch ( ... ) { }

    SECubeGenerateKey(l1, TMP_KEY_ID);

    uint8_t iv[16] = {0};
    L0Support::Se3Rand(16, iv);
    shared_ptr<uint8_t[]> data(new uint8_t[size]);
    L0Support::Se3Rand(size, data.get());

    SEcube_ciphertext encrypted_data;
    try {
        l1->L1Encrypt(size, data, encrypted_data, L1Algorithms::Algorithms::AES, CryptoInitialisation::Modes::CTR, TMP_KEY_ID);
    } catch (exception &e) {
        throw SECubeKeyException(e.what());
    }

    memcpy((void *) password, encrypted_data.ciphertext.get(), size);

    SECubeDeleteKey(l1, TMP_KEY_ID);
}

/**
 * @private
 */
void SECubeGenerateKey(unique_ptr<L1>& l1, uint32_t keyId) {
    if (!l1->L1GetSessionLoggedIn()) throw SECubeLoginException("Not logged in");

    se3Key key;
    key.id = keyId;
    key.dataSize = KEY_LEN;
    key.data = nullptr;

    try {
        l1->L1KeyEdit(key, L1Commands::KeyOpEdit::SE3_KEY_OP_ADD_TRNG);
    }
    catch (exception& e) {
        throw SECubeKeyException(e.what());
    }
}

/**
 * @private
 */
void SECubeDeleteKey(unique_ptr<L1>& l1, uint32_t keyId) {
    if (!l1->L1GetSessionLoggedIn()) throw SECubeLoginException("Not logged in");

    se3Key key;
    key.id = keyId;

    key.dataSize = 0;
    key.data = nullptr;
    try {
        l1->L1KeyEdit(key, L1Commands::KeyOpEdit::SE3_KEY_OP_DELETE);
    }
    catch (exception& e) {
        throw SECubeKeyException(e.what());
    }
}

