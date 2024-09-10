import 'package:dimlpfidex_gui/data/field.dart';
import 'package:flutter/material.dart';

class GlossaryCard extends StatelessWidget {
  final Field field;

  const GlossaryCard({super.key, required this.field});

  @override
  Widget build(BuildContext context) {
    return Card(
      color: Colors.blueGrey[100],
      elevation: 5.0,
      margin: const EdgeInsets.all(30.0),
      child: Padding(
          padding: const EdgeInsets.all(30),
          child:
              Column(crossAxisAlignment: CrossAxisAlignment.start, children: [
            Row(
              crossAxisAlignment: CrossAxisAlignment.center,
              children: [
                _getIconByFieldType(field),
                Container(width: 20),
                Expanded(child: _buildGeneralSpecs(field))
              ],
            ),
            Container(height: 20),
            _buildFieldSpecs(field)
          ])),
    );
  }

  Widget _buildFieldSpecs(Field field) {
    return Column(
      mainAxisAlignment: MainAxisAlignment.start,
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: field.toDataTable(),
    );
  }

  Widget _buildGeneralSpecs(Field field) {
    List<String> typenames = [];

    for (Metadata metadata in field.metadatas) {
      typenames.add(metadata.datatype.name);
    }

    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: <Widget>[
        Text(field.label,
            style: const TextStyle(fontWeight: FontWeight.bold, fontSize: 22)),
        Container(height: 15),
        Column(
            mainAxisAlignment: MainAxisAlignment.start,
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: [
              Card(
                  color: Colors.blueGrey[50],
                  child: DataTable(
                    headingRowHeight: 0,
                    columns: const [
                      DataColumn(label: Text("")),
                      DataColumn(label: Text(""))
                    ],
                    rows: [
                      DataRow(cells: [
                        const DataCell(
                          Text("JSON label", style: TextStyle(fontSize: 15, fontWeight: FontWeight.w500, fontStyle: FontStyle.italic)),
                        ),
                        DataCell(
                          Text(field.jsonLabel,
                              style: const TextStyle(fontSize: 15)),
                        ),
                      ]),
                      DataRow(cells: [
                        const DataCell(
                          Text("Is a mandatory field",
                              style: TextStyle(fontSize: 15, fontWeight: FontWeight.w500, fontStyle: FontStyle.italic)),
                        ),
                        DataCell(
                          Text(field.isRequired ? "yes" : "no",
                              style: const TextStyle(fontSize: 15)),
                        )
                      ]),
                      DataRow(cells: [
                        const DataCell(
                          Text("Has multiple kind of input available",
                              style: TextStyle(fontSize: 15, fontWeight: FontWeight.w500, fontStyle: FontStyle.italic)),
                        ),
                        DataCell(
                          Text(typenames.length > 1 ? "yes" : "no",
                              style: const TextStyle(fontSize: 15)),
                        )
                      ]),
                      if (typenames.length > 1)
                        DataRow(cells: [
                          const DataCell(Text("Kinds of input available",
                              style: TextStyle(fontSize: 15, fontWeight: FontWeight.w500, fontStyle: FontStyle.italic))),
                          DataCell(
                            Text(
                                typenames
                                    .toString()
                                    .replaceAll(RegExp(r'(\[|\])+'), ''),
                                style: const TextStyle(fontSize: 15)),
                          )
                        ])
                    ],
                  ))
            ]),
      ],
    );
  }

// TODO: (nice to have) complete the set of icons for datatypes
  Icon _getIconByFieldType(Field field) {
    const double iconSize = 100.0;

    Icon stringIcon = const Icon(Icons.abc, size: iconSize);
    Icon multipleTypeIcon = const Icon(Icons.more_horiz, size: iconSize);
    Icon numericIcon = const Icon(Icons.onetwothree, size: iconSize);
    Icon unknownIcon = const Icon(Icons.question_mark, size: iconSize);
    Icon arrayIcon = const Icon(Icons.data_array, size: iconSize);
    Icon fileIcon = const Icon(Icons.file_open, size: iconSize);
    Icon directoryIcon = const Icon(Icons.folder, size: iconSize);

    if (field.metadatas.length > 1) {
      return multipleTypeIcon;
    } else {
      switch (field.metadatas[0].datatype) {
        case Datatype.string:
          return stringIcon;
        case Datatype.filePath:
          return fileIcon;
        case Datatype.directoryPath:
          return directoryIcon;
        case Datatype.integer || Datatype.doublePrecision:
          return numericIcon;
        case Datatype.listInteger ||
              Datatype.listDoublePrecision ||
              Datatype.listString:
          return arrayIcon;
        default:
          return unknownIcon;
      }
    }
  }
}
