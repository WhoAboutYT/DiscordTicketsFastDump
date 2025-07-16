#pragma once

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <vector>
#include <future>
#include <string>
#include <nlohmann/json.hpp>
#include <algorithm>
#include "ThreadPool.hpp"

using json = nlohmann::json;

inline std::vector<unsigned char> hexToBytes(const std::string& hex) {
    std::vector<unsigned char> bytes;
    bytes.reserve(hex.size() / 2);
    for (auto it = hex.begin(); it != hex.end();) {
        unsigned char byte = (unsigned char)((std::stoul(std::string(it, it + 2), nullptr, 16) & 0xFF));
        bytes.push_back(byte);
        it += 2;
    }
    return bytes;
}

inline bool decryptCryptr(
    const std::string& encryptedHex,
    const std::string& password,
    std::string& decryptedOut)
{
    constexpr size_t SALT_LEN = 64;
    constexpr size_t IV_LEN = 16;
    constexpr size_t TAG_LEN = 16;
    constexpr int ITERATIONS = 100000;

    std::vector<unsigned char> encryptedBytes = hexToBytes(encryptedHex);
    if (encryptedBytes.size() < (SALT_LEN + IV_LEN + TAG_LEN)) return false;

    const unsigned char* salt = encryptedBytes.data();
    const unsigned char* iv = salt + SALT_LEN;
    const unsigned char* tag = iv + IV_LEN;
    const unsigned char* ciphertext = tag + TAG_LEN;
    size_t ciphertext_len = encryptedBytes.size() - (SALT_LEN + IV_LEN + TAG_LEN);

    unsigned char key[32];
    if (!PKCS5_PBKDF2_HMAC(password.data(), password.size(), salt, SALT_LEN, ITERATIONS, EVP_sha512(), 32, key))
        return false;

    std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
    if (!ctx) return false;

    std::vector<unsigned char> plaintext(ciphertext_len + 16);
    int len = 0, final_len = 0;

    if (EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_gcm(), NULL, NULL, NULL) &&
        EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_GCM_SET_IVLEN, IV_LEN, NULL) &&
        EVP_DecryptInit_ex(ctx.get(), NULL, NULL, key, iv) &&
        EVP_DecryptUpdate(ctx.get(), plaintext.data(), &len, ciphertext, ciphertext_len)) {

        EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_GCM_SET_TAG, TAG_LEN, (void*)tag);

        if (EVP_DecryptFinal_ex(ctx.get(), plaintext.data() + len, &final_len) > 0) {
            plaintext.resize(len + final_len);
            decryptedOut.assign(reinterpret_cast<const char*>(plaintext.data()), plaintext.size());
            return true;
        }
    }
    return false;
}

void decryptNestedFields(json& data, const std::string& key) {
    if (data.is_object()) {
        std::vector<std::shared_ptr<bool>> completionFlags;

        for (auto& [k, v] : data.items())
        {
            if (k == "content" || k == "displayName" ||
                k == "username" || k == "comment" || k == "value" || k == "topic") {

                if (v.is_null() || v == "") {
                    continue; // long time decryption attempts on null values & errors if is null!. error if the string is empty as well
                }

                auto flag = std::make_shared<bool>(false);
                completionFlags.push_back(flag);

                ThreadPool->Run([&v, &key, flag]() {
                    std::string decrypted;
                    if (decryptCryptr(v.get<std::string>(), key, decrypted)) {
                        v = decrypted;
                    }
                    else {
                        std::cerr << "[!] Failed to decrypt key\n";
                    }
                    *flag = true; // !! IMPORTANT !!
                    });
            }
            else if (v.is_object() || v.is_array())
            {
                decryptNestedFields(v, key);
            }
        }

        bool allDone;
        do {
            allDone = true;
            for (const auto& flag : completionFlags)
            {
                if (!*flag) {
                    allDone = false;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    break;
                }
            }
        } while (!allDone);
    }
    else if (data.is_array())
    {
        for (auto& item : data) {
            if (item.is_object() || item.is_array())
            {
                decryptNestedFields(item, key);
            }
        }
    }
}