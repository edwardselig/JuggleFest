#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <queue>
#include <stack>
#include <algorithm>
#define njugglers 12000
#define ncircuits 2000
#define jpercircuit 6
#define nprefs 10
struct juggler{
    int H;
    int E;
    int P;
    int currentpref;
    bool assign;//is it assign to a circuit?
    int prefs[nprefs];//circuit preferences
    int dotpprefs[nprefs];//dotproduct of circuit preferences
};
struct jugglerpoint{//keeps track of a circuit's jugglers
    int jid;
    int dotp;
};
struct circuit{
    int H;
    int E;
    int P;
    jugglerpoint jugglerpoints[jpercircuit];//keeps track of a circuit's jugglers
};
circuit circuits[ncircuits];
juggler jugglers[njugglers];
int main(int argc, char *argv[]){
    void circuitparser(char A,circuit &B,std::string line);
    void jugglerparser(char A,juggler &B,std::string line);
    void jugglerparser2(juggler &B,std::string line);
    bool comp (jugglerpoint i, jugglerpoint j);
    std::fstream f;
    f.open(argv[1]);
    int Cid = 0;
    int Jid = 0;
    std::string line;
    while (std::getline(f,line)){//parse file, update struct values
        if(line[0]=='C'||Cid==0){//avoid bug where C0 was getting skipped
            circuit newcircuit;
            circuitparser('H',newcircuit,line);
            circuitparser('E',newcircuit,line);
            circuitparser('P',newcircuit,line);
            for(int i = 0; i<jpercircuit; i++){
                newcircuit.jugglerpoints[i].jid = -1;
                newcircuit.jugglerpoints[i].dotp = -1;
            }
            circuits[Cid] = newcircuit;
            Cid++;
        }
        if(line[0]=='J'){
            juggler newjuggler;
            jugglerparser('H',newjuggler,line);
            jugglerparser('E',newjuggler,line);
            jugglerparser('P',newjuggler,line);
            jugglerparser2(newjuggler,line);
            newjuggler.currentpref = 0;
            jugglers[Jid] = newjuggler;
            Jid++;
        }
    }
    std::stack<int> unassigned;//jugglers that have not been assigned a circuit
    for(int i = njugglers-1; i>=0;i--){
        unassigned.push(i);
        for(int j = 0; j<nprefs; j++){
            int cir = jugglers[i].prefs[j];
            int dp = jugglers[i].H * circuits[cir].H + jugglers[i].E * circuits[cir].E + jugglers[i].P * circuits[cir].P;
            jugglers[i].dotpprefs[j] = dp;
            //std::cout<<"jid: "<<i<<" circuit: "<<cir<<" dp: "<<dp<<std::endl;
        }
    }
    while(!unassigned.empty()){
        int jid1 = unassigned.top();
        unassigned.pop();
        if(jugglers[jid1].currentpref < 10){
            int cir = jugglers[jid1].prefs[jugglers[jid1].currentpref];
            int dp = jugglers[jid1].dotpprefs[jugglers[jid1].currentpref];
            jugglers[jid1].currentpref++;
            if(dp > circuits[cir].jugglerpoints[jpercircuit-1].dotp){//is the dotproduct greater than the lowest dotproduct in the circuit?
                if(circuits[cir].jugglerpoints[jpercircuit-1].dotp!=-1){
                    unassigned.push(circuits[cir].jugglerpoints[jpercircuit-1].jid);
                    jugglers[circuits[cir].jugglerpoints[jpercircuit-1].jid].assign = false;
                }
                circuits[cir].jugglerpoints[jpercircuit-1].dotp = dp;
                circuits[cir].jugglerpoints[jpercircuit-1].jid = jid1;
                jugglers[circuits[cir].jugglerpoints[jpercircuit-1].jid].assign = true;
                std::sort(std::begin(circuits[cir].jugglerpoints),std::end(circuits[cir].jugglerpoints),comp);
            }else if (dp == circuits[cir].jugglerpoints[jpercircuit-1].dotp && jugglers[jid1].currentpref < jugglers[circuits[cir].jugglerpoints[jpercircuit-1].jid].currentpref){//if the dotproducts are equal assign based on whichever has the higher preference
                unassigned.push(circuits[cir].jugglerpoints[jpercircuit-1].jid);
                jugglers[circuits[cir].jugglerpoints[jpercircuit-1].jid].assign = false;
                circuits[cir].jugglerpoints[jpercircuit-1].dotp = dp;
                circuits[cir].jugglerpoints[jpercircuit-1].jid = jid1;
                jugglers[circuits[cir].jugglerpoints[jpercircuit-1].jid].assign = true;
                std::sort(std::begin(circuits[cir].jugglerpoints),std::end(circuits[cir].jugglerpoints),comp);
            }else{
                unassigned.push(jid1);
            }
        }
    }
    for(int i = 0; i < ncircuits;i++){//assign unassigned jugglers based on the juggler with the highest dotproduct for each circuit
        for(int j = 0; j < jpercircuit; j++){
            int largest = -1;
            int jid1;
            if(circuits[i].jugglerpoints[j].dotp==-1){
                for(int y = 0; y<njugglers;y++){
                    if(!jugglers[y].assign){
                        int dp = jugglers[y].H * circuits[i].H + jugglers[y].E * circuits[i].E + jugglers[y].P * circuits[i].P;
                        if(dp > largest) {largest = dp; jid1 = y;}
                    }
                }
                circuits[i].jugglerpoints[jpercircuit-1].dotp = largest;
                circuits[i].jugglerpoints[jpercircuit-1].jid = jid1;
                jugglers[jid1].assign = true;
                std::sort(std::begin(circuits[i].jugglerpoints),std::end(circuits[i].jugglerpoints),comp);
            }
        }
    }
    for(int i = 0; i < ncircuits; i++){//print loop
        std::cout<<"C"<<i<<" ";
        for(int y = 0; y < jpercircuit; y++){
            std::cout<<"J"<<circuits[i].jugglerpoints[y].jid<<" ";
            for(int j = 0; j<nprefs; j++){
                if(j==nprefs-1){
                    std::cout<<"C"<<jugglers[circuits[i].jugglerpoints[y].jid].prefs[j]<<":"<<jugglers[circuits[i].jugglerpoints[y].jid].dotpprefs[j];
                }else{
                std::cout<<"C"<<jugglers[circuits[i].jugglerpoints[y].jid].prefs[j]<<":"<<jugglers[circuits[i].jugglerpoints[y].jid].dotpprefs[j]<<" ";
                }
            }
            if(y!=jpercircuit-1){
                std::cout<<", ";
            }
        }
        std::cout<<std::endl;
    }
    return 0;
}
void circuitparser(char A,circuit &B,std::string line){//add circuit stats to each circuit
    int ind = line.find_first_of(A);
    int H = line[ind+2] - '0';
    if(A=='H')B.H = H;
    if(A=='E')B.E = H;
    if(A=='P')B.P = H;
    int I = line[ind+3] - '0';
    if(I==0&&A=='H')B.H = 10;
    if(I==0&&A=='E')B.E = 10;
    if(I==0&&A=='P')B.P = 10;
}
void jugglerparser(char A,juggler &B,std::string line){//add juggler stats to each juggler
    int ind = line.find_first_of(A);
    int H = line[ind+2] - '0';
    if(A=='H')B.H = H;
    if(A=='E')B.E = H;
    if(A=='P')B.P = H;
    int I = line[ind+3] - '0';
    if(I==0&&A=='H')B.H = 10;
    if(I==0&&A=='E')B.E = 10;
    if(I==0&&A=='P')B.P = 10;
}
void jugglerparser2(juggler &B,std::string line){//add circuit preferences to each juggler
    int ind = line.find_first_of('C');
    for(int i = 0; i<nprefs; i++){
        int preference = 0;
        while(line[ind + 1]!=','&&ind+1!=line.size()){//no comma for 10th circuit
            preference = preference*10 + line[ind+1] - '0';
            ind++;
        }
        B.prefs[i] = preference;
        ind = ind + 2;
    }
    
}
bool comp (jugglerpoint i, jugglerpoint j){
    if(i.dotp == j.dotp){
        return jugglers[i.jid].currentpref < jugglers[j.jid].currentpref;
    }
    return i.dotp > j.dotp;
}
//sum of jugglers assigned to 1970 is 28762
