#include "SecurityManager.h"

void SecurityManager::Init()
{
    if (sodium_init() < 0)
    {
        throw runtime_error("Failed to initialize libsodium");
    }
}

string SecurityManager::HashPassword(string_view password)
{
    char hashedPassword[crypto_pwhash_STRBYTES];

    if (crypto_pwhash_str_alg(
        hashedPassword,
        password.data(),
        static_cast<unsigned long long>(password.size()),
        crypto_pwhash_OPSLIMIT_MODERATE,
        crypto_pwhash_MEMLIMIT_MODERATE,
        crypto_pwhash_ALG_ARGON2ID13) != 0)
    {
        throw runtime_error("Password hashing failed");
    }

    return string(hashedPassword);
}

bool SecurityManager::VerifyPassword(string_view password, string_view storedHash)
{
    const string hashstring(storedHash);

    return crypto_pwhash_str_verify(
        hashstring.c_str(),
        password.data(),
        static_cast<unsigned long long>(password.size())) == 0;
}

bool SecurityManager::NeedsRehash(string_view storedHash)
{
    const string hashString(storedHash);

    return crypto_pwhash_str_needs_rehash( 
        hashString.c_str(),
        crypto_pwhash_OPSLIMIT_MODERATE,
        crypto_pwhash_MEMLIMIT_MODERATE) == 1;
}


