package org.cryptomator.secube.exceptions;

/**
 * Exception thrown when an error occurs in SECube login operations.
 * This exception is thrown from the native library via JNI.
 *
 * @see SECubeException
 */
public class SECubeLoginException extends SECubeException {
    public SECubeLoginException(String message) {
        super(message);
    }

    public SECubeLoginException(String message, Throwable cause) {
        super(message, cause);
    }
}
