#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <bitset>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <random>
#include <iomanip>

#define mx 100
#define mx_line 1000
#define key_len 80
#define block_size 16 //24bits
#define input_file_name "input_spn.txt"
#define output_file_name "output_spn.txt"
#define output_stat_file_name "output_stat.csv"
#define input_stat_file_name "input_stat.txt"
#define bits_to_byte 8 //8 bits = 1 byte
#define s_box_f_len 16
#define permutation_len 16
#define s_box_cap 4
#define key_possibilities 256
#define active_bits 8
constexpr long long diff[block_size] = { 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
constexpr long long delta_u[block_size] = { 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0 };
constexpr bool active_s_boxes[block_size / s_box_cap] = { 0, 1, 0, 1 };

using namespace std;

long long rd = 0;
long long a, m;
ifstream in;
ofstream out;
bool blocks[mx][block_size], subkeys[mx][block_size], mstr_key[key_len], s_box[s_box_f_len][bits_to_byte], permutation[permutation_len][bits_to_byte], x[mx], tmp_res[mx][block_size], testing_pairs[mx][block_size];

void input()
{
	char tmp[mx];
	long long cntr = 0;
	in.open(input_file_name);

	in.getline(tmp, mx); //master key, 21 * 8 = 168 bits
	for (long long i = 0; i < key_len; i++)
	{
		if (tmp[cntr] != ' ')
		{
			mstr_key[i] = tmp[cntr++] == '1' ? 1 : 0;
		}
		else
		{
			cntr++;
			i--;
		}
	}
	cntr = 0;
	memset(tmp, 0, sizeof(tmp));

	in.getline(tmp, mx); //s_box
	for (long long i = 0; i < s_box_f_len; i++)
	{
		for (long long j = 0; j < bits_to_byte; j++)
		{
			if (tmp[cntr] == ' ')
			{
				cntr++;
				j--;
				continue;
			}
			else
			{
				s_box[i][j] = tmp[cntr++] == '1' ? 1 : 0;
			}
		}
	}
	cntr = 0;
	memset(tmp, 0, sizeof(tmp));

	in.getline(tmp, mx); //permutation
	for (long long i = 0; i < permutation_len; i++)
	{
		for (long long j = 0; j < bits_to_byte; j++)
		{
			if (tmp[cntr] == ' ')
			{
				cntr++;
				j--;
				continue;
			}
			else
			{
				permutation[i][j] = tmp[cntr++] == '1' ? 1 : 0;
			}
		}
	}
	cntr = 0;
	memset(tmp, 0, sizeof(tmp));

	in.getline(tmp, mx); //m
	m = stoi(tmp);
	memset(tmp, 0, sizeof(tmp));

	in.getline(tmp, mx); //x
	for (long long i = 0; i < m; i++)
	{
		if (tmp[cntr] != ' ')
		{
			x[i] = tmp[cntr++] == '1' ? 1 : 0;
		}
		else
		{
			cntr++;
			i--;
		}
	} //input bits
	cntr = 0;
	memset(tmp, 0, sizeof(tmp));

	in.getline(tmp, mx); //a
	//cout << "tmp=" << tmp << endl;
	a = stoi(tmp);
	memset(tmp, 0, sizeof(tmp));

	in.close();
}

void output()
{
	long long cntr = 0;
	out.open(output_file_name, ios::trunc);
	for (bool b : mstr_key) //output master key
	{
		out << b;
		cntr++;
		if (cntr % bits_to_byte == 0 && cntr != 0)
		{
			out << ' ';
		}
	}
	cntr = 0;
	out << endl;

	for (long long i = 0; i < s_box_f_len; i++) //output s_box
	{
		for (long long j = 0; j < bits_to_byte; j++)
		{
			out << s_box[i][j];
		}
		out << ' ';
	}
	out << endl;

	for (long long i = 0; i < permutation_len; i++) //output permutation
	{
		for (long long j = 0; j < bits_to_byte; j++)
		{
			out << permutation[i][j];
		}
		out << ' ';
	}
	out << endl;

	out << m << endl; //output m

	for (long long i = 0; i < m; i++) //output x
	{
		out << x[i];
		if ((i + 1) % bits_to_byte == 0 && i != 0)
		{
			out << ' ';
		}
	}
	out << endl;

	if (a == 0)
	{
		out << 1 << endl; //a'
	}
	else
	{
		out << 0 << endl; //a'
	}

	out.close();
}

void s_box_substitution(long long& i, long long& idx, bool tmp_s_box_bool[s_box_cap], char tmp_s_box_char[s_box_cap])
{
	for (long long j = 0; j < block_size / s_box_cap; j++) //6 blocks, 6 s-boxes substitution
	{
		copy(tmp_res[i] + j * s_box_cap, tmp_res[i] + (j + 1) * s_box_cap, tmp_s_box_bool); //4 bits as input of a s-box
		for (long long z = 0; z < s_box_cap; z++)
		{
			tmp_s_box_char[z] = tmp_s_box_bool[z] == 1 ? '1' : '0';
		}
		idx = stoi(tmp_s_box_char, nullptr, 2);
		if (a == 0) //encryption
		{
			for (long long z = 0; z < s_box_cap; z++)
			{
				tmp_res[i][j * s_box_cap + z] = s_box[idx][z + 4];
			}
		}
		else //decryption, reverse s_boxes
		{
			for (long long z = 0; z < s_box_f_len; z++)
			{
				for (long long y = bits_to_byte / 2; y < bits_to_byte; y++)
				{
					if (tmp_s_box_bool[y - s_box_cap] != s_box[z][y])
					{
						break;
					}
					if (y == bits_to_byte - 1)
					{
						for (long long x = 0; x < s_box_cap; x++)
						{
							tmp_res[i][j * s_box_cap + x] = bitset<s_box_cap>(z)[s_box_cap - x - 1];
						}
					}
				}
			}
		}
	}
}

void perm(long long& i, long long& idx, char tmp_perm[bits_to_byte])
{
	for (long long x = 0; x < block_size; x++)
	{
		for (long long y = 0; y < bits_to_byte; y++)
		{
			tmp_perm[y] = permutation[x][y] == 1 ? '1' : '0';
		}
		idx = stoi(tmp_perm, nullptr, 2);
		blocks[i][idx - 1] = tmp_res[i][x];
	}
	for (long long y = 0; y < block_size; y++)
	{
		tmp_res[i][y] = blocks[i][y];
	}
}

void Xor(long long& i, long long& k)
{
	for (long long j = 0; j < block_size; j++)
	{
		tmp_res[i][j] = tmp_res[i][j] ^ subkeys[k][j];
	}
}

void enc_dec()
{
	bool tmp_s_box_bool[s_box_cap];
	char tmp_s_box_char[s_box_cap], tmp_perm[bits_to_byte];
	long long idx, cntr = 0;
	for (long long i = 0; i < m / block_size; i++) //separate plain bits
	{
		for (long long j = 0; j < block_size; j++)
		{
			tmp_res[i][j] = x[i * block_size + j];
		}
	}

	for (long long i = 0; i < key_len / block_size; i++) //separate master key
	{
		for (long long j = 0; j < block_size; j++)
		{
			if (a == 0) //enc or dec
			{
				subkeys[i][j] = mstr_key[i * block_size + j];
			}
			else
			{
				subkeys[key_len / block_size - i - 1][j] = mstr_key[i * block_size + j];
			}
		}
	}

	if (a == 0) //encryption
	{
		for (long long i = 0; i < m / block_size; i++) //[m / block_size] plain bits segments
		{
			for (long long k = 0; k < key_len / block_size; k++) //7 XORs with subkeys[k], 6 rounds encryption
			{
				Xor(i, k); //XOR operation
				if (k == key_len / block_size - 1) //the last XOR
				{
					break;
				}
				s_box_substitution(i, idx, tmp_s_box_bool, tmp_s_box_char); //6 blocks, 6 s-boxes substitution
				if (k == key_len / block_size - 2)
				{
					continue;
				}
				perm(i, idx, tmp_perm); //permutation
			}
		}
	}
	else //decryption
	{
		for (long long i = 0; i < m / block_size; i++) //[m / block_size] plain bits segments
		{
			for (long long k = 0; k < key_len / block_size; k++) //7 XORs with subkeys[k], 6 rounds decryption
			{
				if (k == 0)
				{
					Xor(i, k); //XOR operation
					continue;
				}
				s_box_substitution(i, idx, tmp_s_box_bool, tmp_s_box_char); //6 blocks, 6 s-boxes substitution
				Xor(i, k);
				if (k == key_len / block_size - 1)
				{
					continue;
				}
				perm(i, idx, tmp_perm); //permutation
			}
		}
	}

	for (long long i = 0; i < m / block_size; i++) //final x[] after encryption or decryption
	{
		for (long long j = 0; j < block_size; j++)
		{
			x[cntr++] = tmp_res[i][j];
		}
	}
}

void analyze()
{
	long long cntr = 0, idx = 0, cntr_delta_u1 = 0, cntr_delta_u2 = 0;
	long long tmp_s_box[s_box_f_len] = { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 };
	long long tmp_perm[permutation_len] = { 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16 };
	long long res_diff[block_size] = { 0 };
	long long cntr_key[key_possibilities] = { 0 };
	char tmp_str[mx_line];
	string tmp_s_box_input(s_box_cap, '0');
	string tmp_x(block_size, '0');
	vector < vector <long long> > key_space(key_possibilities, vector <long long>(block_size, 0));
	vector < vector <long long> > delta_u1(key_possibilities * mx / 2, vector <long long>(block_size, 0));
	vector < vector <long long> > delta_u2(key_possibilities * mx / 2, vector <long long>(block_size, 0));
	uniform_int_distribution <long long> rd_pairs(0, (long long)pow(2, block_size) - 1);
	mt19937 e;

	/*generate possible keys*/
	srand((unsigned)time(NULL));
	for (long long i = 0; i < key_possibilities; i++)
	{
		for (long long j = 0; j < block_size; j += s_box_cap)
		{
			if (active_s_boxes[j / s_box_cap] == true) //active s-box
			{
				for (long long k = 0; k < s_box_cap; k++)
				{
					key_space[i][j + k] = bitset<active_bits>(i)[active_bits - 1 - k - cntr * s_box_cap];
				}
				cntr++;
			}
			else //inactive s-box, randomly filled
			{
				for (long long k = 0; k < s_box_cap; k++)
				{
					key_space[i][j + k] = rand() % 2;
				}
			}
		}
		cntr = 0;
		/*print all possible keys*/
		//for (long long j = 0; j < block_size; j++)
		//{
		//	cout << key_space[i][j];
		//}
		//cout << endl;
	}

	/*generate random 168-bits master key*/
	in.open(input_stat_file_name);
	in.getline(tmp_str, mx);
	srand((unsigned)time(NULL));
	for (long long i = 0; i < key_len; i++)
	{
		mstr_key[i] = tmp_str[i] == '1' ? true : false;
		//mstr_key[i] = rand() % 2;
		//cout << mstr_key[i];
	}
	memset(tmp_str, 0, sizeof(tmp_str));

	/*set s-box*/
	for (long long i = 0; i < s_box_f_len; i++)
	{
		for (long long j = 0; j < bits_to_byte; j++)
		{
			s_box[i][j] = bitset<bits_to_byte>(tmp_s_box[i])[bits_to_byte - j - 1];
		}
	}

	/*set permutation*/
	for (long long i = 0; i < permutation_len; i++)
	{
		for (long long j = 0; j < bits_to_byte; j++)
		{
			permutation[i][j] = bitset<bits_to_byte>(tmp_perm[i])[bits_to_byte - j - 1];
		}
	}

	/*generate 5000 pairs of testing plain/cipher texts*/
	for (long long i = 0; i < mx / 2; i++)
	{
		//in.getline(tmp_str, mx);
		for (long long j = 0; j < block_size; j++)
		{
			testing_pairs[i][j] = bitset<block_size>(rd_pairs(e))[block_size - j - 1] == 1 ? true : false;
			//testing_pairs[i][j] = tmp_str[j] == '1' ? true : false;
		}
		//memset(tmp_str, 0, sizeof(tmp_str));
	}
	in.close();
	for (long long i = mx / 2; i < mx; i++)
	{
		for (long long j = 0; j < block_size; j++)
		{
			testing_pairs[i][j] = testing_pairs[i - mx / 2][j] == true ? 1 ^ diff[j] : 0 ^ diff[j];
		}
	}

	/*print the testing pairs*/
	//cout << endl;
	//for (long long i = 0; i < mx / 2; i++)
	//{
	//	for (long long j = 0; j < block_size; j++)
	//	{
	//		cout << testing_pairs[i][j];
	//	}
	//	cout << endl;
	//	for (long long j = 0; j < block_size; j++)
	//	{
	//		cout << testing_pairs[i + mx / 2][j];
	//	}
	//	cout << endl << "=========================" << endl;
	//}

	/*write generated plain texts to file*/
	//out.open("test", ios::trunc);
	//for (long long i = 0; i < mx / 2; i++)
	//{
	//	for (long long j = 0; j < block_size; j++)
	//	{
	//		out << testing_pairs[i][j];
	//	}
	//	out << endl;
	//}
	//out.close();

	/*set m, a*/
	m = 16;
	a = 0;

	/*set x, generate cipher texts and get ¦¤U*/
	for (long long h = 0; h < mx / 2; h++)
	{
		copy(testing_pairs[h], testing_pairs[h] + block_size, x);
		enc_dec();
		for (long long i = 0; i < key_possibilities; i++)
		{
			for (long long j = 0; j < block_size; j++)
			{
				tmp_x[j] = x[j] == true ? 1 ^ key_space[i][j] + 48 : 0 ^ key_space[i][j] + 48;
			}
			for (long long j = 0; j < block_size; j += 4)
			{
				copy(tmp_x.begin() + j, tmp_x.begin() + j + s_box_cap, tmp_s_box_input.begin());
				idx = find(tmp_s_box, tmp_s_box + s_box_f_len, stoi(tmp_s_box_input.c_str(), nullptr, 2)) - tmp_s_box;
				for (long long k = 0; k < s_box_cap; k++)
				{
					delta_u1[cntr_delta_u1][j + k] = bitset<s_box_cap>(idx)[s_box_cap - k - 1];
				}
			}
			cntr_delta_u1++;
		}

		/*print the 1st cipher of a pair */
		//cout << "1: ";
		//for (long long i = 0; i < m; i++)
		//{
		//	cout << x[i];
		//}
		//cout << endl;


		copy(testing_pairs[h + mx / 2], testing_pairs[h + mx / 2] + block_size, x);
		enc_dec();
		for (long long i = 0; i < key_possibilities; i++)
		{
			for (long long j = 0; j < block_size; j++)
			{
				tmp_x[j] = x[j] == true ? 1 ^ key_space[i][j] + 48 : 0 ^ key_space[i][j] + 48;
			}
			for (long long j = 0; j < block_size; j += 4)
			{
				copy(tmp_x.begin() + j, tmp_x.begin() + j + s_box_cap, tmp_s_box_input.begin());
				idx = find(tmp_s_box, tmp_s_box + s_box_f_len, stoi(tmp_s_box_input.c_str(), nullptr, 2)) - tmp_s_box;
				for (long long k = 0; k < s_box_cap; k++)
				{
					delta_u2[cntr_delta_u2][j + k] = bitset<s_box_cap>(idx)[s_box_cap - k - 1];
				}
			}
			cntr_delta_u2++;
		}

		/*print the 2nd cipher of a pair*/
		//cout << "2: ";
		//for (long long i = 0; i < m; i++)
		//{
		//	cout << x[i];
		//}
		//cout << endl << "============================" << endl;
	}

	/*print ¦¤U1 and ¦¤U2*/
	//for (long long i = 0; i < key_possibilities * mx / 2; i++)
	//{
	//	cout << endl << "¦¤U1:[" << i << "] ";
	//	for (long long j = 0; j < block_size; j++)
	//	{
	//		cout << delta_u1[i][j];
	//	}
	//	cout << endl << "¦¤U2:[" << i << "] ";
	//	for (long long j = 0; j < block_size; j++)
	//	{
	//		cout << delta_u2[i][j];
	//	}
	//}

	/*check diff of each pair of ¦¤U1 and ¦¤U2*/
	for (long long i = 0; i < key_possibilities * mx / 2; i++)
	{
		for (long long j = 0; j < block_size; j++)
		{
			res_diff[j] = delta_u1[i][j] ^ delta_u2[i][j];
			if (res_diff[j] != delta_u[j])
			{
				break;
			}
			if (j == block_size - 1)
			{
				cntr_key[i % key_possibilities]++;
			}
			//if (active_s_boxes[j / s_box_cap])
			//{
			//	if (res_diff[j] != delta_u[j])
			//	{
			//		break;
			//	}
			//	if (j == block_size - 1)
			//	{
			//		cntr_key[i % key_possibilities]++;
			//	}
			//}
		}
	}

	/*write analysis result to csv file*/
	out.open(output_stat_file_name, ios::trunc);
	for (long long i = 0; i < 8; i++)
	{
		out << "K1K2,P (Count),";
	}
	out << endl;
	for (long long i = 0; i < key_possibilities; i++)
	{
		out << bitset<bits_to_byte>(i) << ',' << (double)cntr_key[i] / (mx / 2) << setprecision(5) << '(' << cntr_key[i] << "),";
		//for (long long j = 0; j < block_size; j++)
		//{
		//	out << key_space[i][j];
		//}
		if ((i + 1) % 8 == 0 && i != 0)
		{
			out << endl;
		}
	}
	out.close();

	/*screen output*/
	for (long long i = 0; i < block_size; i++)
	{
		cout << key_space[max_element(cntr_key, cntr_key + key_possibilities) - cntr_key][i];
	}
	cout << " -> " << *(max_element(cntr_key, cntr_key + key_possibilities)) << ", " << fixed << setprecision(5) << (double)*(max_element(cntr_key, cntr_key + key_possibilities)) / (mx / 2) << endl;

	cout << endl << "The right key should be: ";
	for (long long i = 0; i < block_size; i++)
	{
		cout << subkeys[key_len / block_size - 1][i];
	}
	cout << endl;
}

int main()
{
	//input();
	//enc_dec();
	//output();
	analyze();
	return 0;
}