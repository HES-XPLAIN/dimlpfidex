import 'dart:convert';
import 'package:dimlpfidex_gui/data/field.dart';

String? basicInputValidator(String? value, bool isRequired, Metadata metadata) {
  if (isRequired && (value == null || value.isEmpty)) {
    return "This field is required";
  }
  return null;
}

String? doubleInputValidator(String? value, bool isRequired, Metadata metadata) {
  double? minValue = double.tryParse(metadata.minValue);
  double? maxValue = double.tryParse(metadata.maxValue);

  // check if required and if empty
  if (isRequired && (value == null || value.isEmpty)) {
    return "This field is required";
  } else if (value == null || value.isEmpty) {
    return null;
  }

  // check parsing error
  double? parsedValue = double.tryParse(value);
  if (parsedValue == null) {
    return "This field must be a valid floating point number";
  }

  if (minValue != null && minValue > parsedValue) {
    return "Your input cannot be lower than ${metadata.minValue}";
  }

  if (maxValue != null && maxValue < parsedValue) {
    return "Your input cannot be upper than ${metadata.maxValue}";
  }

  return null;
}

String? integerInputValidator(String? value, bool isRequired, Metadata metadata) {
  int? minValue = int.tryParse(metadata.minValue);
  int? maxValue = int.tryParse(metadata.maxValue);

  // check if required and if empty
  if (isRequired && (value == null || value.isEmpty)) {
    return "This field is required";
  } else if (value == null || value.isEmpty) {
    return null;
  }

  // check parsing error
  int? parsedValue = int.tryParse(value);
  if (parsedValue == null) {
    return "This field must be a valid integer";
  }

  if (minValue != null && minValue > parsedValue) {
    return "Your input cannot be lower than ${metadata.minValue}";
  }

  if (maxValue != null && maxValue < parsedValue) {
    return "Your input cannot be upper than ${metadata.maxValue}";
  }

  return null;
}

String? listInputValidator(String? value, bool isRequired, Metadata metadata) {
  if (isRequired && value == null) {
    return "This field is required";
  } else if (value == null || value == "") {
    return null;
  }

  if (metadata.datatype != Datatype.listString &&
      metadata.datatype != Datatype.listFilePath) {
    if (RegExp("[a-zA-Z]").hasMatch(value)) {
      return "An integer collection cannot contain letters";
    }
  }

  if (metadata.datatype == Datatype.pairInteger) {
    if (!RegExp(r"^\[?([0-9]+((,| )+)[0-9]+)(\ )*\]?$").hasMatch(value)) {
      return "Pairs of integers cannot be anything else than a list of only 2 integers";
    }
  }

  try {
    if (metadata.datatype != Datatype.listString &&
        metadata.datatype != Datatype.listFilePath) {
      value = "[${value.replaceAll(RegExp(r'\[|\]'), '')}]";
      json.decode(value) as List<dynamic>;
    }
  } on FormatException {
    return "There's a syntax error in your collection (ex: missing comma)";
  }

  return null;
}

String? dictInputValidator(String? value, bool isRequired, Metadata metadata) {
  if (isRequired && value == null) {
    return "This field is required";
  } else if (value == null || value == "") {
    return null;
  }

  if (!RegExp(r"^\{?(([0-9]+\:[0-9]+(\.[0-9]+)*)(,|\ )*)+\}?$")
      .hasMatch(value)) {
    return "There's a syntax error in your dictionnary (ex: missing ':')";
  }

  return null;
}

// This validator has limitations, it can only guarantee that the input is a numeric value
String? numberInputValidator(String? value, bool isRequired, Metadata metadata) {
  var resDouble = doubleInputValidator(value, isRequired, metadata);
  var resInteger = integerInputValidator(value, isRequired, metadata);
  
  if (resDouble != null && resInteger != null) {
    return "Your input was not recognised as an integer nor floating point number.";
  }

  return null;
}