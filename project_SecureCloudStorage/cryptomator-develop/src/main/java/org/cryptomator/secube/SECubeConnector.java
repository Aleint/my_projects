package org.cryptomator.secube;

import java.util.HashMap;
import java.util.Map;

import org.cryptomator.secube.exceptions.SECubeException;

/**
 * This class is the main entry point for the SECubeConnector library, implementing it as a singleton.
 * It provides methods to interact with the SECube device.
 */
public class SECubeConnector {
    static {
        System.loadLibrary("SECubeConnector");
    }

    private static volatile SECubeConnector instance;

    /**
     * Get the singleton instance of the SECubeConnector.
     *
     * @return The singleton instance of the SECubeConnector.
     */
    public static SECubeConnector getInstance() {
        if (instance == null) {
            synchronized (SECubeConnector.class) {
                if (instance == null) {
                    instance = new SECubeConnector();
                }
            }
        }
        return instance;
    }

    /**
     * Get a list of all connected SECube devices. It is a wrapper for the native method.
     *
     * @return A map of SECube mount point to serial number (mount point : String, serial number : String).
     */
    public Map<String, String> GetDevices() throws SECubeException {
        Map<String, String> devices = EnumerateDevices();
        if (devices == null) {
            devices = new HashMap<>();
            devices.put("Impossible to list the devices", "Impossible to list the devices");
        }
        return devices;
    }

    /**
     * Generate a secure password using the SECube device, store it on the device and return it. It is a wrapper for the native method.
     *
     * @param vaultID      The vault ID of the SECube device.
     * @param SECubeSerial The serial number of the SECube device.
     * @param SECubePIN    The PIN of the SECube device.
     * @return The generated secure password.
     */
    public String GenerateSecurePassword(String vaultID, String SECubeSerial, String SECubePIN) {
    	return GenerateSecurePasswordOnDevice(vaultID, SECubeSerial, SECubePIN);
    }

    /**
     * Get a secure password from the SECube device. It is a wrapper for the native method.
     *
     * @param vaultID      The vault ID of the SECube device.
     * @param SECubeSerial The serial number of the SECube device.
     * @param SECubePIN    The PIN of the SECube device.
     * @return The secure password.
     */
    public String GetSecurePassword(String vaultID, String SECubeSerial, String SECubePIN) {
        return GetSecurePasswordOnDevice(vaultID, SECubeSerial, SECubePIN);
    }


    /**
     * Enumerate the connected SECube devices. It is mapped to the method Java_org_cryptomator_secube_SECubeConnector_EnumerateDevices in the native library.
     *
     * @return A map of SECube serial numbers to device names (serial : String, name : String).
     * @throws SECubeException If an error occurs during the enumeration.
     */
    private native Map<String, String> EnumerateDevices() throws SECubeException;

    /**
     * Generate a secure password using the SECube device, store it on the device and return it. It is mapped to the method Java_org_cryptomator_secube_SECubeConnector_GenerateSecurePasswordOnDevice in the native library.
     *
     * @param vaultID      The vault ID of the SECube device.
     * @param SECubeSerial The serial number of the SECube device.
     * @param SECubePIN    The PIN of the SECube device.
     * @return The generated secure password.
     * @throws SECubeException If an error occurs during the generation.
     */
    private native String GenerateSecurePasswordOnDevice(String vaultID, String SECubeSerial, String SECubePIN) throws SECubeException;

    /**
     * Get a secure password from the SECube device. It is mapped to the method Java_org_cryptomator_secube_SECubeConnector_GetSecurePasswordOnDevice in the native library.
     *
     * @param vaultID      The vault ID of the SECube device.
     * @param SECubeSerial The serial number of the SECube device.
     * @param SECubePIN    The PIN of the SECube device.
     * @return The secure password.
     * @throws SECubeException If an error occurs during the retrieval.
     */
    private native String GetSecurePasswordOnDevice(String vaultID, String SECubeSerial, String SECubePIN) throws SECubeException;
}
