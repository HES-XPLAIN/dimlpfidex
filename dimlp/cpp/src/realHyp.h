#ifndef REALHYP_H
#define REALHYP_H

#include "ante.h"
#include "attrName.h"
#include "bpNN.h"
#include "rulePro.h"
#include "thresD.h"
#include "virtHyp.h"
#include <vector>

////////////////////////////////////////////////////////////////////////

/**
 * @brief The RealHyp class is responsible for extracting and managing decision rules based on hyperplanes in a dataset using a neural network.
 *
 * This class interacts with a neural network and a virtual hyperplane representation to identify, confirm, and refine rules that classify data examples.
 * The extracted rules are pruned, expanded, and evaluated for accuracy and fidelity.
 */
class RealHyp {
  std::shared_ptr<BpNN> Bpnn;                 ///< The neural network used for rule extraction.
  std::shared_ptr<VirtualHyp> Virt;           ///< Virtual hyperplane representation.
  std::shared_ptr<ThresDescr> Descr;          ///< Threshold description for hyperplanes.
  std::shared_ptr<RuleProcessing> SavedRules; ///< Container for the saved rules.

  //----------------------------------------------------------------

  float *In;      ///< Input values for the neural network.
  float *Out;     ///< Output values from the neural network.
  float *Bias;    ///< Biases of the neural network.
  float *Weights; ///< Weights of the neural network.

  int NbBins;         ///< Number of bins for virtual hyperplanes.
  int NbIn;           ///< Number of input variables.
  int Multiple;       ///< Multiplicity factor for hyperplanes.
  int NbHyp;          ///< Number of hyperplanes.
  int NbWeightLayers; ///< Number of layers in the neural network.
  int NbOut;          ///< Number of output classes from the neural network. // Attenzione al numero di classi quando = 1

  std::vector<std::vector<int>> ConfirmedVirt; ///< Confirmed virtual hyperplanes.
  std::vector<int> ConfBefFirstHyp;            ///< Confirmation before first hyperplane, indicates if some examples are lower than the first hyperplane, for each attribute.
  std::vector<int> ClassPatNet;                ///< Class pattern from the network.

  int Aborted; ///< Flag indicating if the tree construction was aborted.
  //----------------------------------------------------------------

  /**
   * @brief Saves a rule by inserting it into the SavedRules object.
   *
   * @param path The rule to be saved.
   */
  void SaveRule(std::shared_ptr<DimlpRule> path) const { SavedRules->Insert(path); }

  /**
   * @brief Finds the maximum value in a vector of integers.
   */
  int MaxOnPos(const std::vector<int> &vec) const;

  /**
   * @brief Finds the index of the maximum value in an array of integers.
   */
  int GiveIndMax(const int *vec, int nbEl) const;

  /**
   * @brief Sets the ConfirmedVirt array based on the saved rules.
   */
  void SetConfirmedVirt2();

  /**
   * @brief Goes left in the decision tree to identify the discriminant hyperplane.
   */
  void Gl1(int var, int indPat, int startVirt, int netAns);

  /**
   * @brief Goes right in the decision tree to identify the discriminant hyperplane.
   */
  void Gr1(int var, int indPat, int startVirt, int netAns);

  /**
   * @brief Virtual function to go left in the decision tree.
   *
   * This function is a virtual wrapper for Gl1, allowing derived classes
   * to override the behavior if needed.
   *
   * @param var The variable index.
   * @param indPat The index of the pattern.
   * @param startVirt The starting index for virtual hyperplanes.
   * @param netAns The network's answer for the pattern.
   */
  virtual void GoLeft(int var, int indPat, int startVirt, int netAns) { Gl1(var, indPat, startVirt, netAns); }

  /**
   * @brief Virtual function to go right in the decision tree.
   *
   * This function is a virtual wrapper for Gr1, allowing derived classes
   * to override the behavior if needed.
   *
   * @param var The variable index.
   * @param indPat The index of the pattern.
   * @param startVirt The starting index for virtual hyperplanes.
   * @param netAns The network's answer for the pattern.
   */
  virtual void GoRight(int var, int indPat, int startVirt, int netAns) { Gr1(var, indPat, startVirt, netAns); }

  /**
   * @brief Processes a single example to estimate the relevance of discriminant hyperplanes.
   */
  void OneExRealHyp(DataSet &data, int indPat);
  /**
   * @brief Finds the most discriminant antecedent.
   */
  std::shared_ptr<Ante> FindMostDiscrAnt(int sel) const;

  /**
   * @brief Computes the number of correct classifications for a given rule.
   *
   * @param listPatLeft The list of patterns classified to the left.
   * @param listPatRight The list of patterns classified to the right.
   * @return The number of correct classifications.
   */
  int ComputeCorrect(std::shared_ptr<StringInt> listPatLeft, std::shared_ptr<StringInt> listPatRight);

  /**
   * @brief Sets the count of patterns that are discriminated by a given rule.
   */
  void SetCountPatDiscr(std::shared_ptr<StringInt> listPat, std::shared_ptr<DimlpRule> r) const;

  /**
   * @brief Sets the count of patterns that are discriminated by a given rule.
   */
  void SetCountPatDiscr2(DataSet &data, std::shared_ptr<DimlpRule> r);

  /**
   * @brief Recursively performs a deep search for rules in the dataset.
   */
  void DeepSearch(DataSet &data, std::shared_ptr<DimlpRule> path, std::shared_ptr<StringInt> subSet);

  /**
   * @brief Recursively performs a deep search for rules in the dataset.
   */
  void DeepSearch2(DataSet &data, std::shared_ptr<DimlpRule> path);

  //----------------------------------------------------------------

public:
  /**
   * @brief Checks if the decision tree construction was aborted.
   *
   * @return Returns 1 if the tree was aborted, otherwise 0.
   */
  int TreeAborted() const { return Aborted; }

  /**
   * @brief Sets confirmed virtual hyperplanes based on the dataset.
   */
  void SetConfirmedVirt(DataSet &data);

  /**
   * @brief Sets real hyperplanes based on the dataset.
   */
  void SetRealHyp(DataSet &data);

  /**
   * @brief Go right in the hyperplane search for rule extraction.
   */
  void Gl2(int var, int indPat, int startVirt, int netAns);

  /**
   * @brief Go left in the hyperplane search for rule extraction.
   */
  void Gr2(int var, int indPat, int startVirt, int netAns);

  /**
   * @brief Extracts rules from the dataset and writes them to the provided output stream.
   */
  void RuleExtraction(
      DataSet &data,
      const DataSet &train,
      const DataSet &trainClass,
      const DataSet &valid,
      const DataSet &validClass,
      const DataSet &test,
      const DataSet &testClass,
      const AttrName &attr,
      std::ostream &ruleFile,
      const std::vector<double> &mus = std::vector<double>(),
      const std::vector<double> &sigmas = std::vector<double>(),
      const std::vector<int> &normalizationIndices = std::vector<int>());

  //----------------------------------------------------------------

  /**
   * @brief Virtual destructor for the RealHyp class.
   */
  virtual ~RealHyp() = default;

  /**
   * @brief Constructs a RealHyp object and sets up hyperplanes based on the provided dataset and neural network.
   */
  RealHyp(
      DataSet &data,
      std::shared_ptr<BpNN> nn,
      int nbBins,
      int nbIn,
      int multiple,
      int nbWeightLayers);

  /**
   * @brief Constructs a RealHyp object with additional parameters for hyperplanes.
   */
  RealHyp(
      DataSet &data,
      std::shared_ptr<BpNN> nn,
      int nbBins,
      int nbIn,
      int multiple,
      int nbWeightLayers,
      int dummy);

  /**
   * @brief Constructs a RealHyp object with a shared virtual hyperplane representation.
   */
  RealHyp(
      std::shared_ptr<VirtualHyp> globalVirt,
      int nbNets,
      float *out,
      int nbOut,
      DataSet &data,
      std::shared_ptr<BpNN> nn,
      int nbBins,
      int nbIn,
      int multiple,
      int nbWeightLayers);

  /**
   * @brief Constructs a RealHyp object with additional parameters for hyperplanes and a shared virtual hyperplane representation.
   */
  RealHyp(
      std::shared_ptr<VirtualHyp> globalVirt,
      int nbNets,
      float *out,
      int nbOut,
      DataSet &data,
      std::shared_ptr<BpNN> nn,
      int nbBins,
      int nbIn,
      int multiple,
      int nbWeightLayers,
      int dummy);
};

////////////////////////////////////////////////////////////////////////

#endif // REALHYP_H
