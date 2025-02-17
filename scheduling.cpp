#include <iostream>
#include <fstream>
#include <set>
#include <queue>
using namespace std;
struct process
{
    int id, arrival, runtime, finish, currentTime;
};
bool cmpArrival(process a, process b){
    if ( a.arrival == b.arrival )   return a.id < b.id;
    return a.arrival < b.arrival;
}
bool cmpTimeLeft(process* a, process* b){
    if ( a->runtime-a->currentTime == b->runtime-b->currentTime )   return a->id < b->id;
    return a->runtime-a->currentTime < b->runtime-b->currentTime;
}
void printProcess(int id, int time, ofstream &out){
    out << "time " << time << ": ";
    if ( id == -1 ) out << "idle" << endl;
    else            out << "running process : " << id << endl;
}
void printStats(vector<process>processes, int time, ofstream &out){
    int totalRuntime=0, totalWaitTime=0, totalTurnaroundTime=0, processCount = processes.size();
    for ( process p : processes ){
        totalRuntime += p.runtime;
        totalTurnaroundTime += p.finish-p.arrival;
        totalWaitTime += p.finish-p.arrival-p.runtime;
    }
    out << setprecision(2) << fixed;
    out << "Average waiting time : " << 1.0*totalWaitTime/processCount << endl;
    out << "Average turnaround time : " << 1.0*totalTurnaroundTime/processCount << endl;
    out << "Average CPU usage : " << 100.0*totalRuntime/time << "%\n";
}
void round_robin(vector<process>processes, int quantum){
    ofstream out;
    out.open("MindUnits_RR.txt");
    queue<process*>q;
    int leftPointer = 0;
    int time=0, curQuanta=0;
    while ( !q.empty() or leftPointer != processes.size() ){
        while ( !processes.empty() and processes[leftPointer].arrival == time ){
            q.push(&processes[leftPointer++]);
        }
        if ( !q.empty() ){
            if ( q.front()->currentTime == q.front()->runtime ){
                q.front()->finish = time;
                q.pop();
                curQuanta = 0;
            }
            else if ( curQuanta == quantum ){
                q.push(q.front());
                q.pop();
                curQuanta=0;
            }
        }
        if ( !q.empty() ){
            curQuanta++;
            q.front()->currentTime++;
            printProcess(q.front()->id, time, out);
        }
        else if(leftPointer != processes.size()){
            printProcess(-1, time, out);
        }
        time++;
    }
    time--;
    printStats(processes, time, out);
    out.close();
}
void shortest_remaining_first(vector<process>processes){
    ofstream out;
    out.open("MindUnits_SRTF.txt");
    set<process*, decltype(cmpTimeLeft)*>s(cmpTimeLeft);
    int leftPointer = 0;
    int time = 0;
    while ( !s.empty() or leftPointer != processes.size() ){
        while ( leftPointer != processes.size() and processes[leftPointer].arrival == time ){
            s.insert(&processes[leftPointer++]);
        }
        if ( !s.empty() and (*s.begin())->runtime == (*s.begin())->currentTime ){
            (*s.begin())->finish = time;
            s.erase(s.begin());
        }
        if ( !s.empty() ){
            process *p = *s.begin();
            s.erase(s.begin());
            p->currentTime++;
            s.insert(p);
            printProcess(p->id, time, out);
        }
        else if(leftPointer != processes.size()){
            printProcess(-1, time, out);
        }
        time++;
    }
    time--;
    printStats(processes, time, out);
    out.close();
}
void first_come(vector<process>processes){
    ofstream out;
    out.open("MindUnits_FCRS.txt");
    int leftPointer = 0, time = 0;
    while ( leftPointer != processes.size() ){
        if ( processes[leftPointer].currentTime == processes[leftPointer].runtime ){
            processes[leftPointer++].finish = time;
        }
        if ( leftPointer == processes.size() )  break;
        if ( time >= processes[leftPointer].arrival ){
            printProcess(processes[leftPointer].id, time, out);
            processes[leftPointer].currentTime++;
        }
        else{
            printProcess(-1, time, out);
        }
        time++;
    }
    printStats(processes, time, out);
    out.close();
}
int main(){
    ifstream in;
    in.open("processes.txt");
    vector<process>processes;
    int id, arrival, runtime;
    int quanta=2;
    while ( in >> id >> arrival >> runtime ){
        processes.push_back({id, arrival, runtime});
    }
    in.close();
    sort(processes.begin(), processes.end(), cmpArrival);
    round_robin(processes, quanta);
    shortest_remaining_first(processes);
    first_come(processes);
}