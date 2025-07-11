/**
 * @authors Alekos Interrante Bonadia
 * @authors Lorenzo Sebastiano Mathis
 * @authors Pietro Mazza
 * @version 1.0
 */

#include <iomanip>
#include "SECubeUtils.h"
#include "SECubeExceptions.h"

#ifndef DISABLE_JNI
/**
 * @public
 * @see SECubeUtils.h
 */
void JNIDebugPrint(JNIEnv *env, const char *msg) {
    jclass systemClass = env->FindClass("java/lang/System");
    if (systemClass == nullptr) {
        std::cerr << "Error: Cannot find java/lang/System" << std::endl;
        return;
    }

    jfieldID outField = env->GetStaticFieldID(systemClass, "out", "Ljava/io/PrintStream;");
    if (outField == nullptr) {
        std::cerr << "Error: Cannot find field 'out'" << std::endl;
        return;
    }

    jobject outObj = env->GetStaticObjectField(systemClass, outField);

    jclass printStreamClass = env->FindClass("java/io/PrintStream");
    if (printStreamClass == nullptr) {
        std::cerr << "Error: Cannot find java/io/PrintStream" << std::endl;
        return;
    }

    jmethodID printlnMethod = env->GetMethodID(printStreamClass, "println", "(Ljava/lang/String;)V");
    if (printlnMethod == nullptr) {
        std::cerr << "Error: Cannot find method 'println'" << std::endl;
        return;
    }

    jstring message = env->NewStringUTF(msg);

    env->CallVoidMethod(outObj, printlnMethod, message);

    env->DeleteLocalRef(message);
    env->DeleteLocalRef(systemClass);
    env->DeleteLocalRef(printStreamClass);
}
#endif
/**
 * @public
 * @see SECubeUtils.h
 */
string uint8ToHex(const uint8_t *data, size_t len) {
    stringstream ss;
    for (int i = 0; i < len; i++) {
        ss << std::hex << setw(2) << setfill('0') << (int)data[i];
    }
    return ss.str();
}

/**
 * @public
 * @see SECubeUtils.h
 */
void SQLite3ErrorHandler(sqlite3 *db, const char *msg) {
    const char *dbmsg = sqlite3_errmsg(db);
    const string sMsg = msg;
    const string message = "Database error: " + string(dbmsg) + (msg != nullptr ? " - " + sMsg : "");
    throw SECubeDatabaseException(message);
}

#ifndef DISABLE_JNI
/**
 * @public
 * @see SECubeUtils.h
 */
void JNIThrowException(JNIEnv *env, exception &e) {
    jclass SECubeException = nullptr;
    SECubeExceptionType type = SECubeExceptionType::Unknown;
    if (dynamic_cast<const class SECubeException*>(&e)) {
        type = dynamic_cast<const class SECubeException*>(&e)->getType();
    }

    switch (type) {
        case SECubeExceptionType::SECubeLoginException:
            SECubeException = env->FindClass("org/cryptomator/secube/exceptions/SECubeLoginException");
            break;
        case SECubeExceptionType::SECubeDatabaseException:
            SECubeException = env->FindClass("org/cryptomator/secube/exceptions/SECubeDatabaseException");
            break;
        case SECubeExceptionType::SECubeKeyException:
            SECubeException = env->FindClass("org/cryptomator/secube/exceptions/SECubeKeyException");
            break;
        case SECubeExceptionType::SECubeException:
            SECubeException = env->FindClass("org/cryptomator/secube/exceptions/SECubeException");
            break;
        case SECubeExceptionType::Unknown:
            SECubeException = env->FindClass("java/lang/RuntimeException");
            break;
    }

    if (SECubeException != nullptr) {
        env->ThrowNew(SECubeException, e.what());
        env->DeleteLocalRef(SECubeException);
    }
}
#endif