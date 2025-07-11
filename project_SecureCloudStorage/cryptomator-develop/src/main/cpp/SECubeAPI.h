/**
 * @authors Alekos Interrante Bonadia
 * @authors Lorenzo Sebastiano Mathis
 * @authors Pietro Mazza
 * @version 1.0
 */

#ifndef CPP_SECUBEAPI_H
#define CPP_SECUBEAPI_H

#include <cstdint>
#include <string>
#include <vector>
#include <cstdint>
#include "SECubeExceptions.h"

using namespace std;

/**
 * Enumerate the SECube devices and return a vector with the mount point and serial number
 * @return [vector(string,string)] : vector with the mount point and serial number
 */
vector<pair<string, string>> EnumerateSECubeDevices();

/**
 * Generate a secure password using the onboard TRNG and save it in the database for the given vault ID
 * @param [string] vaultID : vault ID
 * @param [string] serial : serial number of the SEcube device
 * @param [string] pin : pin of the SEcube device
 * @param [const uint8_t *'] password : password to be encrypted
 * @param [int] size : size of the password
 * @return [void]
 *
 * @throws SECubeException generic errors
 * @throws SECubeLoginException on login failure
 * @throws SECubeKeyException on key generation failure
 * @throws SECubeDatabaseException on database errors
 */
void GenerateSecurePassword(const string &vaultID, const string &serial, const string &pin, const uint8_t * password, int size);

/**
 * Retrieve a secure password from the database for the given vault ID
 * @param [string] vaultID : vault ID
 * @param [string] serial : serial number of the SEcube device
 * @param [string] pin : pin of the SEcube device
 * @param [const uint8_t *'] password : password to be decrypted
 * @param [int] size : size of the password
 * @return [void]
 *
 * @throws SECubeException generic errors
 * @throws SECubeLoginException on login failure
 * @throws SECubeDatabaseException on database errors
 */
void RetrieveSecurePassword(const string &vaultID, const string &serial, const string &pin, const uint8_t * password, int size);

#endif //CPP_SECUBEAPI_H
