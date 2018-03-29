#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <deque>
#include <map>

// Regardless of which method I end up using in the end, I will start by getting an initial greedy solution
// The basic greedy solution seems simple to me: Put the box in the lightest truck that has space
// This will ensure that all truck weights increase at similar rate

// I actually know that very close solutions are possible all the time
// Therefore, I know almost exactly how much weight needs to be on every truck
// Maybe branching is better, try it 

using namespace std;

// Adjust this tolerance to sacrifice time for a better result
static float tolerance = 30.0f;
static float baseTargetWeight;

struct Box
{
	const float weight;
	const float volume;
	const unsigned int ID;

	Box(float weight, float volume, unsigned int ID)
		: weight(weight)
		, volume(volume)
		, ID(ID)
	{}
};

class Truck
{
public:
	
	map<unsigned int, Box*> boxes;
	float totalWeight;
	float totalVolume;
	unsigned int idx;

	Truck()
		: totalWeight(0.0f)
		, totalVolume(0.0f)
	{
	}

	void addBox(Box * box, unsigned int ID) 
	{
		boxes.insert(map<unsigned int, Box*>::value_type(ID, box));
		totalWeight += box->weight;
		totalVolume += box->volume;
	}
};

bool truckWeightComp(const Truck& a, const Truck& b) { return a.totalWeight < b.totalWeight; }

bool boxVolumeComp(const Box* a, const Box* b) { return a->volume < b->volume; }

vector<unsigned int> getSolution(deque<Box*> & shelfBoxes, float targetWeight, float availableVolume)
{
	vector<unsigned int> solution;

	if (targetWeight < 0.0f || availableVolume < 0.0f) { return solution; }

	// See if a single box satisfies the condition
	for (int i = shelfBoxes.size() - 1; i >= 0; i--)
	{
		auto box = shelfBoxes[i];
		if (abs(box->weight - targetWeight) < tolerance && box->volume < availableVolume)
		{
			// Found a candidate, I'm done with this truck
			// Move on to the next
			cout << "Found a truck solution " << shelfBoxes.size() << endl;

			Box* removedBox = shelfBoxes.back();
			solution.push_back(box->ID);
			shelfBoxes.erase(shelfBoxes.begin() + i);

			// If the shelf is empty, I'm done
			if (shelfBoxes.empty()) { return solution; }


			vector<unsigned int> subSolution = getSolution(shelfBoxes, baseTargetWeight, 100.0f);
			if (!subSolution.empty())
			{
				solution.insert(solution.end(), subSolution.begin(), subSolution.end());
				return solution;
			}

			// If I get here, I couldn't find a solution that works, try the next one
			shelfBoxes.insert(shelfBoxes.begin() + i, removedBox);
			cout << "up we go" << endl;
			solution.pop_back();
		}
	}

	// If I get here, a single-box option wasn't found.
	// Need an intermediate box, what's a good greedy way of choosing?
	// Maybe small volume, so I have lots of options?
	// Maybe large volume, so I fail early?
	// The shelf boxes are sorted by descending volume, so go through the boxes in reverse order

	for (int i = shelfBoxes.size() - 1; i >= 0; i--)
	{
		Box* removedBox = shelfBoxes.back();
		solution.push_back(removedBox->ID);
		shelfBoxes.pop_back();
		vector<unsigned int> subSolution = getSolution(shelfBoxes, targetWeight - removedBox->weight, availableVolume - removedBox->volume);
		if (!subSolution.empty())
		{
			solution.insert(solution.end(), subSolution.begin(), subSolution.end());
			return solution;
		}

		// If I get here, I couldn't find a solution that works, try the next one
		shelfBoxes.push_front(removedBox);
		solution.pop_back();
	}

	return solution;
}

int main()
{
	array<Truck, 100> trucks;
	vector<Box> boxes;
	deque<Box*> shelfBoxes;

	for (unsigned int i = 0; i < 100; i++)
	{
		trucks[i].idx = i;
	}

	vector<int> solution;
	unsigned int boxCount, truckIndex;
	float totalBoxWeight = 0.0f;
	float totalBoxVolume = 0.0f;

    cin >> boxCount; cin.ignore();
    for (unsigned int i = 0; i < boxCount; i++) 
	{
        float weight;
        float volume;
        cin >> weight >> volume; cin.ignore();

		boxes.emplace_back(weight, volume, i);
		totalBoxWeight += weight;
		totalBoxVolume += volume;
    }

	for (unsigned int i = 0; i < boxCount; i++)
	{
		shelfBoxes.push_back(&boxes[i]);
	}

	// There is a target weight for each truck;
	baseTargetWeight = totalBoxWeight / 100.0f;

	// There is a average volume for each truck
	float averageVolume = totalBoxVolume / 100.0f;

	// Sort the shelf boxes by descending volume
	sort(shelfBoxes.begin(), shelfBoxes.end(), boxVolumeComp);

	// Try to get the first truck to hold the targetWeight
	auto tempShelfBoxes = shelfBoxes;

	vector<unsigned int> boxList = getSolution(tempShelfBoxes, baseTargetWeight, 100.0f);

	// Look for an optimal swap between the 2


	

	// Print truck status
	for (auto& boxID : boxList)
	{
		cout << "Weight " << boxes[boxID].weight << " Volume " << boxes[boxID].volume << endl;
	}


	//Print solution
	/*for (int& truck : solution)
	{
		cout << truck << " ";
	}*/
	cout << endl;
 
}