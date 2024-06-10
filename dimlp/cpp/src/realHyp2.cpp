#include "realHyp2.h"

///////////////////////////////////////////////////////////////////

/**
  * @brief Constructs a RealHyp2 object using the given dataset and neural network parameters.
  * 
  * @param data The dataset used for rule extraction.
  * @param nn A shared pointer to the neural network.
  * @param nbBins The number of bins for discretization.
  * @param nbIn The number of input variables.
  * @param multiple The multiplicity factor for virtual hyperplanes.
  * @param nbWeightLayers The number of weight layers in the neural network.
  */
RealHyp2::RealHyp2(
    DataSet &data,
    std::shared_ptr<BpNN> nn,
    int nbBins,
    int nbIn,
    int multiple,
    int nbWeightLayers) :

                          RealHyp(data, nn, nbBins, nbIn, multiple, nbWeightLayers, 333) {
  SetConfirmedVirt(data);
  SetRealHyp(data);
}

///////////////////////////////////////////////////////////////////

/**
  * @brief Constructs a RealHyp2 object using the given virtual hyperplanes and neural network parameters.
  * 
  * @param globalVirt A shared pointer to the global virtual hyperplanes.
  * @param nbNets The number of networks.
  * @param out The output values from the neural network.
  * @param nbOut The number of output variables.
  * @param data The dataset used for rule extraction.
  * @param nn A shared pointer to the neural network.
  * @param nbBins The number of bins for discretization.
  * @param nbIn The number of input variables.
  * @param multiple The multiplicity factor for virtual hyperplanes.
  * @param nbWeightLayers The number of weight layers in the neural network.
  */
RealHyp2::RealHyp2(
    std::shared_ptr<VirtualHyp> globalVirt,
    int nbNets,
    float *out,
    int nbOut,
    DataSet &data,
    std::shared_ptr<BpNN> nn,
    int nbBins,
    int nbIn,
    int multiple,
    int nbWeightLayers) :

                          RealHyp(globalVirt, nbNets, out, nbOut,
                                  data, nn, nbBins, nbIn, multiple, nbWeightLayers, 333) {
  SetConfirmedVirt(data);
  SetRealHyp(data);
}

///////////////////////////////////////////////////////////////////
