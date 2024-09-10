import 'package:dimlpfidex_gui/data/field.dart';
import 'package:dimlpfidex_gui/ui/glossary_card.dart';
import 'package:flutter/material.dart';


class GlossaryView extends StatefulWidget {
  final Map<String, List<UnstableField>> fields;

  const GlossaryView({super.key, required this.fields});

  @override
  State<StatefulWidget> createState() => _GlossaryViewState();
}

class _GlossaryViewState extends State<GlossaryView> {
  @override
  Widget build(BuildContext context) {
    return SingleChildScrollView(
        child: Padding(
            padding: const EdgeInsets.only(top: 30.0, bottom: 30.0),
            child: FractionallySizedBox(
                alignment: Alignment.topCenter,
                widthFactor: 0.7,
                child: Column(
                  children: _buildGlossaryExpansionTile(widget.fields),
                ))));
  }

  List<ExpansionTile> _buildGlossaryExpansionTile(
      Map<String, List<UnstableField>> map) {
    List<ExpansionTile> result = [];

    for (MapEntry entry in map.entries) {
      result.add(ExpansionTile(
          title: Text(
            entry.key,
            style: TextStyle(
                color: Colors.grey[80],
                fontWeight: FontWeight.bold,
                fontSize: 22),
          ),
          children: _buildGlossaryCards(entry.value)));
    }

    return result;
  }

  List<GlossaryCard> _buildGlossaryCards(List<UnstableField> fields) {
    List<GlossaryCard> result = [];

    for (UnstableField field in fields) {
      result.add(field.toGlossaryCard());
    }

    return result;
  }
}
