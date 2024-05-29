#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <exception>
#include <string>

/**
 * @brief Base class for handling exceptions with custom messages.
 *
 * ErrorHandler extends the standard std::exception class to allow for exceptions
 * that carry custom messages. These messages are passed during construction and
 * retrieved via the what() method.
 */
class ErrorHandler : public std::exception {
private:
  std::string message_;

public:
  /**
   * @brief Constructs an ErrorHandler with a custom message.
   * @param message The message detailing the cause of the exception.
   */
  explicit ErrorHandler(const std::string &message) : message_(message) {}

  /**
   * @brief Returns the message associated with the exception.
   * @return The message for the exception.
   */
  const char *what() const noexcept override {
    return message_.c_str();
  }
};

/**
 * @brief Exception for invalid command line arguments.
 */
class CommandArgumentException : public ErrorHandler {
public:
  /**
   * @brief Constructs a CommandArgumentException with a custom message.
   * @param message The message detailing the cause of the exception.
   */
  explicit CommandArgumentException(const std::string &message) : ErrorHandler("CommandArgumentException: " + message) {}
};

/**
 * @brief Exception indicating that a required file could not be found.
 */
class FileNotFoundError : public ErrorHandler {
public:
  /**
   * @brief Constructs a FileNotFoundError with a custom message.
   * @param message The message detailing the cause of the exception.
   */
  explicit FileNotFoundError(const std::string &message) : ErrorHandler("FileNotFoundError: " + message) {}
};

/**
 * @brief Exception for errors when opening a file.
 */
class CannotOpenFileError : public ErrorHandler {
public:
  /**
   * @brief Constructs a CannotOpenFileError with a custom message.
   * @param message The message detailing the cause of the exception.
   */
  explicit CannotOpenFileError(const std::string &message) : ErrorHandler("CannotOpenFileError: " + message) {}
};

/**
 * @brief Exception for errors related to incorrect file format.
 */
class FileFormatError : public ErrorHandler {
public:
  /**
   * @brief Constructs a FileFormatError with a custom message.
   * @param message The message detailing the cause of the exception.
   */
  explicit FileFormatError(const std::string &message) : ErrorHandler("FileFormatError: " + message) {}
};

/**
 * @brief Exception for errors related to incorrect or unexpected content within a file.
 */
class FileContentError : public ErrorHandler {
public:
  /**
   * @brief Constructs a FileContentError with a custom message.
   * @param message The message detailing the cause of the exception.
   */
  explicit FileContentError(const std::string &message) : ErrorHandler("FileContentError: " + message) {}
};

/**
 * @brief Exception for internal errors not covered by more specific exception types.
 */
class InternalError : public ErrorHandler {
public:
  /**
   * @brief Constructs an InternalError with a custom message.
   * @param message The message detailing the cause of the exception.
   */
  explicit InternalError(const std::string &message) : ErrorHandler("InternalError: " + message) {}
};

/**
 * @brief Exception for errors when creating a directory.
 */
class CannotCreateDirectoryError : public ErrorHandler {
public:
  /**
   * @brief Constructs a CannotCreateDirectoryError with a custom message.
   * @param message The message detailing the cause of the exception.
   */
  explicit CannotCreateDirectoryError(const std::string &message) : ErrorHandler("CannotCreateDirectoryError: " + message) {}
};

#endif
