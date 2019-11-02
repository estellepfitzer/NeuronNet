#include "network.h"
#include "random.h"

void Network::resize(const size_t &n, double inhib) {
    size_t old = size();
    neurons.resize(n);
    if (n <= old) return;
    size_t nfs(inhib*(n-old)+.5);
    set_default_params({{"FS", nfs}}, old);
}

void Network::set_default_params(const std::map<std::string, size_t> &types,
                                 const size_t start) {
    size_t k(0), ssize(size()-start), kmax(0);
    std::vector<double> noise(ssize);
    _RNG->uniform_double(noise);
    for (auto I : types) 
        if (Neuron::type_exists(I.first)) 
            for (kmax+=I.second; k<kmax && k<ssize; k++) 
                neurons[start+k].set_default_params(I.first, noise[k]);
    for (; k<ssize; k++) neurons[start+k].set_default_params("RS", noise[k]);
}

void Network::set_types_params(const std::vector<std::string> &_types,
                               const std::vector<NeuronParams> &_par,
                               const size_t start) {
    for (size_t k=0; k<_par.size(); k++) {
        neurons[start+k].set_type(_types[k]);
        neurons[start+k].set_params(_par[k]);
    }
}

void Network::set_values(const std::vector<double> &_poten, const size_t start) {
    for (size_t k=0; k<_poten.size(); k++) 
        neurons[start+k].potential(_poten[k]);
}

bool Network::add_link(const size_t &a, const size_t &b, double str) {
    if (a==b || a>=size() || b>=size() || str<1e-6) return false;
    if (links.count({a,b})) return false;
    if (neurons[b].is_inhibitory()) str *= -2.0;
    links.insert({{a,b}, str});
    return true;
}

size_t Network::random_connect(const double &mean_deg, const double &mean_streng) {
    links.clear();
    std::vector<int> degrees(size());
    _RNG->poisson(degrees, mean_deg);
    size_t num_links = 0;
    std::vector<size_t> nodeidx(size());
    std::iota(nodeidx.begin(), nodeidx.end(), 0);
    for (size_t node=0; node<size(); node++) {
        _RNG->shuffle(nodeidx);
        std::vector<double> strength(degrees[node]);
        _RNG->uniform_double(strength, 1e-6, 2*mean_streng);
        int nl = 0;
        for (size_t nn=0; nn<size() && nl<degrees[node]; nn++)
            if (add_link(node, nodeidx[nn], strength[nl])) nl++;
        num_links += nl;
    }
    return num_links;
}

std::vector<double> Network::potentials() const {
    std::vector<double> vals;
    for (size_t nn=0; nn<size(); nn++)
        vals.push_back(neurons[nn].potential());
    return vals;
}

std::vector<double> Network::recoveries() const {
    std::vector<double> vals;
    for (size_t nn=0; nn<size(); nn++)
        vals.push_back(neurons[nn].recovery());
    return vals;
}

void Network::print_params(std::ostream *_out) {
    (*_out) << "Type\ta\tb\tc\td\tInhibitory\tdegree\tvalence" << std::endl;
    for (size_t nn=0; nn<size(); nn++) {
        std::pair<size_t, double> dI = degree(nn);
        (*_out) << neurons[nn].formatted_params() 
                << '\t' << dI.first << '\t' << dI.second
                << std::endl;
    }
}

void Network::print_head(const std::map<std::string, size_t> &_nt, 
                         std::ostream *_out) {
    size_t total = 0;
    for (auto It : _nt) {
        total += It.second;
        for (auto In : neurons)
            if (In.is_type(It.first)) {
                (*_out) << '\t' << It.first << ".v"
                        << '\t' << It.first << ".u"
                        << '\t' << It.first << ".I";
                break;
            }
    }
    if (total<size())
        for (auto In : neurons) 
            if (In.is_type("RS")) {
                (*_out) << '\t' << "RS.v" << '\t' << "RS.u" << '\t' << "RS.I";
                break;
            }
    (*_out) << std::endl;
}

void Network::print_traj(const int time, const std::map<std::string, size_t> &_nt, 
                         std::ostream *_out) {
    (*_out)  << time;
    size_t total = 0;
    for (auto It : _nt) {
        total += It.second;
        for (auto In : neurons) 
            if (In.is_type(It.first)) {
                (*_out) << '\t' << In.formatted_values();
                break;
            }
    }
    if (total<size())
        for (auto In : neurons) 
            if (In.is_type("RS")) {
                (*_out) << '\t' << In.formatted_values();
                break;
            }
    (*_out) << std::endl;
}

std::vector<std::pair<size_t, double> > Network::neighbors(const size_t& n) const
{
	std::vector<std::pair<size_t, double> > linked_neurons;
	for (linkmap::const_iterator it=links.lower_bound({n,0}) ; it!=links.end(); ++it) 
	{	
		if (it->first.first == n) 
		{ 
			std::pair<size_t, double> to_insert((it->first).second, it->second);
			linked_neurons.push_back(to_insert);
		}
	}

	return linked_neurons;
}

std::pair<size_t, double> Network::degree(const size_t& n) const
{
	std::vector<std::pair<size_t, double>> my_neighbors = neighbors(n);
	
	size_t nb_connect(0);
	double total_intensity(0);
	
	for(size_t i(0); i < my_neighbors.size(); ++i)
	{
		++nb_connect;
		total_intensity+= my_neighbors[i].second;
		
	}
	return std::pair<size_t, double> (nb_connect,total_intensity);
	
}

std::set<size_t> Network::step(const std::vector<double>& thalamic_input)
{
	std::set<size_t> firing_neurons;
	std::vector<std::pair<size_t, double> > neurons_connected;
	
	double exit(0);
	double inhib(0);
	double tal(0);
	
	for(size_t i(0); i < neurons.size(); ++i)
	{
		if(neurons[i].firing())
		{
		firing_neurons.insert(i);
		neurons[i].reset();
		}
	}
	
	for(size_t i(0); i < neurons.size(); ++i)
	{
		exit=0;
		inhib=0;
		
		neurons_connected = neighbors(i);
		
		tal = thalamic_input[i];
		if(neurons[i].is_inhibitory())
		{
				tal*=0.4;
		}
		for(size_t j(0); j < neurons_connected.size(); ++j)
		{
			if(firing_neurons.count(j) ==1)
			{
				if (neurons[neurons_connected[j].first].is_inhibitory()) 
				{
					inhib += (neurons_connected[j]).second;
				}
				else
				{
					exit += (neurons_connected[j]).second;
				}
			}
		}
		neurons[i].input(tal + (0.5*exit) + inhib);
		neurons[i].step();
		
		
	}

	return firing_neurons;
}
