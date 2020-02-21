#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <functional>
#include <random>
#include <algorithm>

using namespace std;

// 寮生を表すクラス
class Student {
public:
    string cur_room; // 現在の部屋
    string number;   // 学籍番号
    string name;     // 名前
    vector<function<int(string)>> satis; // 満足度の評価関数

    Student() { }

    Student(string cur_room, string number, string name, vector<function<int(string)>> satis) {
        this->cur_room = cur_room;
        this->number = number;
        this->name = name;
        this->satis = satis;
    }

    bool operator<(const Student &other) const {
        return this->number < other.number;
    };

    bool operator>(const Student &other) const {
        return this->number > other.number;
    };
};

// 部屋の情報
class RoomInfo {
public:
    string number;
    // int yoyaku;
    // int people;
    // int north;
    // int south;
    // int east;
    // int entrance;
    // int corner;
    // int washing;
    // int toilet;
    // int stairs;
    // int veranda;

    RoomInfo() { }

    RoomInfo(string number) {
        this->number = number;
    }

    bool operator<(const RoomInfo &other) const {
        return this->number < other.number;
    };

    bool operator>(const RoomInfo &other) const {
        return this->number > other.number;
    };
};

// 部屋を表すクラス
class Room {
public:
    string number;   // 部屋番号
    Student student; // 寮生

    Room() { }

    Room(string number, Student student) {
        this->number = number;
        this->student = student;
    }

    // 寮生の満足度
    int satisfaction() {
        int sum = 0;
        for (auto sati : this->student.satis)
            sum += sati(this->number);
        return sum;
    }

    bool operator<(const Room &other) const {
        return this->number < other.number;
    };

    bool operator>(const Room &other) const {
        return this->number > other.number;
    };
};

// 文字列の分割
vector<string> split(string& input, char delimiter)
{
    istringstream iss(input);
    string field;
    vector<string> result;
    while (getline(iss, field, delimiter))
        result.push_back(field);
    return result;
}

// 対象の号館か
bool satis_building(string room, char building) {
    return room[0] == building;
}

// 対象の階か
bool satis_floor(string room, char building, char floor) {
    return (room[0] == building && room[1] == floor);
}

// 対象の部屋か
bool satis_room(string room, string room_number) {
    return room == room_number;
}

// 満足度の総和を計算
int satis_sum(vector<Room> rooms) {
    int sum = 0;
    for (auto room : rooms)
        sum += room.satisfaction();
    return sum;
}

int main() {
    // 部屋一覧を読み込み
    vector<RoomInfo> room_infos;
    ifstream room_stream("rooms.csv");
    string line;

    getline(room_stream, line);
    while (getline(room_stream, line))
        room_infos.push_back(RoomInfo(split(line, ',')[0]));

    // 寮生希望一覧を読み込み
    vector<Student> students;
    ifstream student_stream("students.csv");

    getline(student_stream, line);
    while (getline(student_stream, line)) {
        vector<string> strs = split(line, ',');
        vector<function<int(string)>> satis;

        // 第1希望～第5希望
        for (int i = 0; i < 5; i++) {
            string request = strs[i + 3];

            switch (request[0]) {
                // 号館希望
                case 'B':
                    satis.push_back([=](string room) -> int { return satis_building(room, request[1]) ? 5 - i : 0; });
                    break;

                // 階希望
                case 'F':
                    satis.push_back([=](string room) -> int { return satis_floor(room, request[1], request[3]) ? 5 - i : 0; });
                    break;

                // 部屋希望
                case 'R':
                    satis.push_back([=](string room) -> int { return satis_room(room, request.substr(1, 4)) ? 5 - i : 0; });
                    break;
            }
        }

        students.push_back(Student(strs[0], strs[1], strs[2], satis));
    }

    // 部屋をシャッフル
    random_device seed_gen;
    mt19937 engine(seed_gen());
    shuffle(room_infos.begin(), room_infos.end(), engine);

    vector<Room> rooms;

    for (auto room_info : room_infos)
        rooms.push_back(Room(room_info.number, Student("0000", "00000", "", {})));

    for (int i = 0; i < students.size(); i++)
        rooms[i].student = students[i];

    sort(rooms.begin(), rooms.end());

    // for (int k = 0; k < 10; k++) {
        int satis_max = satis_sum(rooms);
        int max_i = 0;
        int max_j = 0;

        for (int i = 0; i < room_infos.size() - 1; i++) {
            for (int j = i + 1; j < room_infos.size(); j++) {
                swap(rooms[i].student, rooms[j].student);

                if (satis_sum(rooms) > satis_max) {
                    satis_max = satis_sum(rooms);
                    max_i = i;
                    max_j = j;
                }

                swap(rooms[j].student, rooms[i].student);
            }
        }

        swap(rooms[max_i].student, rooms[max_j].student);
        cout << "幸福度 : " << satis_max << endl;
    // }

    cout << endl;
    for (auto room : rooms)
        cout << room.number << "号室：" << room.student.number << endl;

    return 0;
}
