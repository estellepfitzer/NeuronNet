#include "network.h"
#include <tclap/CmdLine.h>

/*! \class Simulation
  This is the main class. 
  It manages user inputs, defines the simulation parameters and constructs the \ref Network \ref net.
  It then runs the simulation and prints the results to the output streams.
  These streams can be files with names based on the string \ref output with a suffix.
  Simulation parameters:
  - \ref endtime : total number of time-steps,
  - \ref size : total number of neurons,
  - \ref degree : average connectivity of a neuron,
  - \ref thalam : st. dev. of thalamic input (for excitatory neurons),
  - \ref streng : average intensity of connections, 
  - \ref inhib : fraction of inhibitory neurons in the network, 
  The map \ref ntypes describes the neuron population: 
  its keys are the neuron types from \ref Neuron::NeuronTypes and values are the corresponding counts.
 */

class Simulation {
    
public:
/*! 
  Default constructor initializes the following variables:
  \param _s (int): size of network (number of neurons)
  \param _t (int): duration of simulation (number of time-steps)
  \param _i (double): fraction of inhibitory neurons in the network
 */
    Simulation(const int _s, const int _t, const double _i=_PROP_INHIB_)
        : endtime(_t), size(_s), degree(_DEGREE_), thalam(_THALAM_), streng(_STRENG_), inhib(_i) {}
/*!
  Constructor based on user inputs, takes command-line arguments and passes them to \ref parse.
 */
    Simulation(int, char**);
/*!
  Construct a network as specified in a configuration file
  \param infile (string): filename
 */
    void load_configuration(const std::string &infile);
/*!
  Parses a string such as **FS:0.2,IB:0.2,CH:0.15** and constructs the network accordingly (20% of *FS* neurons, 20% of *IB* and 15%of *CH*). 
  These neuron types are found in \ref Neuron::NeuronTypes. The neuron population is completed with default (*RS*) 
  neurons and randomly connected with \ref Network::random_connect.
 */
    void parse_types(std::string);
/*!
  Returns the number of neurons of \ref Neuron::NeuronTypes "type" _s.
 */
    size_t size_type(const std::string &_s) const;
/*!
  The main operation of this class: runs the simulation through a loop with \ref endtime steps. 
  Each iteration calls \ref Network::step with a random value of thalamic input (RandomNumbers::normal distribution), then writes out the results. 
 */
    void run();

private:
/*!
  Constraint function for a TCLAP argument to restrict a double to [0,1].
 */
    struct intervalConstr : public TCLAP::Constraint<double> {
        std::string shortID()       const {return "[0,1]";}
        std::string description()   const {return "A number in [0,1]";}
        bool check(const double &x) const {return (x >= 0) && (x <= 1);}
    };

/*!
  Uses [TCLAP](http://tclap.sourceforge.net/html/index.html) to parse user inputs.
 */
    void parse(int, char**);

    Network net;
    int endtime;
    size_t size;
    double degree, thalam, streng, inhib;
    std::string output;
    std::map< std::string, size_t > ntypes; 
};
