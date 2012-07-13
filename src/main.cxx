
#include <iostream>
#include <istream>
#include <ostream>

#include <cstdlib>

#include <string>

#include <signal.h>

using namespace std;

string sol;

void terminate(int signal)
{
	signal = signal;

	cout << sol << "A" << endl;

	exit(EXIT_SUCCESS);
}

int main()
{
	signal(SIGINT, &terminate);

	char l;

	cin.unsetf(ios_base::skipws);

	while (! cin.eof())
	{
		cin >> l;

		if (l == '\n' || l == '\r')
		{
			continue;
		}

		cout << l << endl;
	}

	while (true)
	{
	}

	cout << "A" << endl;

	return 0;
}

