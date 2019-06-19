#include <fstream> 
#include <string>
#include <iostream>
#include <cstdlib>
#include <bitset>
#include <map>
#include <algorithm>

#define output_file "difference distribution table.csv"
#define s_box_input_possibilities 16
#define bits_s_box 4
using namespace std; 

ifstream in;
ofstream out;
string diff[s_box_input_possibilities][s_box_input_possibilities];
map <int, int> mp_freq;
//int s_box[s_box_input_possibilities] = { 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
int s_box[s_box_input_possibilities] = { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 };

void init()
{
	for (int i = 0; i < s_box_input_possibilities; i++) //init diff table
	{
		for (int j = 0; j < s_box_input_possibilities; j++)
		{
			diff[i][j].resize(bits_s_box + 1, '0');
		}
	}
	for (int i = 0; i < s_box_input_possibilities * s_box_input_possibilities; i++)
	{
		mp_freq.insert(make_pair(i, 0));
	}
}

void print_diff_table()
{
	for (int i = 0; i < s_box_input_possibilities; i++)
	{
		for (int j = 0; j < s_box_input_possibilities; j++)
		{
			cout << diff[i][j] << '\t';
		}
		cout << endl;
	}
}

void Xor(char a[bits_s_box], char b[bits_s_box], char res[bits_s_box])
{
	int x, y, z;
	for (int i = 0; i < bits_s_box; i++)
	{
		x = a[i] == '1' ? 1 : 0;
		y = b[i] == '1' ? 1 : 0;
		z = x ^ y;
		res[i] = z == 1 ? '1' : '0';
	}
}

void to_binary(char dst[bits_s_box], int i)
{
	for (long long k = 0; k < bits_s_box; k++)
	{
		dst[k] = bitset<bits_s_box>(i)[bits_s_box - k - 1] == 1 ? '1' : '0'; 
	}
}

void statistic()
{
	/*diff[i][j] = ¦¤Y, j = ¦¤X
	convert diff[i][j] and j to integer x and y, then mp_freq[y * s_box_input_possibilities + x]++;*/
	int x, y, cntr = 0;
	for (int i = 0; i < s_box_input_possibilities; i++)
	{
		for (int j = 0; j < s_box_input_possibilities; j++)
		{
			x = strtol(diff[i][j].c_str(), nullptr, 2);
			y = j;
			mp_freq[y * s_box_input_possibilities + x]++;
		}
	}
	out << endl;
	for(pair<int, int> p : mp_freq)
	{
		out << p.second << ',';
		//cout << p.second << '\t';
		if (++cntr == s_box_input_possibilities)
		{
			out << endl;
			//cout << endl;
			cntr = 0;
		}
	}
	out.close();
}

void generate()
{
	char tmp_delta_x[bits_s_box + 1] = { "0000" },
		tmp_delta_y[bits_s_box + 1] = { "0000" },
		tmp_x[bits_s_box + 1] = { "0000" },
		tmp_y[bits_s_box + 1] = { "0000" },
		tmp_x_xored[bits_s_box + 1] = { "0000" },
		tmp_y_xored[bits_s_box + 1] = { "0000" };

	out.open(output_file, ios::out | ios::trunc);
	out << "X,Y,";
	for (int i = 0; i < s_box_input_possibilities; i++)
	{
		out << "¦¤X = " << bitset<bits_s_box>(i) << ',';
	}
	out << endl;
	for (int i = 0; i < s_box_input_possibilities; i++)
	{
		for (int j = 0; j < s_box_input_possibilities; j++)
		{
			to_binary(tmp_x, i);
			to_binary(tmp_y, s_box[i]);
			to_binary(tmp_delta_x, j);
			if (j == 0)
			{
				//cout << "X = " << tmp_x << "\tY = " << tmp_y << '\t';
				out << "X = " << tmp_x << ',' << "Y = " << tmp_y << ',';
			}
			//cout << "¦¤X = " << tmp_delta_x << '\t';
			Xor(tmp_x, tmp_delta_x, tmp_x_xored);
			to_binary(tmp_y_xored, s_box[strtol(tmp_x_xored, nullptr, 2)]);
			Xor(tmp_y, tmp_y_xored, tmp_delta_y);
			//cout << "¦¤Y = " << tmp_delta_y << '\t';
			out << "¦¤Y = " << tmp_delta_y << ',';
			copy(tmp_delta_y, tmp_delta_y + bits_s_box + 1, diff[i][j].begin());
		}
		//cout << endl;
		out << endl;
	}
}

int main()
{
	init();
	generate();
	//print_diff_table();
	statistic();
	system("pause");

	return 0;
}
