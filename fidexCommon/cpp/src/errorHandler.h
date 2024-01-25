#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <exception>
#include <string>

class errorHandler : public std::exception {

private:
  std::string message_;

protected:
  const std::string &getMessage() const { return message_; }

public:
  explicit errorHandler(const std::string &message) : message_(message) {}

  const char *what() const noexcept override {
    return message_.c_str();
  }
  virtual const std::string &getError() const = 0;
};

class CommandArgumentException : public errorHandler {
public:
  using errorHandler::errorHandler;
  const std::string &getError() const override {
    static const std::string errorType = "CommandArgumentException";
    return errorType;
  }
  const char *what() const noexcept override {
    static std::string fullMessage = getError() + ": " + getMessage();
    return fullMessage.c_str();
  }
};

class FileNotFoundError : public errorHandler {
public:
  using errorHandler::errorHandler;
  const std::string &getError() const override {
    static const std::string errorType = "FileNotFoundError";
    return errorType;
  }
  const char *what() const noexcept override {
    static std::string fullMessage = getError() + ": " + getMessage();
    return fullMessage.c_str();
  }
};

class CannotOpenFileError : public errorHandler {
public:
  using errorHandler::errorHandler;
  const std::string &getError() const override {
    static const std::string errorType = "CannotOpenFileError";
    return errorType;
  }
  const char *what() const noexcept override {
    static std::string fullMessage = getError() + ": " + getMessage();
    return fullMessage.c_str();
  }
};

class FileFormatError : public errorHandler {
public:
  using errorHandler::errorHandler;
  const std::string &getError() const override {
    static const std::string errorType = "FileFormatError";
    return errorType;
  }
  const char *what() const noexcept override {
    static std::string fullMessage = getError() + ": " + getMessage();
    return fullMessage.c_str();
  }
};

class FileContentError : public errorHandler {
public:
  using errorHandler::errorHandler;
  const std::string &getError() const override {
    static const std::string errorType = "FileContentError";
    return errorType;
  }
  const char *what() const noexcept override {
    static std::string fullMessage = getError() + ": " + getMessage();
    return fullMessage.c_str();
  }
};

class InternalError : public errorHandler {
public:
  using errorHandler::errorHandler;
  const std::string &getError() const override {
    static const std::string errorType = "InternalError";
    return errorType;
  }
  const char *what() const noexcept override {
    static std::string fullMessage = getError() + ": " + getMessage();
    return fullMessage.c_str();
  }
};

class CannotCreateDirectoryError : public errorHandler {
public:
  using errorHandler::errorHandler;
  const std::string &getError() const override {
    static const std::string errorType = "CannotCreateDirectoryError";
    return errorType;
  }
  const char *what() const noexcept override {
    static std::string fullMessage = getError() + ": " + getMessage();
    return fullMessage.c_str();
  }
};

#endif
