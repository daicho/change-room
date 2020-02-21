#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <functional>
#include <random>
#include <algorithm>

#define INF 1000000000

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
};

// 部屋を表すクラス
class Room {
public:
    string number;   // 部屋番号
    Student student; // 寮生

    Room(string number, Student student) {
        this->number = number;
        this->student = student;
    }

    // 寮生の満足度
    int satisfaction() {
        int max_sati = -INF;
        for (auto sati : this->student.satis)
            max_sati = max(max_sati, sati(this->number));
        return max_sati;
    }
};

// 満足度の総和を計算
int satis_sum(vector<Room> rooms) {
    int sum = 0;
    for (auto room : rooms)
        sum += room.satisfaction();
    return sum;
}

int main() {
    vector<string> room_list = {
        "3214",
        "3402",
        "1406",
        "4411",
        "5107",
        "4409",
        "1412",
        "2216",
        "1205",
    };

    vector<Student> students = {
        Student("1111", "11111", "Hoge", {
            [](string room) -> int { return room[0] == '1' ? 1 : 0; }
        }),
        Student("1112", "11112", "Hoge", {
            [](string room) -> int { return room[0] == '2' ? 1 : 0; }
        }),
        Student("1113", "11113", "Hoge", {
            [](string room) -> int { return room[0] == '3' ? 1 : 0; }
        }),
        Student("1114", "11114", "Hoge", {
            [](string room) -> int { return room[0] == '4' ? 1 : 0; }
        }),
        Student("1115", "11115", "Hoge", {
            [](string room) -> int { return room[0] == '5' ? 1 : 0; }
        }),
        Student("1116", "11116", "Hoge", {
            [](string room) -> int { return room[1] == '1' ? 1 : 0; }
        }),
        Student("1117", "11117", "Hoge", {
            [](string room) -> int { return room[1] == '2' ? 1 : 0; }
        }),
        Student("1118", "11118", "Hoge", {
            [](string room) -> int { return room[1] == '3' ? 1 : 0; }
        }),
        Student("1119", "11119", "Hoge", {
            [](string room) -> int { return room[1] == '4' ? 1 : 0; }
        }),
    };

    random_device seed_gen;
    mt19937 engine(seed_gen());
    shuffle(students.begin(), students.end(), engine);

    vector<Room> rooms;

    for (int i = 0; i < students.size(); i++)
        rooms.push_back(Room(room_list[i], students[i]));

    for (int k = 0; k < 10; k++) {
        int satis_max = satis_sum(rooms);
        int max_i = 0;
        int max_j = 0;

        for (int i = 0; i < room_list.size() - 1; i++) {
            for (int j = i + 1; j < room_list.size(); j++) {
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
        cout << satis_max << endl;
    }

    cout << endl;
    for (auto room : rooms)
        cout << room.number << "号室：" << room.student.number << endl;

    return 0;
}
