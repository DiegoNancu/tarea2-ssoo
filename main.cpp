#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>
#include <string>

using namespace std;

mutex myMutex;
string pckg;

void ipCheck(string ip);

int ipCount(string ipfile);

int main(int argc, char *argv[]) {
	pckg = argv[2];
	//obtener ips
	string filename = argv[1];
	int ipcount = ipCount(filename);
	string iplist[ipcount];
	ifstream file(filename.c_str());
	
	string aux;
	int ip = 0;
	while(getline(file, aux)) {
		iplist[ip] = aux;
		ip++;
	}
	
	file.close();
	
	thread threads[ipcount];
	
	cout << "IP\t\tTrans.\tRec.\tPerd.\tEstado" << endl;
	for(int i = 0; i < ipcount; i++) {
		threads[i] = thread(ipCheck, iplist[i]);
	}

	for(int i = 0; i < ipcount; i++) {
		threads[i].join();
	}
	
	return 0;
}

void ipCheck(string ip) {
	char buffer[128];
	string aux = "";
	string command = "ping " + ip + " -c " + pckg;
	
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
	string status;
	if(stoi(receive) > 0) status = "UP";
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

