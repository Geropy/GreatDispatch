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
	float bestScore = 9999999.0f;
	clock_t startTime = clock();
	while ((clock() - startTime) / CLOCKS_PER_SEC < 40)
	{
		for (unsigned int i = 0; i < 100; i++)
		{
			trucks[i].emptyBoxes();
		}

		vector<int> solution(boxCount, -1);
		random_shuffle(boxes.begin(), boxes.end());

		for (auto& box : boxes)
		{
			// Find the truck with the lowest weight (randomly second lowest), and that has space
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
			/*if ((clock() - startTime) / CLOCKS_PER_SEC > 10)
			{
			int stop = 0;
			}*/

			Truck& heaviestTruck = *max_element(trucks.begin(), trucks.end(), truckWeightComp);
			Box* smallestBox = *min_element(heaviestTruck.boxes.begin(), heaviestTruck.boxes.end(), boxVolumeComp);

			// Find the truck with the lowest weight and that has space
			minWeight = 9999999.0f;
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

			// Make sure it actually improves things
			if (truckIndex != -1 && trucks[truckIndex].totalWeight + smallestBox->weight < heaviestTruck.totalWeight)
			{
				solution[smallestBox->ID] = truckIndex;
				trucks[truckIndex].addBox(*smallestBox);
				heaviestTruck.removeBox(*smallestBox);
			}
			else
			{
				// find the lightest box in the lightest truck
				// put it in the lightest truck with room
				Truck& lightestTruck = *min_element(trucks.begin(), trucks.end(), truckWeightComp);
				Box* lightestBox = *min_element(lightestTruck.boxes.begin(), lightestTruck.boxes.end(), boxWeightComp);

				// Find the truck with the lowest weight and that has space
				minWeight = 9999999.0f;
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

				// If the receiving truck will now be the heaviest, I've probably saturated the usefulness
				if (truckIndex != -1 && trucks[truckIndex].totalWeight + lightestBox->weight < heaviestTruck.totalWeight)
				{
					solution[lightestBox->ID] = truckIndex;
					trucks[truckIndex].addBox(*lightestBox);
					lightestTruck.removeBox(*lightestBox);
				}
				else
				{
					// When I get here, I typically have the lightest truck with plenty of room, which is good!
					// However, the smallest box in the heaviest truck is so heavy that I shouldn't move it
					// Pick an optimal box from this truck to move instead
					float minDelta = abs(targetWeight - heaviestTruck.totalWeight) + abs(targetWeight - lightestTruck.totalWeight);
					int boxIndex = -1;
					Box* box;
					for (unsigned int i = 0; i < heaviestTruck.boxes.size(); i++)
					{
						box = heaviestTruck.boxes[i];
						float delta = abs(targetWeight - (heaviestTruck.totalWeight - box->weight)) + abs(targetWeight - (lightestTruck.totalWeight + box->weight));
						if (delta < minDelta && box->volume < (100.0f - lightestTruck.totalVolume))
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
					}

					else
					{
						break;
					}

				}
			}
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
	
	
	
	// When I exit from the above, it will be because I don't have the space in the lighter trucks to put these boxes
	// I want to remove a box from the lighest truck and put it "on the shelf" temporarily
	// This means I can put other boxes inside
	// I need to find somewhere else to put it though


	//for (int i = 0; i < 10; i++)
	//{
	//	// Find the heaviest and lightest truck
	//	Truck& lightestTruck = *min_element(trucks.begin(), trucks.end(), truckWeightComp);
	//	// Testing something with volume
	//	Truck& heaviestTruck = *min_element(trucks.begin(), trucks.end(), truckVolumeComp);

	//	// Take out all the boxes from these two, and rearrange again
	//	vector<Box*> combinedBoxes = heaviestTruck.boxes;
	//	combinedBoxes.insert(combinedBoxes.end(), lightestTruck.boxes.begin(), lightestTruck.boxes.end());
	//	heaviestTruck.emptyBoxes();
	//	lightestTruck.emptyBoxes();

	//	// For now it's simple, redo the greedy algorithm
	//	for (auto box : combinedBoxes)
	//	{
	//		// Find the truck with the lowest weight, and that has space
	//		truckIndex = -1;
	//		minWeight = 9999999.0f;
	//		if (lightestTruck.totalWeight < minWeight && box->volume < (100.0f - lightestTruck.totalVolume))
	//		{
	//			minWeight = lightestTruck.totalWeight;
	//			truckIndex = lightestTruck.idx;
	//		}

	//		if (heaviestTruck.totalWeight < minWeight && box->volume < (100.0f - heaviestTruck.totalVolume))
	//		{
	//			minWeight = heaviestTruck.totalWeight;
	//			truckIndex = heaviestTruck.idx;
	//		}

	//		if (truckIndex == -1) { break; }

	//		// Add it to the solution, and update the truck
	//		solution[box->ID] = truckIndex;
	//		trucks[truckIndex].addBox(*box);
	//	}

	//	combinedBoxes.clear();
	//}

	// Print truck status
	/*for (auto& truck : trucks)
	{
		cout << "Weight " << truck.totalWeight << " Volume " << truck.totalVolume << endl;
	}*/

	//Print solution
	for (int& truck : bestSolution)
	{
		cout << truck << " ";
	}
	cout << endl;
 
}