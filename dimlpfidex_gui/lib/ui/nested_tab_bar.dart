import 'package:flutter/material.dart';

class NestedTabBar extends StatefulWidget {
  final Map<String, Widget> tabs;

  const NestedTabBar({super.key, required this.tabs});

  @override
  State<NestedTabBar> createState() => _NestedTabBarState();
}

class _NestedTabBarState extends State<NestedTabBar>
    with TickerProviderStateMixin {
  late final TabController _tabController;

  @override
  void initState() {
    super.initState();
    _tabController = TabController(length: widget.tabs.length, vsync: this);
  }

  @override
  void dispose() {
    _tabController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      children: <Widget>[
        Material(
          color: Theme.of(context).colorScheme.primary,
        child: TabBar.secondary(
          indicatorColor: Colors.white,
          controller: _tabController,
          tabs: _buildTabs(widget.tabs.keys.toList()),
        )),
        Expanded(
          child: TabBarView(
            controller: _tabController,
            children: widget.tabs.values.toList(),
          ),
        ),
      ],
    );
  }

  List<Tab> _buildTabs(List<String> tabNameList) {
    List<Tab> result = [];

    for (String tabName in tabNameList) {
      result.add(Tab(text: tabName));
    }

    return result;
  }
}
