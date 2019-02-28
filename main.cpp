#include <iostream>
#include <vector>
#include <map>
#include <thread>
#include <fstream>
#include <memory>

using namespace std;

map<string, int> ids;

struct Image {
	int id;
	char type;
	vector<int> tags;
};

int num_images;

vector<unique_ptr<Image>> images;

auto main(int argc, char ** argv) -> int {

	ifstream inFile;
	inFile.open(argv[1]);

	inFile >> num_images;

	int tags;
	string tempTag;
	int tagCounter = 0;
	for (int i = 0; i < num_images; i++){
		images.push_back(make_unique<Image>());
		images[i]->id = i;
		inFile >> images[i]->type;
		inFile >> tags;
		for (int j = 0; j < tags; j++){
			inFile >> tempTag;
			if (auto it = ids.find(tempTag); it != ids.end()){
				images[i]->tags.push_back(ids[tempTag]);
			} else {
				ids.insert({tempTag, ++tagCounter});
				images[i]->tags.push_back(tagCounter);
			}
		}
	}

	for (auto &image : images){
		cout << image->id << " " << image->type << " ";
		for (auto &tag : image->tags){
			cout << tag << " ";
		}
		cout << endl;
	}

	return 0;
}
