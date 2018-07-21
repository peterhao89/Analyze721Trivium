#include<algorithm>
#include<set>
#include<vector>
#include<iostream>
#include<fstream>
#include<stdio.h>
#include<time.h>
#include<map>
#include<omp.h>
#include<string>


typedef unsigned char uint8;
typedef unsigned long uint32;
typedef  unsigned long long uint64;
#define LROT32(x,s)  ((((x)&0xffffffff)<<(s))|(((x)&0xffffffff)>>(32-(s))))
#define RROT32(x,s)  ((((x)&0xffffffff)>>(s))|(((x)&0xffffffff)<<(32-(s))))
#define LROT8(x,s)  (((((x)&0xff)<<(s))|(((x)&0xff)>>(8-(s))))&0xff)
#define RROT8(x,s)  (((((x)&0xff)>>(s))|(((x)&0xff)<<(8-(s))))&0xff)
#define LROT64(x,s)  (((x)<<(s))|((x)>>(64-(s))))
#define RROT64(x,s)  (((x)>>(s))|((x)<<(64-(s))))
#define bit(x,n)   (((x)>>(n))&1)
#define bit2(x1,x2,pos) (bit(x1,pos)^bit(x2,pos))
#define bit32(x,n)   (((x)>>(n))&1)
#define bitW32(x,n)  bit32((x[((n)/32)]),((n)%32))
#define bit2p(x1,pos1,x2,pos2) (bit(x1,pos1)^bit(x2,pos2))






using namespace std;

#define CORES 6



void NotBit(vector<uint32> & x, int n)
{
	uint32 one = 1;
	one <<= (n % 32);
	x[n / 32] ^= one;
}


void SetBit(uint32 & X, int bitNum, int zro_or_one)
{
	uint32 one;
	if (0 == zro_or_one)
	{
		one = 1;
		one <<= bitNum;
		one = (~one);
		X &= one;
	}
	else
	{
		one = 1;
		one <<= bitNum;
		X |= one;
	}
}

void SetDivBit(vector<uint32>  & InputDiv, int BitNum, uint32 O_or_1)
{
	uint32 mask = 0xffffffff;
	uint32 one = 1;
	one <<= (BitNum % 32);
	mask ^= one;
	if (O_or_1 == 0)
	{
		InputDiv[BitNum / 32] &= mask;
	}
	else
	{
		InputDiv[BitNum / 32] &= mask;
		InputDiv[BitNum / 32] ^= one;
	}
}



uint64 rand_64()//产生64位随机数
{
	static uint64 Z[2] = { 0x375201345e7fa379, 0xcde9fe134e8af6b1 ^ (uint64(rand()) << 32) ^ rand() };//(rand() << 24) + (rand() << 12) + rand() 0xcde9fb8a
	uint64 temp = Z[1] ^ (Z[1] << 63) ^ (Z[0] >> 1);
	Z[0] = Z[1];
	Z[1] = temp;
	return Z[0];
}

uint32 rand_32()//产生32位随机数
{
	static unsigned int Z[2] = { 0x37520134, 0xcde9fb81 ^ rand() };//(rand() << 24) + (rand() << 12) + rand() 0xcde9fb8a
	unsigned int temp = Z[1] ^ (Z[1] << 31) ^ (Z[0] >> 1);
	Z[0] = Z[1];
	Z[1] = temp;
	return Z[0];
}

void RRotOneBitTDiv(vector<uint32>  & td)
{
	uint32 tmp288 = bitW32(td, 287);
	uint32 tmp = bitW32(td, 255);
	td[8] <<= 1;
	SetDivBit(td, 256, tmp);

	tmp = bitW32(td, 223);
	td[7] <<= 1;
	SetDivBit(td, 224, tmp);

	tmp = bitW32(td, 191);
	td[6] <<= 1;
	SetDivBit(td, 192, tmp);

	tmp = bitW32(td, 159);
	td[5] <<= 1;
	SetDivBit(td, 160, tmp);



	tmp = bitW32(td, 127);
	td[4] <<= 1;
	SetDivBit(td, 128, tmp);

	tmp = bitW32(td, 95);
	td[3] <<= 1;
	SetDivBit(td, 96, tmp);

	tmp = bitW32(td, 63);
	td[2] <<= 1;
	SetDivBit(td, 64, tmp);

	tmp = bitW32(td, 31);
	td[1] <<= 1;
	SetDivBit(td, 32, tmp);

	td[0] <<= 1;



	SetDivBit(td, 0, tmp288);


}



class Trivium_Primitive
{
public:
	vector<uint32> State;
	int Round;
	Trivium_Primitive()
	{
		for (int i = 0; i<9; ++i)
		{
			State.push_back(0);
		}
		Round = 0;
	}

	void ZroAll()
	{
		for (int i = 0; i<9; ++i)
		{
			State[i] = 0;
		}
		for (int i = 285; i < 288; ++i)
		{
			SetDivBit(State, i, 1);
		}
		Round = 0;
	}


	void IniKey(vector<uint32> key)
	{
		for (int i = 0; i<80; ++i)
		{
			if (bitW32(key, i) == 1)
			{
				SetDivBit(State, i, 1);
			}
			else
			{
				SetDivBit(State, i, 0);
			}
		}
	}

	void InitIV(vector<uint32> iv, set<int> cubeindex)
	{
		set<int> freeindex;
		for (int i = 0; i<80; ++i)
		{
			if (cubeindex.find(i) == cubeindex.end())
			{
				freeindex.insert(i);
			}
		}
		for (set<int>::iterator ite = freeindex.begin(); ite != freeindex.end(); ++ite)
		{
			if (bitW32(iv, *ite) == 1)
			{
				SetDivBit(State, *ite + 93, 1);
			}
		}
	}


	void InitIV(vector<uint32> iv)
	{
		for (int i = 0; i<80; ++i)
		{
			if (bitW32(iv, i) == 1)
			{
				SetDivBit(State, i + 93, 1);
			}
			else
			{
				SetDivBit(State, i + 93, 0);
			}
		}
	}


	void InitRest(uint32 Rest[9])
	{
		for (int i = 80; i<93; ++i)
		{
			if (bitW32(Rest, i) == 1)
			{
				SetDivBit(State, i, 1);
			}
		}
		for (int i = 93 + 80; i<288; ++i)
		{
			if (bitW32(Rest, i) == 1)
			{
				SetDivBit(State, i, 1);
			}
		}
	}



	void IniRandom()
	{
		for (int i = 0; i<9; ++i)
		{
			State[i] = rand_32();
		}
	}

	void Update_State()
	{
		uint32 t1, t2, t3;
		t1 = (bitW32(State, 92 - 1) & bitW32(State, 91 - 1)) ^ bitW32(State, 66 - 1) ^ bitW32(State, 171 - 1) ^ bitW32(State, 93 - 1);
		t2 = (bitW32(State, 176 - 1) & bitW32(State, 175 - 1)) ^ bitW32(State, 162 - 1) ^ bitW32(State, 264 - 1) ^ bitW32(State, 177 - 1);
		t3 = (bitW32(State, 287 - 1) & bitW32(State, 286 - 1)) ^ bitW32(State, 243 - 1) ^ bitW32(State, 69 - 1) ^ bitW32(State, 288 - 1);

		SetDivBit(State, 93 - 1, t1);
		SetDivBit(State, 177 - 1, t2);
		SetDivBit(State, 288 - 1, t3);
		RRotOneBitTDiv(State);
	}


	uint32 Output_Eq()
	{
		return bitW32(State, 66 - 1) ^ bitW32(State, 93 - 1) ^ bitW32(State, 243 - 1) ^ bitW32(State, 288 - 1) ^ bitW32(State, 162 - 1) ^ bitW32(State, 177 - 1);
	}

};

class Trivium_KeyRecoveryCube
{
public:
	vector<int> CubeIndex;
	int TargRound;
	set<int> RelatedKey;
	Trivium_KeyRecoveryCube(set<int> Cb, int targ = 1152)
	{
		for (set<int>::iterator ite = Cb.begin(); ite != Cb.end(); ++ite)
		{
			CubeIndex.push_back(*ite);
		}
		TargRound = targ;
	}


	Trivium_KeyRecoveryCube(vector<int> Cb, int targ = 1152)
	{
		CubeIndex = Cb;
		TargRound = targ;
	}

	//Related Key 1
	//Non-Related Key 0
	int WheRelatedKey(vector<uint32> IV, int KeyIndex, int TestTime = 100)
	{
		ofstream file1;
		uint32 cubesize = 1;
		cubesize <<= CubeIndex.size();
		for (int tt = 0; tt < TestTime; ++tt)
		{
			vector<uint32> key = { rand_32(), rand_32(), rand_32(), rand_32() };
			Trivium_Primitive Runner[CORES];
			for (int i = 0; i < CORES; ++i)
			{
				Runner[i] = Trivium_Primitive();
			}
			uint32 Sum0 = 0, Sum1 = 0;

			for (uint32 current = 0; current < cubesize; ++current)
			{
				Runner[current%CORES].ZroAll();
				Runner[current%CORES].IniKey(key);
				Runner[current%CORES].InitIV(IV);
				for (int countbt = 0; countbt < CubeIndex.size(); ++countbt)
				{
					if (bit32(current, countbt) == 1)
					{
						SetDivBit(Runner[current%CORES].State, 93 + CubeIndex[countbt], 1);
					}
					else
					{
						SetDivBit(Runner[current%CORES].State, 93 + CubeIndex[countbt], 0);
					}
				}
				for (int rd = 0; rd < TargRound; ++rd)
				{
					Runner[current%CORES].Update_State();
				}
				Sum0 ^= Runner[current%CORES].Output_Eq();

			}
			NotBit(key, KeyIndex);
			//#pragma omp parallel for reduction(^:Sum1)
			for (uint32 current = 0; current < cubesize; ++current)
			{
				Runner[current%CORES].ZroAll();
				Runner[current%CORES].IniKey(key);
				Runner[current%CORES].InitIV(IV);
				for (int countbt = 0; countbt < CubeIndex.size(); ++countbt)
				{
					if (bit32(current, countbt) == 1)
					{
						SetDivBit(Runner[current%CORES].State, 93 + CubeIndex[countbt], 1);
					}
					else
					{
						SetDivBit(Runner[current%CORES].State, 93 + CubeIndex[countbt], 0);
					}
				}
				for (int rd = 0; rd < TargRound; ++rd)
				{
					Runner[current%CORES].Update_State();
				}
				Sum1 ^= Runner[current%CORES].Output_Eq();

			}

#if 0
			//#pragma omp parallel for reduction(^:Sum0)
			for (uint32 current = 0; current < cubesize; ++current)
			{
				Runner[current%CORES].ZroAll();
				Runner[current%CORES].IniKey(key);
				Runner[current%CORES].InitIV(IV);
				for (int countbt = 0; countbt < CubeIndex.size(); ++countbt)
				{
					if (bit32(current, countbt) == 1)
					{
						SetDivBit(Runner[current%CORES].State, 93 + CubeIndex[countbt], 1);
					}
					else
					{
						SetDivBit(Runner[current%CORES].State, 93 + CubeIndex[countbt], 0);
					}
					for (int rd = 0; rd < TargRound; ++rd)
					{
						Runner[current%CORES].Update_State();
					}
					Sum0 ^= Runner[current%CORES].Output_Eq();
				}

			}
			//NotBit(key, KeyIndex);
			//#pragma omp parallel for reduction(^:Sum1)
			for (uint32 current = 0; current < cubesize; ++current)
			{
				Runner[current%CORES].ZroAll();
				Runner[current%CORES].IniKey(key);
				Runner[current%CORES].InitIV(IV);
				for (int countbt = 0; countbt < CubeIndex.size(); ++countbt)
				{
					if (bit32(current, countbt) == 1)
					{
						SetDivBit(Runner[current%CORES].State, 93 + CubeIndex[countbt], 1);
					}
					else
					{
						SetDivBit(Runner[current%CORES].State, 93 + CubeIndex[countbt], 0);
					}
					for (int rd = 0; rd < TargRound; ++rd)
					{
						Runner[current%CORES].Update_State();
					}
					Sum1 ^= Runner[current%CORES].Output_Eq();
				}

			}
#endif
			if (Sum0 != Sum1)
			{
				RelatedKey.insert(KeyIndex);
				return 1;
			}

		}//end for (int tt = 0; tt < TestTime; ++tt)
		return 0;
	}


	int TermEvaluate(vector<uint32> IV, vector<int> KeyIndex, int TestTime = 100)
	{
		ofstream file1;
		string filename = "PrintScreen";
		for (int i = 0; i < IV.size(); ++i)filename = filename + to_string(IV[i]);
		filename += ".txt";
		file1.open(filename, ios::app);
		for (int i = 0; i < IV.size(); ++i)
		{
			file1 << hex << IV[i] << ",";
		}
		file1 << endl;
		for (int i = 0; i < KeyIndex.size(); ++i)
		{
			file1 << dec << KeyIndex[i] << ",";
		}
		file1 << endl;
		file1.close();
		uint32 cubesize = 1;
		cubesize <<= CubeIndex.size();
		uint32 keysize = 1;
		keysize <<= KeyIndex.size();
		for (uint32 ckey = 0; ckey < keysize; ++ckey)
		{

			for (int tt = 0; tt < TestTime; ++tt)
			{
				vector<uint32> key = { rand_32(), rand_32(), rand_32() & 0xffff };
				vector<uint32> tmp = { 0, 0, 0, 0 };
				for (int bt = 0; bt < KeyIndex.size(); ++bt)
				{
					SetDivBit(key, KeyIndex[bt], bit(ckey, bt));
				}
				Trivium_Primitive Runner[CORES];
				for (int i = 0; i < CORES; ++i)
				{
					Runner[i] = Trivium_Primitive();
				}
				uint32 Sum0 = 0, Sum1 = 0;

				for (uint32 current = 0; current < cubesize; ++current)
				{
					Runner[current%CORES].ZroAll();
					Runner[current%CORES].IniKey(key);
					Runner[current%CORES].InitIV(IV);
					for (int countbt = 0; countbt < CubeIndex.size(); ++countbt)
					{
						if (bit32(current, countbt) == 1)
						{
							SetDivBit(Runner[current%CORES].State, 93 + CubeIndex[countbt], 1);
						}
						else
						{
							SetDivBit(Runner[current%CORES].State, 93 + CubeIndex[countbt], 0);
						}
					}
					for (int rd = 0; rd < TargRound; ++rd)
					{
						Runner[current%CORES].Update_State();
					}
					Sum0 ^= Runner[current%CORES].Output_Eq();

				}
				//ofstream fil
				file1.open(filename, ios::app);
				//for (int i = 0; i < 3; ++i)file1 << hex << "0x" << IV[i] << ",";
				//file1 << dec << endl;
				if (TestTime == 1)file1 << "[";
				for (int bt = 0; bt < KeyIndex.size(); ++bt)
				{
					file1 << bitW32(key, KeyIndex[bt]) << ",";
					//SetDivBit(key, KeyIndex[bt], bit(ckey, bt));
				}
				if (TestTime == 1)file1 << "],";
				file1 << Sum0 << endl;
				file1.close();
				//getchar();

			}



		}


		return 0;
	}

};

//Generate the name of the file according to different cubes.
string FileNameSet(set<int> Cube)
{
	string str = "TRCb";
	vector<uint32> vc = { 0, 0, 0, 0 };
	for (set<int>::iterator ite = Cube.begin(); ite != Cube.end(); ++ite)
	{
		SetDivBit(vc, *ite, 1);
	}
	for (int i = 0; i < 4; ++i)
	{
		str.append(to_string(vc[i]));
		str.append(",");
	}
	str.erase(str.end() - 1);
	str.append(".txt");
	return str;
}


class Trivium_DynamicCube
{
public:
	vector<int> CubeIndex;
	vector<set<int>> S_NULL;
	set<int> NullIV;
	int IniRound;


	Trivium_DynamicCube(set<int> Cb, vector<set<int>> snull, set<int> nulliv, int TargRoud = 721)
	{
		for (set<int>::iterator ite = Cb.begin(); ite != Cb.end(); ++ite)
		{
			CubeIndex.push_back(*ite);
		}
		IniRound = TargRoud;
		S_NULL = snull;
		NullIV = nulliv;
	}


	Trivium_DynamicCube(vector<int> Cb, vector<set<int>> snull, set<int> nulliv, int TargRoud = 721)
	{
		CubeIndex = Cb;
		IniRound = TargRoud;
		S_NULL = snull;
		NullIV = nulliv;
	}

	//Test Zero-Sum property for pure z^R
	bool WheZrosumPureZ(vector<uint32> IV, int TestTime = 100)
	{
		set<int> Idx;
		for (int i = 0; i < CubeIndex.size(); ++i)Idx.insert(CubeIndex[i]);
		ofstream file1;
		uint64 cubesize = 1;
		cubesize <<= CubeIndex.size();
		bool ALL1USED = false;
		for (int tt = 0; tt < TestTime; ++tt)
		{
			cout << "Time:" << tt << endl;
			vector<uint32> key;
			if (ALL1USED == false)
			{
				key = { 0xffffffff, 0xffffffff, 0xffffffff };
				ALL1USED = true;
			}
			else
			{
				key = { rand_32(), rand_32(), rand_32() };
			}
			Trivium_Primitive Runner;
			Runner = Trivium_Primitive();
			uint32 Sum0 = 0, Sum1 = 0;
			uint32 DyBit = 0;
			//omp_set_num_threads(CORES);
			for (uint64 current = 0; current < cubesize; ++current)
			{
				if (current % (0xfffffff + 1) == 0)
				{
					ofstream file1("Intermediate" + FileNameSet(Idx) + ".txt", ios::app);
					for (int i = 0; i < CubeIndex.size(); ++i)
					{
						file1 << CubeIndex[i] << ",";
					}
					file1 << endl;
					file1 << current << "," << Sum0 << endl;
					file1 << "IVconst: " << IV[0] << "," << IV[1] << "," << IV[2] << endl;
					file1 << "Correct key: " << key[0] << "," << key[1] << "," << key[2] << endl;
					file1.close();
				}
				if ((current & 0xfffffff) == 0xfffffff)
				{
					cout << hex << current << endl;
				}
				Runner.ZroAll();
				Runner.IniKey(key);
				Runner.InitIV(IV);
				for (int countbt = 0; countbt < CubeIndex.size(); ++countbt)
				{
					if (bit32(current, countbt) == 1)
					{
						SetDivBit(Runner.State, 93 + CubeIndex[countbt], 1);
					}
					else
					{
						SetDivBit(Runner.State, 93 + CubeIndex[countbt], 0);
					}
				}
				for (int rd = 0; rd < IniRound; ++rd)
				{
					if (S_NULL[0].find(rd) != S_NULL[0].end())
					{
						DyBit = bitW32(Runner.State, 0);
					}
					else if (S_NULL[2].find(rd) != S_NULL[2].end())
					{
						DyBit = bitW32(Runner.State, 177);
					}
					else if (S_NULL[1].find(rd) != S_NULL[1].end())
					{
						DyBit = bitW32(Runner.State, 93);
					}
					Runner.Update_State();
				}
				Sum0 ^= (Runner.Output_Eq());
			}
			if (Sum0 != 0)
			{
				cout << dec << "False Key Found!" << endl;
				ofstream file1("FindKey.txt", ios::app);
				for (int i = 0; i < CubeIndex.size(); ++i)
				{
					file1 << CubeIndex[i] << ",";
				}
				file1 << endl;

				file1 << "FalseKey:";
				for (int i = 0; i < 3; ++i)
				{
					file1 << key[i] << ",";
				}
				file1 << endl;
				return false;
			}
		}//end for (int tt = 0; tt < TestTime; ++tt)
		return true;
	}

	//Dynamic Z for Correct Key Guess (1+s^t_i)*z^R
	bool WheZrosumDyZ(vector<uint32> IV, int TestTime = 100)
	{
		set<int> Idx;
		for (int i = 0; i < CubeIndex.size(); ++i)Idx.insert(CubeIndex[i]);
		ofstream file1;
		uint64 cubesize = 1;
		cubesize <<= CubeIndex.size();
		bool ALL1USED = false;
		for (int tt = 0; tt < TestTime; ++tt)
		{
			cout << "Time:" << tt << endl;
			vector<uint32> key;
			if (ALL1USED == false)
			{
				key = { 0xffffffff, 0xffffffff, 0xffffffff };
				ALL1USED = true;
			}
			else
			{
				key = { rand_32(), rand_32(), rand_32() };
			}
			Trivium_Primitive Runner;
			Runner = Trivium_Primitive();
			uint32 Sum0 = 0, Sum1 = 0;
			uint32 DyBit = 0;
			//omp_set_num_threads(CORES);
			for (uint64 current = 0; current < cubesize; ++current)
			{
				if (current % (0xfffffff + 1) == 0)
				{
					ofstream file1("Intermediate" + FileNameSet(Idx) + ".txt", ios::app);
					for (int i = 0; i < CubeIndex.size(); ++i)
					{
						file1 << CubeIndex[i] << ",";
					}
					file1 << endl;
					file1 << current << "," << Sum0 << endl;
					file1 << "IVconst: " << IV[0] << "," << IV[1] << "," << IV[2] << endl;
					file1 << "Correct key: " << key[0] << "," << key[1] << "," << key[2] << endl;
					file1.close();
				}
				if ((current & 0xfffffff) == 0xfffffff)
				{
					cout << hex << current << endl;
				}
				Runner.ZroAll();
				Runner.IniKey(key);
				Runner.InitIV(IV);
				for (int countbt = 0; countbt < CubeIndex.size(); ++countbt)
				{
					if (bit32(current, countbt) == 1)
					{
						SetDivBit(Runner.State, 93 + CubeIndex[countbt], 1);
					}
					else
					{
						SetDivBit(Runner.State, 93 + CubeIndex[countbt], 0);
					}
				}
				for (int rd = 0; rd < IniRound; ++rd)
				{
					if (S_NULL[0].find(rd) != S_NULL[0].end())
					{
						DyBit = bitW32(Runner.State, 0);
					}
					else if (S_NULL[2].find(rd) != S_NULL[2].end())
					{
						DyBit = bitW32(Runner.State, 177);
					}
					else if (S_NULL[1].find(rd) != S_NULL[1].end())
					{
						DyBit = bitW32(Runner.State, 93);
					}
					Runner.Update_State();
				}
				Sum0 ^= (Runner.Output_Eq()&(1 ^ DyBit));
			}
			if (Sum0 != 0)
			{
				cout << dec << "False Key Found!" << endl;
				ofstream file1("FindKey.txt", ios::app);
				for (int i = 0; i < CubeIndex.size(); ++i)
				{
					file1 << CubeIndex[i] << ",";
				}
				file1 << endl;

				file1 << "FalseKey:";
				for (int i = 0; i < 3; ++i)
				{
					file1 << key[i] << ",";
				}
				file1 << endl;
				return false;
			}
		}//end for (int tt = 0; tt < TestTime; ++tt)
		return true;
	}

	//Dynamic Z for Wrong Key Guess (1+s^t_i+\xi)*z^R
	bool WheZrosumDyZWrong(vector<uint32> IV, int TestTime = 100)
	{
		set<int> Idx;
		for (int i = 0; i < CubeIndex.size(); ++i)Idx.insert(CubeIndex[i]);
		ofstream file1;
		uint64 cubesize = 1;
		cubesize <<= CubeIndex.size();
		bool ALL1USED = false;
		int MaxWrongRound = 0;
		for (int i = 0; i < 3; ++i)
		{
			for (set<int>::iterator ite = S_NULL[i].begin(); ite != S_NULL[i].end(); ++ite)
			{
				if ((*ite) > MaxWrongRound)MaxWrongRound = *ite;
			}
		}

		for (int tt = 0; tt < TestTime; ++tt)
		{
			cout << "Time:" << tt << endl;
			//Correct key and Wrong key guess
			vector<uint32> key, keyWrong;
			key = { rand_32(), rand_32(), rand_32() };
			keyWrong = { rand_32(), rand_32(), rand_32() };

			//Runner for computing z^R and RunnerWrong for computing \hat{s}^t_i corresponding to wrong key guess
			Trivium_Primitive Runner, RunnerWrong;
			Runner = Trivium_Primitive();
			RunnerWrong = Trivium_Primitive();
			uint32 Sum0 = 0, Sum1 = 0;
			uint32 DyBit = 0;
			//omp_set_num_threads(CORES);
			for (uint64 current = 0; current < cubesize; ++current)
			{
				if (current % (0xfffffff + 1) == 0)
				{
					ofstream file1("Intermediate" + FileNameSet(Idx) + ".txt", ios::app);
					for (int i = 0; i < CubeIndex.size(); ++i)
					{
						file1 << CubeIndex[i] << ",";
					}
					file1 << endl;
					file1 << current << "," << Sum0 << endl;
					file1 << "IVconst: " << IV[0] << "," << IV[1] << "," << IV[2] << endl;
					file1 << "Correct key: " << key[0] << "," << key[1] << "," << key[2] << endl;
					file1 << "Wrong key: " << keyWrong[0] << "," << keyWrong[1] << "," << keyWrong[2] << endl;
					file1.close();
				}
				if ((current & 0xfffffff) == 0xfffffff)
				{
					cout << hex << current << endl;
				}
				Runner.ZroAll();
				Runner.IniKey(key);
				Runner.InitIV(IV);
				RunnerWrong.ZroAll();
				RunnerWrong.IniKey(keyWrong);
				RunnerWrong.InitIV(IV);
				for (int countbt = 0; countbt < CubeIndex.size(); ++countbt)
				{
					if (bit32(current, countbt) == 1)
					{
						SetDivBit(Runner.State, 93 + CubeIndex[countbt], 1);
						SetDivBit(RunnerWrong.State, 93 + CubeIndex[countbt], 1);
					}
					else
					{
						SetDivBit(Runner.State, 93 + CubeIndex[countbt], 0);
						SetDivBit(RunnerWrong.State, 93 + CubeIndex[countbt], 0);
					}
				}
				for (int rd = 0; rd < IniRound; ++rd)
				{
					if (S_NULL[0].find(rd) != S_NULL[0].end())
					{
						DyBit = bitW32(RunnerWrong.State, 0);
					}
					else if (S_NULL[2].find(rd) != S_NULL[2].end())
					{
						DyBit = bitW32(RunnerWrong.State, 177);
					}
					else if (S_NULL[1].find(rd) != S_NULL[1].end())
					{
						DyBit = bitW32(RunnerWrong.State, 93);
					}
					Runner.Update_State();
					if (rd < MaxWrongRound)RunnerWrong.Update_State();
				}
				Sum0 ^= (Runner.Output_Eq()&(1 ^ DyBit));
			}
			if (Sum0 != 0)
			{
				cout << dec << "False Key Found!" << endl;
				ofstream file1("FindKey.txt", ios::app);
				for (int i = 0; i < CubeIndex.size(); ++i)
				{
					file1 << CubeIndex[i] << ",";
				}
				file1 << endl;

				file1 << "FalseKey:";
				for (int i = 0; i < 3; ++i)
				{
					file1 << key[i] << ",";
				}
				file1 << endl;
				return false;
			}
		}//end for (int tt = 0; tt < TestTime; ++tt)
		return true;
	}


	bool WheZrosumDyZWrong_HalfWay(vector<uint32> CorrectKey, vector<uint32> WrongKey, uint64 StartIV, uint32 CurrentSum, vector<uint32> IV, int TestTime = 100)
	{
		set<int> Idx;
		for (int i = 0; i < CubeIndex.size(); ++i)Idx.insert(CubeIndex[i]);
		ofstream file1;
		uint64 cubesize = 1;
		cubesize <<= CubeIndex.size();
		bool ALL1USED = false;
		int MaxWrongRound = 0;
		for (int i = 0; i < 3; ++i)
		{
			for (set<int>::iterator ite = S_NULL[i].begin(); ite != S_NULL[i].end(); ++ite)
			{
				if ((*ite) > MaxWrongRound)MaxWrongRound = *ite;
			}
		}

		for (int tt = 0; tt < TestTime; ++tt)
		{
			cout << "Time:" << tt << endl;
			//Correct key and Wrong key guess
			vector<uint32> key, keyWrong;
			if (tt == 0)
			{
				key = CorrectKey;
				keyWrong = WrongKey;
			}
			else
			{
				key = { rand_32(), rand_32(), rand_32() };
				keyWrong = { rand_32(), rand_32(), rand_32() };
			}
			//Runner for computing z^R and RunnerWrong for computing \hat{s}^t_i corresponding to wrong key guess
			Trivium_Primitive Runner, RunnerWrong;
			Runner = Trivium_Primitive();
			RunnerWrong = Trivium_Primitive();
			uint32 Sum0 = CurrentSum;
			uint32 DyBit = 0;
			//omp_set_num_threads(CORES);
			for (uint64 current = StartIV; current < cubesize; ++current)
			{
				if (current % (0xfffffff + 1) == 0)
				{
					ofstream file1("Intermediate" + FileNameSet(Idx) + ".txt", ios::app);
					for (int i = 0; i < CubeIndex.size(); ++i)
					{
						file1 << CubeIndex[i] << ",";
					}
					file1 << endl;
					file1 << current << "," << Sum0 << endl;
					file1 << "IVconst: " << IV[0] << "," << IV[1] << "," << IV[2] << endl;
					file1 << "Correct key: " << key[0] << "," << key[1] << "," << key[2] << endl;
					file1 << "Wrong key: " << keyWrong[0] << "," << keyWrong[1] << "," << keyWrong[2] << endl;
					file1.close();
				}
				if ((current & 0xfffffff) == 0xfffffff)
				{
					cout << hex << current << endl;
				}
				Runner.ZroAll();
				Runner.IniKey(key);
				Runner.InitIV(IV);
				RunnerWrong.ZroAll();
				RunnerWrong.IniKey(keyWrong);
				RunnerWrong.InitIV(IV);
				for (int countbt = 0; countbt < CubeIndex.size(); ++countbt)
				{
					if (bit32(current, countbt) == 1)
					{
						SetDivBit(Runner.State, 93 + CubeIndex[countbt], 1);
						SetDivBit(RunnerWrong.State, 93 + CubeIndex[countbt], 1);
					}
					else
					{
						SetDivBit(Runner.State, 93 + CubeIndex[countbt], 0);
						SetDivBit(RunnerWrong.State, 93 + CubeIndex[countbt], 0);
					}
				}
				for (int rd = 0; rd < IniRound; ++rd)
				{
					if (S_NULL[0].find(rd) != S_NULL[0].end())
					{
						DyBit = bitW32(RunnerWrong.State, 0);
					}
					else if (S_NULL[2].find(rd) != S_NULL[2].end())
					{
						DyBit = bitW32(RunnerWrong.State, 177);
					}
					else if (S_NULL[1].find(rd) != S_NULL[1].end())
					{
						DyBit = bitW32(RunnerWrong.State, 93);
					}
					Runner.Update_State();
					if (rd < MaxWrongRound)RunnerWrong.Update_State();
				}
				Sum0 ^= (Runner.Output_Eq()&(1 ^ DyBit));
			}
			if (Sum0 != 0)
			{
				cout << dec << "False Key Found!" << endl;
				ofstream file1("FindKey.txt", ios::app);
				for (int i = 0; i < CubeIndex.size(); ++i)
				{
					file1 << CubeIndex[i] << ",";
				}
				file1 << endl;

				file1 << "FalseKey:";
				for (int i = 0; i < 3; ++i)
				{
					file1 << key[i] << ",";
				}
				file1 << endl;
				return false;
			}
		}//end for (int tt = 0; tt < TestTime; ++tt)
		return true;
	}


};



set<int> RandCube(int Dim)
{
	set<int> ex;
	//for (int i = 0; i < 80; ++i)Free.insert(i);
	while (ex.size() != Dim)
	{
		//int pick=
		ex.insert(rand_32() % 80);
	}
	return ex;
}

const set<int> ALL_IV_INDEX = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79 };

set<int> RandCube(int dim, set<int> Free = ALL_IV_INDEX)
{
	//set<int> Free;
	if (dim >= Free.size())return Free;
	//for (int i = 0; i < 96; ++i)Free.insert(i);
	while (Free.size() != dim)
	{
		int rd = rand_32() % 96;
		Free.erase(rd);
	}
	return Free;
}

int RandPick(set<int> Free)
{
	int pos = rand() % Free.size();
	int count = 0;
	for (set<int>::iterator ite = Free.begin(); ite != Free.end(); ite++)
	{
		if (count == pos)return *ite;
		count++;
	}
}

set<int> RandSet(int dim, set<int> Free)
{
	if (dim >= Free.size())return Free;
	set<int> res;
	while (res.size() < dim)
	{
		res.insert(RandPick(Free));
	}
	return res;
}

