#include "ElasticContactInteraction.hpp"

ElasticContactInteraction::ElasticContactInteraction()
{
	createIndex();
	
}

ElasticContactInteraction::~ElasticContactInteraction()
{
}

YADE_PLUGIN((ElasticContactInteraction));
