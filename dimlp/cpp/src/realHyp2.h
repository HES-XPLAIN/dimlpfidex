#ifndef REALHYP2_H
#define REALHYP2_H

#include "realHyp.h"

///////////////////////////////////////////////////////////////////

/**
 * @brief RealHyp2 is a derived class from RealHyp that overrides the behavior of going left or right in the decision tree.
 * 
 * This class provides specific implementations for the methods to move left and right in the decision tree during the rule extraction process.
 */
class RealHyp2 : public RealHyp {

  //----------------------------------------------------------------

  /**
   * @brief Overrides the method to go left in the decision tree.
   * 
   * @param var The variable index.
   * @param indPat The index of the pattern.
   * @param startVirt The starting index for virtual hyperplanes.
   * @param netAns The network's answer for the pattern.
   */
  void GoLeft(int var, int indPat, int startVirt, int netAns) override { Gl2(var, indPat, startVirt, netAns); }

  /**
   * @brief Overrides the method to go right in the decision tree.
   * 
   * @param var The variable index.
   * @param indPat The index of the pattern.
   * @param startVirt The starting index for virtual hyperplanes.
   * @param netAns The network's answer for the pattern.
   */
  void GoRight(int var, int indPat, int startVirt, int netAns) override { Gr2(var, indPat, startVirt, netAns); }

  //----------------------------------------------------------------

public:

  /**
   * @brief Constructs a RealHyp2 object using the given dataset and neural network parameters.
   */
  RealHyp2(
      DataSet &data,
      std::shared_ptr<BpNN> nn,
      int nbBins,
      int nbIn,
      int multiple,
      int nbWeightLayers);

  /**
   * @brief Constructs a RealHyp2 object using the given virtual hyperplanes and neural network parameters.
   */
  RealHyp2(
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
};

///////////////////////////////////////////////////////////////////

#endif // REALHYP2_H
