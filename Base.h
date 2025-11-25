#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <map>

using namespace std;

class Base {
private:
    string filename;
    static const int TOTAL_RECORDS = 1000;
    static const int ID_SIZE = 3;
    static const int TITLE_SIZE = 19;
    static const int AUTHOR_SIZE = 14;
    static const int RATING_SIZE = 1;
    static const int FLAG_SIZE = 1;
    static const int RECORD_SIZE = ID_SIZE + TITLE_SIZE + AUTHOR_SIZE + RATING_SIZE + FLAG_SIZE;

    class Book {
    public:
        string id;
        string title;
        string author;
        char rating;
        char flag;

        string form_db_string() const {
            string res;
            string id_temp = id;
            if (id_temp.size() < ID_SIZE) {
                id_temp = string(ID_SIZE - id_temp.size(), '0') + id_temp;
            }
            res += id_temp;
            string title_temp = title;
            if (title_temp.size() < TITLE_SIZE) {
                title_temp += string(TITLE_SIZE - title_temp.size(), '/');
            }
            res += title_temp;
            string author_temp = author;
            if (author_temp.size() < AUTHOR_SIZE) {
                author_temp += string(AUTHOR_SIZE - author_temp.size(), '/');
            }
            res += author_temp;
            res += rating;
            res += flag;
            return res;
        }

        static Book form_book_from_db_string(const string& data) {
            if (data.size() < RECORD_SIZE) {
                return Book{ string(ID_SIZE, '0'), string(TITLE_SIZE, '/'), string(AUTHOR_SIZE, '/'), ' ', '0' };
            }
            Book b;
            b.id = data.substr(0, ID_SIZE);
            b.title = data.substr(ID_SIZE, TITLE_SIZE);
            b.author = data.substr(ID_SIZE + TITLE_SIZE, AUTHOR_SIZE);
            b.rating = data[ID_SIZE + TITLE_SIZE + AUTHOR_SIZE];
            b.flag = data[ID_SIZE + TITLE_SIZE + AUTHOR_SIZE + RATING_SIZE];
            return b;
        }

        //display database string without empty symbols
        void display() const {
            if (flag == '1') {
                string clean_id;
                if (id == string(ID_SIZE, '0')) {
                    clean_id = '0';
                }
                else {
                    int j1 = 0;
                    while ((j1 < ID_SIZE) && (id[j1] == '0')) {
                        ++j1;
                    }
                    while (j1 < ID_SIZE) {
                        clean_id.push_back(id[j1]);
                        ++j1;
                    }
                }
                int j2 = 0;
                string clean_title;
                while ((j2 < TITLE_SIZE) && (title[j2] != '/')) {
                    clean_title.push_back(title[j2]);
                    ++j2;
                }
                int j3 = 0;
                string clean_author;
                while ((j3 < AUTHOR_SIZE) && (author[j3] != '/')) {
                    clean_author.push_back(author[j3]);
                    ++j3;
                }
                cout << "ID: " << setw(ID_SIZE) << clean_id
                    << " | Title: " << setw(TITLE_SIZE) << left << clean_title
                    << " | Author: " << setw(AUTHOR_SIZE) << left << clean_author
                    << " | Rating: " << rating << endl;
            }
        }


        string getDisplayString() const {
            if (flag == '1') {
                string result;
                string clean_id;
                if (id == string(ID_SIZE, '0')) {
                    clean_id = '0';
                }
                else {
                    int j1 = 0;
                    while ((j1 < ID_SIZE) && (id[j1] == '0')) {
                        ++j1;
                    }
                    while (j1 < ID_SIZE) {
                        clean_id.push_back(id[j1]);
                        ++j1;
                    }
                }
                int j2 = 0;
                string clean_title;
                while ((j2 < TITLE_SIZE) && (title[j2] != '/')) {
                    clean_title.push_back(title[j2]);
                    ++j2;
                }
                int j3 = 0;
                string clean_author;
                while ((j3 < AUTHOR_SIZE) && (author[j3] != '/')) {
                    clean_author.push_back(author[j3]);
                    ++j3;
                }

                char buffer[200];
                snprintf(buffer, sizeof(buffer), "ID: %3s | Title: %-19s | Author: %-14s | Rating: %c",
                    clean_id.c_str(), clean_title.c_str(), clean_author.c_str(), rating);
                return string(buffer);
            }
            return "";
        }
    };

    //void database string
    Book voidBook() {
        return Book{ string(ID_SIZE, '0'), string(TITLE_SIZE, '/'), string(AUTHOR_SIZE, '/'), ' ', '0' };
    }

    //read book from database
    Book readRecord(int num) {
        if ((num > TOTAL_RECORDS) || (num < 0)) {
            cout << "ReadRecord failed. ID " << num << " is outside the database limits." << endl;
            return voidBook();
        }
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            cout << "ReadRecord failed. File opening error." << endl;
            return voidBook();
        }
        file.seekg(num * RECORD_SIZE);
        char buffer[RECORD_SIZE + 1];
        file.read(buffer, RECORD_SIZE);
        buffer[RECORD_SIZE] = '\0';
        return Book::form_book_from_db_string(string(buffer, RECORD_SIZE));
    }

    //write book to database
    //in general, num = b.id, except case of void book
    bool writeRecord(const Book& b, int num) {
        if ((num > TOTAL_RECORDS) || (num < 0)) {
            cout << "WriteRecord failed. ID " << num << " is outside the database limits." << endl;
            return false;
        }
        fstream file(filename, ios::binary | ios::in | ios::out);
        if (!file.is_open()) {
            return false;
        }
        file.seekp(num * RECORD_SIZE);
        string data = b.form_db_string();
        file.write(data.c_str(), RECORD_SIZE);
        file.close();
        return true;
    }

public:
    Base(const string& filename) : filename(filename) {}

    //create database - O(n)
    bool create() {
        ofstream file(filename, ios::binary);
        if (!file.is_open()) {
            return false;
        }
        Book empty_record = voidBook();
        string empty_data = empty_record.form_db_string();
        for (int i = 0; i < TOTAL_RECORDS; ++i) {
            file.write(empty_data.c_str(), RECORD_SIZE);
        }
        file.close();
        cout << "Database created with " << TOTAL_RECORDS << " records." << endl;
        return true;
    }

    //open database - O(1)
    bool open() {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            cout << "Database not found. Creating new one." << endl;
            return create();
        }

        //check file size
        file.seekg(0, ios::end);
        long long file_size = file.tellg();
        if (file_size != TOTAL_RECORDS * RECORD_SIZE) {
            cout << "Invalid database size. Creating new one." << endl;
            file.close();
            return create();
        }
        file.close();
        cout << "Database opened successfully." << endl;
        return true;
    }

    //delete database - O(1)
    bool delete_db() {
        if (remove(filename.c_str()) != 0) {
            cout << "Error deleting database." << endl;
            return false;
        }
        cout << "Database deleted successfully." << endl;
        return true;
    }

    //clear database = create database with the same filename - O(n)
    bool clear_db() {
        return create();
    }

    //add book to database - O(1)
    bool addRecord(int new_id, const string& new_title, const string& new_author, char new_rating) {
        if ((new_id < 0) || (new_id >= TOTAL_RECORDS)) {
            cout << "AddRecord failed. ID " << new_id << " is outside the database limits." << endl;
            return false;
        }
        if (new_title.size() > TITLE_SIZE) {
            cout << "AddRecord failed. Title is too long. " << endl;
            return false;
        }
        if (new_author.size() > AUTHOR_SIZE) {
            cout << "AddRecord failed. Author's name is too long. " << endl;
            return false;
        }
        Book temp = readRecord(new_id);
        if (temp.flag == '1') {
            cout << "AddRecord failed. ID " << new_id << " already exists." << endl;
            return false;
        }
        Book new_b;
        new_b.id = to_string(new_id);
        new_b.title = new_title;
        new_b.author = new_author;
        new_b.rating = new_rating;
        new_b.flag = '1';
        if (writeRecord(new_b, new_id)) {
            cout << "Book added at position " << new_id << "." << endl;
            return true;
        }
        return false;
    }

    //delete one book by ID - O(1)
    bool deleteByID(int id) {
        if ((id < 0) || (id >= TOTAL_RECORDS)) {
            cout << "DeleteByID failed. ID " << id << " is outside the database limits." << endl;
            return false;
        }
        Book temp = readRecord(id);
        if (temp.flag == '0') {
            cout << "Record with ID " << id << " not found." << endl;
            return false;
        }
        else {
            Book Void = voidBook();
            if (writeRecord(Void, id)) {
                cout << "Record with ID " << id << " deleted." << endl;
                return true;
            }
            else {
                return false;
            }
        }

    }

    //delete some books not by ID - O(n)
    bool deleteByField(const string& field, const string& v) {
        if (field != "title" && field != "author" && field != "rating") {
            cout << "Error: Unknown field '" << field << "'. Use: title / author / rating." << endl;
            return false;
        }
        if (((field == "title") && (v.size() > TITLE_SIZE)) ||
            ((field == "author") && (v.size() > AUTHOR_SIZE)) ||
            ((field == "rating") && (v.size() > RATING_SIZE))
            )
        {
            cout << "Value with this size don't exist in database." << endl;
            return false;
        }
        int count = 0;
        for (int i = 0; i < TOTAL_RECORDS; ++i) {
            Book temp = readRecord(i);
            if (temp.flag == '1') {
                bool f = false;
                if (field == "title") {
                    string clean_title;
                    int j = 0;
                    while ((j < TITLE_SIZE) && (temp.title[j] != '/')) {
                        clean_title.push_back(temp.title[j]);
                        ++j;
                    }
                    if (clean_title == v) {
                        f = true;
                    }
                }
                else if (field == "author") {
                    string clean_author;
                    int j = 0;
                    while ((j < AUTHOR_SIZE) && (temp.author[j] != '/')) {
                        clean_author.push_back(temp.author[j]);
                        ++j;
                    }
                    if (clean_author == v) {
                        f = true;
                    }
                }
                else if ((field == "rating") && (temp.rating == v[0])) {
                    f = true;
                }

                if (f) {
                    Book Void = voidBook();
                    writeRecord(Void, i);
                    count++;
                }
            }
        }
        cout << "Deleted " << count << " records where " << field << " is '" << v << "'." << endl;
        return count > 0;
    }

    //find one book by ID - O(1)
    void searchByID(int id) {
        if ((id < 0) || (id >= TOTAL_RECORDS)) {
            cout << "SearchByID failed. ID " << id << " is outside the database limits." << endl;
        }
        else {
            Book res = readRecord(id);
            if (res.flag == '0') {
                cout << "Record with ID " << id << " not found." << endl;
            }
            else {
                cout << "Search result:" << endl;
                res.display();
            }
        }
    }

    //find some books not by ID - O(n)
    void searchByField(const string& field, const string& v) {
        if (field != "title" && field != "author" && field != "rating") {
            cout << "Error: Unknown field '" << field << "'. Use: title / author / rating." << endl;
        }
        else {
            if (((field == "title") && (v.size() > TITLE_SIZE)) ||
                ((field == "author") && (v.size() > AUTHOR_SIZE)) ||
                ((field == "rating") && (v.size() > RATING_SIZE))
                )
            {
                cout << "Value with this size don't exist in database." << endl;
            }
            else {
                vector<Book> res;
                for (int i = 0; i < TOTAL_RECORDS; ++i) {
                    Book temp = readRecord(i);
                    if (temp.flag == '1') {
                        bool f = false;
                        if (field == "title") {
                            string clean_title;
                            int j = 0;
                            while ((j < TITLE_SIZE) && (temp.title[j] != '/')) {
                                clean_title.push_back(temp.title[j]);
                                ++j;
                            }
                            if (clean_title == v) {
                                f = true;
                            }
                        }
                        else if (field == "author") {
                            string clean_author;
                            int j = 0;
                            while ((j < AUTHOR_SIZE) && (temp.author[j] != '/')) {
                                clean_author.push_back(temp.author[j]);
                                ++j;
                            }
                            if (clean_author == v) {
                                f = true;
                            }
                        }
                        else if ((field == "rating") && (temp.rating == v[0])) {
                            f = true;
                        }
                        if (f) {
                            res.push_back(temp);
                        }
                    }
                }
                if (res.size() == 0) {
                    cout << "No records found where " << field << " is '" << v << "'." << endl;
                }
                else {
                    cout << "Found " << res.size() << " records where " << field << " is '" << v << "':" << endl;
                    for (const auto& record : res) {
                        record.display();
                    }
                }
            }
        }
    }

    //change book ¹ID info (can't change ID) - O(1)
    bool changeRecord(int id, const string& new_title, const string& new_author, char new_rating) {
        if (id < 0 || id >= TOTAL_RECORDS) {
            cout << "ChangeRecord failed. ID " << id << " is outside the database limits." << endl;
            return false;
        }
        if (new_title.size() > TITLE_SIZE) {
            cout << "ChangeRecord failed. Title is too long. " << endl;
            return false;
        }
        if (new_author.size() > AUTHOR_SIZE) {
            cout << "ChangeRecord failed. Author's name is too long. " << endl;
            return false;
        }
        Book temp = readRecord(id);
        if (temp.flag == '0') {
            cout << "Record with ID " << id << " not found." << endl;
            return false;
        }
        temp.title = new_title;
        temp.author = new_author;
        temp.rating = new_rating;
        if (writeRecord(temp, id)) {
            cout << "Record with ID " << id << " updated." << endl;
            return true;
        }
        return false;
    }

    //make backup - O(n)
    bool Backup(const string& backup_filename) {
        ifstream source(filename, ios::binary);
        ofstream db_backup(backup_filename, ios::binary);
        if ((!source.is_open()) || (!db_backup.is_open())) {
            cout << "Error creating backup." << endl;
            return false;
        }
        db_backup << source.rdbuf();
        source.close();
        db_backup.close();
        cout << "Backup created: " << backup_filename << endl;
        return true;
    }

    //restore database from backup - O(n)
    bool restoreFromBackup(const string& backup_filename) {
        ifstream source(backup_filename, ios::binary | ios::ate);
        if (!source.is_open()) {
            cout << "Error: Database file not found for backup." << endl;
            return false;
        }
        long long source_size = source.tellg();
        source.seekg(0);

        if (source_size != TOTAL_RECORDS * RECORD_SIZE) {
            cout << "Error: Database file has invalid size. Backup aborted." << endl;
            source.close();
            return false;
        }
        ofstream db(filename, ios::binary);
        if (!db.is_open()) {
            cout << "Error creating backup file." << endl;
            source.close();
            return false;
        }
        db << source.rdbuf();

        db.seekp(0, ios::end);
        long long backup_size = db.tellp();
        source.close();
        db.close();
        if (backup_size != source_size) {
            cout << "Error: Backup file size mismatch. Backup may be corrupted." << endl;
            remove(backup_filename.c_str());
            return false;
        }
        cout << "Backup restored successfully: " << backup_filename << endl;
        return true;
    }

    //make CSV file - O(n)
    bool exportToCSV(const string& csv_filename) {
        ofstream csv_file(csv_filename);
        if (!csv_file.is_open()) {
            cout << "Error creating CSV file." << endl;
            return false;
        }
        csv_file << "ID,Title,Author,Rating\n";
        for (int i = 0; i < TOTAL_RECORDS; ++i) {
            Book temp = readRecord(i);
            if (temp.flag == '1') {
                string clean_id;
                if (temp.id == string(ID_SIZE, '0')) {
                    clean_id = '0';
                }
                else {
                    int j1 = 0;
                    while ((j1 < ID_SIZE) && (temp.id[j1] == '0')) {
                        ++j1;
                    }
                    while (j1 < ID_SIZE) {
                        clean_id.push_back(temp.id[j1]);
                        ++j1;
                    }
                }
                int j2 = 0;
                string clean_title;
                while ((j2 < TITLE_SIZE) && (temp.title[j2] != '/')) {
                    clean_title.push_back(temp.title[j2]);
                    ++j2;
                }
                int j3 = 0;
                string clean_author;
                while ((j3 < AUTHOR_SIZE) && (temp.author[j3] != '/')) {
                    clean_author.push_back(temp.author[j3]);
                    ++j3;
                }
                csv_file << clean_id << ",\"" << clean_title << "\",\""
                    << clean_author << "\"," << temp.rating << "\n";
            }
        }
        csv_file.close();
        cout << "Exported to CSV: " << csv_filename << endl;
        return true;
    }

    //cout all non-empty records to console - O(n)
    void displayAll() {
        cout << "All records in database:" << endl;
        cout << "------------------------" << endl;
        bool isEmpty = false;
        for (int i = 0; i < TOTAL_RECORDS; ++i) {
            Book record = readRecord(i);
            if (record.flag == '1') {
                record.display();
                isEmpty = true;
            }
        }
        if (!isEmpty) {
            cout << "Database is empty." << endl;
        }
    }

    //show number of empty and non-empty records - O(n)
    void showStats() {
        int count = 0;
        for (int i = 0; i < TOTAL_RECORDS; ++i) {
            Book temp = readRecord(i);
            if (temp.flag == '1') {
                count++;
            }
        }
        cout << "Database statistics:" << endl;
        cout << "Total records: " << TOTAL_RECORDS << endl;
        cout << "Used records: " << count << endl;
        cout << "Free records: " << TOTAL_RECORDS - count << endl;
    }

    //O(1)
    string searchByID_GUI(int id) {
        if ((id < 0) || (id >= TOTAL_RECORDS)) {
            return "SearchByID failed. ID " + to_string(id) + " is outside the database limits.";
        }

        Book res = readRecord(id);
        if (res.flag == '0') {
            return "Record with ID " + to_string(id) + " not found.";
        }
        else {
            return "Search result:\n" + res.getDisplayString();
        }
    }

    //O(n)
    string searchByField_GUI(const string& field, const string& v) {
        if ((field != "title") && (field != "author") && (field != "rating")) {
            return "Error: Unknown field '" + field + "'. Use: title / author / rating.";
        }
        if (((field == "title") && (v.size() > TITLE_SIZE)) ||
            ((field == "author") && (v.size() > AUTHOR_SIZE)) ||
            ((field == "rating") && (v.size() > RATING_SIZE))) {
            return "Value with this size don't exist in database.";
        }
        vector<Book> res;
        for (int i = 0; i < TOTAL_RECORDS; ++i) {
            Book temp = readRecord(i);
            if (temp.flag == '1') {
                bool f = false;
                if (field == "title") {
                    string clean_title;
                    int j = 0;
                    while ((j < TITLE_SIZE) && (temp.title[j] != '/')) {
                        clean_title.push_back(temp.title[j]);
                        ++j;
                    }
                    if (clean_title == v) {
                        f = true;
                    }
                }
                else if (field == "author") {
                    string clean_author;
                    int j = 0;
                    while ((j < AUTHOR_SIZE) && (temp.author[j] != '/')) {
                        clean_author.push_back(temp.author[j]);
                        ++j;
                    }
                    if (clean_author == v) {
                        f = true;
                    }
                }
                else if ((field == "rating") && (temp.rating == v[0])) {
                    f = true;
                }
                if (f) {
                    res.push_back(temp);
                }
            }
        }
        if (res.size() == 0) {
            return "No records found where " + field + " is '" + v + "'.";
        }
        else {
            string result = "Found " + to_string(res.size()) + " records where " + field + " is '" + v + "':\n";
            for (const auto& record : res) {
                result += record.getDisplayString() + "\n";
            }
            return result;
        }
    }

    string displayAll_GUI() {
        string res = "All records in database:\n" + string(ID_SIZE + TITLE_SIZE + AUTHOR_SIZE + RATING_SIZE + FLAG_SIZE, '-') + "\n";
        bool isEmpty = true;
        for (int i = 0; i < TOTAL_RECORDS; ++i) {
            Book record = readRecord(i);
            if (record.flag == '1') {
                res += record.getDisplayString() + "\n";
                isEmpty = false;
            }
        }
        if (isEmpty) {
            res += "Database is empty.";
        }
        return res;
    }

    //O(n)
    string showStats_GUI() {
        int count = 0;
        for (int i = 0; i < TOTAL_RECORDS; ++i) {
            Book temp = readRecord(i);
            if (temp.flag == '1') {
                count++;
            }
        }
        string result = "Database statistics:\n";
        result += "Total records: " + to_string(TOTAL_RECORDS) + "\n";
        result += "Used records: " + to_string(count) + "\n";
        result += "Free records: " + to_string(TOTAL_RECORDS - count);
        return result;
    }
};
