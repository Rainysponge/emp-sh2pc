#include <ctime>
#include <chrono>
#include "emp-sh2pc/emp-sh2pc.h"
using namespace emp;
using namespace std;
const string circuit_file_location = macro_xstr(EMP_CIRCUIT_PATH);

int port, party;
string file = circuit_file_location+"/bristol_fashion/sha256.txt";


vector<Bit> cat_vector(vector<Bit> key, vector<Bit> plaintext){
	vector<Bit> result = key;
    result.insert(result.end(), plaintext.begin(), plaintext.end());
    return result;
}

string bits2string(vector<Bit> bits){
	string result;
    for (const Bit &b : bits) {
        result += b.reveal<bool>() ? '1' : '0';
    }
    return result;
}

string reverse_string(string str){
	reverse(str.begin(), str.end());
    return str;
}

string bits2hexString(vector<Bit> bits){
	stringstream ss;
    for (size_t i = 0; i < bits.size(); i += 4) {
        int hex_value = 0;
        for (size_t j = 0; j < 4 && i + j < bits.size(); ++j) {
            hex_value = (hex_value << 1) | bits[i + j].reveal<bool>(); 
        }
        ss << hex << hex_value;
    }
    return ss.str();
}

string biString2hexString(string str) {
    stringstream ss;
    int len = str.length();
    while (len % 4 != 0) {
        str = '0' + str;
        len++;
    }
    for (int i = 0; i < len; i += 4) {
        string byte_str = str.substr(i, 4);
        int byte_val = stoi(byte_str, nullptr, 2); 
        ss << hex << setw(1) << setfill('0') << byte_val;
    }
    
    return ss.str();
}

void test() {
    std::srand(static_cast<unsigned int>(
        std::chrono::system_clock::now().time_since_epoch().count()
    ));
    auto start1 = clock_start();
	BristolFashion cf(file.c_str());
	cout << "Time for Reading File and Creating Circuits: " << time_from(start1) << endl;
	vector<Bit> message_block(512);
	vector<Bit> hash_state(256);
    for (auto& bit : message_block) {
        bit = (rand() % 2) == 1;
    }
    for (auto& bit : hash_state) {
        bit = (rand() % 2) == 1;
    }

	cout << "message block: 0x" << bits2hexString(message_block) << endl;
	cout << "hash state   : 0x" << bits2hexString(hash_state) << endl;
	Integer a(cat_vector(message_block, hash_state));

	Integer c(256, 0, PUBLIC);
    auto start2 = clock_start();
	cf.compute((block*)c.bits.data(), (block*)a.bits.data());
	cout << "ciphertext   : 0x"<< biString2hexString(c.reveal<string>()) << endl;
    cout << "Time for Computation: " << time_from(start2) << endl;
}
int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	NetIO* io = new NetIO(party==ALICE?nullptr:"127.0.0.1", port);

	setup_semi_honest(io, party);
	test();
    cout << "party " << party << ": send rounds: " << io->send_rounds << "; recv rounds: " << io->recv_rounds << endl;
    cout << "party " << party << ": send bytes: " << io->send_bytes << "; recv bytes: " << io->recv_bytes << endl;
	finalize_semi_honest();
	delete io;
}
