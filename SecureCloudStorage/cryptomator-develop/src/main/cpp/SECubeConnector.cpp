/**
 * @authors Alekos Interrante Bonadia
 * @authors Lorenzo Sebastiano Mathis
 * @authors Pietro Mazza
 * @version 1.0
 */

#include <jni.h>
#include <map>
#include "org_cryptomator_secube_SECubeConnector.h"
#include "SECubeAPI.h"
#include "SECubeUtils.h"
#include "configuration.h"

using namespace std;

/**
 * Enumerate the SECube devices and return a map with the device name and serial number
 * @param [JNIEnv] env : JNI environment
 * @param [jobject] obj : object
 * @return [jobject] : java.util.HashMap<String, String> with the mount point and serial number
 * @see org_cryptomator_secube_SECubeL0.h - EnumerateDevices
 */
JNIEXPORT jobject JNICALL Java_org_cryptomator_secube_SECubeConnector_EnumerateDevices(JNIEnv *env, jobject obj) {
    vector<pair<string, string>> devices = EnumerateSECubeDevices();

    jclass hashMapClass = env->FindClass("java/util/HashMap");
    jmethodID hashMapConstructor = env->GetMethodID(hashMapClass, "<init>", "()V");
    jmethodID hashMapPut = env->GetMethodID(hashMapClass, "put",
                                            "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    jobject hashMap = env->NewObject(hashMapClass, hashMapConstructor);

    for (const pair<string, string> &d: devices) {
        jstring key = env->NewStringUTF(d.first.c_str());
        jstring value = env->NewStringUTF(d.second.c_str());

        env->CallObjectMethod(hashMap, hashMapPut, key, value);

        env->DeleteLocalRef(key);
        env->DeleteLocalRef(value);
    }

    // env->DeleteLocalRef(hashMapClass);
    // env->DeleteLocalRef(hashMapConstructor);
    // env->DeleteLocalRef(hashMapPut);

    return hashMap;
}

/**
 * Generate a secure password for the given vault ID, this function is used by cryptomator when a new vault is created.
 * A new secure password is generated, stored in the database for the given vault ID and returned.
 * @param [JNIEnv] env : JNI environment
 * @param [jobject] obj : object
 * @param [jstring] vaultID : vault ID
 * @param [jstring] serial : serial number of the SEcube device
 * @param [jstring] pin : pin of the SEcube device
 * @return [jstring] : secure password for the given vault ID
 *
 * @throws SECubeDatabaseException in the JNI environment if the database query fails
 * @throws SECubeLoginException in the JNI environment if login to the SEcube device fails
 * @throws SECubeException in the JNI environment if the serial number or pin is too long
 *
 * @see org_cryptomator_secube_SECubeL1.h - GenerateSecurePassword
 */
JNIEXPORT jstring JNICALL
Java_org_cryptomator_secube_SECubeConnector_GenerateSecurePasswordOnDevice(JNIEnv *env, jobject obj, jstring vaultID,
                                                                    jstring serial, jstring pin) {
    const char *cSerial = env->GetStringUTFChars(serial, nullptr);
    const char *cPin = env->GetStringUTFChars(pin, nullptr);
    const char *cVaultID = env->GetStringUTFChars(vaultID, nullptr);

    try {
        uint8_t password[SEC_PASS_LEN];
        GenerateSecurePassword(cVaultID, cSerial, cPin, password, SEC_PASS_LEN);
        return env->NewStringUTF(uint8ToHex(password, SEC_PASS_LEN).c_str());
    } catch (exception &e) {
        JNIThrowException(env, e);
        return env->NewStringUTF(e.what());
    }
}

/**
 * Get the secure password for the given vault ID, this function is used by cryptomator when a vault is opened.
 * @param [JNIEnv] env : JNI environment
 * @param [jobject] obj : object
 * @param [jstring] vaultID : vault ID
 * @param [jstring] serial : serial number of the SEcube device
 * @param [jstring] pin : pin of the SEcube device
 * @return [jstring] : secure password for the given vault ID
 *
 * @throws SECubeDatabaseException in the JNI environment if the database query fails
 * @throws SECubeLoginException in the JNI environment if login to the SEcube device fails
 * @throws SECubeException in the JNI environment if the serial number or pin is too long
 *
 * @see org_cryptomator_secube_SECubeL1.h - RetrieveSecurePassword
 */
JNIEXPORT jstring JNICALL
Java_org_cryptomator_secube_SECubeConnector_GetSecurePasswordOnDevice(JNIEnv *env, jobject obj, jstring vaultID,
                                                               jstring serial, jstring pin) {
    const char *cSerial = env->GetStringUTFChars(serial, nullptr);
    const char *cPin = env->GetStringUTFChars(pin, nullptr);
    const char *cVaultID = env->GetStringUTFChars(vaultID, nullptr);

    try {
        uint8_t password[SEC_PASS_LEN];
        RetrieveSecurePassword(cVaultID, cSerial, cPin, password, SEC_PASS_LEN);
        return env->NewStringUTF(uint8ToHex(password, SEC_PASS_LEN).c_str());
    } catch (exception &e) {
        JNIThrowException(env, e);
        return env->NewStringUTF(e.what());
    }
}
