import 'package:dimlpfidex_gui/data/data_transformers.dart';
import 'package:dimlpfidex_gui/data/data_validators.dart';
import 'package:dimlpfidex_gui/data/field.dart';
import 'package:dimlpfidex_gui/ui/simple_button.dart';
import 'package:file_selector/file_selector.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:flutter_form_builder/flutter_form_builder.dart';

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

      case Datatype.restrictedChoiceString:
        return _buildStringDropdownField(context);

      case Datatype.boolean:
        return _buildBooleanField(context);

      case Datatype.filePath || Datatype.directoryPath:
        return _buildFilePathPickerField(context);

      case Datatype.listFilePath:
        return _buildFilePathPickerField(context);

      case Datatype.listInteger ||
            Datatype.listDoublePrecision ||
            Datatype.pairInteger ||
            Datatype.listString:
        return _buildTextField(
            context, listInputValidator, listValueTransformer);

      case Datatype.dictionary:
        return _buildTextField(
            context, dictInputValidator, iterableAsStringValueTransformer);

      default:
        return _buildTextField(context, basicInputValidator, null);
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
                  "[${field.datatype.name}] ${field.label}",
                  style: const TextStyle(fontSize: 17),
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
                                  const Text("≤",
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
                            label: Text("[${field.datatype.name}] ${field.label}"),
                            border: const OutlineInputBorder()),
                        validator: (value) => field.datatype == Datatype.integer
                            ? integerInputValidator(value, field)
                            : doubleInputValidator(value, field),
                        valueTransformer: (value) =>
                            field.datatype == Datatype.integer
                                ? integerValueTransformer(value)
                                : doubleValueTransformer(value),
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
                                  const Text("≤",
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

  Widget _buildTextField(
      BuildContext context,
      Function(String?, Field) validator,
      Function(String?, Field)? valueTransformer) {
    return Padding(
        padding: const EdgeInsets.symmetric(vertical: 10.0),
        child: Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
          ConstrainedBox(
              constraints: BoxConstraints(
                  maxWidth: MediaQuery.of(context).size.width * 0.5),
              child: FormBuilderTextField(
                key: inputFieldKey,
                name: field.jsonLabel,
                decoration: InputDecoration(label: Text("[${field.datatype.name}] ${field.label}")),
                keyboardType: TextInputType.multiline,
                validator: (value) => validator(value, field),
                valueTransformer: (value) =>
                    valueTransformer?.call(value, field) ?? value,
              ))
        ]));
  }

  Widget _buildStringDropdownField(BuildContext context) {
    return Padding(
        padding: const EdgeInsets.symmetric(vertical: 10.0),
        child: Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
          ConstrainedBox(
              constraints: BoxConstraints(
                  maxWidth: MediaQuery.of(context).size.width * 0.5),
              child: FormBuilderDropdown<String>(
                key: inputFieldKey,
                name: field.jsonLabel,
                items: field.items
                    .map((item) => DropdownMenuItem(
                        alignment: Alignment.center,
                        value: item,
                        child: Text(item)))
                    .toList(),
                decoration: InputDecoration(label: Text("[${field.datatype.name}] ${field.label}")),
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
                decoration: InputDecoration(label: Text("[${field.datatype.name}] ${field.label}")),
                keyboardType: TextInputType.multiline,
                validator: (field.datatype == Datatype.listFilePath)
                    ? (value) => listInputValidator(value, field)
                    : null,
                valueTransformer: (field.datatype == Datatype.listFilePath)
                    ? (value) => iterableAsStringValueTransformer(value, field)
                    : null,
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

  void _askForPath(BuildContext context, Datatype type) async {
    String? path;

    try {
      switch (type) {
        case Datatype.listFilePath:
          await openFiles().then((files) => path = files
              .map((file) => file.path)
              .toList()
              .toString()
              .replaceAll(RegExp(r'(\[|\])+'), ''));

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
