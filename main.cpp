#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <functional>
#include <cstdlib>

using namespace std;

/*---------- クラス定義 ----------*/
// 部屋の情報
class Room {
public:
    string number;  // 部屋番号
    int people;     // 部屋の人数

    // 0:北側, 1:南側, 2:東側, 3:角部屋, 4:玄関の近く, 5:洗濯室の近く, 6:トイレの近く, 7:階段の近く, 8:ベランダ有
    vector<bool> infos;

    Room() { }

    Room(string number, int people, vector<bool> infos) {
        this->number = number;
        this->people = people;
        this->infos = infos;
    }

    // 比較演算子
    bool operator<(const Room &other) const {
        return this->number < other.number;
    };

    bool operator>(const Room &other) const {
        return this->number > other.number;
    };
};

// 寮生を表すクラス
class Student {
public:
    string cur_room; // 現在の部屋
    string number;   // 学籍番号
    string name;     // 名前
    function<int(Room)> satis; // 満足度の評価関数

    Student() { }

    Student(string cur_room, string number, string name, function<int(Room)> satis) {
        this->cur_room = cur_room;
        this->number = number;
        this->name = name;
        this->satis = satis;
    }

    // 比較演算子
    bool operator<(const Student &other) const {
        return this->number < other.number;
    };

    bool operator>(const Student &other) const {
        return this->number > other.number;
    };
};

// 住人を表すクラス
class Resident {
public:
    Room room;       // 部屋の情報
    Student student; // 寮生

    Resident() { }

    Resident(Room room, Student student) {
        this->room = room;
        this->student = student;
    }

    // 寮生の満足度
    int satis() {
        return this->student.satis(this->room);
    }

    // 比較演算子
    bool operator<(const Resident &other) const {
        return this->room.number < other.room.number;
    };

    bool operator>(const Resident &other) const {
        return this->room.number > other.room.number;
    };
};

/*---------- 関数定義 ----------*/
// 文字列の分割
vector<string> split(string& input, char delimiter) {
    istringstream iss(input);
    string field;
    vector<string> result;

    while (getline(iss, field, delimiter))
        result.push_back(field);
    return result;
}

// 対象の号館か
bool satis_building(Room room, char building) {
    return room.number[0] == building;
}

// 対象の階か
bool satis_floor(Room room, char building, char floor) {
    return (room.number[0] == building && room.number[1] == floor);
}

// 対象の部屋か
bool satis_room(Room room, string room_number) {
    return room.number == room_number;
}

/*---------- メイン関数 ----------*/
int main() {
    // 部屋一覧を読み込み
    vector<Room> rooms;
    ifstream room_stream("rooms.csv");
    string line;

    getline(room_stream, line);
    while (getline(room_stream, line)) {
        vector<string> strs = split(line, ',');
        vector<bool> infos;

        for (int i = 0; i < 9; i++)
            infos.push_back(strs[i + 3] != "0");

        if (strs[1] == "0")
            rooms.push_back(Room(strs[0], stoi(strs[2]), infos));
    }

    // 寮生希望一覧を読み込み
    vector<Student> students;
    ifstream student_stream("students.csv");

    getline(student_stream, line);
    while (getline(student_stream, line)) {
        vector<string> strs = split(line, ',');

        // 寮生の評価関数を作成
        students.push_back(Student(strs[0], strs[1], strs[2], [=](Room room) -> int {
            int satis_value = 0;
            string request;

            // 第1希望～第5希望
            for (int i = 4; i >= 0; i--) {
                request = strs[i + 3];

                if ((request[0] == 'B' && satis_building(room, request[1])) ||
                    (request[0] == 'F' && satis_floor(room, request[1], request[3])) ||
                    (request[0] == 'R' && satis_room(room, request.substr(1, 4))))
                    satis_value = 5 - i;
            }

            // 人数
            if (room.people == stoi(strs[8]))
                satis_value += 10000;

            // その他希望
            for (int i = 0; i < 9; i++) {
                if (strs[i + 9] != "" && room.infos[i])
                    satis_value += stoi(strs[i + 9]);
            }

            // 以上
            if (strs[18] != "" && room.number[1] >= stoi(strs[18]))
                satis_value += 1;

            // 以下
            if (strs[19] != "" && room.number[1] <= stoi(strs[19]))
                satis_value += 1;

            // 除外
            request = strs[20];

            if ((request[0] == 'B' && satis_building(room, request[1])) ||
                (request[0] == 'F' && satis_floor(room, request[1], request[3])) ||
                (request[0] == 'R' && satis_room(room, request.substr(1, 4))))
                satis_value -= 1;

            return satis_value;
        }));
    }

    // 部屋をシャッフル
    random_device rnd;
    mt19937 mt(rnd());
    shuffle(rooms.begin(), rooms.end(), mt);

    // 住人
    vector<Resident> residents;

    for (auto room : rooms)
        residents.push_back(Resident(room, Student("0000", "00000", "", [](Room room) { return 0; })));

    for (int i = 0; i < students.size(); i++)
        residents[i].student = students[i];

    sort(residents.begin(), residents.end());

    // 満足度の総和を計算
    int satis_sum = 0;
    for (auto resident : residents)
        satis_sum += resident.satis();

    while (satis_sum < 1510680) {
        int i, j;
        uniform_int_distribution<int> dist(0, residents.size() - 1);

        // 入れ替えたときに満足度の総和が増大するなら入れ替える
        while (true) {
            int i = dist(mt);
            int j = dist(mt);

            int src_satis = residents[i].satis() + residents[j].satis();
            int dst_satis = residents[i].student.satis(residents[j].room) + residents[j].student.satis(residents[i].room);

            if (dst_satis >= src_satis) {
                satis_sum += dst_satis - src_satis;
                swap(residents[i].student, residents[j].student);
                break;
            }
        }

        cout << "満足度 : " << satis_sum << endl;
    }

    cout << endl;
    for (auto resident : residents)
        cout << resident.room.number << "号室：" << resident.student.number << " (" << resident.satis() << ")" << endl;

    return 0;
}
