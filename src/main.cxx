
#include <iostream>
#include <istream>
#include <ostream>

#include <cstdlib>

#include <algorithm>
#include <deque>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <vector>
#include <utility>

#include <signal.h>

using namespace std;

#define INTERACTIVE

string sol;

void terminate(int signal)
{
	signal = signal;

	cout << sol << "A" << endl;

	exit(EXIT_SUCCESS);
}

class bd_map
{
	public:
	int n_, m_, i_;
	int r_x_, r_y_;
	int l_x_, l_y_;
	vector<vector<char> > x_;

	bd_map(): n_(0), m_(0), i_(0),
			r_x_(0), r_y_(0), l_x_(0), l_y_(0),
			x_(vector<vector<char> >(1))
	{
		x_.push_back(vector<char>(0));
	}

	void add(char c)
	{
		switch (c)
		{
			case '#':
			case ' ':
			case '.':
			case '*':
			case '\\':
			case 'R':
			case 'L':
				if (c == 'R')
				{
					r_x_ = i_;
					r_y_ = m_;
				}
				if (c == 'L')
				{
					l_x_ = i_;
					l_y_ = m_;
				}
				x_[m_].push_back(c);
				n_ = max(n_, i_);
				++i_;
				break;
			case '\n':
				x_.push_back(vector<char>());
				++m_;
				i_ = 0;
				break;
			default:
				return;
		}
	}

	void fin_init()
	{
		--m_;
		r_y_ = m_ - r_y_;
		l_y_ = m_ - l_y_;
	}

	void disp()
	{
		cout << "Playing field: " << (n_ + 1) << "x" << (m_ + 1) << endl;
		cout << "Robot at (" << (r_x_ + 1) << ", " << (r_y_ + 1) << "), ";
		cout << "exit at (" << (l_x_ + 1) << ", " << (l_y_ + 1) << ")" << endl;
		for (int i = 0; i <= m_; ++i)
		{
			for (int j = 0; j <= n_; ++j)
			{
				cout << (*this)(j, i);
			}
			cout << endl;
		}
		cout << "OK" << endl;
	}

	char &operator()(int x, int y)
	{
		return x_[y][x];
	}
};

class bd_game
{
	public:
	bd_map m_;

	bd_game(): m_(bd_map())
	{
	}
};

int main()
{
	signal(SIGINT, &terminate);

	bd_game g;

	char c;

	cin.unsetf(ios_base::skipws);

	while (true)
	{
		cin >> c;

		if (cin.eof())
		{
			break;
		}

		g.m_.add(c);
	}

	g.m_.fin_init();

#ifdef INTERACTIVE

	g.m_.disp();

#else

	/*
	while (true)
	{
	}
	*/

	cout << "A" << endl;

#endif

	return 0;
}

