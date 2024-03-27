import 'dart:convert';
import 'package:dimlpfidex_gui/data/field.dart';
import 'package:dimlpfidex_gui/ui/simple_button.dart';
import 'package:file_selector/file_selector.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:flutter_form_builder/flutter_form_builder.dart';

// TODO(nice to have): improve different inputs decorations
class InputField extends StatefulWidget {
  final Field field;

  const InputField({super.key, required this.field});

  @override
  State<StatefulWidget> createState() => _InputFieldState();
}

class _InputFieldState extends State<InputField> {
  late Field field = widget.field;
  final inputFieldKey = GlobalKey<FormBuilderFieldState>();

  @override
  void initState() {
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return Padding(
        padding: const EdgeInsets.symmetric(vertical: 10.0),
        child: Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
          _buildField(),
          _buildDefaultValueButton(),
          _buildHelpIcon(context),
          _buildRequirementIcon()
        ]));
  }

  Widget _buildField() {
    switch (field.datatype) {
      case Datatype.integer || Datatype.doublePrecision:
        return _buildNumericField(context);
      case Datatype.boolean:
        return _buildBooleanField(context);
      case Datatype.filePath || Datatype.directoryPath:
        // TODO: differenciate file and directory
        return _buildFilePathPickerField(context);
      case Datatype.listInteger ||
            Datatype.listDoublePrecision ||
            Datatype.listString:
        return _buildTextField(
            context, _listInputValidator, _listValueTransformer);
      default:
        return _buildTextField(context, _basicInputValidator, null);
    }
  }

  Widget _buildBooleanField(BuildContext context) {
    return Padding(
        padding: const EdgeInsets.symmetric(vertical: 10.0),
        child: Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
          ConstrainedBox(
              constraints: BoxConstraints(
                  maxWidth: MediaQuery.of(context).size.width * 0.5),
              child: FormBuilderCheckbox(
                title: Text(
                  field.label,
                  style: TextStyle(fontSize: 17),
                ),
                key: inputFieldKey,
                name: field.jsonLabel,
                initialValue: field.defaultValue.toLowerCase() == 'true',
              ))
        ]));
  }

  Widget _buildNumericField(BuildContext context) {
    return Padding(
        padding: const EdgeInsets.symmetric(vertical: 10.0),
        child: Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
          ConstrainedBox(
              constraints: BoxConstraints(
                  maxWidth: MediaQuery.of(context).size.width * 0.5),
              child: Row(
                mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                children: [
                  SizedBox(
                      width: MediaQuery.of(context).size.width * 0.05,
                      child: field.minValue.isNotEmpty
                          ? Tooltip(
                              message:
                                  "Minimum value allowed is ${field.minValue}",
                              child: Row(
                                mainAxisAlignment:
                                    MainAxisAlignment.spaceBetween,
                                children: [
                                  Flexible(
                                      child: Text(field.minValue,
                                          overflow: TextOverflow.ellipsis,
                                          style:
                                              const TextStyle(fontSize: 17))),
                                  const Text("<",
                                      style: TextStyle(fontSize: 17)),
                                ],
                              ))
                          : null),
                  ConstrainedBox(
                      constraints: BoxConstraints(
                          maxWidth: MediaQuery.of(context).size.width * 0.25),
                      child: FormBuilderTextField(
                        key: inputFieldKey,
                        name: field.jsonLabel,
                        keyboardType: TextInputType.number,
                        decoration: InputDecoration(
                            label: Text(field.label),
                            border: const OutlineInputBorder()),
                        validator: (value) => _numericInputValidator(value),
                        valueTransformer: (value) {
                          if (value != null) {
                            switch (field.datatype) {
                              case Datatype.integer:
                                return int.parse(value);
                              case Datatype.doublePrecision:
                                return double.parse(value);
                              default:
                                break;
                            }
                          }
                          return null;
                        },
                      )),
                  SizedBox(
                      width: MediaQuery.of(context).size.width * 0.05,
                      child: field.maxValue.isNotEmpty
                          ? Tooltip(
                              message:
                                  "Maximum value allowed is ${field.maxValue}",
                              child: Row(
                                mainAxisAlignment:
                                    MainAxisAlignment.spaceBetween,
                                children: [
                                  const Text("<",
                                      style: TextStyle(fontSize: 17)),
                                  Flexible(
                                      child: Text(field.maxValue,
                                          overflow: TextOverflow.ellipsis,
                                          style:
                                              const TextStyle(fontSize: 17))),
                                ],
                              ))
                          : null),
                ],
              ))
        ]));
  }

  Widget _buildTextField(BuildContext context, Function(String?) validator,
      Function(String?, Datatype)? valueTransformer) {
    return Padding(
        padding: const EdgeInsets.symmetric(vertical: 10.0),
        child: Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
          ConstrainedBox(
              constraints: BoxConstraints(
                  maxWidth: MediaQuery.of(context).size.width * 0.5),
              child: FormBuilderTextField(
                key: inputFieldKey,
                name: field.jsonLabel,
                decoration: InputDecoration(label: Text(field.label)),
                keyboardType: TextInputType.multiline,
                validator: (value) => validator(value),
                valueTransformer: (value) =>
                    valueTransformer?.call(value, field.datatype) ?? value,
              ))
        ]));
  }

  Widget _buildFilePathPickerField(BuildContext context) {
    return Padding(
        padding: const EdgeInsets.symmetric(vertical: 10.0),
        child: Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
          ConstrainedBox(
              constraints: BoxConstraints(
                  maxWidth: MediaQuery.of(context).size.width * 0.5),
              child: FormBuilderTextField(
                key: inputFieldKey,
                name: field.jsonLabel,
                decoration: InputDecoration(label: Text(field.label)),
                keyboardType: TextInputType.multiline,
              )),
          if (!kIsWeb)
            Tooltip(
                message:
                    "Get the path of the file by browsing it. \nBe careful ! You should not use this feature if you already specified the root folder field!\nIf you did so, only specify the file name.",
                child: Padding(
                    padding: const EdgeInsets.only(top: 10),
                    child: IconButton(
                      icon: Icon(
                        Icons.file_open,
                        color: Theme.of(context).colorScheme.primary,
                      ),
                      onPressed: () => _askForPath(context, field.datatype),
                    )))
        ]));
  }

  Widget _buildDefaultValueButton() {
    return Tooltip(
        message: field.defaultValue.isNotEmpty
            ? "Autocomplete this field's value for me"
            : "There's no default value available to autocomplete this field",
        child: Padding(
            padding: const EdgeInsets.only(top: 10),
            child: SimpleButton(
                label: "Default value",
                isEnabled: field.defaultValue != "",
                padding: const EdgeInsets.symmetric(
                    vertical: 15.0, horizontal: 10.0),
                onPressed: () {
                  if (field.datatype != Datatype.boolean) {
                    inputFieldKey.currentState
                        ?.didChange(field.defaultValue.toString());
                  } else {
                    inputFieldKey.currentState
                        ?.didChange(field.defaultValue.toLowerCase() == 'true');
                  }
                },
                buttonType: ButtonType.action)));
  }

  Widget _buildHelpIcon(BuildContext context) {
    return Padding(
        padding: const EdgeInsets.only(top: 10),
        child: Tooltip(
            message: field.description,
            child: Icon(Icons.info,
                size: 25, color: Theme.of(context).colorScheme.primary)));
  }

  Widget _buildRequirementIcon() {
    Color iconColor = field.isRequired ? Colors.red[900]! : Colors.grey;
    String tooltipMsg = field.isRequired
        ? "This field must be filled"
        : "This field is optional";
    Icon icon = Icon(Icons.new_releases, color: iconColor);

    return Padding(
        padding: const EdgeInsets.only(top: 10),
        child: Tooltip(message: tooltipMsg, child: icon));
  }

  String? _basicInputValidator(String? value) {
    if (field.isRequired && (value == null || value.isEmpty)) {
      return "This field is required";
    }
    return null;
  }

  String? _numericInputValidator(String? value) {
    double? minValue = double.tryParse(field.minValue);
    double? maxValue = double.tryParse(field.maxValue);

    if (field.isRequired && value == null) {
      return "This field is required";
    } else if (value == null) {
      return null;
    }

    try {
      double? parsedValue = double.parse(value);
      if (minValue != null &&
          maxValue != null &&
          !(minValue <= parsedValue && parsedValue <= maxValue)) {
        return "This field does not respect the specified range of [${field.minValue},${field.maxValue}]";
      }
    } on FormatException {
      return "This field must be a numeric value";
    }

    return null;
  }

  String? _listInputValidator(String? value) {
    if (field.isRequired && value == null) {
      return "This field is required";
    } else if (value == null) {
      return null;
    }

    if (field.datatype != Datatype.listString) {
      if (RegExp("[^0-9, .]").hasMatch(value)) {
        return "There's a syntax error in your input";
      }
    }

    try {
      if (field.datatype != Datatype.listString) {
        json.decode("[$value]") as List<dynamic>;
      }
    } on FormatException {
      return "There's a JSON syntax error in your input";
    }

    return null;
  }

  List<dynamic> _listValueTransformer(String? value, Datatype datatype) {
    String formattedValue;

    if (datatype == Datatype.listString && value != null) {
      List<String> words = value.split(RegExp(r'\s*,\s*|\s+'));
      words = [for (String word in words) "\"$word\""];
      formattedValue = words.toString();
    } else {
      formattedValue = "[$value]";
    }

    return json.decode(formattedValue) as List<dynamic>;
  }

  void _askForPath(BuildContext context, Datatype type) async {
    String? path;
    
    try {
      switch (type) {
        case Datatype.filePath:
          path = await openFile().then((value) => value!.path);
          break;

        case Datatype.directoryPath:
          path = await getDirectoryPath();
          break;

        default:
          return;
      }

      if (path != null) {
        inputFieldKey.currentState?.didChange(path);
      }
    } catch (e) {
      // ignore: use_build_context_synchronously
      showSnackBar(context,
          "File choser has failed. Please, try again. (error: ${e.toString()})",
          color: Colors.red[500]!);
    }
  }
}

void showSnackBar(BuildContext context, String message,
    {Color color = const Color.fromARGB(255, 90, 90, 90),
    Duration duration = const Duration(seconds: 4)}) {
  ScaffoldMessenger.of(context).showSnackBar(SnackBar(
    content: Text(message),
    backgroundColor: color,
    duration: duration,
  ));
}
