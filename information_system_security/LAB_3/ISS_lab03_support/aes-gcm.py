#!/usr/bin/python3

import os
import sys
import base64

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.ciphers import (
    Cipher, algorithms, modes
)

def encrypt(key, iv, plaintext, associated_data):
    # Generate a random 96-bit IV.
    # iv = os.urandom(12)

    # Construct an AES-GCM Cipher object with the given key and a
    # randomly generated IV.
    encryptor = Cipher(
        algorithms.AES(key),
        modes.GCM(iv),
        backend=default_backend()
    ).encryptor()

    # associated_data will be authenticated but not encrypted,
    # it must also be passed in on decryption.
    encryptor.authenticate_additional_data(associated_data)

    # Encrypt the plaintext and get the associated ciphertext.
    # GCM does not require padding.
    ciphertext = encryptor.update(plaintext) + encryptor.finalize()

    return (ciphertext, encryptor.tag)

def decrypt(key, associated_data, iv, ciphertext, tag):
    # Construct a Cipher object, with the key, iv, and additionally the
    # GCM tag used for authenticating the message.
    decryptor = Cipher(
        algorithms.AES(key),
        modes.GCM(iv, tag),
        backend=default_backend()
    ).decryptor()

    # We put associated_data back in or the tag will fail to verify
    # when we finalize the decryptor.
    decryptor.authenticate_additional_data(associated_data)

    # Decryption gets us the authenticated plaintext.
    # If the tag does not match an InvalidTag exception will be raised.
    return decryptor.update(ciphertext) + decryptor.finalize()


# PARAMTERS
# sys.argv[1] = operation {"-d","-e"}
# sys.argv[2] = plaintext
# sys.argv[3] = associated data
# sys.argv[4] = ciphertext
# sys.argv[5] = tag
# sys.argv[6] = "-K" (fixed)
# sys.argv[7] = key value (128 bits)
# sys.argv[8] = "-iv" (fixed)
# sys.argv[9] = iv value (96 bits)

def main():
    if len(sys.argv) != 10:
        print("ERROR: Wrong parameters number. Ten parameters expected.")
        sys.exit()

    if sys.argv[1] != "-e" and sys.argv[1] != "-d":
        print("ERROR: Wrong parameters. Either -e or -d are allowed.")
        sys.exit()

    if sys.argv[6] != "-K" or sys.argv[8] != "-iv":
        print("ERROR: Wrong parameters. Use -K and -iv to pass key and IV.")
        sys.exit()

    print(sys.argv[7])
    key = bytes.fromhex(sys.argv[7])
    iv = bytes.fromhex(sys.argv[9])

    if sys.argv[1] == "-e" :
        sys.stdout.write("Encryption ")

        ciphertext, tag = encrypt(
            key,
            iv,
            open(sys.argv[2],"rb").read(),
            open(sys.argv[3],"rb").read()
            )

        # print("TAG = "+str(base64.b64encode(tag)))

        file_out = open(sys.argv[4],"wb")
        file_out.write(ciphertext)
        file_out.close()

        file_tag = open(sys.argv[5],"wb")
        file_tag.write(tag)
        file_tag.close()



    if sys.argv[1] == "-d" :
        sys.stdout.write("Decryption ")

        plaintext = decrypt(
            key,
            open(sys.argv[3],"r").read().encode('ascii'),
            iv,
            open(sys.argv[4],"rb").read(),
            open(sys.argv[5],"rb").read()
        )
        file_plain = open(sys.argv[2],"wb")
        file_plain.write(plaintext)
        file_plain.close()

    print("successfully completed.")

if __name__ == '__main__':
        main()
