//
// pedsim - A microscopic pedestrian simulation system.
// Copyright (c) 2003 - 2014 by Christian Gloor
//
// Adapted for Low Level Parallel Programming 2017
//
// Model coordinates a time step in a scenario: for each
// time step all agents need to be moved by one position if
// possible.
//
#ifndef _ped_model_h_
#define _ped_model_h_

#include <vector>
#include <map>
#include <set>
#include <omp.h>
#include <list>

#include "ped_agent.h"

namespace Ped{
	class Tagent;

	// The implementation modes for Assignment 1 + 2:
	// chooses which implementation to use for tick()
	enum IMPLEMENTATION { CUDA, VECTOR, OMP, PTHREAD, SEQ, OMPMOVE };

	class Model
	{
	public:

		// Sets everything up
		void setup(std::vector<Tagent*> agentsInScenario, std::vector<Twaypoint*> destinationsInScenario);

		// Coordinates a time step in the scenario: move all agents by one step (if applicable).
		void tick();
		
		// TODO: Document
		void setAgentsPositionSerial();
		void setAgentsPositionOMP(int);
		void setAgentsPositionOMPMove(int);
		void spawnThreads(int);
		void setAgentsPositionThreads(int, int);
		void setAgentsPositionSIMD();
		void change_mode(int);

		// stuffelistuff
		void moveOMP(int);

		// Returns the agents of this scenario
		const std::vector<Tagent*> getAgents() const { return agents; };

		// Adds an agent to the tree structure
		void placeAgent(const Ped::Tagent *a);

		// Cleans up the tree and restructures it. Worth calling every now and then.
		void cleanup();
		~Model();

		// Returns the heatmap visualizing the density of agents
		int const * const * getHeatmap() const { return blurred_heatmap; };
		int getHeatmapSize() const;
		Ped::Twaypoint* Ped::Model::getNextDestination(int);
		void Ped::Model::computeNextDesiredPosition(Twaypoint*, int);
	private:

		// Denotes which implementation (sequential, parallel implementations..)
		// should be used for calculating the desired positions of
		// agents (Assignment 1)
		IMPLEMENTATION implementation;

		// The agents in this scenario
		std::vector<Tagent*> agents;
		float* agentsX;
		float* agentsY;
		float* agentsDestX;
		float* agentsDestY;
		float* agentsDestR;

		std::vector < std::vector < Ped::Tagent* >> regions;
//		Concurrency::concurrent_vector<Concurrency::concurrent_vector<Ped::Tagent*>> regions;
		std::vector < std::vector < omp_lock_t* >> borders;
//		Concurrency::concurrent_vector<Concurrency::concurrent_vector<Ped::Tagent*>> borders;


		int nrOfRegions = 4;
		int nthreads = 4;
		void initBorders();
		int findRegion(Ped::Tagent*);
		void splitAgents(std::vector<Ped::Tagent*>);

		std::vector<std::pair<int, int>> getNeighbors2(std::vector < Ped::Tagent* >,  Ped::Tagent*);

		omp_lock_t* lock;

		int xMax = 800 / 5;
		int yMax = 600 / 5;

		int yRegions = floor(sqrt(nrOfRegions));
		int xRegions = ceil(nrOfRegions / yRegions);

		int regionHeight = yMax / yRegions;
		int regionWidth = xMax / xRegions;

		// The waypoints in this scenario
		std::vector<Twaypoint*> destinations;

		// Moves an agent towards its next position
		void move(Ped::Tagent *agent);

		////////////
		/// Everything below here won't be relevant until Assignment 3
		///////////////////////////////////////////////

		// Returns the set of neighboring agents for the specified position
		set<const Ped::Tagent*> getNeighbors(int x, int y, int dist) const;

		////////////
		/// Everything below here won't be relevant until Assignment 4
		///////////////////////////////////////////////



#define SIZE 1024
#define CELLSIZE 5
#define SCALED_SIZE SIZE*CELLSIZE

		// The heatmap representing the density of agents
		int ** heatmap;

		// The scaled heatmap that fits to the view
		int ** scaled_heatmap;

		// The final heatmap: blurred and scaled to fit the view
		int ** blurred_heatmap;

		void setupHeatmapSeq();
		void updateHeatmapSeq();
	};
}
#endif