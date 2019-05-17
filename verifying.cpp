#include<filesystem>
#include<iostream>

using namespace std;

int main()
{
	FILE* f1;
	FILE* f2;

	char f1Name[20] = "Adata";
	char f2Name[20] = "Bdata";

	errno_t err1, err2;
	err1 = fopen_s(&f1, f1Name, "rb");
	err2 = fopen_s(&f2, f2Name, "rb");

	if (!(err1 && err2))
	{
		cout << "Verifying Test " << f1Name << " and " << f2Name << endl << endl;
		
		double f1Size, f2Size;
		double count = 0;

		fseek(f1, 0, SEEK_END);
		f1Size = ftell(f1);
		fseek(f1, 0, SEEK_SET);

		fseek(f2, 0, SEEK_END);
		f2Size = ftell(f2);
		fseek(f2, 0, SEEK_SET);

		if (f1Size == f2Size)
		{
			cout << f1Name << "'s Size " << f1Size << " | " << f2Name << "'s Size " << f2Size << "\n\n";
			char a;
			char b;

			while (count != f1Size)
			{
				cout << "test... \n";
				
				if ((feof(f1) == 0) && (feof(f2) == 0))			// 두 개의 파일이 모두 끝에 도달하지 않을 경우
				{
					a = fgetc(f1);
					b = fgetc(f2);
					if (a != b)
					{
						cout << "Not the same " << f1Name << " and " << f2Name << endl;
						return 0;
					}
					count++;
				}
				else if ((feof(f1) != 0) && (feof(f2) == 0))	// 첫번째 파일만 끝에 도달할 경우
				{
					cout << "Not the same " << f1Name << " and " << f2Name << endl;
					return 0;
				}
				else if ((feof(f1) == 0) && (feof(f2) != 0))	// 두번째 파일만 끝에 도달할 경우
				{
					cout << "Not the same " << f1Name << " and " << f2Name << endl;
					return 0;
				}
				else if ((feof(f1) != 0) && (feof(f2) != 0))	// 두개의 파일 모두 끝에 도달한 경우
				{
					cout << "\nSame " << f1Name << " and " << f2Name << endl;
					cout << "count: " << count << " byte (s) read.\n";
					return 0;
				}
			}
			cout << "\nSame " << f1Name << " and " << f2Name << endl;
			cout << count << " byte (s) read.\n";
		}
		else
		{
			cout << "Sizes are not same!\n";
			return 0;
		}
	}
	else
	{
		cout << "failed opening file..\n";
		return 0;
	}
	return 0;
}