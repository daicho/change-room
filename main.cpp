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
    string number; // 学籍番号
    string name;   // 名前
    function<int(Room)> satis; // 満足度の評価関数

    Student() { }

    Student(string number, string name, function<int(Room)> satis) {
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
bool matchBuilding(Room room, char building) {
    return room.number[0] == building;
}

// 対象の階か
bool matchFloor(Room room, char building, char floor) {
    return (room.number[0] == building && room.number[1] == floor);
}

// 対象の部屋か
bool matchRoom(Room room, string room_number) {
    return room.number == room_number;
}

// 文字列が表す部屋か
bool matchRequest(Room room, string request) {
    return false;
    // return (request[0] == 'B' && matchBuilding(room, request[1])) ||
    //        (request[0] == 'F' && matchFloor(room, request[1], request[3])) ||
    //        (request[0] == 'R' && matchRoom(room, request.substr(1, 4)));
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

    room_stream.close();

    // 寮生希望一覧を読み込み
    vector<Student> students;
    ifstream student_stream("students.csv");

    getline(student_stream, line);
    while (getline(student_stream, line)) {
        vector<string> strs = split(line, ',');

        // 寮生の評価関数を作成
        students.push_back(Student(strs[0], strs[1], [=](Room room) -> int {
            return 0;
            int satis_value = 0;

            // 第1希望～第5希望
            for (int i = 4; i >= 0; i--) {
                if (matchRequest(room, strs[i + 3]))
                    satis_value = 5 - i;
            }

            // 確定
            if (!matchRequest(room, strs[2]))
                satis_value -= 1000;

            // 学年
            switch (strs[1][1]) {
                case '5':
                case '6':
                case '7':
                    if (!matchFloor(room, '1', '2') && !matchFloor(room, '1', '3') && !matchBuilding(room, '3') && !matchBuilding(room, '5'))
                        satis_value -= 1000;
                    break;

                case '8':
                    if (!matchBuilding(room, '1') && !matchFloor(room, '2', '1') && !matchFloor(room, '2', '3'))
                        satis_value -= 1000;
                    break;

                case '9':
                    if (!matchBuilding(room, '1') && !matchBuilding(room, '2') && !matchBuilding(room, '4'))
                        satis_value -= 1000;
                    break;
            }

            // 人数
            if (room.people != stoi(strs[8]))
                satis_value -= 1000;

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
            if (matchRequest(room, strs[20]))
                satis_value -= 1;

            return satis_value;
        }));
    }

    student_stream.close();

    // 部屋をシャッフル
    random_device rnd;
    mt19937 mt(rnd());
    shuffle(rooms.begin(), rooms.end(), mt);

    // 住人
    vector<Resident> residents;

    for (auto room : rooms)
        residents.push_back(Resident(room, Student("00000", "", [](Room room) { return 0; })));

    for (int i = 0; i < students.size(); i++)
        residents[i].student = students[i];

    sort(residents.begin(), residents.end());

    // 満足度の総和を計算
    int satis_sum = 0;
    for (auto resident : residents)
        satis_sum += resident.satis();

    // ファイルを作成
    ofstream("continue");

    while (ifstream("continue").is_open()) {
        // ランダム入れ替えたときに満足度の総和が増大するなら入れ替える
        uniform_int_distribution<int> dist(0, residents.size() - 1);
        int i = dist(mt);
        int j = dist(mt);

        int src_satis = residents[i].satis() + residents[j].satis();
        int dst_satis = residents[i].student.satis(residents[j].room) + residents[j].student.satis(residents[i].room);

        if (dst_satis >= src_satis) {
            satis_sum += dst_satis - src_satis;
            swap(residents[i].student, residents[j].student);
        }

        if (dst_satis > src_satis)
            cout << "満足度 : " << satis_sum << endl;
    }

    cout << endl;
    for (auto resident : residents)
        cout << resident.room.number << "号室：" << resident.student.number << " (" << resident.satis() << ")" << endl;

    return 0;
}
