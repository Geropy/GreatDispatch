#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <map>
#include <time.h>
#include <ctime>

// Regardless of which method I end up using in the end, I will start by getting an initial greedy solution
// The basic greedy solution seems simple to me: Put the box in the lightest truck that has space
// This will ensure that all truck weights increase at similar rate

// I am currently just under 10
// I have odd behaviour. I get worse scores typically when there's less boxes
// I think I can explain this. When there's more boxes, there's a greater chance for a useful swap to occur, since there's more options.
// Therefore, my path can extend a while longer before I shuffle and restart
// Why do I restart completely? Instead of starting from scratch, why don't I look a random improving swap instead?
// Out of the heaviest and lightest truck, see which has the bigger delta
// Randomly search for a truck whose weight is on the other side of the target
// Search for a good swap, repeat

using namespace std;

struct Box
{
	float weight;
	float volume;
	unsigned int ID;

	Box(float weight, float volume, unsigned int ID)
		: weight(weight)
		, volume(volume)
		, ID(ID)
	{}

	Box() {}
};

class Truck
{
public:

	vector<Box*> boxes;
	float totalWeight;
	float totalVolume;
	unsigned int idx;

	Truck()
		: totalWeight(0.0f)
		, totalVolume(0.0f)
	{
	}

	void addBox(Box & box)
	{
		boxes.push_back(&box);
		totalWeight += box.weight;
		totalVolume += box.volume;
	}

	void removeBox(Box & box)
	{
		for (int i = 0; i < boxes.size(); i++)
		{
			if (boxes[i]->ID == box.ID)
			{
				boxes.erase(boxes.begin() + i);
				totalVolume -= box.volume;
				totalWeight -= box.weight;
				return;
			}
		}
	}

	void emptyBoxes()
	{
		boxes.clear();
		totalWeight = 0.0f;
		totalVolume = 0.0f;
	}
};

bool truckWeightComp(const Truck& a, const Truck& b) { return a.totalWeight < b.totalWeight; }
bool truckVolumeComp(const Truck& a, const Truck& b) { return a.totalVolume < b.totalVolume; }
bool boxVolumeComp(Box* a, Box* b) { return a->volume < b->volume; }
bool boxWeightComp(Box* a, Box* b) { return a->weight < b->weight; }
bool boxSortDensity(const Box& a, const Box& b) { return a.weight / a.volume > b.weight / b.volume; }

bool boxSwap(Truck& heavyTruck, Truck& lightTruck, vector<int> & solution)
{
	// Look for a simple 1-1 swap between heavy and light truck
	int heavyIndex = -1;
	int lightIndex = -1;
	Box* heavyBox = nullptr;
	Box* lightBox = nullptr;
	float minDelta = heavyTruck.totalWeight - lightTruck.totalWeight;

	for (unsigned int i = 0; i < heavyTruck.boxes.size(); i++)
	{
		heavyBox = heavyTruck.boxes[i];
		for (unsigned int j = 0; j < lightTruck.boxes.size(); j++)
		{
			lightBox = lightTruck.boxes[j];

			// Volumes need to work out, and score needs to improve
			if (heavyTruck.totalVolume + lightBox->volume - heavyBox->volume > 100.0f || lightTruck.totalVolume - lightBox->volume + heavyBox->volume > 100.0f) { continue; }
			float delta = abs((heavyTruck.totalWeight - heavyBox->weight + lightBox->weight) - (lightTruck.totalWeight + heavyBox->weight - lightBox->weight));

			if (delta < minDelta)
			{
				minDelta = delta;
				heavyIndex = i;
				lightIndex = j;
			}
		}

	}

	if (heavyIndex != -1)
	{
		lightBox = lightTruck.boxes[lightIndex];
		heavyBox = heavyTruck.boxes[heavyIndex];
		solution[lightBox->ID] = heavyTruck.idx;
		solution[heavyBox->ID] = lightTruck.idx;
		heavyTruck.addBox(*lightBox);
		lightTruck.addBox(*heavyBox);
		heavyTruck.removeBox(*heavyBox);
		lightTruck.removeBox(*lightBox);

		return true;
	}

	return false;
}

int main()
{
	srand(time(NULL));

	array<Truck, 100> trucks;
	vector<Box> boxes;
	for (unsigned int i = 0; i < 100; i++)
	{
		trucks[i].idx = i;
	}

	unsigned int boxCount, truckIndex;
	float minWeight;

	float totalBoxWeight = 0.0f;
	float totalBoxVolume = 0.0f;

	cin >> boxCount; cin.ignore();
	for (unsigned int i = 0; i < boxCount; i++)
	{
		float weight;
		float volume;
		cin >> weight >> volume; cin.ignore();

		totalBoxWeight += weight;
		totalBoxVolume += volume;

		boxes.emplace_back(weight, volume, i);
	}

	vector<int> bestSolution;
	vector<int> solution(boxCount, -1);
	float bestScore = 9999999.0f;
	clock_t startTime = clock();
	while ((clock() - startTime) / CLOCKS_PER_SEC < 48)
	{
		for (unsigned int i = 0; i < 100; i++)
		{
			trucks[i].emptyBoxes();
		}

		random_shuffle(boxes.begin(), boxes.end());

		for (auto& box : boxes)
		{
			// Find the truck with the lowest weight and that has space
			minWeight = 9999999.0f;
			truckIndex = -1;
			for (unsigned int i = 0; i < trucks.size(); i++)
			{
				Truck& truck = trucks[i];
				if (truck.totalWeight < minWeight && box.volume < (100.0f - truck.totalVolume))
				{
					minWeight = truck.totalWeight;
					truckIndex = i;
				}
			}

			if (truckIndex == -1)
			{
				cout << "error" << endl;
			}

			// Add it to the solution, and update the truck
			solution[box.ID] = truckIndex;
			trucks[truckIndex].addBox(box);
		}

		// Since I know the total box weight, I know the precise target weight for each truck. This is very useful
		float targetWeight = totalBoxWeight / 100.0f;

		// I also know the avergae volume that the trucks will have, and therefore an average density
		float averageVolume = totalBoxVolume / 100.0f;
		float averageDensity = targetWeight / averageVolume;

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

		// The first thing I can try to do is move boxes out of the heaviest truck
		// Take the smallest box from that truck and put it in the lighest truck with room
		// Repeat until it no longer works


		while ((clock() - startTime) / CLOCKS_PER_SEC < 40)
		{
			Truck& heaviestTruck = *max_element(trucks.begin(), trucks.end(), truckWeightComp);
			Truck& lightestTruck = *min_element(trucks.begin(), trucks.end(), truckWeightComp);

			//Box* smallestBox = *min_element(heaviestTruck.boxes.begin(), heaviestTruck.boxes.end(), boxVolumeComp);

			// Find the truck with the lowest weight and that has space
			// Make sure it actually improves things
			/*minWeight = heaviestTruck.totalWeight - smallestBox->weight;
			truckIndex = -1;
			for (unsigned int i = 0; i < trucks.size(); i++)
			{
				Truck& truck = trucks[i];
				if (truck.totalWeight < minWeight && smallestBox->volume < (100.0f - truck.totalVolume))
				{
					minWeight = truck.totalWeight;
					truckIndex = i;
				}
			}


			if (truckIndex != -1)
			{
				solution[smallestBox->ID] = truckIndex;
				trucks[truckIndex].addBox(*smallestBox);
				heaviestTruck.removeBox(*smallestBox);

				continue;
			}*/


			// When I get here, I typically have the lightest truck with plenty of room, which is good!
			// However, the smallest box in the heaviest truck is so heavy that I shouldn't move it
			// Pick an optimal box from this truck to move instead
			float minDelta = heaviestTruck.totalWeight - lightestTruck.totalWeight;
			int boxIndex = -1;
			Box* box;
			for (unsigned int i = 0; i < heaviestTruck.boxes.size(); i++)
			{
				box = heaviestTruck.boxes[i];
				if (lightestTruck.totalVolume + box->volume > 100.0f) { continue; }
				float delta = abs(heaviestTruck.totalWeight - box->weight - (lightestTruck.totalWeight + box->weight));
				if (delta < minDelta)
				{
					minDelta = delta;
					boxIndex = i;
				}
			}

			if (boxIndex != -1)
			{
				box = heaviestTruck.boxes[boxIndex];
				solution[box->ID] = lightestTruck.idx;
				lightestTruck.addBox(*box);
				heaviestTruck.removeBox(*box);

				continue;
			}

			// find the lightest box in the lightest truck
			// put it in the lightest truck with room
			Box* lightestBox = *min_element(lightestTruck.boxes.begin(), lightestTruck.boxes.end(), boxWeightComp);

			// Find the truck with the lowest weight and that has space
			// If the receiving truck will now be the heaviest, I've probably saturated the usefulness
			minWeight = heaviestTruck.totalWeight - lightestBox->weight;
			truckIndex = -1;
			for (unsigned int i = 0; i < trucks.size(); i++)
			{
				if (i == lightestTruck.idx) { continue; }
				Truck& truck = trucks[i];
				if (truck.totalWeight < minWeight && lightestBox->volume < (100.0f - truck.totalVolume))
				{
					minWeight = truck.totalWeight;
					truckIndex = i;
				}
			}

			if (truckIndex != -1)
			{
				solution[lightestBox->ID] = truckIndex;
				trucks[truckIndex].addBox(*lightestBox);
				lightestTruck.removeBox(*lightestBox);

				continue;
			}

			// Look for a simple 1-1 swap between heaviest and lightest truck
			if (boxSwap(heaviestTruck, lightestTruck, solution)) { continue; }

			// Then look for swaps with random trucks on the other side of the weight spectrum
			bool found = false;

			for (int i = 0; i < 100; i++)
			{
				truckIndex = i;
				if (truckIndex == lightestTruck.idx) { continue; }
				if (boxSwap(trucks[truckIndex], lightestTruck, solution))
				{
					found = true;
					break;
				}
			}


			if (found) { continue; }


			for (int i = 0; i < 100; i++)
			{
				truckIndex = i;
				if (truckIndex == heaviestTruck.idx) { continue; }
				if (boxSwap(heaviestTruck, trucks[truckIndex], solution))
				{
					found = true;
					break;
				}
			}


			if (found) { continue; }

			break;
		}

		Truck& heaviestTruck = *max_element(trucks.begin(), trucks.end(), truckWeightComp);
		Truck& lightestTruck = *min_element(trucks.begin(), trucks.end(), truckWeightComp);
		float score = heaviestTruck.totalWeight - lightestTruck.totalWeight;
		if (score < bestScore)
		{
			bestSolution = solution;
			bestScore = score;
		}
	}

	//Print solution
	for (int& truck : bestSolution)
	{
		cout << truck << " ";
	}
	cout << endl;

}