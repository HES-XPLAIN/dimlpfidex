#include "fidexGloRulesFct.h"

////////////////////////////////////////////////////////////

int main(int nbParam, char **param)

{
  std::string command = "fidexGloRules";
  if (nbParam >= 1) {
    for (int p = 1; p < nbParam; p++) {
      command += " ";
      command += param[p];
    }
  }
  // fidexGloRules(command);

  vector<Rule> v = Rule::fromJsonFile("/home/eldado/Documents/hepia/dimlpfidex-json/dimlpfidex/fidexGlo/tests/rules.json");

  vector<Antecedant> a{Antecedant(1, 1, 0.9999), Antecedant(2, 0, 0.0)};
  vector<int> cs{99, 100, 101, 102};

  Rule r = Rule(a, cs, 47, 0.5, 0.5, 0.0);

  v.push_back(r);

  for (Rule r : v) {
    cout << r << endl;
  }

  Rule::toJsonFile("/home/eldado/Documents/hepia/dimlpfidex-json/dimlpfidex/fidexGlo/tests/new-rules.json", v);
}
