import 'package:carousel_slider/carousel_slider.dart';
import 'package:dimlpfidex_gui/data/data_transformers.dart';
import 'package:dimlpfidex_gui/data/data_validators.dart';
import 'package:dimlpfidex_gui/data/field.dart';
import 'package:dimlpfidex_gui/ui/alerts.dart';
import 'package:dimlpfidex_gui/ui/simple_button.dart';
import 'package:file_selector/file_selector.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:flutter_form_builder/flutter_form_builder.dart';

class InputUnstableField extends StatefulWidget {
  final UnstableField field;

  const InputUnstableField({super.key, required this.field});

  @override
  State<StatefulWidget> createState() => _InputFieldState();
}

class _InputFieldState extends State<InputUnstableField> {
  late UnstableField field = widget.field;
  final inputFieldKey = GlobalKey<FormBuilderFieldState>();

  @override
  void initState() {
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return Padding(
        padding: const EdgeInsets.symmetric(vertical: 10.0),
        child: _buildFields());
  }

  Widget _buildFields() {
    List<Row> fields = [];
    for (var metadata in field.metadatas) {
      fields
          .add(Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
        _buildField(metadata),
        _buildDefaultValueButton(metadata),
        _buildHelpIcon(context, metadata),
        _buildRequirementIcon()
      ]));
    }

    return CarouselSlider(items: fields, options: CarouselOptions());
  }

  Widget _buildField(Metadata metadata) {
    switch (metadata.datatype) {
      case Datatype.integer || Datatype.doublePrecision:
        return _buildNumericField(context, metadata);

      case Datatype.restrictedChoiceString:
        return _buildStringDropdownField(context, metadata);

      case Datatype.boolean:
        return _buildBooleanField(context, metadata);

      case Datatype.filePath || Datatype.directoryPath:
        return _buildFilePathPickerField(context, metadata);

      case Datatype.listFilePath:
        return _buildFilePathPickerField(context, metadata);

      case Datatype.listInteger ||
            Datatype.listDoublePrecision ||
            Datatype.pairInteger ||
            Datatype.listString:
        return _buildTextField(
            context, metadata, listInputValidator, listValueTransformer);

      case Datatype.dictionary:
        return _buildTextField(context, metadata, dictInputValidator,
            iterableAsStringValueTransformer);

      default:
        return _buildTextField(context, metadata, basicInputValidator, null);
    }
  }

  Widget _buildBooleanField(BuildContext context, Metadata metadata) {
    return Padding(
        padding: const EdgeInsets.symmetric(vertical: 10.0),
        child: Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
          ConstrainedBox(
              constraints: BoxConstraints(
                  maxWidth: MediaQuery.of(context).size.width * 0.5),
              child: FormBuilderCheckbox(
                title: Text(
                  "[${metadata.datatype.name}] ${field.label}",
                  style: const TextStyle(fontSize: 17),
                ),
                key: inputFieldKey,
                name: field.jsonLabel,
                initialValue: metadata.defaultValue.toLowerCase() == 'true',
              ))
        ]));
  }

  Widget _buildNumericField(BuildContext context, Metadata metadata) {
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
                      child: metadata.minValue.isNotEmpty
                          ? Tooltip(
                              message:
                                  "Minimum value allowed is ${metadata.minValue}",
                              child: Row(
                                mainAxisAlignment:
                                    MainAxisAlignment.spaceBetween,
                                children: [
                                  Flexible(
                                      child: Text(metadata.minValue,
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
                            label: Text(
                                "[${metadata.datatype.name}] ${field.label}"),
                            border: const OutlineInputBorder()),
                        // validator: (value) => metadata.datatype == Datatype.integer
                        //     ? integerInputValidator(value, field)
                        //     : doubleInputValidator(value, field),
                        valueTransformer: (value) =>
                            metadata.datatype == Datatype.integer
                                ? integerValueTransformer(value)
                                : doubleValueTransformer(value),
                      )),
                  SizedBox(
                      width: MediaQuery.of(context).size.width * 0.05,
                      child: metadata.maxValue.isNotEmpty
                          ? Tooltip(
                              message:
                                  "Maximum value allowed is ${metadata.maxValue}",
                              child: Row(
                                mainAxisAlignment:
                                    MainAxisAlignment.spaceBetween,
                                children: [
                                  const Text("≤",
                                      style: TextStyle(fontSize: 17)),
                                  Flexible(
                                      child: Text(metadata.maxValue,
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
      Metadata metadata,
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
                decoration: InputDecoration(
                    label: Text("[${metadata.datatype.name}] ${field.label}")),
                keyboardType: TextInputType.multiline,
                // validator: (value) => validator(value, field),
                // valueTransformer: (value) =>
                //     valueTransformer?.call(value, field) ?? value,
              ))
        ]));
  }

  Widget _buildStringDropdownField(BuildContext context, Metadata metadata) {
    return Padding(
        padding: const EdgeInsets.symmetric(vertical: 10.0),
        child: Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
          ConstrainedBox(
              constraints: BoxConstraints(
                  maxWidth: MediaQuery.of(context).size.width * 0.5),
              child: FormBuilderDropdown<String>(
                key: inputFieldKey,
                name: field.jsonLabel,
                items: metadata.items
                    .map((item) => DropdownMenuItem(
                        alignment: Alignment.center,
                        value: item,
                        child: Text(item)))
                    .toList(),
                decoration: InputDecoration(
                    label: Text("[${metadata.datatype.name}] ${field.label}")),
              ))
        ]));
  }

  Widget _buildFilePathPickerField(BuildContext context, Metadata metadata) {
    return Padding(
        padding: const EdgeInsets.symmetric(vertical: 10.0),
        child: Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
          ConstrainedBox(
              constraints: BoxConstraints(
                  maxWidth: MediaQuery.of(context).size.width * 0.5),
              child: FormBuilderTextField(
                key: inputFieldKey,
                name: field.jsonLabel,
                decoration: InputDecoration(
                    label: Text("[${metadata.datatype.name}] ${field.label}")),
                keyboardType: TextInputType.multiline,
                // validator: (metadata.datatype == Datatype.listFilePath)
                //     ? (value) => listInputValidator(value, field)
                //     : null,
                // valueTransformer: (metadata.datatype == Datatype.listFilePath)
                //     ? (value) => iterableAsStringValueTransformer(value, field)
                //     : null,
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
                      onPressed: () => _askForPath(context, metadata.datatype),
                    )))
        ]));
  }

  Widget _buildDefaultValueButton(Metadata metadata) {
    return Tooltip(
        message: metadata.defaultValue.isNotEmpty
            ? "Autocomplete this field's value for me"
            : "There's no default value available to autocomplete this field",
        child: Padding(
            padding: const EdgeInsets.only(top: 10),
            child: SimpleButton(
                label: "Default value",
                isEnabled: metadata.defaultValue != "",
                padding: const EdgeInsets.symmetric(
                    vertical: 15.0, horizontal: 10.0),
                onPressed: () {
                  if (metadata.datatype != Datatype.boolean) {
                    inputFieldKey.currentState
                        ?.didChange(metadata.defaultValue.toString());
                  } else {
                    inputFieldKey.currentState?.didChange(
                        metadata.defaultValue.toLowerCase() == 'true');
                  }
                },
                buttonType: ButtonType.action)));
  }

  Widget _buildHelpIcon(BuildContext context, Metadata metadata) {
    return Padding(
        padding: const EdgeInsets.only(top: 10),
        child: Tooltip(
            message: metadata.description,
            child: Icon(Icons.info,
                size: 25, color: Theme.of(context).colorScheme.primary)));
  }

  Widget _buildRequirementIcon() {
    Color iconColor =
        field.isRequired ? AlertColor.warning : AlertColor.disabled;
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
          color: AlertColor.failure);
    }
  }
}
