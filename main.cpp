#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>
#include <string>

using namespace std;

mutex myMutex;
string pckg;

//realiza la verificacion del estado de la ip
//parametros: ip
void ipCheck(string ip);

//retorna la cantidad de lineas que tiene el archivo .txt
//parametros: nombre del archivo .txt
int ipCount(string ipfile);

int main(int argc, char *argv[]) {
	pckg = argv[2];
	//recuparacion de ips del archivo .txt
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
	//definicion de los threads
	thread threads[ipcount];
	//base de la tabla de resultados
	cout << "IP\t\tTrans.\tRec.\tPerd.\tEstado" << endl;
	//creacion de los threads
	for(int i = 0; i < ipcount; i++) {
		threads[i] = thread(ipCheck, iplist[i]);
	}
	//uso de los threads
	for(int i = 0; i < ipcount; i++) {
		threads[i].join();
	}
	return 0;
}

void ipCheck(string ip) {
	//armado del comando a usar
	string command = "ping " + ip + " -c " + pckg;
	//definicion de variables necesarias para el uso de popen
	char buffer[128];
	string aux = "";
	//uso de pipe para la obtencion del resultado del uso del comando	
	FILE *pipe = popen(command.c_str(), "r");
	if(!pipe) cout << "error" << endl;
	while(!feof(pipe)) {
		if(fgets(buffer, 128, pipe) != NULL)
			aux += buffer;
	}
	pclose(pipe);
	//recuperacion de los datos guardados en la variable aux
	//busqueda de los paquetes recibidos
	string receive = "";
	int comma = 0;
	for(int i = 0; i < aux.length(); i++) {
		if(aux[i-2] == ',') comma = 1;
		if(comma == 1 && aux[i] == ' ') break;
		if(comma == 1) receive += aux[i];
	}
	//definicion del estado de la conexion
	string status;
	if(stoi(receive) > 0) status = "UP";
	else status = "DOWN";
	//definicion de la cantidad de paquetes perdidos
	int lost = stoi(pckg) - stoi(receive);
	//muestra en pantalla de los resultados
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

