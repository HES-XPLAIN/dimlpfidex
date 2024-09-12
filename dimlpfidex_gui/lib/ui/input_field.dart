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
  final Field field;

  const InputUnstableField({super.key, required this.field});

  @override
  State<StatefulWidget> createState() => _InputFieldState();
}

class _InputFieldState extends State<InputUnstableField> {
  final inputFieldKey = GlobalKey<FormBuilderFieldState>();
  late Field field = widget.field;
  late Metadata _currentMetadata;

  @override
  void initState() {
    super.initState();
    _currentMetadata = field.metadatas[0];
  }

  @override
  Widget build(BuildContext context) {
    return Padding(
        padding: const EdgeInsets.only(bottom: 30),
        child: Row(mainAxisAlignment: MainAxisAlignment.center, children: [
          _buildRadioButtons(field),
          Flexible(child: _buildField()),
          _buildInteractibles(context),
          const SizedBox(width: 100)
        ]));
  }

  Widget _buildField() {
    switch (_currentMetadata.datatype) {
      case Datatype.integer || Datatype.doublePrecision:
        return _buildNumericField(context);

      case Datatype.restrictedChoiceString:
        return _buildStringDropdownField(context);

      case Datatype.boolean:
        return _buildBooleanField(context);

      case Datatype.filePath || Datatype.directoryPath || Datatype.listFilePath:
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
    return FormBuilderCheckbox(
      key: inputFieldKey,
      name: field.jsonLabel,
      title: Text(
        "[${_currentMetadata.datatype.name}] ${field.label}",
        style: const TextStyle(fontSize: 17),
      ),
      initialValue: _currentMetadata.defaultValue.toLowerCase() == 'true',
    );
  }

  Widget _buildNumericField(BuildContext context) {
    Container spacing = Container(width: 30);

    return Row(
        mainAxisAlignment: MainAxisAlignment.spaceBetween,
        crossAxisAlignment: CrossAxisAlignment.center,
        children: [
          if (_currentMetadata.minValue.isNotEmpty) _buildIntervalText(false),
          if (_currentMetadata.minValue.isNotEmpty) spacing,
          Flexible(
              child: FormBuilderTextField(
            key: inputFieldKey,
            name: field.jsonLabel,
            keyboardType: TextInputType.number,
            decoration: InputDecoration(
                label:
                    Text("[${_currentMetadata.datatype.name}] ${field.label}"),
                border: const OutlineInputBorder()),
            validator: (value) => _currentMetadata.datatype == Datatype.integer
                ? integerInputValidator(
                    value, field.isRequired, _currentMetadata)
                : doubleInputValidator(
                    value, field.isRequired, _currentMetadata),
            valueTransformer: (value) =>
                _currentMetadata.datatype == Datatype.integer
                    ? integerValueTransformer(value)
                    : doubleValueTransformer(value),
          )),
          if (_currentMetadata.maxValue.isNotEmpty) spacing,
          if (_currentMetadata.maxValue.isNotEmpty) _buildIntervalText(true)
        ]);
  }

  Widget _buildTextField(
      BuildContext context,
      Function(String?, bool, Metadata) validator,
      Function(String?, Metadata)? valueTransformer) {
    return FormBuilderTextField(
      key: inputFieldKey,
      name: field.jsonLabel,
      decoration: InputDecoration(
          label: Text("[${_currentMetadata.datatype.name}] ${field.label}")),
      keyboardType: TextInputType.multiline,
      validator: (value) =>
          validator(value, field.isRequired, _currentMetadata),
      valueTransformer: (value) =>
          valueTransformer?.call(value, _currentMetadata) ?? value,
    );
  }

  Widget _buildStringDropdownField(BuildContext context) {
    return FormBuilderDropdown<String>(
      key: inputFieldKey,
      name: field.jsonLabel,
      items: _currentMetadata.items
          .map((item) => DropdownMenuItem(
              alignment: Alignment.center, value: item, child: Text(item)))
          .toList(),
      decoration: InputDecoration(
          label: Text("[${_currentMetadata.datatype.name}] ${field.label}")),
    );
  }

  Widget _buildFilePathPickerField(BuildContext context) {
    return FormBuilderTextField(
      key: inputFieldKey,
      name: field.jsonLabel,
      decoration: InputDecoration(
          label: Text("[${_currentMetadata.datatype.name}] ${field.label}")),
      keyboardType: TextInputType.multiline,
      validator: (_currentMetadata.datatype == Datatype.listFilePath)
          ? (value) =>
              listInputValidator(value, field.isRequired, _currentMetadata)
          : null,
      valueTransformer: (_currentMetadata.datatype == Datatype.listFilePath)
          ? (value) => iterableAsStringValueTransformer(value, _currentMetadata)
          : null,
    );
  }

  Widget _buildRadioButtons(Field field) {
    final List<Widget> radioBtns = [];
    double width = 100.0;

    if (field.metadatas.length < 2) {
      return SizedBox(width: width);
    }

    for (final metadata in field.metadatas) {
      radioBtns.add(Radio<Metadata>(
          value: metadata,
          groupValue: _currentMetadata,
          onChanged: (Metadata? value) {
            setState(() => _currentMetadata = value!);
          }));
    }

    return SizedBox(
        width: width,
        child: Row(
            mainAxisAlignment: MainAxisAlignment.center, children: radioBtns));
  }

  Widget _buildInteractibles(BuildContext context) {
    Container spacing = Container(width: 10);

    bool isFileRelatedField = _currentMetadata.datatype == Datatype.filePath ||
        _currentMetadata.datatype == Datatype.listFilePath ||
        _currentMetadata.datatype == Datatype.directoryPath;

    return Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
      !kIsWeb && isFileRelatedField
          ? _buildFilePickerButton()
          : Container(width: 30),
      spacing,
      _buildDefaultValueButton(),
      spacing,
      _buildHelpIcon(context),
      spacing,
      _buildRequirementIcon()
    ]);
  }

  Widget _buildIntervalText(bool isGreater) {
    const double fontSize = 20.0;

    return Tooltip(
        message: isGreater
            ? "Maximum value allowed is ${_currentMetadata.maxValue}"
            : "Minimum value allowed is ${_currentMetadata.minValue}",
        child: Row(
          mainAxisAlignment: MainAxisAlignment.spaceEvenly,
          crossAxisAlignment: CrossAxisAlignment.center,
          children: [
            // if is lower bound display minValue ≤ ...
            if (!isGreater)
              Text(_currentMetadata.minValue,
                  overflow: TextOverflow.ellipsis,
                  style: const TextStyle(fontSize: fontSize)),
            if (!isGreater)
              const Text("≤", style: TextStyle(fontSize: fontSize)),

            // if is lower bound display ... > maxValue
            if (isGreater)
              const Text("≤", style: TextStyle(fontSize: fontSize)),
            if (isGreater)
              Text(_currentMetadata.maxValue,
                  overflow: TextOverflow.ellipsis,
                  style: const TextStyle(fontSize: fontSize)),
          ],
        ));
  }

  Widget _buildFilePickerButton() {
    return Tooltip(
        message:
            "Get the path of the file by browsing it. \nBe careful ! You should not use this feature if you already specified the root folder field!\nIf you did so, only specify the file name.",
        child: Padding(
            padding: const EdgeInsets.only(top: 10),
            child: IconButton(
              icon: Icon(
                Icons.file_open,
                color: Theme.of(context).colorScheme.primary,
              ),
              onPressed: () => _askForPath(context, _currentMetadata.datatype),
            )));
  }

  Widget _buildDefaultValueButton() {
    return Tooltip(
        message: _currentMetadata.defaultValue.isNotEmpty
            ? "Autocomplete this field's value for me"
            : "There's no default value available to autocomplete this field",
        child: Padding(
            padding: const EdgeInsets.only(top: 10),
            child: SimpleButton(
                label: "Default value",
                isEnabled: _currentMetadata.defaultValue != "",
                padding: const EdgeInsets.symmetric(
                    vertical: 15.0, horizontal: 10.0),
                onPressed: () {
                  if (_currentMetadata.datatype != Datatype.boolean) {
                    inputFieldKey.currentState
                        ?.didChange(_currentMetadata.defaultValue.toString());
                  } else {
                    inputFieldKey.currentState?.didChange(
                        _currentMetadata.defaultValue.toLowerCase() == 'true');
                  }
                },
                buttonType: ButtonType.action)));
  }

  Widget _buildHelpIcon(BuildContext context) {
    return Padding(
        padding: const EdgeInsets.only(top: 10),
        child: Tooltip(
            message: _currentMetadata.description,
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
    try {
      switch (type) {
        case Datatype.listFilePath:
          await openFiles().then((files) => inputFieldKey.currentState
              ?.didChange(files
                  .map((file) => file.path)
                  .toList()
                  .toString()
                  .replaceAll(RegExp(r'(\[|\])+'), '')));

        case Datatype.filePath:
          await openFile().then(
              (value) => inputFieldKey.currentState?.didChange(value!.path));
          break;

        case Datatype.directoryPath:
          await getDirectoryPath()
              .then((path) => inputFieldKey.currentState?.didChange(path));
          break;

        default:
          return;
      }
    } catch (e) {
      // ignore: use_build_context_synchronously
      showSnackBar(context,
          "File choser has failed. Please, try again. (error: ${e.toString()})",
          color: AlertColor.failure);
    }
  }
}
