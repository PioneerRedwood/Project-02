#include<filesystem>
#include<iostream>
#include<string>
#include<windows.h>

using namespace std;

#define bufLen 4096 * 4

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
			int n = 1;

			char* buf1;
			if ((buf1 = (char*) calloc(bufLen, sizeof(char))) == NULL)
			{
				cout << "failed calloc! buf1\n";
				return 0;
			}
			char* buf2; 
			if ((buf2 = (char*) malloc(bufLen * sizeof(char))) == NULL)
			{
				cout << "failed malloc buf2\n";
				return 0;
			}

			while (count != f1Size)
			{
				if ((feof(f1) == 0) && (feof(f2) == 0))			// 두 개의 파일이 모두 끝에 도달하지 않을 경우
				{
					memset(buf2, '\0', sizeof(buf2));

					fread(buf1, sizeof(char), bufLen, f1);
					fread(buf2, sizeof(char), bufLen, f2);
					
					if (memcmp(buf1, buf2, bufLen) != 0)
					{
						cout << "Not the same " << f1Name << " and " << f2Name << endl;
						cout << "Count Num: " << count << "\n";
						return 0;
					}
					else
					{
						count += bufLen;
						if (count / 1024 == n * 1024)
						{
							cout << "Conducting.. " << count / f1Size * 100 << " %\n";
							n++;
						}
					}
				}
				if (count >= f1Size)
				{
					cout << "\nSame " << f1Name << " and " << f2Name << endl;
					cout << count << " byte (s) read.\n";
					break;
				}
			}
			free(buf1);
			free(buf2);
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
	fclose(f1);
	fclose(f2);
	return 0;
}