#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <exception>
#include <string>

class errorHandler : public std::exception {
public:
  explicit errorHandler(const std::string &message) : message_(message) {}

  const char *what() const noexcept override {
    return message_.c_str();
  }

private:
  std::string message_;
};

class CommandArgumentException : public errorHandler {
public:
  using errorHandler::errorHandler;
};

class FileNotFoundError : public errorHandler {
public:
  using errorHandler::errorHandler;
};

class CannotOpenFileError : public errorHandler {
public:
  using errorHandler::errorHandler;
};

class FileFormatError : public errorHandler {
public:
  using errorHandler::errorHandler;
};

class FileContentError : public errorHandler {
public:
  using errorHandler::errorHandler;
};

class InternalError : public errorHandler {
public:
  using errorHandler::errorHandler;
};

class CannotCreateDirectoryError : public errorHandler {
public:
  using errorHandler::errorHandler;
};

#endif
