#pragma once

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <vector>
#include <string>

std::vector<unsigned char> hexToBytes(const std::string& hex) {
    std::vector<unsigned char> bytes;
    bytes.reserve(hex.length() / 2);
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        unsigned char byte = (unsigned char)strtol(byteString.c_str(), nullptr, 16);
        bytes.push_back(byte);
    }
    return bytes;
}

bool decryptCryptr(
    const std::string& encryptedHex,
    const std::string& password,
    std::string& decryptedOut)
{
    constexpr int SALT_LEN = 64;
    constexpr int IV_LEN = 16;
    constexpr int TAG_LEN = 16;
    constexpr int ITERATIONS = 100000;

    std::vector<unsigned char> encryptedBytes = hexToBytes(encryptedHex);

    if (encryptedBytes.size() < (SALT_LEN + IV_LEN + TAG_LEN)) return false;

    const unsigned char* salt = encryptedBytes.data();
    const unsigned char* iv = salt + SALT_LEN;
    const unsigned char* tag = iv + IV_LEN;
    const unsigned char* ciphertext = tag + TAG_LEN;
    size_t ciphertext_len = encryptedBytes.size() - (SALT_LEN + IV_LEN + TAG_LEN);

    unsigned char key[32];
    if (!PKCS5_PBKDF2_HMAC(
        password.c_str(), password.length(),
        salt, SALT_LEN,
        ITERATIONS,
        EVP_sha512(),
        32, key)) return false;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    std::vector<unsigned char> plaintext(ciphertext_len + 16);
    int len = 0, final_len = 0;

    bool success = false;
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) &&
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_LEN, NULL) &&
        EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv) &&
        EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext, ciphertext_len)) {

        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_LEN, (void*)tag);

        if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &final_len) > 0) {
            plaintext.resize(len + final_len);
            decryptedOut = std::string(plaintext.begin(), plaintext.end());
            success = true;
        }
    }

    EVP_CIPHER_CTX_free(ctx);
    return success;
}