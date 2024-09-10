import "dart:convert";
import "dart:io";
import "package:dimlpfidex_gui/data/field.dart";
import "package:dimlpfidex_gui/ui/alerts.dart";
import "package:dimlpfidex_gui/ui/simple_button.dart";
import "package:flutter/foundation.dart";
import "package:flutter/material.dart";
import "package:flutter_form_builder/flutter_form_builder.dart";
import "package:shared_preferences/shared_preferences.dart";
import "package:universal_html/html.dart" as universal_html;

class FormView extends StatefulWidget {
  final List<Field> fields;
  final TabController tabController;
  const FormView(
      {super.key, required this.fields, required this.tabController});

  @override
  State<StatefulWidget> createState() => _FormViewState();
}

class _FormViewState extends State<FormView>
    with AutomaticKeepAliveClientMixin {
  final GlobalKey<FormBuilderState> _key = GlobalKey<FormBuilderState>();

  @override
  bool get wantKeepAlive => true;

  @override
  Widget build(BuildContext context) {
    super.build(context);
    return SingleChildScrollView(
        child: Padding(
      padding: const EdgeInsets.only(top: 30.0, bottom: 100.0),
      child: FormBuilder(
          key: _key,
          child: Column(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              crossAxisAlignment: CrossAxisAlignment.center,
              children: _createFields(
                  widget.fields, _key, context, widget.tabController))),
    ));
  }
}

List<Widget> _createFields(
    List<Field> fields,
    GlobalKey<FormBuilderState> key,
    BuildContext context,
    TabController tabController) {
  List<Widget> result = [];

  for (Field field in fields) {
    result.add(field.toInputField());
  }

// action buttons
  result.add(Row(
    mainAxisAlignment: MainAxisAlignment.spaceEvenly,
    children: [
      SimpleButton(
          onPressed: () => _validateForm(key, context), label: "Generate"),
      SimpleButton(
        onPressed: () => _resetForm(context, tabController, key),
        label: "Reset",
        buttonType: ButtonType.cancel,
      )
    ],
  ));
  return result;
}

void _validateForm(GlobalKey<FormBuilderState> key, BuildContext context) {
  if (key.currentState!.saveAndValidate()) {
    showSnackBar(context, "Generating JSON file...",
        duration: const Duration(milliseconds: 800));
    _generateJson(key, context);
  } else {
    showSnackBar(context, 'Input(s) are missing.', color: AlertColor.failure);
  }
}

Future<void> _resetForm(BuildContext context, TabController tabController,
    GlobalKey<FormBuilderState> key) {
  return showDialog(
      context: context,
      barrierDismissible: false,
      builder: (BuildContext context) {
        return AlertDialog(
          title: const Text("Warning"),
          content: const SingleChildScrollView(
            child: ListBody(
              children: [
                Text("You're about to reset all fields present in this form."),
                Text(
                    "If you do so, all fields will be emptied and progression will be lost."),
                Text("\nWould you like to proceed ?"),
              ],
            ),
          ),
          actions: [
            SimpleButton(
                onPressed: () {
                  key.currentState!.reset();
                  showSnackBar(context, 'All fields have been reset');
                  Navigator.of(context).pop();
                },
                label: "Confirm"),
            SimpleButton(
                onPressed: () {
                  Navigator.of(context).pop();
                },
                label: "Cancel",
                buttonType: ButtonType.cancel)
          ],
        );
      });
}

Future<void> _generateJson(
    GlobalKey<FormBuilderState> key, BuildContext context) async {
  Map<String, dynamic> values = Map.from(key.currentState!.value)
    ..removeWhere((dynamic key, dynamic value) => value == null || value == "");

  try {
    JsonEncoder encoder = const JsonEncoder.withIndent("  ");
    String json = encoder.convert(values);

    if (kIsWeb) {
      _produceFileForWeb(json, context);
    } else {
      _produceFileForOS(json, context);
    }
  } on JsonUnsupportedObjectError catch (e) {
    showSnackBar(context, "JSON generation as failed... (error: $e)",
        color: AlertColor.failure);
  }
}

void _produceFileForWeb(String json, BuildContext context) {
  try {
    universal_html.AnchorElement(
        href: universal_html.Url.createObjectUrlFromBlob(
                universal_html.Blob([json], "text/plain", "native"))
            .toString())
      ..setAttribute("download", "config.json")
      ..click();
    showSnackBar(context, 'JSON file successfully generated !',
        color: AlertColor.success);
  } catch (error) {
    showSnackBar(context, "JSON file generation error: $error",
        color: AlertColor.failure);
  }
}

void _produceFileForOS(String json, BuildContext context) async {
  String pathKey = "generationPath";
  String? path;

  await SharedPreferences.getInstance()
      .then((sp) => sp.getString(pathKey))
      .then((res) {
        path = res;
        return File('$path/config.json');
      })
      .then((file) => file.writeAsString(json))
      .then((_) => showSnackBar(
          context, 'Configuration file successfully generated at $path',
          color: AlertColor.success))
      .onError((error, _) => showSnackBar(
          context, 'Configuration file generation error: $error',
          color: AlertColor.failure));
}
