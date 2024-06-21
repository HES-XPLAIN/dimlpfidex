import 'dart:convert';
import 'package:dimlpfidex_gui/data/field.dart';

String? basicInputValidator(String? value, Field field) {
  if (field.isRequired && (value == null || value.isEmpty)) {
    return "This field is required";
  }
  return null;
}

String? doubleInputValidator(String? value, Field field) {
  double? minValue = double.tryParse(field.minValue);
  double? maxValue = double.tryParse(field.maxValue);

  // check if required and if empty
  if (field.isRequired && (value == null || value.isEmpty)) {
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
    return "Your input cannot be lower than ${field.minValue}";
  }

  if (maxValue != null && maxValue < parsedValue) {
    return "Your input cannot be upper than ${field.maxValue}";
  }

  return null;
}

String? integerInputValidator(String? value, Field field) {
  int? minValue = int.tryParse(field.minValue);
  int? maxValue = int.tryParse(field.maxValue);

  // check if required and if empty
  if (field.isRequired && (value == null || value.isEmpty)) {
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
    return "Your input cannot be lower than ${field.minValue}";
  }

  if (maxValue != null && maxValue < parsedValue) {
    return "Your input cannot be upper than ${field.maxValue}";
  }

  return null;
}

String? listInputValidator(String? value, Field field) {
  if (field.isRequired && value == null) {
    return "This field is required";
  } else if (value == null || value == "") {
    return null;
  }

  if (field.datatype != Datatype.listString &&
      field.datatype != Datatype.listFilePath) {
    if (RegExp("[a-zA-Z]").hasMatch(value)) {
      return "An integer collection cannot contain letters";
    }
  }

  if (field.datatype == Datatype.pairInteger) {
    if (!RegExp(r"^\[?([0-9]+((,| )+)[0-9]+)(\ )*\]?$").hasMatch(value)) {
      return "Pairs of integers cannot be anything else than a list of only 2 integers";
    }
  }

  try {
    if (field.datatype != Datatype.listString &&
        field.datatype != Datatype.listFilePath) {
      value = "[${value.replaceAll(RegExp(r'\[|\]'), '')}]";
      json.decode(value) as List<dynamic>;
    }
  } on FormatException {
    return "There's a syntax error in your collection (ex: missing comma)";
  }

  return null;
}

String? dictInputValidator(String? value, Field field) {
  if (field.isRequired && value == null) {
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
