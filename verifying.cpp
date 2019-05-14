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
			// �� ���� ������ ��� ���� �������� ���� ���
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
			// ù��° ���ϸ� ���� ������ ���
			else if ((feof(f1) != 0) && (feof(f2) == 0))
			{
				cout << "Not the same" << f1Name << " and " << f2Name << endl;
				return 0;
			}
			// �ι�° ���ϸ� ���� ������ ���
			else if ((feof(f1) == 0) && (feof(f2) != 0))
			{
				cout << "Not the same" << f1Name << " and " << f2Name << endl;
				return 0;
			}
			// �ΰ��� ���� ��� ���� ������ ���
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