import 'dart:convert';
import 'package:dimlpfidex_gui/data/field.dart';

int? integerValueTransformer(String? value) {
  if (value == null || value.isEmpty) {
    return null;
  }
    
  return int.tryParse(value);
}

double? doubleValueTransformer(String? value) {
  if (value == null || value.isEmpty) {
    return null;
  }
    
  return double.tryParse(value);
}

// for iterables like lists or dictionaries that are represented by strings
String? iterableAsStringValueTransformer(String? value, Field field) {
  if (value == null || value.isEmpty) return null;

  // removing  spaces
  value = value.replaceAll(RegExp(r',(\ )+'), ',');

  if (field.datatype == Datatype.listFilePath) {
    // ensure no single brackets are present and surround value with brackets
    value = "[${value.replaceAll(RegExp(r'\[|\]'), '')}]";
  } else if (field.datatype == Datatype.dictionary) {
    value = "{${value.replaceAll(RegExp(r'\{|\}'), '')}}";
  }

  return value;
}

List<dynamic>? listValueTransformer(String? value, Field field) {
  if (value == null || value.isEmpty) return null;

  String formattedValue;
  List<String> words;

  if (field.datatype == Datatype.listString) {
    // add double quotes to words
    words = value.split(RegExp(r'\s*,\s*|\s+'));
    words = [for (String word in words) "\"$word\""];
    formattedValue = words.toString();
  } else {
    value = "[${value.replaceAll(RegExp(r'\[|\]'), '')}]";
    formattedValue = value;
  }

  return json.decode(formattedValue) as List<dynamic>;
}
