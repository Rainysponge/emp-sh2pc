#include "emp-sh2pc/emp-sh2pc.h"
using namespace emp;
using namespace std;
const string circuit_file_location = macro_xstr(EMP_CIRCUIT_PATH);

int port, party;
string file = circuit_file_location+"/bristol_fashion/aes_128.txt";


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

void test() {
	std::srand(static_cast<unsigned int>(
        std::chrono::system_clock::now().time_since_epoch().count()
    ));
	auto start1 = clock_start();
	BristolFashion cf(file.c_str());
	cout << "Time for Reading File and Creating Circuits: " << time_from(start1) << endl;
	
	vector<Bit> key(128);
    for (auto& bit : key) {
        bit = (rand() % 2) == 1;
    }

    vector<Bit> plaintext(128);
    for (auto& bit : plaintext) {
        bit = (rand() % 2) == 1;
    }
	cout << "key      : " << bits2string(key) << endl;
	cout << "plaintext: " << bits2string(plaintext) << endl;

	vector<Bit> bit_vec = cat_vector(key, plaintext);
	Integer a(bit_vec);

	Integer c(128, 1, PUBLIC);
	auto start2 = clock_start();
	cf.compute((block*)c.bits.data(), (block*)a.bits.data());
	cout << "ciphertext: "<<reverse_string(c.reveal<string>())<<endl;
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
