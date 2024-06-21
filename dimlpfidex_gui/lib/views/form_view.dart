import "dart:convert";
import "dart:io";
import "package:dimlpfidex_gui/data/field.dart";
import "package:dimlpfidex_gui/ui/input_field.dart";
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
            child: FractionallySizedBox(
              alignment: Alignment.topCenter,
              widthFactor: 0.7,
              child: FormBuilder(
                  key: _key,
                  child: Column(
                      children: _createFields(
                          widget.fields, _key, context, widget.tabController))),
            )));
  }
}

List<Widget> _createFields(List<Field> fields, GlobalKey<FormBuilderState> key,
    BuildContext context, TabController tabController) {
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
    showSnackBar(context, 'Input(s) are missing.', color: Colors.red[500]!);
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

  // handle sketchy edge case
  if (values.containsKey("gamma_1") && values.containsKey("gamma_2")) {
    showSnackBar(context,
        "Cannot generate JSON with both gamma fields set. Please fill only one of them",
        color: Colors.red[500]!);
    return;
  } else if (values.containsKey("gamma_2")) {
    values["gamma"] = values["gamma_2"];
    values.remove("gamma_2");
  } else if (values.containsKey("gamma_1")) {
    values["gamma"] = values["gamma_1"];
    values.remove("gamma_1");
  }

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
        color: Colors.red[500]!);
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
        color: Colors.green[700]!);
  } catch (error) {
    showSnackBar(context, "JSON file generation error: $error",
        color: Colors.red[500]!);
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
          color: Colors.green[700]!))
      .onError((error, _) => showSnackBar(
          context, 'Configuration file generation error: $error',
          color: Colors.red[500]!));
}
