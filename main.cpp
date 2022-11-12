#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>
#include <string>

using namespace std;

mutex myMutex;
string pckg;

string getIp(string command);

void ipCheck(string command);

int ipCount(string ipfile);

int main(int argc, char *argv[]) {
	pckg = argv[2];
	//obtener ips
	string filename = argv[1];
	string iplist[ipCount(filename)];
	ifstream file(filename.c_str());
	
	string aux;
	int ip = 0;
	while(getline(file, aux)) {
		iplist[ip] = aux;
		ip++;
	}
	
	file.close();
	
	thread threads[ipCount(filename)];
	
	cout << "IP\t\tTrans.\tRec.\tPerd.\tEstado" << endl;
	for(int i = 0; i < ipCount(filename); i++) {
		string command = "ping " + iplist[i] + " -c " + argv[2];
		threads[i] = thread(ipCheck, command);
	}

	for(int i = 0; i < ipCount(filename); i++) {
		threads[i].join();
	}
	
	return 0;
}

string getIp(string command) {
	string ip = "";
	for(int i = 5; command[i] != ' '; i++) ip += command[i];
	return ip;
}

void ipCheck(string command) {
	char buffer[128];
	string aux = "";
	
	FILE *pipe = popen(command.c_str(), "r");
	if(!pipe) cout << "error" << endl;
	while(!feof(pipe)) {
		if(fgets(buffer, 128, pipe) != NULL)
			aux += buffer;
	}
	pclose(pipe);
	
	string receive = "";
	int comma = 0;
	for(int i = 0; i < aux.length(); i++) {
		if(aux[i-2] == ',') comma = 1;
		if(comma == 1 && aux[i] == ' ') break;
		if(comma == 1) receive += aux[i];
	}
	string ip = getIp(command);
	string status;
	if(pckg == receive) status = "UP";
	else status = "DOWN";
	int lost = stoi(pckg) - stoi(receive);
	myMutex.lock();
	if(ip.length() < 10)
	cout << ip << "\t\t" << pckg << "\t" << receive << "\t" << lost << "\t" << status << endl;
	else
	cout << ip << "\t" << pckg << "\t" << receive << "\t" << lost << "\t" << status << endl;
	myMutex.unlock();
}

int ipCount(string ipfile) {
	ifstream file(ipfile.c_str());
	
	string aux;
	int count = 0;
	while(getline(file, aux)) {
		count++;
	}
	
	file.close();
	return count;
}

