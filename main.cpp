#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <memory>
#include <algorithm>

using namespace std;

map<string, int> ids;

struct Image {
	int id;
	bool taken = false;
	char type;
	vector<int> tags;
};

struct Slide {
	int id[2];
	bool taken = false;
	vector<int> tags;
};

int num_images;

vector<unique_ptr<Image>> images;
vector<unique_ptr<Slide>> slides;
vector<unique_ptr<Slide>> new_slides;

auto get_input(char*) -> void;
auto generate_slides() -> void;
auto make_best_show() -> void;
auto make_best_show_2() -> void;
auto output_slides(char*) -> void;
auto scoreSingle(unique_ptr<Slide>&, unique_ptr<Slide>&) -> int;
auto scoreSingle(unique_ptr<Image>&, unique_ptr<Image>&) -> int;
auto score_slides() -> int;
auto print_slides() -> void;

auto main(int argc, char ** argv) -> int {

	get_input(argv[1]);

	cout << "Done Getting Input" << endl;

	generate_slides();

	cout << "Done Generating" << endl;

	make_best_show();
	
	cout << "Score: " << score_slides() << endl;

	output_slides(argv[2]);

	//print_slides();

	return 0;
}

auto get_input(char *filename) -> void {
	ifstream inFile;

	inFile.open(filename);

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

	inFile.close();
}

auto generate_slides() -> void {
	int slideCounter = 0;
	for (int i = 0; i < images.size(); i++){
		if (i % 1000 == 0) cout << i << endl;
		if (images[i]->taken){
			continue;
		}
		if (images[i]->type == 'H'){
			slides.push_back(make_unique<Slide>());
			slides[slideCounter]->id[0] = images[i]->id;
			slides[slideCounter]->tags = images[i]->tags;
			slides[slideCounter]->id[1] = -1;
		} else if (images[i]->type == 'V'){
			slides.push_back(make_unique<Slide>());
			slides[slideCounter]->id[0] = images[i]->id;
			slides[slideCounter]->tags = images[i]->tags;
			int bestIndex = -1;
			int bestScore = -1;
			int tempScore;
			for (int j = i + 1; j < images.size(); j++){
				if (images[j]->type == 'V' && !images[j]->taken){
					tempScore = scoreSingle(images[i], images[j]);
					if (tempScore > bestScore){
						bestScore = tempScore;
						bestIndex = j;
					}
				}
			}
			slides[slideCounter]->id[1] = images[bestIndex]->id;
			for (auto &tag : images[bestIndex]->tags){
				if (find(slides[slideCounter]->tags.begin(), slides[slideCounter]->tags.end(), tag) == slides[slideCounter]->tags.end()){
					slides[slideCounter]->tags.push_back(tag);
				}
			}
			images[bestIndex]->taken = true;
		}
		slideCounter++;
	}
}

auto make_best_show() -> void {
	new_slides.push_back(make_unique<Slide>());
	new_slides[0]->id[0] = slides[0]->id[0];
	new_slides[0]->id[1] = slides[0]->id[1];
	new_slides[0]->tags = slides[0]->tags;
	int bestScore, bestIndex, tempScore;
	for (int i = 1; i < slides.size(); i++){
		bestScore = -1;
		for (int j = 1; j < slides.size(); j++){
			if (slides[j]->taken == false){
				tempScore = scoreSingle(new_slides[new_slides.size() - 1], slides[j]);
				if (tempScore > bestScore){
					bestScore = tempScore;
					bestIndex = j;
				}
			}
		}
		new_slides.push_back(make_unique<Slide>());
		new_slides[new_slides.size() - 1]->id[0] = slides[bestIndex]->id[0];
		new_slides[new_slides.size() - 1]->id[1] = slides[bestIndex]->id[1];
		new_slides[new_slides.size() - 1]->tags = slides[bestIndex]->tags;
		slides[bestIndex]->taken = true;
		if (i % 10 == 0) cout << "i = " << i << endl;
	}
}

auto make_best_show_2() -> void {
	new_slides.push_back(make_unique<Slide>());
	new_slides[0]->id[0] = slides[0]->id[0];
	new_slides[0]->id[1] = slides[0]->id[1];
	new_slides[0]->tags = slides[0]->tags;
	
	for (int i = 1; i < slides.size(); i++){
		if (scoreSingle(new_slides[0], slides[i]) >= scoreSingle(new_slides[new_slides.size() - 1], slides[i])){
			new_slides.insert(new_slides.begin(), make_unique<Slide>());
			new_slides[0]->id[0] = slides[i]->id[0];
			new_slides[0]->id[1] = slides[i]->id[1];
			new_slides[0]->tags = slides[i]->tags;
		} else {
			new_slides.push_back(make_unique<Slide>());
			new_slides[new_slides.size() - 1]->id[0] = slides[i]->id[0];
			new_slides[new_slides.size() - 1]->id[1] = slides[i]->id[1];
			new_slides[new_slides.size() - 1]->tags = slides[i]->tags;
		}
	}
}

auto output_slides(char *filename) -> void {
	ofstream outFile;
	outFile.open(filename);

	outFile << slides.size() << '\n';

	for (auto &slide : slides){
		outFile << slide->id[0];
		if (slide->id[1] != -1){
			outFile << " " << slide->id[1];
		}
		outFile << '\n';
	}

	outFile.close();
}

auto scoreSingle(unique_ptr<Slide> &s1, unique_ptr<Slide> &s2) -> int {
	int s1_u_tags = 0;
	int s2_u_tags = 0;
	int same_tags = 0;

	for (auto &tag : s1->tags){
		if (find(s2->tags.begin(), s2->tags.end(), tag) != s2->tags.end()){
			same_tags++;
		} else {
			s1_u_tags++;
		}
	}
	for (auto &tag : s2->tags){
		if (find(s1->tags.begin(), s1->tags.end(), tag) == s1->tags.end()){
			s2_u_tags++;
		}
	}

	return min({s1_u_tags, s2_u_tags, same_tags});
}

auto scoreSingle(unique_ptr<Image> &s1, unique_ptr<Image> &s2) -> int {
	int s1_u_tags = 0;
	int s2_u_tags = 0;
	int same_tags = 0;

	for (auto &tag : s1->tags){
		if (find(s2->tags.begin(), s2->tags.end(), tag) != s2->tags.end()){
			same_tags++;
		} else {
			s1_u_tags++;
		}
	}
	for (auto &tag : s2->tags){
		if (find(s1->tags.begin(), s1->tags.end(), tag) == s1->tags.end()){
			s2_u_tags++;
		}
	}

	return min({s1_u_tags, s2_u_tags, same_tags});
}

auto score_slides() -> int {
	int total = 0;
	for (int i = 0; i < new_slides.size() - 1; i++){
		total += scoreSingle(new_slides[i], new_slides[i + 1]);
	}
	return total;
}

auto print_slides() -> void {
	for (auto &slide : slides){
		cout << slide->id[0] << " " << slide->id[1] << " ";
		for (auto &tag : slide->tags){
			cout << tag << " ";
		}
		cout << endl;
	}
}
