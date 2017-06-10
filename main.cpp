/*Hash Table*/

#include <iostream>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <fstream>

#define ID_MAX 999999
#define MAX_COLLISIONS 3
#define START_SIZE 10

using namespace std;

/*Student struct to hold student data*/
struct student {
	student() { next = NULL; }

	char firstname[10];
	char lastname[10];
	int id;
	float gpa;

	student* next;
};

/*Global vars for the table*/
int tablelen;
student** table;

/*Hash function*/
int hash(int id) {
	return id%tablelen;
}	

/*Search for a student*/
student* getStudent(int id) {
	student* s = table[hash(id)];
	
	while (s != NULL && s->id != id) {
		s = s->next;
	}

	return s;
}

/*Add a student to the table*/
int addstudent(student *s) {
	int index = hash(s->id);
	int collisions = 0;

	//Check if already a student with id present
	if (getStudent(s->id)) {
		return 1;	//Error
	}

	//No collision
	if (table[index] == NULL) {
		table[index] = s;
		return 0;
	}

	//Collision, add in next in chain
	collisions++;
	student* chainlink = table[index];
	while (chainlink->next != NULL) {
		collisions++;
		chainlink = chainlink->next;
	}
	chainlink->next = s;

	//More than acceptable collisions, need to rehash
	if (collisions > MAX_COLLISIONS) {
		student** oldtable = table;
		tablelen *= 2;
		table = new student*[tablelen];
		
		for (int i = 0; i < tablelen; i++)
			table[i] = NULL;

		//Add all the values into new table
		for (int i = 0; i < tablelen/2; i++) {
			student* toadd = oldtable[i];

			while (toadd != NULL) {
				student* st = toadd;
				toadd = toadd->next;
				st->next = NULL;
				addstudent(st);
			}
		}

		//Delete old table
		delete[] oldtable;
	}
	
	return 0;
}


/*Print the table*/
void print() {
	int count = 0;

	for (int i = 0; i < tablelen; i++) {
		
		student* s = table[i];

		while (s != NULL) {
			cout << "--------------------------" << endl;
			cout << s->firstname << " " << s->lastname << endl;
			cout << "ID: " << s->id << endl;
			cout << "GPA: " << s->gpa << endl;
			s = s->next;

			++count;
		}
	}

	cout << "_________________________" << endl;
	cout << count << " students in table" << endl;
}

/*Remove a student*/
int remove(int id) {

	int index = hash(id);

	student *s = table[index];

	//Nothing at index
	if (s == NULL)
		return 1;

	//Find in list
	if (s->id == id) {
		table[index] = s->next;
		delete s;
		return 0;
	}

	while (s->next != NULL) {
		if (s->next->id == id) {
			student *target = s->next;
			s->next = target->next;
			delete target;
			return 0;
		}

		s = s->next;
	}

	return 1;

}

main() {
	tablelen = START_SIZE;
	table = new student*[tablelen];
	char input[20];
	bool done = false;
	char firstnames[50][20];
	char lastnames[50][20];

	srand(time(NULL));

	for (int i = 0; i < tablelen; i++)
		table[i] = NULL;
	
	//Load names
	ifstream firstf ("firstnames.txt"), lastf ("lastnames.txt");
	if (firstf.is_open()  && lastf.is_open()) {
		int name = 0;
		while (name++ < 50) {
			firstf >> firstnames[name];
			lastf >> lastnames[name];
		}

		firstf.close();
		lastf.close();
	}
	else {
		cout << "Unable to read the list of names for random generation" << endl;
		return 1;
	}


	while (!done) {
		cout << "Enter command: ADD (a), PRINT (p), DELETE (d), GENERATE (g), or QUIT (q): " << flush;
		cin >> input;

		//Add a student
		if (strcmp(input, "ADD") == 0 || strcmp(input, "a") == 0) {
			student* s = new student();
			
			cout << "Enter the name: " << flush;
			cin >> s->firstname >> s->lastname;

			cout << "Enter the ID number (000000-999999): " << flush;
			cin >> s->id;

			cout << "Enter GPA: " << flush;
			cin >> s->gpa;
			
			if (s->id > 999999 || s->id < 0) {
				cout << "ID must be between 0 and 999999" << endl;
				continue;
			}
			

			//Add and check errors
			if (addstudent(s) == 1) {
				cout << "ID already taken" << endl;
				continue;
			}


			cout << "Added" << endl;
		}

		//Print the table
		else if (strcmp(input, "PRINT") == 0 || strcmp(input, "p") == 0) {
			print();
		}

		//Delete a student
		else if (strcmp(input, "DELETE") == 0 || strcmp(input, "d") == 0) {
                        int id;
			
			cout << "Enter the id number (000000-999999): " << flush;
                        cin >> id;

			if (remove(id) == 1)
				cout << "No such student" << endl;
			else
				cout << "Deleted" << endl;
		}

		//Generate random students
		else if (strcmp(input, "GENERATE") == 0 || strcmp(input, "g") == 0) {
			int num;
			
			cout << "Enter how many students to generate: " << flush;
			cin >> num;

			while (num--) {
			
				student* s = new student();
				
				strcpy(s->firstname, firstnames[rand()%50]);
				strcpy(s->lastname, lastnames[rand()%50]);
				s->gpa = (float)(rand()%4000)/1000;
				

				//Add at next available id
				s->id = 0;
				while(getStudent(s->id) != NULL)
					s->id++;

				addstudent(s);

			}

			cout << "Generated" << endl;	
		}

		//Quit
		else if (strcmp(input, "QUIT") == 0 || strcmp(input, "q") == 0) {
			done = true;
		}

		//Invalid command
		else {
			cout << "Invalid command" << endl;
		}

	}
}
