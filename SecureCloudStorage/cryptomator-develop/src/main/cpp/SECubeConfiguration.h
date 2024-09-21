/**
 * @authors Alekos Interrante Bonadia
 * @authors Lorenzo Sebastiano Mathis
 * @authors Pietro Mazza
 * @version 1.0
 */

#ifndef CPP_SECUBECONFIGURATION_H
#define CPP_SECUBECONFIGURATION_H

#include <vector>
#include <string>

using namespace std;

enum class ERROR_CODE {
    NONE = 0,
    NO_DEVICE_FOUND = 1,
    COMM_ERROR = 2,
    INVALID_DEVICE = 3,
    MALFORMED_INPUT = 4,
    DEVICE_ALREADY_INITIALIZED = 5,
    DEVICE_ERROR = 6,
    LOGIN_ERROR = 7,
    KEY_ERROR = 8,
    DATABASE_ERROR = 9,
};

/**
 * @brief List all the available SECube devices, if select is true the user is asked to select one
 * @param [vector(string,string)] devices : with the mount point and serial number of the devices
 * @param [bool] select : if true the user is asked to select a device
 * @return [ERROR_CODE] : error code, ERROR_CODE:NONE if no error
 */
ERROR_CODE SECubeDeviceList(vector<pair<string, string>>& devices, bool select = false);

/**
 * @brief Initialize the SECube device
 * @return [ERROR_CODE] : error code, ERROR_CODE:NONE if no error
 */
ERROR_CODE SECubeFactoryInitRoutine();

/**
 * @brief Initialize the SECube device key
 * @return [ERROR_CODE] : error code, ERROR_CODE:NONE if no error
 */
ERROR_CODE SECubeDBKeyInitRoutine();

/**
 * @brief Initialize the SECube database
 * @return [ERROR_CODE] : error code, ERROR_CODE:NONE if no error
 */
ERROR_CODE SECubeDBInitRoutine();

/**
 * @brief Read the SECube database on stdout
 * @return [ERROR_CODE] : error code, ERROR_CODE:NONE if no error
 */
ERROR_CODE SECubeDBDebugRead();

#endif //CPP_SECUBECONFIGURATION_H
