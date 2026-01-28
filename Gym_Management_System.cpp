#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
using namespace std;

/* MEMBER */
class Member {
public:
    int id;
    string name;
    string membershipType;
    int duration;
    int price;

    int startDay, startMonth, startYear;
    int expDay, expMonth, expYear;

    void input(int generatedId) {
        id = generatedId;
        cin.ignore();

        cout << "Enter Name: ";
        getline(cin, name);

        int choice;
        cout << "\nMembership Packages:\n";
        cout << "1. Monthly   - 2500 Birr\n";
        cout << "2. 3-Month   - 6000 Birr\n";
        cout << "3. 6-Month   - 10000 Birr\n";
        cout << "4. Yearly    - 18000 Birr\n";
        cout << "Enter choice (1-4): ";

        do {
            cin >> choice;
            if (choice < 1 || choice > 4)
                cout << "Invalid choice! Please enter 1-4: ";
        } while (choice < 1 || choice > 4);

        switch (choice) {
        case 1: membershipType = "Monthly"; duration = 1; price = 2500; break;
        case 2: membershipType = "3-Month"; duration = 3; price = 6000; break;
        case 3: membershipType = "6-Month"; duration = 6; price = 10000; break;
        case 4: membershipType = "Yearly";  duration = 12; price = 18000; break;
        }

        // Automatic start date
        time_t now = time(0);
        tm *t = localtime(&now);

        startDay = t->tm_mday;
        startMonth = 1 + t->tm_mon;
        startYear = 1900 + t->tm_year;

        calculateExpiry();
    }

    void calculateExpiry() {
        expDay = startDay;
        expMonth = startMonth + duration;
        expYear = startYear;
        while (expMonth > 12) { expMonth -= 12; expYear++; }
    }

    bool isExpired(int d, int m, int y) {
        if (y > expYear) return true;
        if (y == expYear && m > expMonth) return true;
        if (y == expYear && m == expMonth && d >= expDay) return true;
        return false;
    }

    void display(int d, int m, int y) {
        cout << "Name: " << name << " (ID: " << id << ")\n";
        cout << "Package: " << membershipType << " (" << price << " Birr)\n";
        cout << "Start Date: " << startDay << "/" << startMonth << "/" << startYear << "\n";
        cout << "Expiry Date: " << expDay << "/" << expMonth << "/" << expYear << "\n";
        cout << "Status: " << (isExpired(d, m, y) ? "EXPIRED" : "ACTIVE") << "\n";
        cout << "-----------------------------\n";
    }

    // Save member to file (append)
    void saveToFile() {
        ofstream fout("member.txt", ios::app);
        fout << id << "," << name << "," << membershipType << "," << duration << "," 
             << price << "," << startDay << "," << startMonth << "," << startYear << "\n";
        fout.close();
    }

    // Load member from line
    void loadFromLine(string line) {
        size_t pos = 0;
        size_t idx = 0;
        string token;
        while ((pos = line.find(',')) != string::npos) {
            token = line.substr(0,pos);
            switch(idx) {
                case 0: id = stoi(token); break;
                case 1: name = token; break;
                case 2: membershipType = token; break;
                case 3: duration = stoi(token); break;
                case 4: price = stoi(token); break;
                case 5: startDay = stoi(token); break;
                case 6: startMonth = stoi(token); break;
                case 7: startYear = stoi(token); break;
            }
            line.erase(0,pos+1);
            idx++;
        }
    }
};

/* TRAINER */
class Trainer {
public:
    int id;
    string name;
    string specialization;

    void input(int generatedId) {
        id = generatedId;
        cin.ignore();
        cout << "Enter Trainer Name: ";
        getline(cin, name);
        cout << "Enter Specialization: ";
        getline(cin, specialization);
    }

    void display() {
        cout << id << ". " << name << " - " << specialization << endl;
    }

    void saveToFile() {
        ofstream fout("trainer.txt", ios::app);
        fout << id << "," << name << "," << specialization << "\n";
        fout.close();
    }

    void loadFromLine(string line) {
        size_t pos = line.find(',');
        id = stoi(line.substr(0,pos));
        line = line.substr(pos+1);
        pos = line.find(',');
        name = line.substr(0,pos);
        specialization = line.substr(pos+1);
    }
};

/* SYSTEM */
class GymSystem {
private:
    Member members[100];
    Trainer trainers[50];
    int memberCount = 0;
    int trainerCount = 0;
    int nextMemberId = 1;
    int nextTrainerId = 1;

public:
    GymSystem() {
        loadMembers();
        loadTrainers();
    }

    /* ---------- Members ---------- */
    void addMember() {
        if (memberCount >= 100) { cout << "Member limit reached.\n"; return; }
        members[memberCount].input(nextMemberId);
        members[memberCount].saveToFile();
        memberCount++;
        nextMemberId++;
        cout << "Member added successfully.\n";
    }

    void viewMembers() {
        if (memberCount == 0) { cout << "No members.\n"; return; }
        time_t now = time(0);
        tm *t = localtime(&now);
        int d = t->tm_mday, m = 1+t->tm_mon, y = 1900+t->tm_year;
        for(int i=0;i<memberCount;i++) members[i].display(d,m,y);
    }

    void removeMember() {
        int id;
        cout << "Enter Member ID to remove: ";
        cin >> id;
        ifstream fin("member.txt");
        ofstream fout("temp.txt");
        string line;
        bool found = false;
        while(getline(fin,line)) {
            Member m;
            m.loadFromLine(line);
            if(m.id == id) { found=true; continue; }
            fout << line << "\n";
        }
        fin.close(); fout.close();
        remove("member.txt");
        rename("temp.txt","member.txt");
        if(found) cout << "Member removed.\n"; else cout << "Member not found.\n";
        loadMembers();
    }

    void loadMembers() {
        memberCount = 0;
        ifstream fin("member.txt");
        string line;
        int maxId=0;
        while(getline(fin,line)) {
            members[memberCount].loadFromLine(line);
            if(members[memberCount].id > maxId) maxId = members[memberCount].id;
            memberCount++;
        }
        nextMemberId = maxId + 1;
        fin.close();
    }

    /* ---------- Trainers ---------- */
    void addTrainer() {
        if(trainerCount >=50) { cout << "Trainer limit reached.\n"; return; }
        trainers[trainerCount].input(nextTrainerId);
        trainers[trainerCount].saveToFile();
        trainerCount++; nextTrainerId++;
        cout << "Trainer added successfully.\n";
    }

    void viewTrainers() {
        if(trainerCount==0) { cout << "No trainers.\n"; return; }
        for(int i=0;i<trainerCount;i++) trainers[i].display();
    }

    void removeTrainer() {
        int id;
        cout << "Enter Trainer ID to remove: ";
        cin >> id;
        ifstream fin("trainer.txt");
        ofstream fout("temp.txt");
        string line;
        bool found=false;
        while(getline(fin,line)) {
            Trainer t;
            t.loadFromLine(line);
            if(t.id==id) { found=true; continue; }
            fout << line << "\n";
        }
        fin.close(); fout.close();
        remove("trainer.txt");
        rename("temp.txt","trainer.txt");
        if(found) cout << "Trainer removed.\n"; else cout << "Trainer not found.\n";
        loadTrainers();
    }

    void loadTrainers() {
        trainerCount=0; int maxId=0;
        ifstream fin("trainer.txt"); string line;
        while(getline(fin,line)) {
            trainers[trainerCount].loadFromLine(line);
            if(trainers[trainerCount].id>maxId) maxId = trainers[trainerCount].id;
            trainerCount++;
        }
        nextTrainerId = maxId + 1;
        fin.close();
    }
};

/* MAIN */
int main() {
    GymSystem gym;
    int choice;
    do {
        cout << "\n1. Add Member\n2. View Members\n3. Remove Member\n";
        cout << "4. Add Trainer\n5. View Trainers\n6. Remove Trainer\n7. Exit\n";
        cout << "Enter choice: "; cin >> choice;
        switch(choice){
            case 1: gym.addMember(); break;
            case 2: gym.viewMembers(); break;
            case 3: gym.removeMember(); break;
            case 4: gym.addTrainer(); break;
            case 5: gym.viewTrainers(); break;
            case 6: gym.removeTrainer(); break;
            case 7: cout<<"Exiting...\n"; break;
            default: cout<<"Invalid choice.\n";
        }
    } while(choice!=7);
    return 0;
}
