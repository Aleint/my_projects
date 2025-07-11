/**
 * @authors Alekos Interrante Bonadia
 * @authors Lorenzo Sebastiano Mathis
 * @authors Pietro Mazza
 * @version 1.0
 */

#ifndef CPP_SECUBEEXCEPTIONS_H
#define CPP_SECUBEEXCEPTIONS_H

#include <exception>
#include <string>

using namespace std;

enum class SECubeExceptionType {
    SECubeException,
    SECubeLoginException,
    SECubeDatabaseException,
    SECubeKeyException,
    Unknown
};


class SECubeException : public exception {
private:
    string message;

public:
    explicit SECubeException(const string& msg) : message(msg) {}

    virtual const char* what() const noexcept override {
        return message.c_str();
    }

    virtual SECubeExceptionType getType() const noexcept {
        return SECubeExceptionType::SECubeException;
    }
};

class SECubeLoginException : public SECubeException {
private:
    string message;

public:
    explicit SECubeLoginException(const string &msg) : SECubeException(msg), message(msg) {}

    virtual const char* what() const noexcept override {
        return message.c_str();
    }

    virtual SECubeExceptionType getType() const noexcept {
        return SECubeExceptionType::SECubeLoginException;
    }
};

class SECubeDatabaseException : public SECubeException {
private:
    string message;

public:
    explicit SECubeDatabaseException(const string &msg) : SECubeException(msg), message(msg) {};

    virtual const char* what() const noexcept override {
        return message.c_str();
    }

    virtual SECubeExceptionType getType() const noexcept {
        return SECubeExceptionType::SECubeDatabaseException;
    }
};

class SECubeKeyException : public SECubeException {
private:
    string message;

public:
    explicit SECubeKeyException(const string &msg) : SECubeException(msg), message(msg) {};

    virtual const char* what() const noexcept override {
        return message.c_str();
    }

    virtual SECubeExceptionType getType() const noexcept {
        return SECubeExceptionType::SECubeDatabaseException;
    }
};

#endif //CPP_SECUBEEXCEPTIONS_H
