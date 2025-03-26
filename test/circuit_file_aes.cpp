#include "emp-sh2pc/emp-sh2pc.h"
using namespace emp;
using namespace std;
const string circuit_file_location = macro_xstr(EMP_CIRCUIT_PATH);

int port, party;
string file = circuit_file_location+"/bristol_fashion/aes_128.txt";
BristolFashion cf(file.c_str());

vector<Bit> cat_vector(vector<Bit> key, vector<Bit> plaintext){
	vector<Bit> result = key; // 先复制 key
    result.insert(result.end(), plaintext.begin(), plaintext.end()); // 追加 plaintext
    return result;
}

string bits2string(vector<Bit> bits){
	string result;
    for (const Bit &b : bits) {
        result += b.reveal<bool>() ? '1' : '0'; // 将 Bit 转换为 bool，并转换为字符
    }
    return result;
}

string reverse_string(string str){
	reverse(str.begin(), str.end());
    return str;
}

void test() {
	auto start = clock_start();

	vector<Bit> key(128, Bit(false));
	vector<Bit> plaintext(128, Bit(false));
	key[126] = 1;
	plaintext[127] = 1;
	cout << "key      : " << bits2string(key) << endl;
	cout << "plaintext: " << bits2string(plaintext) << endl;

	vector<Bit> bit_vec = cat_vector(key, plaintext);
	Integer a(bit_vec);

	Integer c(128, 1, PUBLIC);

	cf.compute((block*)c.bits.data(), (block*)a.bits.data());
	cout << time_from(start)<<" "<<party<<" "<<reverse_string(c.reveal<string>())<<endl;

}
int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	NetIO* io = new NetIO(party==ALICE?nullptr:"127.0.0.1", port);

	setup_semi_honest(io, party);
	test();
	
	finalize_semi_honest();
	delete io;
}
