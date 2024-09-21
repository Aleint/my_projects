/**
 * @authors Alekos Interrante Bonadia
 * @authors Lorenzo Sebastiano Mathis
 * @authors Pietro Mazza
 * @version 1.0
 */

#ifndef CPP_SECUBEUTILS_H
#define CPP_SECUBEUTILS_H

#include <cstdint>
#include <iostream>
#include <sstream>
#ifndef DISABLE_JNI
#include <jni.h>
#endif
#include "sqlite/sqlite3.h"
#include <cstdint>

using namespace std;

#ifndef DISABLE_JNI
/**
 * Print a message to the Java console
 * @param [JNIEnv *] env : JNI environment
 * @param [const char *] msg : message to be printed
 * @return [void]
 */
void JNIDebugPrint(JNIEnv *env, const char *msg);
#endif

/**
 * Convert a byte array to a hex string
 * @param [uint8_t *'] data : byte array
 * @param [size_t] len : length of the byte array
 * @return [string] : hex string
 */
string uint8ToHex(const uint8_t *data, size_t len);

/**
 * Handle SQLite3 errors
 * @param [sqlite3 *] db : SQLite3 database
 * @param [const char *] msg : error message
 * @throws SECubeDatabaseException with the SQLite3 error message
 * @return [void]
 */
void SQLite3ErrorHandler(sqlite3 *db, const char *msg = nullptr);

#ifndef DISABLE_JNI
/**
 * Throw a Java exception from a C++ exception
 * @param [JNIEnv *] env : JNI environment
 * @param [exception &] e : exception to be thrown
 * @return [void]
 */
void JNIThrowException(JNIEnv *env, exception &e);
#endif

#endif //CPP_SECUBEUTILS_H
