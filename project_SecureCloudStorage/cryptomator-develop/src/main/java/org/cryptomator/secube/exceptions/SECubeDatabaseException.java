package org.cryptomator.secube.exceptions;

/**
 * Exception thrown when an error occurs in SEcureDB operations.
 * This exception is thrown from the native library via JNI.
 *
 * @see SECubeException
 */
public class SECubeDatabaseException extends SECubeException {
    public SECubeDatabaseException(String message) {
        super(message);
    }

    public SECubeDatabaseException(String message, Throwable cause) {
        super(message, cause);
    }
}
