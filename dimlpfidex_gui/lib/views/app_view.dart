import 'package:dimlpfidex_gui/ui/nested_tab_bar.dart';
import 'package:dimlpfidex_gui/views/form_view.dart';
import 'package:dimlpfidex_gui/data/fields_data.dart';
import 'package:dimlpfidex_gui/views/glossary_view.dart';
import 'package:dimlpfidex_gui/views/home_view.dart';
import 'package:flutter/material.dart';

class App extends StatelessWidget {
  const App({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
        home: const AppPage(),
        debugShowCheckedModeBanner: false,
        theme: ThemeData(
          colorScheme: ColorScheme.fromSeed(seedColor: Colors.blue[900]!),
          useMaterial3: false,
        ));
  }
}

class AppPage extends StatefulWidget {
  const AppPage({super.key});

  @override
  State<AppPage> createState() => _AppPageState();
}

class _AppPageState extends State<AppPage> with TickerProviderStateMixin {
  late TabController _tabController;
  final _tabs = const [
    Padding(padding: EdgeInsets.symmetric(vertical: 10), child: Text("Home")),
    Padding(padding: EdgeInsets.symmetric(vertical: 10), child: Text("Fidex")),
    Padding(padding: EdgeInsets.symmetric(vertical: 10), child: Text("Dimlp")),
    Padding(padding: EdgeInsets.symmetric(vertical: 10), child: Text("Training methods")),
    Padding(
        padding: EdgeInsets.symmetric(vertical: 10), child: Text("Glossary"))
  ];

  @override
  void initState() {
    super.initState();
    _tabController = TabController(length: _tabs.length, vsync: this);
  }

  @override
  void dispose() {
    super.dispose();
    _tabController.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Dimlpfidex config generator'),
        bottom: TabBar(
          controller: _tabController,
          tabs: _tabs,
          indicatorColor: Colors.white,
        ),
      ),
      body: TabBarView(
        controller: _tabController,
        children: [
          const MainMenuView(),
          NestedTabBar(tabs: {
            // "tests":
            //     FormView(fields: testFields, tabController: _tabController),
            "Fidex":
                FormView(fields: fidexFields, tabController: _tabController),
            "Fidex Glo":
                FormView(fields: fidexGloFields, tabController: _tabController),
            "Fidex Glo Rules": FormView(
                fields: fidexGloRulesFields, tabController: _tabController),
            "Fidex Glo Stats": FormView(
                fields: fidexGloStatsFields, tabController: _tabController)
          }),
          NestedTabBar(tabs: {
            "DimlpCls":
                FormView(fields: dimlpClsFields, tabController: _tabController),
            "DimlpTrn":
                FormView(fields: dimlpTrnFields, tabController: _tabController),
            "DimlpPred": FormView(
                fields: dimlpPredFields, tabController: _tabController),
            "DimlpRul":
                FormView(fields: dimlpRulFields, tabController: _tabController),
            "DimlpBT":
                FormView(fields: dimlpBTFields, tabController: _tabController),
            "DensCls":
                FormView(fields: densClsFields, tabController: _tabController)
          }),
          NestedTabBar(tabs: {
            "ComputeRocCurve": FormView(
                fields: computeRocCurveFields, tabController: _tabController),
            "CnnTrn": FormView(
                fields: cnnTrnFields, tabController: _tabController),
            "GradBoostTrn": FormView(
                fields: gradBoostTrnFields, tabController: _tabController),
            "MlpTrn":
                FormView(fields: mlpTrnFields, tabController: _tabController),
            "RandForestsTrn": FormView(
                fields: randForestsTrnFields, tabController: _tabController),
            "SvmTrn":
                FormView(fields: svmTrnFields, tabController: _tabController)
          }),
          const GlossaryView(fields: {
            "Fidex": fidexFields,
            "Fidex Glo": fidexGloFields,
            "Fidex Glo Stats": fidexGloStatsFields,
            "Fidex Glo Rules": fidexGloRulesFields,
            "DimlpCls": dimlpClsFields,
            "DimlpTrn": dimlpTrnFields,
            "DimlpPred": dimlpPredFields,
            "DimlpRul": dimlpRulFields,
            "DimlpBT": dimlpBTFields,
            "DensCls": densClsFields,
            "ComputeRocCurve": computeRocCurveFields,
            "CnnTrn": cnnTrnFields,
            "GradBoostTrn": gradBoostTrnFields,
            "MlpTrn": mlpTrnFields,
            "RandForestsTrn": randForestsTrnFields,
            "SvmTrn": svmTrnFields
          })
        ],
      ),
    );
  }
}
