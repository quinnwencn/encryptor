# Overview
This repository contains an Encryptor tool that allows users to encrypt data using AES encryption securely. The tool generates a random AES key, uses it to encrypt the provided data, and then encrypts this AES key using public keys from a device certificate. The encrypted AES key is then embedded in the encrypted data, along with the initialization vector (IV), ensuring that the data can be safely transmitted or stored while remaining protected against unauthorized access.

# Key Features
* AES Encryption: Encrypts data using the widely-used AES encryption algorithm, providing strong data protection.
* Key Encryption: The generated AES key is itself encrypted using public keys from a device certificate, adding an extra layer of security.
* Integration of IV: The initialization vector (IV) is used during encryption to ensure that the same data encrypted multiple times with the same key results in different ciphertexts.
* Portable: The tool is built to work across different systems, allowing easy integration into various environments.
* Simple Command-Line Interface: Users can easily encrypt files by specifying input and output paths through simple commands.
  
# Dependencies
OpenSSL: The encryptor uses OpenSSL for cryptographic operations, including generating AES keys and encrypting/decrypting the data with public/private keys.
# Install & Usage
## Prerequisites
Before using the tool, ensure you have the following installed on your machine:
* OpenSSL
* CMake (Version 3.18 or above)

## Build Instructions
To build the Encryptor tool from the source, follow these steps:
1. Create a build directory:
```bash
cmake -Bbuild
```
2. Build the project:
```bash
cmake --build build -j`nproc`
```
Once the build is complete, the `encryptor` executable will be located in the build directory.

## Usage
To encrypt a file, use the following command:
```bash
encryptor -b <path_to_input_file> -o <path_to_output_file>
```
* `-b`: Path to the file you want to encrypt
* `o`: Path where the encrypted file will be saved.

Example:
```bash
encryptor -b ./input.txt -o /tmp/ciphertext.txt
```
This will encrypt the file `input.txt` and save the encrypted data in `/tmp/ciphertext.txt`.

# File format
The encrypted file will contain:
1. The AES-encrypted data
2. The encrypted AES key, encrypted using the device's public key
3. The initialization vector(IV) used during encryption.

# Decryption
The decryption process is the reverse of encryption. You will need the private key corresponding to the public key used during encryption in order to decrypt the AES key. Please refer to the [pyDecryptor](https://github.com/quinnwencn/pyDecryptor) or [decryptor](https://github.com/quinnwencn/decryptor) for more information on how to perform the decryption.
