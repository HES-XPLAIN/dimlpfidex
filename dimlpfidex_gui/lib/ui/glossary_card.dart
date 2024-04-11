import 'package:dimlpfidex_gui/data/field.dart';
import 'package:flutter/material.dart';

class GlossaryCard extends StatelessWidget {
  final Field field;

  const GlossaryCard({super.key, required this.field});

  @override
  Widget build(BuildContext context) {
    return Card(
      color: Colors.grey[300],
      elevation: 5.0,
      margin: const EdgeInsets.all(30.0),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Padding(
              padding: const EdgeInsets.all(15.0),
              child: Column(children: [
                Row(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    _getIconByFieldType(field),
                    Container(width: 20),
                    Expanded(
                        child: Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: <Widget>[
                        // Add some spacing between the top of the card and the title
                        Container(height: 5),
                        // Add a title widget
                        Text(field.label,
                            style: const TextStyle(
                                fontWeight: FontWeight.bold, fontSize: 22)),
                        // Add some spacing between the title and the subtitle
                        Container(height: 5),
                        // Add a subtitle widget
                        Text(field.datatype.toString(),
                            style: TextStyle(
                                color: Colors.grey[600], fontSize: 17)),
                        // Add some spacing between the subtitle and the text
                        Container(height: 10),
                        // Add a text widget to display some text
                        ConstrainedBox(
                            constraints: const BoxConstraints(maxHeight: 150),
                            child: SingleChildScrollView(
                                child: Text(field.description,
                                    style: TextStyle(
                                        color: Colors.grey[800],
                                        fontSize: 17))))
                      ],
                    ))
                  ],
                ),
                Container(height: 20),
                _buildFieldSpecs(field)
              ]))
        ],
      ),
    );
  }

  Widget _buildFieldSpecs(Field field) {
    return Column(
      mainAxisAlignment: MainAxisAlignment.start,
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [field.toDataTable()],
    );
  }

  Icon _getIconByFieldType(Field field) {
    const double iconSize = 100.0;

    Icon stringIcon = const Icon(Icons.abc, size: iconSize);
    Icon numericIcon = const Icon(Icons.onetwothree, size: iconSize);
    Icon unknownIcon = const Icon(Icons.question_mark, size: iconSize);
    Icon arrayIcon = const Icon(Icons.data_array, size: iconSize);
    Icon fileIcon = const Icon(Icons.file_open, size: iconSize);
    Icon directoryIcon = const Icon(Icons.folder, size: iconSize);


    switch (field.datatype) {
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
