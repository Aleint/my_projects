package org.cryptomator.secube.exceptions;

/**
 * Exception thrown when an error occurs in SECube key operations.
 * This exception is thrown from the native library via JNI.
 *
 * @see SECubeException
 */
public class SECubeKeyException extends SECubeException {
    public SECubeKeyException(String message) {
        super(message);
    }

    public SECubeKeyException(String message, Throwable cause) {
        super(message, cause);
    }
}
