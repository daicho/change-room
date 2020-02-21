#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <functional>
#include <random>
#include <algorithm>

using namespace std;

class Student {
public:
    string cur_room;
    string number;
    string name;
    vector<function<int(string)>> satis;

    Student (string cur_room, string number, string name, vector<function<int(string)>> satis) {
        this->cur_room = cur_room;
        this->number = number;
        this->name = name;
        this->satis = satis;
    }
};

int satis_sum(vector<tuple<string, Student>> rooms) {
    int sum = 0;
    for (auto room : rooms)
        sum += get<1>(room).satis[0](room);
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

    vector<tuple<string, Student>> rooms;

    for (int i = 0; i < students.size(); i++)
        rooms.push_back(make_tuple(room_list[i], students[i]));

    random_device seed_gen;
    mt19937 engine(seed_gen());
    shuffle(rooms.begin(), rooms.end(), engine);

    for (int k = 0; k < 10; k++) {
        int satis_max = satis_sum(rooms);
        int max_i = 0;
        int max_j = 0;

        for (int i = 0; i < room_list.size() - 1; i++) {
            for (int j = i + 1; j < room_list.size(); j++) {
                swap(rooms[room_list[i]], rooms[room_list[j]]);

                if (satis_sum(rooms) > satis_max) {
                    satis_max = satis_sum(rooms);
                    max_i = i;
                    max_j = j;
                }

                swap(rooms[room_list[j]], rooms[room_list[i]]);
            }
        }

        swap(rooms[room_list[max_i]], rooms[room_list[max_j]]);
        cout << satis_max << endl;
    }

    cout << endl;
    for (auto [room, student] : rooms)
        cout << room << "号室：" << student.number << endl;

    return 0;
}
