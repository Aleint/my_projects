cmake_minimum_required(VERSION 3.10)
project(L1)

set(CMAKE_CXX_STANDARD 14)

# Aggiunta di sorgenti della libreria
set(LIB_SOURCES
        L1.cpp
        L1_Base/L1_base.cpp
        L1_login_logout.cpp
        L1_security.cpp
        L1_sekey.cpp
        Crypto_Libraries/aes256.cpp
        Crypto_Libraries/pbkdf2.c
        Crypto_Libraries/sha256.c
)

# Aggiunta degli header della libreria
set(LIB_HEADERS
        L1.h
        L1_Base/L1_base.h
        L1_enumerations.h
        L1_error_manager.h
        Login_Logout_API/login_logout_api.h
        Security_API/security_api.h
        Utility_API/utility_api.h
        Crypto_Libraries/aes256.h
        Crypto_Libraries/pbkdf2.h
        Crypto_Libraries/sha256.h
)

# Aggiunta della libreria statica
add_library(L1 STATIC ${LIB_SOURCES} ${LIB_HEADERS})