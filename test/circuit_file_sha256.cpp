#include "emp-sh2pc/emp-sh2pc.h"
using namespace emp;
using namespace std;
const string circuit_file_location = macro_xstr(EMP_CIRCUIT_PATH);

int port, party;
string file = circuit_file_location+"/bristol_fashion/sha256.txt";
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
    
    // 补齐至8位的倍数
    while (len % 4 != 0) {
        str = '0' + str;  // 在左侧补零
        len++;
    }

    // 遍历每4位二进制字符串并转为十六进制
    for (int i = 0; i < len; i += 4) {
        string byte_str = str.substr(i, 4);  // 每4位二进制
        int byte_val = stoi(byte_str, nullptr, 2);  // 转换为十进制
        ss << hex << setw(1) << setfill('0') << byte_val;  // 转为十六进制
    }
    
    return ss.str();
}

void test() {
	auto start = clock_start();
	vector<Bit> message_block(512, Bit(false));
	vector<Bit> hash_state(256, Bit(false));
	cout << "message block: 0x" << bits2hexString(message_block) << endl;
	cout << "hash state   : 0x" << bits2hexString(hash_state) << endl;
	Integer a(cat_vector(message_block, hash_state));

	Integer c(256, 0, PUBLIC);

	cf.compute((block*)c.bits.data(), (block*)a.bits.data());
	cout << "ciphertext   : 0x"<< biString2hexString(c.reveal<string>()) << endl;
	cout << time_from(start) <<endl;

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
