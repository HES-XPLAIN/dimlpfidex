#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <exception>
#include <string>

class ErrorHandler : public std::exception {
private:
  std::string message_;

public:
  ErrorHandler(const std::string &message) : message_(message) {}

  const char *what() const noexcept override {
    return message_.c_str();
  }
};

class CommandArgumentException : public ErrorHandler {
public:
  CommandArgumentException(const std::string &message) : ErrorHandler("CommandArgumentException: " + message) {}
};

class FileNotFoundError : public ErrorHandler {
public:
  FileNotFoundError(const std::string &message) : ErrorHandler("FileNotFoundError: " + message) {}
};

class CannotOpenFileError : public ErrorHandler {
public:
  CannotOpenFileError(const std::string &message) : ErrorHandler("CannotOpenFileError: " + message) {}
};

class FileFormatError : public ErrorHandler {
public:
  FileFormatError(const std::string &message) : ErrorHandler("FileFormatError: " + message) {}
};

class FileContentError : public ErrorHandler {
public:
  FileContentError(const std::string &message) : ErrorHandler("FileContentError: " + message) {}
};

class InternalError : public ErrorHandler {
public:
  InternalError(const std::string &message) : ErrorHandler("InternalError: " + message) {}
};

class CannotCreateDirectoryError : public ErrorHandler {
public:
  CannotCreateDirectoryError(const std::string &message) : ErrorHandler("CannotCreateDirectoryError: " + message) {}
};

#endif
