#include "random.h"
using namespace std;

RandomNumbers::RandomNumbers(unsigned long int s) : seed(s)
{
		if(seed==0) 
	{
		random_device rd;
		seed = rd(); 
	}
	rng = mt19937 (seed);
}

void RandomNumbers::uniform_double(std::vector<double>& v, double lower, double upper)
{
	uniform_real_distribution<> distribution(lower, upper);
	for (size_t i=0; i<v.size(); ++i)
		{ 
		v[i]=(distribution(rng)); 
		}
}

double RandomNumbers::uniform_double(double lower, double upper)
{
	uniform_real_distribution<> distribution(lower, upper);
	return distribution(rng);
}

void RandomNumbers::normal(std::vector<double>& v, double mean, double sd)
{
	normal_distribution<> distribution(mean,sd);
	for (size_t i=0; i<v.size(); ++i)
		{ 
		v[i]=(distribution(rng)); 
		}
}

double RandomNumbers::normal(double mean, double sd)
{
	normal_distribution<> distribution(mean, sd);
	return distribution(rng);
}

void RandomNumbers::poisson(std::vector<int>& v, double mean)
{
		poisson_distribution<> distribution(mean);
	for (size_t i=0; i<v.size(); ++i)
		{ 
		v[i]=(distribution(rng)); 
		}
}

int RandomNumbers::poisson(double mean)
{
	poisson_distribution<> distribution(mean);
	return distribution(rng);
}
