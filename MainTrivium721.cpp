#include"Trivium.hpp"

int main()
{
	vector<set<int>> S_NULL = { {},{ 290 },{} };
	set<int> NullIV = { 58, 64, 72 };
	for (int i = 0; i < 40; ++i)NullIV.insert(2 * i + 1);

	set<int> Free;
	for (int i = 0; i < 80; ++i)
	{
		if (NullIV.find(i) == NullIV.end())
		{
			Free.insert(i);
		}
	}


	int Round = 721;
	//Dimension of Test Cubes.
	//32: Prove/Disprove the key recovery attack related results
	//30: Prove the pure z721 has IV-degree 29.
	int TargDim = 32;
	//vector<uint32> IVconst = { 0,0,0 };

	int TestTime = 1000;


	//omp_set_num_threads(CORES);
#pragma omp parallel for num_threads(CORES)  
	for (int i = 0; i < TestTime; ++i)
	{
		set<int> TmpCube = RandCube(TargDim, Free);
		vector<uint32> IVconst = { 0,0,0 };
		for (set<int>::iterator ite = Free.begin(); ite != Free.end(); ++ite)
		{
			if (TmpCube.find(*ite) == TmpCube.end())
				SetDivBit(IVconst, *ite, (rand_32() & 1));
		}
		Trivium_DynamicCube tdy = Trivium_DynamicCube(TmpCube, S_NULL, NullIV, Round);

		//Prove Wrong Key Guesses Also Get Zero Sums (TargDim=32).
		bool whe = tdy.WheZrosumDyZWrong(IVconst, 1);

		//Prove the Correct Key guess has Zero Sum (TargDim=32).
		//bool whe = tdy.WheZrosumDyZ(IVconst, 1);

		//Prove the IV-degree of z721 is 29 (TargDim=30).
		//bool whe = tdy.WheZrosumPureZ(IVconst, 1);
		if (whe == true)
		{
			cout << Round << "Correct\n";
		}
		else
		{
			cout << Round << "Wrong\n";
		}
	}

	getchar();


	return 0;

}


