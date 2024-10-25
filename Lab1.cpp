#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>//for sscanf
#include <vector>
#include <cstring>
#include <sstream>//for istringstream
#include <map>
#include <iomanip>//for get_time
using namespace std;

struct Log_string {
    string remote_addr;  
    string local_time;   
    string request;      
    int status;   
    int bytes_send;            
};
vector<Log_string> logs5XX;
vector<Log_string> logs;

int output_mode = 0; // 0=none, 1=stdout, 2=file
string log_file;
string output_file;
bool print_to_stdout = false;
int stats = 10;
int window = 0;
long long from = 0;
long long to ;

// Function to parse the log string
Log_string tt_Log(const string& Log){
    Log_string new_log;
    size_t start=0, end;

    end = Log.find(' ', start);
    new_log.remote_addr = Log.substr(start, end-start);

    start = Log.find('[',end) + 1;
    end = Log.find(']', start);
    new_log.local_time = Log.substr(start, end-start);

    start = Log.find('"',end) + 1;
    end = Log.find('"', start);
    new_log.request = Log.substr(start, end-start);

    start = end + 2; 
    end = Log.find(' ', start);
    string status_str = Log.substr(start, end - start);
    sscanf(status_str.c_str(), "%d", &new_log.status); 
    
    start = end + 1;
    string bytes_send_str = Log.substr(start);
    sscanf(bytes_send_str.c_str(), "%d", &new_log.bytes_send); 
    
    return new_log;
}
// Convert timestamp to seconds
time_t to_seconds(const std::string& timestamp) {
    std::tm timeInfo = {};
    std::string timePart, timezonePart;

    std::istringstream ss(timestamp);
    ss >> timePart >> timezonePart;

    std::istringstream timeStream(timePart);
    timeStream >> std::get_time(&timeInfo, "%d/%b/%Y:%H:%M:%S");

    if (timeStream.fail()) {
        throw std::runtime_error("Failed to parse time");
    }

    time_t unixTime = std::mktime(&timeInfo);

    int timezoneOffset = 0;
    if (!timezonePart.empty()) {
        int hoursOffset = std::stoi(timezonePart.substr(0, 3));
        int minutesOffset = std::stoi(timezonePart.substr(3, 2));
        timezoneOffset = hoursOffset * 3600 + minutesOffset * 60; 
    }

    return unixTime - timezoneOffset;  
}
// Error_Log function
void error_Log(const string& log_file, const string& output_file){
    ifstream in(log_file); 
    ofstream out(output_file);
    vector<string> error_logs;
    int error = 0;

    string str_log; 
    while(getline(in, str_log)){
        Log_string error_new_log = tt_Log(str_log);
        string log_entry = error_new_log.remote_addr + " " + to_string(to_seconds(error_new_log.local_time)) + " " + error_new_log.request + " " + to_string(error_new_log.status) + " " +to_string(error_new_log.bytes_send);
        logs.push_back(error_new_log);
        if(error_new_log.status >= 500 && error_new_log.status < 600){
            string log_entry_error = error_new_log.remote_addr + " " + to_string(to_seconds(error_new_log.local_time)) + " " + error_new_log.request + " " + to_string(error_new_log.status) + " " +to_string(error_new_log.bytes_send);
            error_logs.push_back(log_entry_error); 
            logs5XX.push_back(error_new_log);
            error++;
        }
    }
    for (const auto& log : error_logs) {
        out << log << endl; 
    }
    out << error << endl; 
}
 // Time_window function
void Time_window(string file_name, int window_in, int from_in, int to_in) { 
    ifstream file(file_name);
    int mx_requests=0;
    int mx_start=0;
    int mx_end=0;

    for (int i = 0; i < logs.size(); i++) {
        if (to_seconds(logs[i].local_time)<from_in) {
            continue;
        }
        if (to_seconds(logs[i].local_time)>to_in) {
            break;
        }
        int rq=0;
        for (int j = i; j < logs.size(); j++) {
            if (to_seconds(logs[i].local_time)-to_seconds(logs[i].local_time)>window_in) {
                break;
            }
            rq++;
        }
        if (rq > mx_requests) {
            mx_start = to_seconds(logs[i].local_time);
            mx_requests = rq;
            mx_end = to_seconds(logs[i+rq-1].local_time);
        }
    }
    cout << "Max requests: " << mx_requests << " in window [" << mx_start << ", " << mx_end<< "]" << endl;
}

void printferror5xx(string output_file) {
    ofstream file(output_file);
    if (!file.is_open()) {
        cerr << "error: Could not open file " << output_file << endl;
        return;
    }
    file.close();
}
//function to sort logs
void SortLog(vector<pair<string, int>>& a) {
    int n = a.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (a[j].second < a[j + 1].second) {
                swap(a[j], a[j + 1]);
            }
        }
    }
}
//function to find frequently occurring url requests
void Main_frequency(string file_name, int n) {
    ifstream file(file_name);
    map<string, int> frequently;

    for (int i = 0; i < logs5XX.size(); i++) {
        cout << logs5XX[i].remote_addr << " " << to_seconds(logs5XX[i].local_time) << " " << logs5XX[i].request << " " << logs5XX[i].status << " " << logs5XX[i].bytes_send << endl;
        string req = logs5XX[i].remote_addr + " " + logs5XX[i].request;
        frequently[req]++;
    }

    vector<pair<string, int>> fq_a(frequently.begin(), frequently.end());
    SortLog(fq_a);

    cout << "top " << n << " most frequently requested url:" << endl;
    for (int i = 0; i < n && i < fq_a.size(); i++) {
        cout << fq_a[i].first << ": " << fq_a[i].second << " times" << endl;
    }
}

int main(int argc, char*argv[]){

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        for (int i = 1; i < argc; i++) {
            if (string(argv[i]) == "-o") {
                if (i + 1 < argc) {
                    output_file = string(argv[++i]);
                    output_mode = 2;
                }
                else {
                cerr << "error" << endl;
                return 1;
                }
            }
            else if (strncmp(argv[i], "--output=", 9) == 0) {
                output_file = string(argv[i]).substr(9);
                output_mode = 2;
            }
            else if (string(argv[i]) == "-p" || strncmp(argv[i], "--print", 7) == 0) {
                output_mode = 1;
            }
            else if (arg == "-s") {
                if (i + 1 < argc) {
                stats = stoi(argv[++i]); 
            }
            else {
                cerr << "Missing value for --stats " << endl;
                return 1;
                }
            }
            else if(strncmp(argv[i], "--stats=", 8) == 0){
                stats = stoi(string(argv[i]).substr(8));
            }
            else if (arg == "-w") {
                if (i + 1 < argc) {
                    window = stoi(argv[++i]);
                } 
                else {
                    cerr << "Missing value for --window " << endl;
                    return 1;
                }
            }
            else if (strncmp(argv[i], "--window=", 9) == 0) {
                window = stoi(string(argv[i]).substr(9));
            }
            else if (arg == "-f") {
                if (i + 1 < argc) {
                    from = stoi(argv[++i]); 
                } 
                else {
                cerr << "Missing value for --from " << endl;
                }
            }
            else if (strncmp(argv[i], "--from=", 7) == 0) {
                from = stoi(string(argv[i]).substr(7));
            }
            else if (arg == "-e") {
                if (i + 1 < argc) {
                    to = stoi(argv[++i]); 
                } 
                else {
                cerr << "Missing value for --to " << endl;
                }
            }
            else if (strncmp(argv[i], "--to=", 5) == 0) {
            to = stoi(string(argv[i]).substr(5));
            }
            else {
            log_file = string(argv[i]);
            }
        }
    }
    cout << "output_mode: " << output_mode << endl;
    cout << "Output file: " <<  output_file << endl;
    cout << "Stats: " << stats << endl;
    cout << "Window: " << window << endl;
    cout << "From time: " << from << endl;
    cout << "To time: " << to << endl;


    if (!output_file.empty()) {
        error_Log(log_file, output_file);
    }
    //printferror5xx(output_file);
    Main_frequency(log_file, stats);
    if (window != 0) {
        Time_window(log_file, window, from, to);
    }
    return 0;
}