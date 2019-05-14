#include<filesystem>
#include<iostream>

using namespace std;

int main()
{
	FILE* f1;
	FILE* f2;

	char f1Name[20] = "A.mp4";
	char f2Name[20] = "B.mp4";

	errno_t err1, err2;
	err1 = fopen_s(&f1, f1Name, "rb");
	err2 = fopen_s(&f2, f2Name, "rb");

	if (!(err1 && err2))
	{
		cout << "Verifying Test " << f1Name << " and " << f2Name << endl << endl;
		
		char a;
		char b;
		while (1)
		{
			// 두 개의 파일이 모두 끝에 도달하지 않을 경우
			if ((feof(f1) == 0) && (feof(f2) == 0))
			{
				a = fgetc(f1);
				b = fgetc(f2);
				if (a != b)
				{
					cout << "Not the same" << f1Name << " and " << f2Name << endl;
					return 0;
				}
			}
			// 첫번째 파일만 끝에 도달할 경우
			else if ((feof(f1) != 0) && (feof(f2) == 0))
			{
				cout << "Not the same" << f1Name << " and " << f2Name << endl;
				return 0;
			}
			// 두번째 파일만 끝에 도달할 경우
			else if ((feof(f1) == 0) && (feof(f2) != 0))
			{
				cout << "Not the same" << f1Name << " and " << f2Name << endl;
				return 0;
			}
			// 두개의 파일 모두 끝에 도달한 경우
			else if ((feof(f1) != 0) && (feof(f2) != 0))
			{
				cout << "Same " << f1Name << " and " << f2Name << endl;
				return 0;
			}
		}
	}
	else
	{
		cout << "failed opening file..\n";
		return 0;
	}

	return 0;
}