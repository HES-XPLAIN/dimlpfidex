import 'package:dimlpfidex_gui/ui/glossary_card.dart';
import 'package:dimlpfidex_gui/ui/input_field.dart';
import 'package:flutter/material.dart';

enum Datatype {
  integer,
  doublePrecision,
  string,
  restrictedChoiceString,
  filePath,
  boolean,
  directoryPath,
  dictionary,
  listString,
  listInteger,
  listFilePath,
  listDoublePrecision,
  pairInteger
}

class Field {
  final String label;
  final String jsonLabel;
  final Datatype datatype;
  final bool isRequired;
  final String description;
  final String defaultValue;
  final String minValue;
  final String maxValue;
  final List<String> items;

  const Field(this.label, this.jsonLabel, this.datatype,
      {this.isRequired = false,
      this.defaultValue = "",
      this.minValue = "",
      this.maxValue = "",
      this.items = const [],
      this.description = "No description available"});

  Widget toInputField() {
    return InputField(field: this);
  }

  GlossaryCard toGlossaryCard() {
    return GlossaryCard(field: this);
  }

  // used by the glossary card builder
  DataTable toDataTable() {
    return DataTable(columns: const [
      DataColumn(
          label: Expanded(
        child: Text(
          "Property",
          style: TextStyle(fontStyle: FontStyle.italic),
        ),
      )),
      DataColumn(
          label: Expanded(
        child: Text(
          "Value",
          style: TextStyle(fontStyle: FontStyle.italic),
        ),
      ))
    ], rows: [
      DataRow(cells: [
        const DataCell(Text("JSON label")),
        DataCell(Text(jsonLabel))
      ]),
      DataRow(cells: [
        const DataCell(Text("Is required")),
        DataCell(Text(isRequired.toString()))
      ]),
      if (datatype == Datatype.restrictedChoiceString)
        DataRow(cells: [
          const DataCell(Text("Choices")),
          DataCell(Text(items.isEmpty
              ? "None"
              : items.toString().replaceAll(RegExp(r'(\[|\])+'), '')))
        ]),
      DataRow(cells: [
        const DataCell(Text("Default value")),
        DataCell(Text(defaultValue != "" ? defaultValue : "None"))
      ]),
      if (datatype == Datatype.integer || datatype == Datatype.doublePrecision)
        DataRow(cells: [
          const DataCell(Text("Min value")),
          DataCell(Text(minValue != "" ? minValue : "None"))
        ]),
      if (datatype == Datatype.integer || datatype == Datatype.doublePrecision)
        DataRow(cells: [
          const DataCell(Text("Max value")),
          DataCell(Text(maxValue != "" ? maxValue : "None"))
        ])
    ]);
  }
}
