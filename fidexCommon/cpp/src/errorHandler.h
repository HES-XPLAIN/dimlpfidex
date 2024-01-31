#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <exception>
#include <string>

class errorHandler : public std::exception {
private:
  std::string message_;

public:
  errorHandler(const std::string &message) : message_(message) {}

  const char *what() const noexcept override {
    return message_.c_str();
  }
};

class CommandArgumentException : public errorHandler {
public:
  CommandArgumentException(const std::string &message) : errorHandler("CommandArgumentException: " + message) {}
};

class FileNotFoundError : public errorHandler {
public:
  FileNotFoundError(const std::string &message) : errorHandler("FileNotFoundError: " + message) {}
};

class CannotOpenFileError : public errorHandler {
public:
  CannotOpenFileError(const std::string &message) : errorHandler("CannotOpenFileError: " + message) {}
};

class FileFormatError : public errorHandler {
public:
  FileFormatError(const std::string &message) : errorHandler("FileFormatError: " + message) {}
};

class FileContentError : public errorHandler {
public:
  FileContentError(const std::string &message) : errorHandler("FileContentError: " + message) {}
};

class InternalError : public errorHandler {
public:
  InternalError(const std::string &message) : errorHandler("InternalError: " + message) {}
};

class CannotCreateDirectoryError : public errorHandler {
public:
  CannotCreateDirectoryError(const std::string &message) : errorHandler("CannotCreateDirectoryError: " + message) {}
};

#endif
