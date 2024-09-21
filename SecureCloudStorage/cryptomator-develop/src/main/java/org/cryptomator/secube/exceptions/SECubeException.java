package org.cryptomator.secube.exceptions;

/**
 * Exception thrown when an error occurs in SECube operations.
 * This exception is thrown from the native library via JNI.
 */
public class SECubeException extends RuntimeException {
    public SECubeException(String message) {
        super(message);
    }

    public SECubeException(String message, Throwable cause) {
        super(message, cause);
    }
}
