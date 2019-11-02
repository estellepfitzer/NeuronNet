#include "globals.h"

/*! \class Neuron
  A neuron type is defined by four parameters \p a, \p b, \p c, \p d, and the "inhibitory" or "excitatory" quality. 
  Standard types are provided in \ref NeuronTypes and identified by a 2 or 3-letter string.
  The dynamic variables are the membrane potential, the recovery variable and the input. 
 */

struct NeuronParams {double a, b, c, d; bool inhib;};

class Neuron {
    static const std::map<std::string, NeuronParams> NeuronTypes;
    static double firing_thresh;

public:
/*!
  Default constructor: a *RS* neuron.
 */
    Neuron();
    void set_params(const NeuronParams&, const double n=0);
    void set_type(std::string);
    void set_default_params(const std::string&, double n=0);
    bool is_type(const std::string&);
    std::string type() {return _type->first;}
    bool is_inhibitory() const {return params.inhib;}
    void set_inhibitory() {params.inhib=true;}
/*!
  A neuron is firing if its membrane potential exceeds the firing threshold \ref firing_thresh
 */
    bool firing() const {return (_poten>firing_thresh);}
/*!
  After firing, the neuron is reset: \ref _poten is set to \p c, \ref _recov is increased by \p d.
 */
    void reset() {_poten=params.c; _recov+=params.d;}
/*!
  One step of time evolution: \ref _poten and \ref _recov are updated according to the Izhikevich equations.
 */
    void step();
    void potential(const double &_p) {_poten = _p;}
    double potential() const {return _poten;}
    double recovery() const {return _recov;}
    void input(const double i) {_input=i;}
    double input() const {return _input;}
/*! @name Output strings
  For printing purposes: all parameters and dynamic values are returned as a formatted string (tab-delimited concatenation of values).
 */
///@{
    std::string formatted_params() const;
    std::string formatted_values() const;
///@}

/*! @name Static helpers
  \ref type_exists checks if the string \p s exists in the map \ref NeuronTypes and 
  \ref type_default returns the corresponding \ref NeuronParams from \ref NeuronTypes.
 */
///@{
    static bool type_exists(const std::string &s) {return NeuronTypes.count(s);}
    static NeuronParams type_default(const std::string &s) {
        if (type_exists(s)) return NeuronTypes.at(s);
        return NeuronTypes.at("RS");
    }
///@}

private:
/*! @name Neuron parameters 
  \p a, \p b, \p c, \p d, and the boolean \p inhib if neuron is inhibitory.
 */
    NeuronParams params;
/*! @name Reference to the neuron type 
  This is an iterator to the relevant element of Neuron::NeuronTypes.
 */
    std::map<std::string, NeuronParams>::const_iterator _type;
/*! @name Dynamic variables
 */
///@{
    double _poten, _recov, _input;
///@}

};
