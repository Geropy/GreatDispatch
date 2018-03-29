#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <map>

// Regardless of which method I end up using in the end, I will start by getting an initial greedy solution
// The basic greedy solution seems simple to me: Put the box in the lightest truck that has space
// This will ensure that all truck weights increase at similar rate

using namespace std;

struct Box
{
	const float weight;
	const float volume;
	//unsigned int ID;

	Box(float weight, float volume)
		: weight(weight)
		, volume(volume)
	{}
};

class Truck
{
public:
	
	map<unsigned int, Box> boxes;
	float totalWeight;
	float totalVolume;
	unsigned int idx;

	Truck()
		: totalWeight(0.0f)
		, totalVolume(0.0f)
	{
	}

	void addBox(float weight, float volume, unsigned int ID) 
	{
		boxes.insert(map<unsigned int, Box>::value_type(ID, Box(weight, volume)));
		totalWeight += weight;
		totalVolume += volume;
	}
};

bool truckWeightComp(const Truck& a, const Truck& b) { return a.totalWeight < b.totalWeight; }

int main()
{
	array<Truck, 100> trucks;
	for (unsigned int i = 0; i < 100; i++)
	{
		trucks[i].idx = i;
	}

	vector<int> solution;
	unsigned int boxCount, truckIndex;
	float minWeight;

    cin >> boxCount; cin.ignore();
    for (unsigned int i = 0; i < boxCount; i++) 
	{
        float weight;
        float volume;
        cin >> weight >> volume; cin.ignore();

		// Find the truck with the lowest weight, and that has space
		minWeight = 9999999.0f;
		truckIndex = -1;
		for (unsigned int i = 0; i < trucks.size(); i++)
		{
			Truck& truck = trucks[i];
			if (truck.totalWeight < minWeight && volume < (100.0f - truck.totalVolume))
			{
				minWeight = truck.totalWeight;
				truckIndex = i;
			}
		}

		// Add it to the solution, and update the truck
		solution.push_back(truckIndex);
		trucks[truckIndex].addBox(weight, volume, i);
    }

	// This solution passes all the validators, but the score is pretty bad
	// Some options from here : Branch and Bound, Iterative Improvement, Local Search
	// Probably end up trying them all, start with the simplest first
	// A simple iterative improvement would take a box out of the heaviest truck and put it in the lightest truck, volume permitting
	// The weight of the box needs to be less than the weight of the gap, preferably half the weight of the gap
	// Give this a shot

	// I've seen that this doesn't really work
	// The issue with the greedy algorithm is that the lightest truck tends to be very full on volume, otherwise it would have been the target for new boxes
	// Therefore there's no space to simply move a box one way, a swap must be made
	// Look for an optimized swap

	// Find the heaviest and lightest truck
	Truck& lightestTruck = *min_element(trucks.begin(), trucks.end(), truckWeightComp);
	Truck& heaviestTruck = *max_element(trucks.begin(), trucks.end(), truckWeightComp);

	// Look for an optimal swap between the 2


	// Look for a box in the heaviest truck to transfer
	float halfGapDelta = weightHalfGap;
	unsigned int transferBoxID = 4000;
	for (auto& boxIter : heaviestTruck.boxes)
	{
		// Find the box who's weight is closest to half the gap, and is transferable
		float tempDelta = abs(boxIter.second.weight - weightHalfGap);
		if (tempDelta < halfGapDelta && (lightestTruck.totalVolume + boxIter.second.volume) < 100.0f)
		{
			transferBoxID = boxIter.first;
			halfGapDelta = tempDelta;
		}
	}
	
	// Do the transfer
	if (transferBoxID < 4000)
	{
		solution[transferBoxID] = lightestTruck.idx;
		cerr << "Moved box " << transferBoxID << " from truck " << heaviestTruck.idx << " to " << lightestTruck.idx << endl;
	}

	// Print truck status
	for (auto& truck : trucks)
	{
		cout << "Weight " << truck.totalWeight << " Volume " << truck.totalVolume << endl;
	}


	//Print solution
	/*for (int& truck : solution)
	{
		cout << truck << " ";
	}*/
	cout << endl;
 
}