// Test.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <numeric>

using namespace std;

int day_of_week(int year, int month, int day);
int main()
{
    string weekday[7] = { "日", "月",  "火", "水", "木", "金", "土"};
    for (int year = 2022; year <= 2024; year++) {
        for (int month = 1; month <= 12; month++) {
            for (int day = 1; day <= 31; day++) {
                cout << year << "年" << month << "月" << day << "日 / " << weekday[day_of_week(year, month, day)] << endl;
            }
        }
    }
    
    cin.get();
}

int day_of_week(int year, int month, int day) {
    if (month == 1 || month == 2) {
        year--;
        month += 12;
    }

    int week = (year + year / 4 - year / 100 + year / 400 +
        (13 * month + 8) / 5 + day) % 7;
    return week;
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
