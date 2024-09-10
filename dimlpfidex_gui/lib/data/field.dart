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

//! WARNING: boolean metadata cannot be binded with another datatype. It must be alone in its own field
class Field {
  final String label;
  final String jsonLabel;
  final bool isRequired;
  final List<Metadata> metadatas;

  const Field(
      this.label, this.jsonLabel, this.isRequired, this.metadatas);

  Widget toInputField() {
    return InputUnstableField(field: this);
  }

  GlossaryCard toGlossaryCard() {
    return GlossaryCard(field: this);
  }

  List<Card> toDataTable() {
    List<Card> rows = [];

    for (Metadata metadata in metadatas) {
      rows.add(Card(
          color: Colors.blueGrey[50],
          child: DataTable(columns: const [
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
          ], rows: metadata.toDataRow())));
    }

    return rows;
  }
}

class Metadata {
  final Datatype datatype;
  final String description;
  final String defaultValue;
  final String minValue;
  final String maxValue;
  final List<String> items;

  const Metadata(this.datatype,
      {this.defaultValue = "",
      this.minValue = "",
      this.maxValue = "",
      this.items = const [],
      this.description = "No description available"});

  @override
  String toString() {
    return "Instance of 'Metadata':\n\tDatatype: ${datatype.name}\n\tMin value: $minValue\n\tMax value: $maxValue\n\tItems: $items\n\tDesc: $description";
  }

  List<DataRow> toDataRow() {
    return [
      DataRow(cells: [
        const DataCell(Text("Datatype")),
        DataCell(Text(datatype.name)),
      ]),
      DataRow(cells: [
        const DataCell(Text("Description")),
        DataCell(Text(description)),
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
    ];
  }
}
