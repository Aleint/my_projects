# Cryptomator-SECube Integration

This project focuses on the successful integration of the SECube hardware security module (HSM) with Cryptomator, an open-source cloud encryption tool. The goal is to leverage the SECube's hardware-based cryptographic capabilities for generating and securely storing strong passphrases used to protect Cryptomator vaults.

## About This Project

The core of this project was to develop a secure process for generating strong, exportable cryptographic passphrases using the SECube's True Random Number Generator (TRNG) without directly exporting the hardware-generated keys. This was achieved by creating a C++ library that interfaces with the SECube SDK and exposing its functionality to the Java-based Cryptomator application through a Java Native Interface (JNI) connector.

The final integration allows a user to create and unlock a Cryptomator vault using a passphrase that is generated and stored securely on the SECube device.

## Technology & Architecture

The system architecture combines three main components:

* **Cryptomator**: An open-source tool that encrypts files and folders by creating virtual drives called "vaults". It uses AES-256 for encryption and is compatible with various cloud storage services.
* **SECube**: A hardware security module (HSM) featuring an ARM Cortex M4 microcontroller, an FPGA, and an EAL5+ certified smart card. It provides a suite of libraries for hardware interaction:
    * **L0 Library**: Handles fundamental communication, device discovery, and provisioning.
    * **L1 Library**: Provides higher-level functions for authentication, key management, and cryptographic operations like AES-256 encryption.
    * **L2 Library (SEFile)**: An abstraction layer that includes a secure file system interface. It was used to store the generated passphrases in a custom, encrypted SQLite3 database on the SECube's internal memory.
* **JNI (Java Native Interface)**: The framework used to create a bridge between Cryptomator's Java code and the C++ library developed to control the SECube. This allows the Java application to call native C++ functions to perform key generation and retrieval operations on the hardware.

## Core Implementation: Secure Passphrase Generation

A primary challenge was that the SECube's L1 library does not permit the export of keys generated directly by its hardware TRNG. To overcome this, the following password generation process was implemented:

1.  A secure, non-exportable 32-bit cryptographic key is generated on the SECube using the L1 library's function, which utilizes the onboard TRNG for strong entropy.
2.  Two other values, a plaintext payload (32 bytes) and an initialization vector (IV, 16 bytes), are generated using the L0 library. The L0 library relies on system-level pseudorandom number generators, such as `/dev/urandom` on Unix systems or `BCryptGenRandom` on Windows.
3.  The payload is then encrypted using the AES-256 algorithm in Counter (CTR) mode. This operation uses the TRNG-generated key and the IV, and is performed by the `L1Encrypt` function on the SECube device.
4.  The resulting ciphertext, a 64-character hexadecimal string, becomes the final, exportable passphrase for the Cryptomator vault. This method is secure against chosen-plaintext attacks (IND-CPA) because the key, payload, and IV are regenerated for every operation.
5.  The generated passphrase, along with its associated vault ID, is securely stored within an encrypted SQLite database on the SECube's memory using the `SEFileDatabaseStoreKey` function from the L2 library.
6.  To unlock a vault, the `SEFileDatabaseRetrieveKey` function is called to retrieve the correct passphrase from the secure database using the vault's ID.

## Features and UI Integration

The project's functionality is seamlessly integrated into the Cryptomator user interface:

* **Create Vault**: The "Add Vault" screen now includes options to select a connected SECube device from a list and enter the corresponding User PIN. A "Create password with SEcube" button initiates the hardware-based generation process.
* **Unlock Vault**: If a vault was created using the SECube, the unlock screen prompts the user to select the device and enter their PIN to retrieve the stored passphrase and unlock the vault.